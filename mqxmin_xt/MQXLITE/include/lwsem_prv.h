/*HEADER**********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: lwsem_prv.h$
* $Version : 3.8.4.1$
* $Date    : Feb-16-2012$
*
* Comments:
*
*   This include file is used to define constants and data types private
*   to the lightweight semaphore component.
*
*END*************************************************************************/

#ifndef __lwsem_prv_h__
#define __lwsem_prv_h__ 1

/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */

/* The correct value for the lwsem VALID field */
#define LWSEM_VALID                (_mqx_uint)(0x6C77736D)    /* "lwsm" */

/*--------------------------------------------------------------------------*/
/*                         ANSI C PROTOTYPES                                */
#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_uint _lwsem_create_internal(LWSEM_STRUCT_PTR, _mqx_int, boolean, boolean);
extern _mqx_uint _lwsem_destroy_internal(LWSEM_STRUCT_PTR sem_ptr, boolean user);
extern _mqx_uint _lwsem_wait_timed_internal(LWSEM_STRUCT_PTR, TD_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
