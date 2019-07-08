# Library
C++20 Coroutines on Microcontrollers - What We Learned

## Overview

This file describes the runtime library created for the projects in this repo.

## Objectives

- Minimal dependencies on Standard Template Library (STL).
- Ideally, no dependencies on STL.
- No use of heap memory.
- No use of exceptions.
- Transparency of coding techniques, particularly from the point-of-view of C programmers (as opposed to C++ programmers).

## First iteration

The first version of the library included the following files:

| File | Classes | Description |
|---|---|--|
| experimental/resumable | `coroutine_traits<>` `coroutine_handle<>` `suspend_never` `suspend_always` | Coroutine contract from N4680 & N4736 |
| core_crit_sec.h | `mutex_t` | Critical section adapter class |
| core_resumable.h | `resumable<>` | Base resumable type, used for tasks. |
| core_future.h | `future_t<>` `promise_t<>` `awaitable_state<>` | Futures with (i) embedded awaitable contract and (ii) shared state on heap |
| core_scheduler.h | `task_t` `scheduler_t` | Simple scheduler |

The application project [k22fawait1](./k22fawait1/readme.md) makes use of these classes to build hardware abstraction layers as follows:

| File | Content |
|---|---|
| api_adc.h/.cpp | Asynchronously read an analog to digital converter channel |
| api_i2c.h/.cpp | Asynchronously read and write to a I2C bus (for use with the development board's accelerometer) |
| api_timer.h/.cpp | Asynchronous timer event stream |

For the application project, application-level test classes were created, based on the HAL code,  including:

| File | Content |
|---|---|
| task_adc.cpp | Asynchronously read a pair of analog to digital converter channels (x and y coordinates from a joystick) |
| task_i2c.cpp | Asynchronously read and write to a I2C bus, thereby reading the development board's accelerometer |
| task_timer.cpp | Asynchronous timer event stream |

### Benefits

The library delivered significant benefits both to the application programmer and the HAL library writer.

For example, the ADC task could include efficient & transparent asynchronous code such as: 

```c++
co_await start_adc(ADC_CHANNEL_X);
co_await start_adc(ADC_CHANNEL_Y);

for (;;) {
    auto x = co_await read_adc(ADC_CHANNEL_X);
    auto y = co_await read_adc(ADC_CHANNEL_Y);

    // Use x, y

    co_await wait_on_ticks(10);
}
```

### Problems

There are a number of problems with the first iteration.

- The split phase event class `split_phase_event_t` makes use of lambda expressions. These may be considered to fail the transparency objective.

- The split phase event class `split_phase_event_t` makes use of STL's `<functional>`, to store lambda expressions for deferred operations. (These lambdas cannot be stored as function pointers because they use captures.) For the next iteration, therefore, we decided to avoid lambdas and instead to use void function pointers; in place of captures, we will use global static data.

- `future_t` and its related classes use dynamic memory allocation. Because `future_t` and `promise_t` maintain a shared `awaitable_state_t` via a usage count, the memory is dynamically allocated (through a smart pointer). We will avoid this in the second iteration by by using global static data storage for promise and state.

- The scheduler uses the STL `stack<>` container to hold the current set of blocked coroutines for a task.

- The I2C api is too complex (see [version 1](archive/iteration1/api_i2c.cpp)); it contains too much secondary level code, which should be split out into an implementation layer. Low-level calls which read and write byte arrays should be implemented as asynchronous primitives delivered as coroutines - each matching a synchronous primitive. The current read and write calls should be coroutines composed of these primitives.

## Second iteration

### Problem

```c++
future_t<byte> read_i2c(uint8_t slave_address, 
  uint8_t reg, uint8_t* data, word len) {
  byte rc = I2C_SelectSlave(slave_address);
  if (rc == 0) {
    rc = co_await I2C_SendChar_async(reg);
  }
  if (rc == 0) {
    word recv;
    rc = co_await I2C_RecvBlock_async(data, len, &recv);
  }
  I2C_SendStop();
  co_return rc;
}
```

Composition of coroutines (such as the code above) fails with the initial `future_t<>` implementation. The classes `future_t<>` and `promise_t<>` contain both the core code of the classes - including the constructors - and the methods of the awaitable contract. 

Thus the coroutine above attempts to call a constructor for `future_t<>` with all four of the arguments. 

### Objective

- Make composition of coroutines work correctly by separating the awaitable classes' core implementation and their awaitable contracts. In particular, the types `future_t::promise_type` and `promise_t<>` must be separated.

### Description

The `future_t<>` awaitable contract was moved to the `std::experimental::coroutine_traits<>` extension class.

```c++
namespace std { namespace experimental {
template<class T,
	class... Args>
	struct coroutine_traits<future_t<T>, Args...>
{
    struct promise_type {
        promise_t<T> _promise;
        ...
    };
};
```

### Benefits

- Composition of coroutines is now functional. 

### Observation

The version 1 code appeared correct according to the standard; however, it failed to compile as desired. It is arguable that the standard is too complex to implement in a straightforward fashion.

## Third iteration

### Objectives

- Remove remaining STL `stack<>`, used for blocked coroutine list.
- Remove heap dependency of `future_t<>` (the shared state).

### Discussion

#### A linked list for coroutines

We must change the architecture - add active coroutine stack to `task_t`, implemented as a linked list. 
All awaitables must now inherit from a class which links to the next awaitable.

But which object can become a member of the linked list? The `future<>` is not eligible, because it has move semantics but no copy semantics. Nor is `promise_t<>`, because it is not always created explicitly - the `future_t<>::promise_type` creates the `promise_t<>`.
It appears that the best (only?) candidate is the shared state class.

#### A heap-free shared state

The shared state is currently created within a counted shared smart pointer, `counted_ptr<state_t>`. The `state_t` is a template parameter of `future_t<>` (and, in parallel, of `promise_t<T, state_t>`).

We can make this arrangement less inflexible by promoting the shared state holder (e.g. `counted_ptr<>`) to become a template argument. This will allow us to use a weak pointer instead of a shared pointer if we wish. Such an arrangement would allow the state to be embedded within the `promise_t<>`, when the promise is static in scope and lifetime.

### Two creation patterns for `future_t<>`

1. Explicitly created in a split-phase coroutine, using `promise_t::get_future()`.

1. Implicitly created during a `co_return` from a coroutine, via `future_t<>::promise_type::get_return_object()`.

First we investigate these in detail, then examine their suitability under the new state architecture.

#### Explicit construction during split-phase

1. During coroutine, `promise_t<>` is created as a local variable, with default value.

1. `promise_t<>`'s state is passed to the deferred lambda as a capture.

1. After immediate action, `promise_t<>::get_future()` is called explicitly by coroutine just before exit. (Deferred action may or may not ave taken place by now.)

#### Created during co_return

1. During coroutine ramp, `promise_type` is instantiated. Nested structure `promise_t<>` is constructed within `promise_type`. There are no constructor parameters for `promise_t<>`. As a result, the awaitable state's data is initialised to the default value: `T()`.

1. During `co_return`, `promise_type::return_value(rc)` is called, which sets the value of the `promise_t<>`'s state, via `_promise.set_value(rc)`.

1. Next, `promise_type::get_return_object()` calls `_promise.get_future()` which creates the `future_t<>` using the (now resolved) state as the constructor's parameter.

## Fourth iteration

### Objectives

- Remove remaining heap usage.
- Remove remaining STL dependencies.
