# freertos2_xt

This file is subject to the terms and conditions defined in file 'LICENSE.txt', which is part of this source code package.

The project contains the FreeRTOS version of a minimal K22F application. 
It is to be compared to the coroutine version to measure the overhead of task switching.

An oscilloscope is used to measure the time taken for a complete sequence.

# Build & run

## Build

To build the project, ensure that the active build configuration is `DebugLLVM`, then build the project.

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
1. Set C/C++ Application to `DebugLLVM/freertos2_xt.elf`
1. Select the `Debugger` tab.
1. Set Interface to `OpenSDA Embedded Debug - USB Port`
1. If no device is selected, press `Select Device`.
1. `NXP` > `K2x` > `K22FN512M12`
1. Press `Apply` then `Close`. 

## Debug

1. Press the Debug tool, or `F11`.
1. If prompted, select the debug configuration `/freertos2_xt/DebugLLVM/freertos2_xt.elf`.
1. If prompted for a configuration, select `gdbserver`.

# Project setup

The steps below were used to create the project.

## Create project

1. Kinetis Design Studio 3.2.0
1. File > New > Processor Expert Project...
1. Project name = `freertos2_xt`
1. Boards > Kinetis > FRDM-K22F
1. Kinetis SDK = None; Processor Expert = True
1. Target compiler = GNU C Compiler

## Tool-chain

Follow the instructions in the main [ProjectConfig.md](../ProjectConfig.md) to alter the tool chain to use LLVM/clang.

## Components

1. Follow the instructions in the main [ProjectConfig.md](../ProjectConfig.md) to change the CPU component clock speed and to add the Bit1 component.
1.  Open the Processor Expert view and select the Components Library.
1. Find and add component `FreeRTOS`.
1. Open `FRTOS1` in Component Inspector.
1. `Memory` > `Static Allocation` > `Static Allocation`> true
1. Generate Processor Expert Code.

# Project development

## main.c
Include the LED:

```
#include "LED1.h"
```

Add a task:
```
static void blinky_task(void *param) {
  int state = *(int*)param ? 1 : 0;
  for(;;) {
  	LED1_Put(state);
  	taskYIELD();
  } /* for */
}

```

Add the following to `main()`:

```
  static int state0 = 0, state1 = 1;
  if ((xTaskCreate(
  		blinky_task,  /* task function */
			"Blinky0", /* task name for kernel awareness */
			configMINIMAL_STACK_SIZE, /* task stack size */
			(void*)&state0, /* optional task startup argument */
			tskIDLE_PRIORITY,  /* initial priority */
			NULL /* task handle */
  ) != pdPASS)
  || (xTaskCreate(
  		blinky_task,  /* task function */
			"Blinky1", /* task name for kernel awareness */
			configMINIMAL_STACK_SIZE, /* task stack size */
			(void*)&state1, /* optional task startup argument */
			tskIDLE_PRIORITY,  /* initial priority */
			NULL /* task handle */
  ) != pdPASS))
  {
  	for(;;){} /* error! probably out of memory */
  }
```


## Size

```
Invoking: Cross ARM GNU Print Size
arm-none-eabi-size --format=berkeley "freertos2_xt.elf"
   text	   data	    bss	    dec	    hex	filename
   6656	    148	   4284	  11088	   2b50	freertos2_xt.elf
Finished building: freertos2_xt.siz
```

