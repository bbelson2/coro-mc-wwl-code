# Project configuration
C++20 Coroutines on Microcontrollers - What We Learned

## Overview

This file describes the configuration details common to projects in this repository.

## Common components

The Processor Expert (PEx) tools were used to generate hardware access layer code wherever possible. Some of the steps for setting up the PEx components are unobvious. To aid reproducability, the steps for all common components are described below. 

### K22F CPU clock speed

1. Switch the Component Inspector to Advanced mode, without Tabs.
1. Clock settings > Clock source settings. Click minus until only 1 remains.
1. Clock settings > Clock source settings > Clock source setting 0. > External reference clock.
1. OSC0ERCLK clock > `Enabled`
1. Clock settings > Clock source settings > Clock source setting 0. > MCG settings. 
1. MCG mode => `PEE` 
1. Clock settings > Clock source settings > Clock source setting 0. > MCG settings > PLL 0 settings. 
1. PLL output (MHz) => `120.0`
1. Clock settings > Clock configurations. Click minus until only 1 remains.
1. Clock settings > Clock configurations > Clock configuration 0 > System clocks.
1. Core clock => `120` (120 MHz)
1. Bus  clock => `60` (60 MHz)
1. External bus clock => `60` (60 MHz)
1. Flash clock => `24` (24 MHz)

### BitIO

1. Processor Expert perspective 
1. Components Library
1. BitIO
1. New Component [Kinetis/BitIO]
1. Component Inspector for Bit1 
1. Device = Kinetis/I2C_LDD
1. Pin for I/O => UART1_RX_TGTMCU
1. Direction = Output
1. Init. value = 0
1. Switch to advanced mode; Safe mode => Off 
1. Check enabled: OnTransmitData & OnReceiveData

## Common settings

The steps below contain detailed instructions for changing the settings of a default project to one that uses the clang compiler and supports C++20 coroutines.

### Parallel build

1. Project properties > C/C++ Build > Behavior tab.
1. Enable parallel build => `true`.

### Refresh policy

This needs to be fixed when projects are copied; otherwise the project will be rebuilt every time. 

1. Project properties > C/C++ Build > Refresh Policy tab.
1. Ensure that the only item in Resources is the current project.

### Artifact size

1. Project properties > C/C++ Build > Settings > Toolchains tab.
1. Print size => `true`

## Tool-chain

The steps below contain detailed instructions for changing the settings of a default project to one that uses the clang compiler and supports C++20 coroutines.

Here we alter the tool chain to use `LLVM/clang`.

If necessary, restart Kinetic Design Studio with `clang` in the PATH, e.g.:

```
set PATH=E:\llvm-trunk\install\bin;%PATH%
E:\Freescale\KDS_v3\eclipse\kinetis-design-studio.exe -data E:\Source\repos\coro-mc-wwl.code
```

Create a project configuration which uses `clang` as a compiler in place of `gcc`. 

### New configuration

1. Project > Build Configurations > Manage...
1. New...
1. Name: DebugLLVM; Copy existing settings from Debug. OK.
1. Select DebugLLVM and Set active.
1. Project > Properties > C/C++ Build > Settings
1. Ensure Configuration = DebugLLVM
1. C/C++ Build > Build Variables
1. Add ARM_INCLUDE => C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q1-update\arm-none-eabi\include *(Your folder name may differ.)*

### C++ Settings

1. C/C++ Build > Settings > Tool settings > Cross ARM C++ Compiler 
1. Command => clang++
1. Optimization > Language standard => Toolchain default (GNU ISO 1998 C++)
1. Optimization > ABI version => Toolchain default (2)
1. Optimization > Do not use exceptions => checked
1. Optimization > Do not use RTTI => checked.
1. Optimization > Do not use _cxa_atexit() => checked
1. Optimization > Do not use thread-safe statics => checked.
1. Miscellaneous > Other compiler flags => -fshort-enums -target armv7m-none-eabi "-fcoroutines-ts" "-stdlib=libc++" -std=c++14 
1. Tool settings > Cross ARM C++ Compiler > Includes
1. Add the following to the list:
    - "${ARM_INCLUDE}\c++\6.3.1\arm-none-eabi"
    - "${ARM_INCLUDE}\c++\6.3.1"
    - "${ARM_INCLUDE}"

### C Settings

1. Tool settings > Cross ARM C Compiler 
1. Command => clang
1. Miscellaneous > Other compiler flags => -fshort-enums -target armv7m-none-eabi
1. Includes. Add the following to the list:
    - "${ARM_INCLUDE}"
1. Change startup.c (see http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0774g/jhg1476893564298.html)
	```
	__attribute__((naked)) void __thumb_startup(void)
	```
	becomes:
	```
	#if defined (__clang__)
	void __thumb_startup(void)
	#else
	__attribute__((naked)) void __thumb_startup(void)
	#endif
	```
