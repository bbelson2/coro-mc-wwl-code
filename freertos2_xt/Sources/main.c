/* ###################################################################
**     Filename    : main.c
**     Project     : freertos2
**     Processor   : MK22FN512VLH12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-04-16, 20:21, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "pin_mux.h"
#include "FRTOS1.h"
#include "MCUC1.h"
#include "UTIL1.h"
#include "LED1.h"
#include "LEDpin1.h"
#include "BitIoLdd1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

#ifndef FREERTOS_BUILD_EXTERNAL_TIMER
volatile unsigned long int __ft_g_cycles = 0;
#endif

static void blinky_task(void *param) {
  int state = *(int*)param ? 1 : 0;
  for(;;) {
  	LED1_Put(state);
#ifndef FREERTOS_BUILD_EXTERNAL_TIMER
  	__ft_g_cycles++;
#endif
  	taskYIELD();
  	//LED1_Neg(); /* toggle red LED */
    //vTaskDelay(500/portTICK_RATE_MS); /* wait for 500 ms */
  } /* for */
}

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */
  /*
  for(;;) {
  	for (int i = 0; i < 1000000; i++) {
  	}
  	LED1_Neg();
  }
  */

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

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
