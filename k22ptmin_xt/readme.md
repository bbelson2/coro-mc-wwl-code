# k22ptmin_xt

This file is subject to the terms and conditions defined in file 'LICENSE.txt', which is part of this source code package.

The project contains the protothreads version of a minimal K22F C++ application. 
It is to be compared to the coroutine version to measure the overhead of task switching.

The application contains a simple scheduler, in which each task is implemented as a protothread.
The task is initially suspended and intermittently resumed by the scheduler.  

For this test, there are two tasks: one sets a GPIO pin high, the other sets it low.
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
1. Set C/C++ Application to `DebugLLVM_NS/k22ptmin_xt.elf`
1. Select the `Debugger` tab.
1. Set Interface to `OpenSDA Embedded Debug - USB Port`
1. If no device is selected, press `Select Device`.
1. `NXP` > `K2x` > `K22FN512M12`
1. Press `Apply` then `Close`. 

## Debug

1. Press the Debug tool, or `F11`.
1. If prompted, select the debug configuration `/k22ptmin_xt/DebugLLVM_NS/k22ptmin_xt.elf`.
1. If prompted for a configuration, select `gdbserver`.

# Project setup

The steps below were used to create the project.

## Create project

1. Kinetis Design Studio 3.2.0
1. File > New > Processor Expert Project...
1. Project name = `k22ptmin_xt`
1. Boards > Kinetis > FRDM-K22F
1. Kinetis SDK = None; Processor Expert = True
1. Target compiler = GNU C Compiler

## Tool-chain

Follow the instructions in the main [ProjectConfig.md](../ProjectConfig.md) to alter the tool chain to use LLVM/clang.

## Components

Follow the instructions in the main [ProjectConfig.md](../ProjectConfig.md) to change the CPU component clock speed and to add the Bit1 component.

# Project development

Add the following files from `ptShared/ptSources`:

- ptcore_scheduler.cpp
- ptmain_cpp.cpp
- pttask_test.cpp

Add the following files from `Shared/Sources`:

- min_services.cpp

## main.c

```
extern void pt_main_cpp();
...
  pt_main_cpp();
```
## Size

```
Invoking: Cross ARM GNU Print Size
arm-none-eabi-size --format=berkeley "k22ptmin_xt.elf"
   text	   data	    bss	    dec	    hex	filename
   3012	    140	   1060	   4212	   1074	k22ptmin_xt.elf
Finished building: k22ptmin_xt.siz
```

