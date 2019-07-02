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
| 

The application project [k22fawait1](./k22fawait1/readme.md) makes use of these classes to build hardware abstraction layers as follows:

| File | Content |
|---|---|
| apiadc.h/.cpp | Asynchronously read an analog to digital converter channel |
| api_i2c.h/.cpp | Asynchronously read and write to a I2C bus (for use with the development board's accelerometer) |
| api_timer.h | Asynchronous timer event stream |

### Benefits

The library delivered significant benefits both to the application programmer and the HAL library writer. (See above...)

### Problems

There are a number of problems with the first iteration.

- The split phase event class `split_phase_event_t` makes use of lambda expressions. These may be considered to fail the transparency objective.

- The split phase event class `split_phase_event_t` makes use of STL's `<functional>`, to store lambda expressions for deferred operations. (These lambdas cannot be stored as function pointers because they use captures.) For this iteration, therefore, we decided to avoid lambdas and instead to use void function pointers; in place of captures, we use global static data.

- `future_t` and its related classes use dynamic memory allocation. Because `future_t` and `promise_t` maintain a shared `awaitable_state_t` via a usage count, the memory is dynamically allocated (through a smart pointer). We avoid this in the second iteration by by using global static data.

- The scheduler uses the STL `stack<>` container to hold the current set of blocked coroutines for a task.

- The I2C api is too complex; it contains too much secondary level code, which should be split out into an implementation. The low-level calls which (i) read and (ii) write byte arrays should be implemented as primitive coroutines, and the current read and write calls should be coroutines composed of these primitives.

## Second iteration

