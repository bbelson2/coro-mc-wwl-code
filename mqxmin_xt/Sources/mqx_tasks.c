/* ###################################################################
**     Filename    : mqx_tasks.c
**     Project     : mqxmin_xt
**     Processor   : MK22FN512VLH12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-04-22, 04:52, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Task1_task - void Task1_task(uint32_t task_init_data);
**         Task2_task - void Task2_task(uint32_t task_init_data);
**
** ###################################################################*/
/*!
** @file mqx_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup mqx_tasks_module mqx_tasks module documentation
**  @{
*/         
/* MODULE mqx_tasks */

#include "Cpu.h"
#include "Events.h"
#include "mqx_tasks.h"
#include "Init_Config.h"
#include "PDD_Includes.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Bit1.h"

#ifndef MQXBUILD_EXTERNAL_TIMER
extern unsigned long __mqx_g_cycles;
#endif

/*
** ===================================================================
**     Event       :  Task1_task (module mqx_tasks)
**
**     Component   :  Task1 [MQXLite_task]
**     Description :
**         MQX task routine. The routine is generated into mqx_tasks.c
**         file.
**     Parameters  :
**         NAME            - DESCRIPTION
**         task_init_data  - 
**     Returns     : Nothing
** ===================================================================
*/
void Task1_task(uint32_t task_init_data)
{
  int counter = 0;

  while(1) {
    counter++;

    /* Write your code here ... */
#ifndef MQXBUILD_EXTERNAL_TIMER
    __mqx_g_cycles++;
#endif
    Bit1_PutVal(0);
    _sched_yield();
  }
}

/*
** ===================================================================
**     Event       :  Task2_task (module mqx_tasks)
**
**     Component   :  Task2 [MQXLite_task]
**     Description :
**         MQX task routine. The routine is generated into mqx_tasks.c
**         file.
**     Parameters  :
**         NAME            - DESCRIPTION
**         task_init_data  - 
**     Returns     : Nothing
** ===================================================================
*/
void Task2_task(uint32_t task_init_data)
{
  int counter = 0;

  while(1) {
    counter++;

    /* Write your code here ... */
#ifndef MQXBUILD_EXTERNAL_TIMER
    __mqx_g_cycles++;
#endif
    Bit1_PutVal(1);
    _sched_yield();
  }
}

/* END mqx_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
