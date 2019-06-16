# k22fawaitmin_xt

This file is subject to the terms and conditions defined in file 'LICENSE.txt', which is part of this source code package.

The project contains the co_await version of a minimal K22F application. 
It is to be compared to the protothreads version to measure the overhead of task switching.

This version uses an external timer; it is otherwise the same as k22fawaitmin.

An oscilloscope is used to measure the time taken for a complete sequence.

# Build & run

## Build

To build the project, ensure that the active build configuration is `DebugLLVM_NS`, then build the project.

## Prepare to run

To run the project:

1. Check that the development board is connected to a breadboard as shown in [the diagram](../coro_min_test_bb.png).
1. Ensure that an oscilloscope is connected to the breadboard as shown in the diagram.
1. Ensure that the development board is connected to the PC's USB port.

## Prepare to run/debug

1. Select the project in the Project Explorer.
1. `Run` > `Debug Configurations...`
1. Select `GDB PEMicro Interface Debugging`.
1. Press the `New launch configuration` button.
1. Select the `Main` tab.
1. Set C/C++ Application to `DebugLLVM_NS/k22awaitmin_xt.elf`
1. Select the `Debugger` tab.
1. Set Interface to `OpenSDA Embedded Debug - USB Port`
1. If no device is selected, press `Select Device`.
1. `NXP` > `K2x` > `K22FN512M12`
1. Press `Apply` then `Close`. 

## Debug

1. Press the Debug tool, or `F11`.
1. If prompted, select the debug configuration `/k22awaitmin_xt/DebugLLVM_NS/k22awaitmin_xt.elf`.
1. If prompted for a configuration, select `gdbserver`.

# Project setup

The steps below were used to create the project.

## Create project

1. Kinetis Design Studio 3.2.0
1. File > New > Processor Expert Project...
1. Project name = `k22awaitmin_xt`
1. Boards > Kinetis > FRDM-K22F
1. Kinetis SDK = None; Processor Expert = True
1. Target compiler = GNU C Compiler

## Tool-chain

Follow the instructions in the main [ProjectConfig.md](../ProjectConfig.md) to alter the tool chain to use LLVM/clang.

## Components

Follow the instructions in the main [ProjectConfig.md](../ProjectConfig.md) to change the CPU componentclock speed and to add the Bit1 component.

# Project development

Add the following files from `Shared/Sources`:

- main_cpp.cpp
- task_test.cpp
- min_services.cpp

## main.c

```
extern void main_cpp();
...
  main_cpp();
```
## Size

```
Invoking: Cross ARM GNU Print Size
arm-none-eabi-size --format=berkeley "k22awaitmin_xt.elf"
   text	   data	    bss	    dec	    hex	filename
   3920	    144	   1080	   5144	   1418	k22awaitmin_xt.elf
Finished building: k22awaitmin_xt.siz
```

