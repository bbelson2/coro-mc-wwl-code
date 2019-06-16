/**HEADER***********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
********************************************************************************
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
********************************************************************************
*
* $FileName: kinetis.h$
* $Version : 3.8.18.0$
* $Date    : Dec-7-2011$
*
* Comments:
*
*   This file contains the type definitions for the Kinetis microcontrollers.
*
*END***************************************************************************/

#ifndef __kinetis_h__
#define __kinetis_h__

#ifndef __ASM__

/* Include header file for specific Kinetis platform */
#include "IO_Map.h"

#endif /* __ASM__ */

#ifdef __cplusplus
extern "C" {
#endif


/*
*******************************************************************************
**
**                  CONSTANT DEFINITIONS
**
*******************************************************************************
*/

/* Cache and MMU definition values */
#ifndef PSP_HAS_MMU
#define PSP_HAS_MMU                             0
#endif

#ifndef PSP_HAS_CODE_CACHE
#define PSP_HAS_CODE_CACHE                      0
#endif

#ifndef PSP_HAS_DATA_CACHE
#define PSP_HAS_DATA_CACHE                      0
#endif

#ifndef PSP_HAS_FPU
#define PSP_HAS_FPU                             0
#endif

#define PSP_CACHE_LINE_SIZE                     (0x10)

#ifndef __ASM__

/* Standard cache macros */
#define _DCACHE_FLUSH()
#define _DCACHE_FLUSH_LINE(p)
#define _DCACHE_FLUSH_MBYTES(p, m)
#define _DCACHE_INVALIDATE()
#define _DCACHE_INVALIDATE_LINE(p)
#define _DCACHE_INVALIDATE_MBYTES(p, m)

#define _CACHE_ENABLE(n)
#define _CACHE_DISABLE()

#define _ICACHE_INVALIDATE()
#define _ICACHE_INVALIDATE_LINE(p)
#define _ICACHE_INVALIDATE_MBYTES(p, m)

#define PSP_INTERRUPT_TABLE_INDEX               IRQInterruptIndex


/*
*******************************************************************************
**
**              FUNCTION PROTOTYPES AND GLOBAL EXTERNS
**
*******************************************************************************
*/

#define _psp_mem_check_access(addr, size, flags)    \
                    _kinetis_mpu_sw_check(addr, size, flags)

#define _psp_mem_check_access_mask(addr, size, flags, mask) \
                    _kinetis_mpu_sw_check_mask(addr, size, flags, mask)

#define _psp_mpu_add_region(start, end, flags)  \
                    _kinetis_mpu_add_region(start, end, flags)


#endif /* __ASM__ */

#ifdef __cplusplus
}
#endif

#endif /* __kinetis_h__ */
