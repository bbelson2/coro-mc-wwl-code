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

### Term1

1. Processor Expert perspective 
1. Components Library
1. Term
1. Component Inspector for Term1 > Inhr1:AsynchroSerial
1. Settings > Channel = UART1
1. Settings > Baud rate = 115200 baud
1. Settings > Receiver > RxD = PTE1
1. Settings > Transmitter > TxD = PTE0

### ADC1

1. Processor Expert perspective 
1. Components Library
1. ADC
1. Component Inspector for AD1
1. A/D Converter = ADC0
1. Interrupt service/event > Interrupt service/event = true
1. Interrupt service/event > A/D interrupt = INT_ADC0
1. Interrupt service/event > A/D interrupt priority = medium priority (112)
1. A/D channels > A/D channel (pin) = ADC0_DM0/ADC1_DM3
1. Sample time = 20 = long
1. Conversion time = 17.166138 micro s
1. Enable methods: Calibrate() and GetCalibrationStatus()

### ADC2

1. Processor Expert perspective 
1. Components Library
1. ADC
1. Component Inspector for AD1
1. A/D Converter = ADC1
1. Interrupt service/event > Interrupt service/event = true
1. Interrupt service/event > A/D interrupt = INT_ADC1
1. Interrupt service/event > A/D interrupt priority = medium priority (112)
1. A/D channels > A/D channel (pin) = ADC1_DM1/ADC0_DM3
1. Sample time = 20 = long
1. Conversion time = 17.166138 micro s
1. Enable methods: Calibrate() and GetCalibrationStatus()

### CS1

1. Processor Expert perspective 
1. Components Library
1. CriticalSection
1. Component Inspector for CS1 
1. Component name = CS1
1. SDK = MCUC1
1. Use ProcessorExpert Default = false
1. Use FreeRTOS = false

### TU1

1. Processor Expert perspective 
1. Components Library
1. TimerUnit
1. Component Inspector for TU1 
1. Component name = TU1
1. Device = FTM0_MOD
1. Counter = FTM0_CNT
1. Input clock source > Counter frequency = 256 Hz
1. Counter restart = On-match
1. Counter restart > Period device = FTM0_MOD
1. Counter restart > Period = 1 Hz
1. Counter restart > Interrupt = Enabled
1. Initialization > Auto initialization = yes
1. Interrupt service/event = Enabled
1. Enable: TU1_OnCounterRestart

### FC1

1. Processor Expert perspective 
1. Components Library
1. FreeCntr
1. New Component [Kinetis/TimerUnit_LDD]
1. Component Inspector for FC1 
1. Component name = FC1
1. Device = FTM1_MOD
1. Counter = FTM1_CNT
1. Interrupt service/event = Enabled 
1. Period/offset = 3.125 ms
1. Interrupt service/event = true
1. Enable: FC1_OnInterrupt

### I2C

1. Processor Expert perspective 
1. Components Library
1. InternalI2C
1. New Component [Kinetis/InternalI2C]
1. Component Inspector for CI2C1 
1. Component name = Rename to I2C
1. Device = Kinetis/I2C_LDD
1. I2C channel => I2C0
1. Mode selection = MASTER
1. Bits 0-2 of Frequency divider register = 001
1. Bits 3-5 of Frequency divider register = 100
1. Internal frequency (multipler factor) = 10.48576 MHz
1. Interrupt service/event > Interrupt service/event = on
1. Interrupt service/event > Interrupt = INT_I2C0
1. Interrupt service/event > Interrupt priority = medium priority (112)
1. Data and Clock > SDA Pin = PTB3
1. Data and Clock > SDA CLn = PTB2
1. MASTER mode > Automatic stop mode = yes
1. MASTER mode > Initialization > Address mode = 7-bit addressing
1. MASTER mode > Initialization > Target slave address init = 0x1C
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
