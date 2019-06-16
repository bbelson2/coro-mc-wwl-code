/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : LED1.h
**     Project     : freertos2_xt
**     Processor   : MK22FN512VLH12
**     Component   : LED
**     Version     : Component 01.075, Driver 01.00, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-06-16, 16:38, # CodeGen: 3
**     Abstract    :
**          This component implements a universal driver for a single LED.
**     Settings    :
**          Component name                                 : LED1
**          Turned On with initialization                  : no
**          HW Interface                                   : 
**            On/Off                                       : Enabled
**              Pin                                        : LEDpin
**            PWM                                          : Disabled
**            High Value means ON                          : no
**          Shell                                          : Disabled
**     Contents    :
**         On         - void LED1_On(void);
**         Off        - void LED1_Off(void);
**         Neg        - void LED1_Neg(void);
**         Get        - uint8_t LED1_Get(void);
**         Put        - void LED1_Put(uint8_t val);
**         SetRatio16 - void LED1_SetRatio16(uint16_t ratio);
**         Deinit     - void LED1_Deinit(void);
**         Init       - void LED1_Init(void);
**
**Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**All Rights Reserved.
**
**Redistribution and use in source and binary forms, with or without modification,
**are permitted provided that the following conditions are met:
**
**o Redistributions of source code must retain the above copyright notice, this list
**  of conditions and the following disclaimer.
**
**o Redistributions in binary form must reproduce the above copyright notice, this
**  list of conditions and the following disclaimer in the documentation and/or
**  other materials provided with the distribution.
**
**o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**  contributors may be used to endorse or promote products derived from this
**  software without specific prior written permission.
**
**THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**http: www.freescale.com
**mail: support@freescale.com
** ###################################################################*/
/*!
** @file LED1.h
** @version 01.00
** @brief
**          This component implements a universal driver for a single LED.
*/         
/*!
**  @addtogroup LED1_module LED1 module documentation
**  @{
*/         

#ifndef __LED1_H
#define __LED1_H

/* MODULE LED1. */
#include "MCUC1.h" /* SDK and API used */
#include "LED1config.h" /* configuration */

/* Include inherited components */
#include "MCUC1.h"
#include "LEDpin1.h"

#define LED1_ClrVal()    LEDpin1_ClrVal() /* put the pin on low level */
#define LED1_SetVal()    LEDpin1_SetVal() /* put the pin on high level */
#define LED1_SetInput()  LEDpin1_SetInput() /* use the pin as input pin */
#define LED1_SetOutput() LEDpin1_SetOutput() /* use the pin as output pin */

#define LED1_PARSE_COMMAND_ENABLED  0 /* set to 1 if method ParseCommand() is present, 0 otherwise */


#define LED1_On() LEDpin1_ClrVal()
/*
** ===================================================================
**     Method      :  LED1_On (component LED)
**     Description :
**         This turns the LED on.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#define LED1_Off() LEDpin1_SetVal()
/*
** ===================================================================
**     Method      :  LED1_Off (component LED)
**     Description :
**         This turns the LED off.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#define LED1_Neg() LEDpin1_NegVal()
/*
** ===================================================================
**     Method      :  LED1_Neg (component LED)
**     Description :
**         This negates/toggles the LED
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#define LED1_Get() (!(LEDpin1_GetVal()))
/*
** ===================================================================
**     Method      :  LED1_Get (component LED)
**     Description :
**         This returns logical 1 in case the LED is on, 0 otherwise.
**     Parameters  : None
**     Returns     :
**         ---             - Status of the LED (on or off)
** ===================================================================
*/

void LED1_Init(void);
/*
** ===================================================================
**     Method      :  LED1_Init (component LED)
**     Description :
**         Performs the LED driver initialization.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

#define LED1_Put(val)  ((val) ? LED1_On() : LED1_Off())
/*
** ===================================================================
**     Method      :  LED1_Put (component LED)
**     Description :
**         Turns the LED on or off.
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - value to define if the LED has to be on or
**                           off.
**     Returns     : Nothing
** ===================================================================
*/

void LED1_Deinit(void);
/*
** ===================================================================
**     Method      :  LED1_Deinit (component LED)
**     Description :
**         Deinitializes the driver
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void LED1_SetRatio16(uint16_t ratio);
/*
** ===================================================================
**     Method      :  LED1_SetRatio16 (component LED)
**     Description :
**         Method to specify the duty cycle. If using a PWM pin, this
**         means the duty cycle is set. For On/off pins, values smaller
**         0x7FFF means off, while values greater means on.
**     Parameters  :
**         NAME            - DESCRIPTION
**         ratio           - Ratio value, where 0 means 'off' and
**                           0xffff means 'on'
**     Returns     : Nothing
** ===================================================================
*/

/* END LED1. */

#endif
/* ifndef __LED1_H */
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
