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

Fo the application project, application-level test classes were created, based on the HAL code,  including:

| File | Content |
|---|---|
| task_adc.cpp | Asynchronously read a pair of analog to digital converter channels (x and y coordinates from a joystick) |
| task_i2c.cpp | Asynchronously read and write to a I2C bus, thereby reading the development board's accelerometer |
| task_timer.cpp | Asynchronous timer event stream |

### Benefits

The library delivered significant benefits both to the application programmer and the HAL library writer.

For example, the ADC task could include efficient & transparent asynchronous code such as: 

```
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

```
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

### Benefits

- Composition of coroutines is now functional. 

### Observation

The version 1 code appeared correct according to the standard; however, it failed to compile as desired. It is arguable that the standard is too complex to implement in a straightforward fashion.

## Third iteration

### Objectives

- Remove remaining STL `stack<>`, used for blocked coroutine list.

### Description

- Change architecture - add active coroutine stack to `task_t` as a linked list. 
- All awaitables must now inherit from a class which links to the next .

## Fourth iteration

### Objectives

- Remove remaining heap usage.
- Remove remaining STL dependencies.
- 