/**HEADER*********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*
* $FileName: comp.h$
* $Version : 3.8.5.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file defines the GCC ARM build tools specific macros for MQX
*
*END*************************************************************************/
#ifndef __comp_h__
#define __comp_h__   1


#ifdef __cplusplus
extern "C" {
#endif


#define _ASM_NOP()      __asm volatile ("nop")
#define _ASM_STOP(x)
#define _ASM_WFI()      __asm volatile ("wfi")
#define _ASM_SLEEP()    __asm volatile ("wfi")

#if PSP_MQX_CPU_IS_ARM_CORTEX_M4
#define __set_BASEPRI(basePri) __asm volatile ("msr BASEPRI, %0" : : "r" (basePri))
#define _PSP_SET_ENABLE_SR(x)   __set_BASEPRI(x)
#define _PSP_SET_DISABLE_SR(x)  _PSP_SET_ENABLE_SR(x)
#elif PSP_MQX_CPU_IS_ARM_CORTEX_M0P
#define _PSP_SET_ENABLE_SR(x)   __asm volatile ("cpsie i")
#define _PSP_SET_DISABLE_SR(x)  __asm volatile ("cpsid i")
#else
#error Invalid platform selected
#endif

#define _PSP_SYNC()


/* Kinetis User mode definitions */
#if MQX_ENABLE_USER_MODE
#error "Usermode is not supported"

#else /* MQX_ENABLE_USER_MODE */

#define KERNEL_ACCESS
#define USER_RW_ACCESS
#define USER_RO_ACCESS
#define USER_NO_ACCESS

#endif /* MQX_ENABLE_USER_MODE */

/*
 *      DATATYPE MODIFIERS
 */

#define _WEAK_SYMBOL(x)     x __attribute__((weak))
#define _WEAK_FUNCTION(x)   x __attribute__((weak))

/* compiler dependent structure packing option */
#define PACKED_STRUCT_BEGIN
#define PACKED_STRUCT_END   __attribute__((__packed__))

/* compiler dependent union packing option */
#define PACKED_UNION_BEGIN
#define PACKED_UNION_END    __attribute__((__packed__))

#ifdef __cplusplus
}
#endif

#endif   /* __comp_h__ */
