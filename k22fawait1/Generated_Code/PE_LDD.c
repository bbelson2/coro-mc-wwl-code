/** ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : PE_LDD.c
**     Project     : k22fawait1
**     Processor   : MK22FN512VLH12
**     Version     : Component 01.048, Driver 01.00, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-06-24, 14:20, # CodeGen: 42
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file PE_LDD.c                                                  
** @version 01.00
*/         
/*!
**  @addtogroup PE_LDD_module PE_LDD module documentation
**  @{
*/         

/* MODULE PE_LDD. */

/* {Default RTOS Adapter} No RTOS includes */
/* {Default RTOS Adapter} No RTOS driver includes */

#include "PE_LDD.h"
#include "Cpu.h"

/*lint -esym(765,PE_PeripheralUsed,LDD_SetClockConfiguration,PE_CpuClockConfigurations,PE_FillMemory) Disable MISRA rule (8.10) checking for symbols (PE_PeripheralUsed,LDD_SetClockConfiguration,PE_CpuClockConfigurations,PE_FillMemory). */

/*
** ===========================================================================
** Array of initialized device structures of LDD components.
** ===========================================================================
*/
LDD_TDeviceData *PE_LDD_DeviceDataList[8] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  };

/*
** ===========================================================================
** The array of clock frequencies in configured clock configurations.
** ===========================================================================
*/
/*! The array of clock configurations (frequencies) configured in configured clock configurations of the CPU component. */
const TCpuClockConfiguration PE_CpuClockConfigurations[CPU_CLOCK_CONFIG_NUMBER] = {
  /* Clock configuration 0 */
  {
    CPU_CORE_CLK_HZ_CONFIG_0,          /*!< Core clock frequency in clock configuration 0 */
    CPU_BUS_CLK_HZ_CONFIG_0,           /*!< Bus clock frequency in clock configuration 0 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_0,       /*!< Flexbus clock frequency in clock configuration 0 */
    CPU_FLASH_CLK_HZ_CONFIG_0,         /*!< FLASH clock frequency in clock configuration 0 */
    CPU_USB_CLK_HZ_CONFIG_0,           /*!< USB clock frequency in clock configuration 0 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_0,       /*!< PLL/FLL clock frequency in clock configuration 0 */
    CPU_MCGIR_CLK_HZ_CONFIG_0,         /*!< MCG internal reference clock frequency in clock configuration 0 */
    CPU_OSCER_CLK_HZ_CONFIG_0,         /*!< System OSC external reference clock frequency in clock configuration 0 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_0,      /*!< External reference clock 32k frequency in clock configuration 0 */
    CPU_MCGFF_CLK_HZ_CONFIG_0          /*!< MCG fixed frequency clock */
  },
  /* Clock configuration 1 */
  {
    CPU_CORE_CLK_HZ_CONFIG_1,          /*!< Core clock frequency in clock configuration 1 */
    CPU_BUS_CLK_HZ_CONFIG_1,           /*!< Bus clock frequency in clock configuration 1 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_1,       /*!< Flexbus clock frequency in clock configuration 1 */
    CPU_FLASH_CLK_HZ_CONFIG_1,         /*!< FLASH clock frequency in clock configuration 1 */
    CPU_USB_CLK_HZ_CONFIG_1,           /*!< USB clock frequency in clock configuration 1 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_1,       /*!< PLL/FLL clock frequency in clock configuration 1 */
    CPU_MCGIR_CLK_HZ_CONFIG_1,         /*!< MCG internal reference clock frequency in clock configuration 1 */
    CPU_OSCER_CLK_HZ_CONFIG_1,         /*!< System OSC external reference clock frequency in clock configuration 1 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_1,      /*!< External reference clock 32k frequency in clock configuration 1 */
    CPU_MCGFF_CLK_HZ_CONFIG_1          /*!< MCG fixed frequency clock */
  },
  /* Clock configuration 2 */
  {
    CPU_CORE_CLK_HZ_CONFIG_2,          /*!< Core clock frequency in clock configuration 2 */
    CPU_BUS_CLK_HZ_CONFIG_2,           /*!< Bus clock frequency in clock configuration 2 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_2,       /*!< Flexbus clock frequency in clock configuration 2 */
    CPU_FLASH_CLK_HZ_CONFIG_2,         /*!< FLASH clock frequency in clock configuration 2 */
    CPU_USB_CLK_HZ_CONFIG_2,           /*!< USB clock frequency in clock configuration 2 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_2,       /*!< PLL/FLL clock frequency in clock configuration 2 */
    CPU_MCGIR_CLK_HZ_CONFIG_2,         /*!< MCG internal reference clock frequency in clock configuration 2 */
    CPU_OSCER_CLK_HZ_CONFIG_2,         /*!< System OSC external reference clock frequency in clock configuration 2 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_2,      /*!< External reference clock 32k frequency in clock configuration 2 */
    CPU_MCGFF_CLK_HZ_CONFIG_2          /*!< MCG fixed frequency clock */
  },
  /* Clock configuration 3 */
  {
    CPU_CORE_CLK_HZ_CONFIG_3,          /*!< Core clock frequency in clock configuration 3 */
    CPU_BUS_CLK_HZ_CONFIG_3,           /*!< Bus clock frequency in clock configuration 3 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_3,       /*!< Flexbus clock frequency in clock configuration 3 */
    CPU_FLASH_CLK_HZ_CONFIG_3,         /*!< FLASH clock frequency in clock configuration 3 */
    CPU_USB_CLK_HZ_CONFIG_3,           /*!< USB clock frequency in clock configuration 3 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_3,       /*!< PLL/FLL clock frequency in clock configuration 3 */
    CPU_MCGIR_CLK_HZ_CONFIG_3,         /*!< MCG internal reference clock frequency in clock configuration 3 */
    CPU_OSCER_CLK_HZ_CONFIG_3,         /*!< System OSC external reference clock frequency in clock configuration 3 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_3,      /*!< External reference clock 32k frequency in clock configuration 3 */
    CPU_MCGFF_CLK_HZ_CONFIG_3          /*!< MCG fixed frequency clock */
  },
  /* Clock configuration 4 */
  {
    CPU_CORE_CLK_HZ_CONFIG_4,          /*!< Core clock frequency in clock configuration 4 */
    CPU_BUS_CLK_HZ_CONFIG_4,           /*!< Bus clock frequency in clock configuration 4 */
    CPU_FLEXBUS_CLK_HZ_CONFIG_4,       /*!< Flexbus clock frequency in clock configuration 4 */
    CPU_FLASH_CLK_HZ_CONFIG_4,         /*!< FLASH clock frequency in clock configuration 4 */
    CPU_USB_CLK_HZ_CONFIG_4,           /*!< USB clock frequency in clock configuration 4 */
    CPU_PLL_FLL_CLK_HZ_CONFIG_4,       /*!< PLL/FLL clock frequency in clock configuration 4 */
    CPU_MCGIR_CLK_HZ_CONFIG_4,         /*!< MCG internal reference clock frequency in clock configuration 4 */
    CPU_OSCER_CLK_HZ_CONFIG_4,         /*!< System OSC external reference clock frequency in clock configuration 4 */
    CPU_ERCLK32K_CLK_HZ_CONFIG_4,      /*!< External reference clock 32k frequency in clock configuration 4 */
    CPU_MCGFF_CLK_HZ_CONFIG_4          /*!< MCG fixed frequency clock */
  }
};

/*
** ===================================================================
**     Method      :  Cpu_PE_FillMemory (component MK22FN512LH12)
*/
/*!
**     @brief
**         Fills a memory area block by a specified value.
**     @param
**       SourceAddressPtr - Source address pointer.
**     @param
**       c - A value used to fill a memory block.
**     @param
**       len - Length of a memory block to fill.
*/
/* ===================================================================*/
void PE_FillMemory(register void* SourceAddressPtr, register uint8_t c, register uint32_t len)
{
  register uint8_t *ptr = (uint8_t*)SourceAddressPtr;

  if (len > 0U) {
    while (len--) {
      *ptr++ = c;
    }
  }
}

/*
** ===================================================================
**     Method      :  Cpu_PE_PeripheralUsed (component MK22FN512LH12)
*/
/*!
**     @brief
**         Returns information whether a peripheral is allocated by PEx 
**         or not.
**     @param
**       PrphBaseAddress - Base address of a peripheral.
**     @return
**       TRUE if a peripheral is used by PEx or FALSE if it isn't used.
*/
/* ===================================================================*/
bool PE_PeripheralUsed(uint32_t PrphBaseAddress)
{
  bool result = FALSE;

  switch (PrphBaseAddress) {
    /* Base address allocated by peripheral(s) UART1 */
    case 0x4006B000UL:
    /* Base address allocated by peripheral(s) ADC0 */
    case 0x4003B000UL:
    /* Base address allocated by peripheral(s) ADC1 */
    case 0x40027000UL:
    /* Base address allocated by peripheral(s) FTM0 */
    case 0x40038000UL:
    /* Base address allocated by peripheral(s) FTM1 */
    case 0x40039000UL:
    /* Base address allocated by peripheral(s) I2C0 */
    case 0x40066000UL:
    /* Base address allocated by peripheral(s) PTB */
    case 0x400FF040UL:
      result = TRUE;
      break;
    default:
      break;
  }
  return result;
}

/*
** ===================================================================
**     Method      :  Cpu_LDD_SetClockConfiguration (component MK22FN512LH12)
*/
/*!
**     @brief
**         Changes the clock configuration of all LDD components in a 
**         project.
**     @param
**       ClockConfiguration - New CPU clock configuration changed by CPU SetClockConfiguration method.
*/
/* ===================================================================*/
void LDD_SetClockConfiguration(LDD_TClockConfiguration ClockConfiguration)
{
  /* Component ASerialLdd1 (Serial_LDD). */
  if (PE_LDD_DeviceDataList[PE_LDD_COMPONENT_ASerialLdd1_ID] != NULL) {
    ASerialLdd1_SetClockConfiguration(PE_LDD_DeviceDataList[PE_LDD_COMPONENT_ASerialLdd1_ID], ClockConfiguration);
  }
  /* Component AdcLdd1 (ADC_LDD). */
  if (PE_LDD_DeviceDataList[PE_LDD_COMPONENT_AdcLdd1_ID] != NULL) {
    AdcLdd1_SetClockConfiguration(PE_LDD_DeviceDataList[PE_LDD_COMPONENT_AdcLdd1_ID], ClockConfiguration);
  }
  /* Component AdcLdd2 (ADC_LDD). */
  if (PE_LDD_DeviceDataList[PE_LDD_COMPONENT_AdcLdd2_ID] != NULL) {
    AdcLdd2_SetClockConfiguration(PE_LDD_DeviceDataList[PE_LDD_COMPONENT_AdcLdd2_ID], ClockConfiguration);
  }
  /* Component TU1 (TimerUnit_LDD). */
  if (PE_LDD_DeviceDataList[PE_LDD_COMPONENT_TU1_ID] != NULL) {
    TU1_SetClockConfiguration(PE_LDD_DeviceDataList[PE_LDD_COMPONENT_TU1_ID], ClockConfiguration);
  }
  /* Component TU2 (TimerUnit_LDD). */
  if (PE_LDD_DeviceDataList[PE_LDD_COMPONENT_TU2_ID] != NULL) {
    TU2_SetClockConfiguration(PE_LDD_DeviceDataList[PE_LDD_COMPONENT_TU2_ID], ClockConfiguration);
  }
  /* Component FreeCntrLdd1 (FreeCntr_LDD). */
  if (PE_LDD_DeviceDataList[PE_LDD_COMPONENT_FreeCntrLdd1_ID] != NULL) {
    FreeCntrLdd1_SetClockConfiguration(PE_LDD_DeviceDataList[PE_LDD_COMPONENT_FreeCntrLdd1_ID], ClockConfiguration);
  }
  /* Component IntI2cLdd1 (I2C_LDD). */
  if (PE_LDD_DeviceDataList[PE_LDD_COMPONENT_IntI2cLdd1_ID] != NULL) {
    IntI2cLdd1_SetClockConfiguration(PE_LDD_DeviceDataList[PE_LDD_COMPONENT_IntI2cLdd1_ID], ClockConfiguration);
  }
  Inhr1_SetClockConfiguration(ClockConfiguration);
  AD1_SetClockConfiguration(ClockConfiguration);
  AD2_SetClockConfiguration(ClockConfiguration);
  I2C_SetClockConfiguration(ClockConfiguration);
}

/* END PE_LDD. */

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
