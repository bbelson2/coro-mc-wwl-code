# k22ffsm2

This file is subject to the terms and conditions defined in file 'LICENSE.txt', which is part of this source code package.

# Project setup

## Create project

1. Kinetis Design Studio 3.2.0
1. File > New > Processor Expert Project...
1. Project name = `k22ffsm2`
1. Boards > Kinetis > FRDM-K22F
1. Kinetis SDK = None; Processor Expert = True
1. Target compiler = GNU C Compiler

## Tool-chain

Now alter the tool chain to use LLVM/clang version 5.0, following the instructions under **Common settings** and **Tool-chain** in [ProjectConfig.md](../ProjectConfig.md).

## Components

Add or modify the following components as described under **Common components** in [ProjectConfig.md](../ProjectConfig.md).

1. Cpu:MK22FN512VLH12 - K22F CPU clock speed
1. Term1
1. ADC1
1. ADC2
1. CS1
1. TU1
1. FC1
1. I2C
1. BitIO

## Shared Code

1. Right click on project > New > Folder > Advanced
1. Link to alternate location = true
1. Add location = `PROJECT_LOC/../Shared/Include`
1. Repeat for location = `PROJECT_LOC/../Shared/IncludeARM`
1. Repeat for location = `PROJECT_LOC/../fsmShared`
1. Right click on project > Properties
1. C/C++ General > Paths and Symbols
1. Tab > Includes
1. Languages = GNU C
1. Add... `${ProjDirPath}/../Shared/Include`
1. Add... `${ProjDirPath}/../fsmShared/fsmInclude`
1. Languages = GNU C++
1. Add... `${ProjDirPath}/../Shared/Include`
1. Add... `${ProjDirPath}/../Shared/IncludeARM`
1. Add... `${ProjDirPath}/../fsmShared/fsmInclude`

## Code

### main.c

```
extern void main_cpp();
...
  main_cpp();
```

## Debug

### Configuration

1. Run > Debug configurations...
1. Select GDB PEMicro Interface Debugging
1. New
1. Main > C.C++ Application = `DebugLLVM/k22ffsm2.elf`
1. Debugger > Interface = OpenSDA Embedded Debug - USB Port
1. Debugger > Port

### Test

1. PuTTY 
1. Serial; COM22; 11520
1. Open
1. Debug the configuration above
1. Observe `Hello K22F world` in terminal - FIX

# Project development

# TODO

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


