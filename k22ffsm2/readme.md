# k22fawait1

This file is subject to the terms and conditions defined in file 'LICENSE.txt', which is part of this source code package.

# Project setup

## Create project

1. Kinetis Design Studio 3.2.0
1. File > New > Processor Expert Project...
1. Project name = `k22fawait1`
1. Boards > Kinetis > FRDM-K22F
1. Kinetis SDK = None; Processor Expert = True
1. Target compiler = GNU C Compiler

## Tool-chain

Now we alter the tool chain to use LLVM/clang version 5.0.

If necessary, restart Kinetic Studio with clang in the PATH, e.g.:

```
set PATH=E:\llvm-trunk\install\bin;%PATH%
E:\Freescale\KDS_v3\eclipse\kinetis-design-studio.exe -data E:\Source\repos\K64F
```

Create a project configuration which uses clang as a compiler in place of gcc. (We will also set the C++ compiler settings so that they will be copied into any projects cloned from this.)

1. Project > Build Configurations > Manage...
1. New...
1. Name: DebugLLVM; Copy existing settings from Debug. OK.
1. Select DebugLLVM and Set active.
1. Project > Properties > C/C++ Build > Settings
1. Ensure Configuration = DebugLLVM
1. Tool settings > Cross ARM C++ Compiler 
1. Command => clang++
1. Optimization > Language standard => Toolchain default (GNU ISO 1998 C++)
1. Optimization > ABI version => Toolchain default (2)
1. Optimization > Do not use exceptions => checked
1. Optimization > Do not use RTTI => checked.
1. Optimization > Do not use _cxa_atexit() => checked
1. Optimization > Do not use thread-safe statics => checked.
1. Miscellaneous > Other compiler flags => -fshort-enums -target armv7m-none-eabi "-fcoroutines-ts" "-stdlib=libc++" -std=c++14 
1. Includes. Add the following to the list:
    - "C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q1-update\arm-none-eabi\include\c++\6.3.1\arm-none-eabi"
    - "C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q1-update\arm-none-eabi\include\c++\6.3.1"
    - "C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q1-update\arm-none-eabi\include"
1. Tool settings > Cross ARM C Compiler 
1. Command => clang
1. Miscellaneous > Other compiler flags => -fshort-enums -target armv7m-none-eabi
1. Includes. Add the following to the list:
    - "C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q1-update\arm-none-eabi\include"
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
1. Properties > Settings > C/C++ Build > Behavior 
1. Enable Parallel Build => true
1. Project > Properties > C/C++ Build > Settings
1. Toolchains > Print size => `true`


## Components

### Processors

- Cpu:MK22FN512VLH12

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

## Shared Code

1. Right click on project > New > Folder > Advanced
1. Folder name = `Shared` 
1. Link to alternate location = true
1. Add location = `PROJECT_LOC\..\Shared`
1. Right click on project > Properties
1. C/C++ General > Paths and Symbols
1. Tab > Source Location > Add Folder...
1. Select /[projectdir]/Shared/Sources
1. Tab > Includes
1. Languages = GNU C
1. Add... `${ProjDirPath}/../Shared/Include`
1. Languages = GNU C++
1. Add... `${ProjDirPath}/../Shared/Include`
1. Add... `${ProjDirPath}/../Shared/IncludeARM`

## Code

### main.c

```
#include "services.h"
...
  trace("Hello K22F world\r\n");
```

## Debug

### Configuration

1. Run > Debug configurations...
1. Select GDB PEMicro Interface Debugging
1. New
1. Main > C.C++ Application = `DebugLLVM/k22fawait1.elf`
1. Debugger > Interface = OpenSDA Embedded Debug - USB Port
1. Debugger > Port

### Test

1. PuTTY 
1. Serial; COM22; 11520
1. Open
1. Debug the configuration above
1. Observe `Hello K22F world` in terminal

# Project development

Added the following files:

- adc.cpp
- app_ids.h
- main_cpp.cpp
- pe_polyfill.h
- core_crit_sec.h
- scheduling_events.cpp
- core_events.h
- core_future.h
- core_resumable.h
- scheduling_scheduler.cpp
- core_scheduler.h
- scheduling_sim.cpp
- core_simulator.h
- core_split_phase.h
- core_types.h
- services.cpp
- services.h
- experimental/resumable

## main.c

```
extern void main_cpp();
...
  Term1_Cls();
  main_cpp();
```

## Events.c
```
#include "core_types.h"
#include "core_events.h"
#include "app_ids.h"

void AD1_OnEnd(void)
{
	handle_async_event(EVENT_ID_READ_ADCX);
}

void AD1_OnCalibrationEnd(void)
{
	handle_async_event(EVENT_ID_START_ADCX);
}

void AD2_OnEnd(void)
{
	handle_async_event(EVENT_ID_READ_ADCY);
}

void AD2_OnCalibrationEnd(void)
{
	handle_async_event(EVENT_ID_START_ADCY);
}
```

## Build chain experiments
`
E:\Source\repos\k64f_coro>
clang++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q1-update\arm-none-eabi\include\c++\6.3.1\arm-none-eabi" -I"C:\Program Files (x86)\GNU ToolsARM Embedded\6 2017-q1-update\arm-none-eabi\include\c++\6.3.1" -I"C:\Program Files (x86)\GNU Tools ARM Embedded\6 2017-q1-update\arm-none-eabi\include" -I"E:/Source/repos/k64f_coro/k22fawait1/Static_Code/System" -I"E:/Source/repos/k64f_coro/k22fawait1/Static_Code/PDD" -I"E:/Source/repos/k64f_coro/k22fawait1/Static_Code/IO_Map" -I"E:\Freescale\KDS_v3\eclipse\ProcessorExpert/lib/Kinetis/pdd/inc" -I"E:/Source/repos/k64f_coro/k22fawait1/Sources" -I"E:/Source/repos/k64f_coro/k22fawait1/Generated_Code" -I"E:/Source/repos/k64f_coro/k22fawait1/../Shared/Include" -I"E:/Source/repos/k64f_coro/k22fawait1/../Shared/IncludeARM" -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -fshort-enums -target armv7m-none-eabi "-fcoroutines-ts" "-stdlib=libc++" -std=c++14 -MMD -MP -MF"Shared/Sources/task_adc.d" -MT"Shared/Sources/task_adc.o" -S -emit-llvm "E:/Source/repos/k64f_coro/Shared/Sources/task_adc.cpp"
`

The important part here is:

`-S -emit-llvm`

See https://stackoverflow.com/questions/9148890/how-to-make-clang-compile-to-llvm-ir

# Size

-Os

```
Invoking: Cross ARM GNU Print Size
arm-none-eabi-size --format=berkeley "k22fawait1.elf"
   text	   data	    bss	    dec	    hex	filename
  36560	    244	   1784	  38588	   96bc	k22fawait1.elf
Finished building: k22fawait1.siz
 ```

-O3

```
Invoking: Cross ARM GNU Print Size
arm-none-eabi-size --format=berkeley "k22fawait1.elf"
   text	   data	    bss	    dec	    hex	filename
  39576	    244	   1784	  41604	   a284	k22fawait1.elf
Finished building: k22fawait1.siz
 

19:11:43 Build Finished (took 5m:14s.592ms)
```


