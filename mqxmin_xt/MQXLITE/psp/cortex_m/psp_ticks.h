/**HEADER********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor
* All Rights Reserved
*
***************************************************************************
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
**************************************************************************
*
* $FileName: psp_time.h$
* $Version : 3.8.4.0$
* $Date    : Dec-7-2011$
*
* Comments:
*   This file contains the definitions for time as used by the PSP.
*
*END************************************************************************/

#ifndef __psp_ticks_h__
#define __psp_ticks_h__

/*
** Macros and defines to manipulate the tick struct
*/
#define PSP_NUMBER_OF_TICK_FIELDS   (1)
#define PSP_NUMBER_OF_HWTICK_FIELDS (1)

/* Macro to retrieve the least significant _mqx_uint from a tick struct */
#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
   #define PSP_GET_LS_TICK_FROM_TICK_STRUCT(t_ptr) \
      (((MQX_TICK_STRUCT_PTR)(t_ptr))->TICKS[0])
#else
   #define PSP_GET_LS_TICK_FROM_TICK_STRUCT(t_ptr) \
      (((MQX_TICK_STRUCT_PTR)(t_ptr))->TICKS[MQX_NUM_TICK_FIELDS-1])
#endif

/* Macro to retrieve a _mqx_uint from a tick struct */
#define PSP_GET_ELEMENT_FROM_TICK_STRUCT(t_ptr, idx) \
   (((PSP_TICK_STRUCT_PTR)(t_ptr))->TICKS[0])

/* Macro increments to incrment tick structure */
#define PSP_INC_TICKS(t_ptr)  (++(((PSP_TICK_STRUCT_PTR)(t_ptr))->TICKS[0]))

#define PSP_NORMALIZE_TICKS(tick_ptr)	_psp_normalize_ticks((PSP_TICK_STRUCT_PTR)(tick_ptr))

/* Macro to add two tick structures including hw ticks */
#define PSP_ADD_TICKS(a_ptr, b_ptr, r_ptr)                                    \
   _psp_add_ticks((PSP_TICK_STRUCT_PTR)(a_ptr), (PSP_TICK_STRUCT_PTR)(b_ptr), \
      (PSP_TICK_STRUCT_PTR)(r_ptr))

/* Macro to subtract two tick structures NOT including hw ticks */
#define PSP_SUB_TICKS(a_ptr, b_ptr, r_ptr)    \
   _psp_subtract_ticks((PSP_TICK_STRUCT_PTR)(a_ptr), (PSP_TICK_STRUCT_PTR)(b_ptr), \
      (PSP_TICK_STRUCT_PTR)(r_ptr))

/* Macro to subtract two tick structures NOT including hw ticks and clamp into int32 interval */
#define PSP_SUB_TICKS_INT32(a_ptr, b_ptr, o_ptr)    \
   _psp_subtract_ticks_int32((PSP_TICK_STRUCT_PTR)(a_ptr), (PSP_TICK_STRUCT_PTR)(b_ptr), \
      (boolean _PTR_)(o_ptr))

/* Macro to add a single quantity of ticks to a tick structure */
#define PSP_ADD_TICKS_TO_TICK_STRUCT(a_ptr, v, r_ptr) \
   ((PSP_TICK_STRUCT_PTR)(r_ptr))->TICKS[0] = \
   ((PSP_TICK_STRUCT_PTR)(a_ptr))->TICKS[0] + (uint_64)(v)

/* Macro to subtract a single quantity of ticks from a tick structure */
#define PSP_SUB_TICKS_FROM_TICK_STRUCT(a_ptr, v, r_ptr) \
   ((PSP_TICK_STRUCT_PTR)(r_ptr))->TICKS[0] = \
   ((PSP_TICK_STRUCT_PTR)(a_ptr))->TICKS[0] - (uint_64)(v)

/* Macro to multipy the ticks by a 32 bit quantity */
#define PSP_MULTIPLY_TICKS_BY_UINT_32(m_ptr, m, r_ptr) \
   ((PSP_TICK_STRUCT_PTR)(r_ptr))->TICKS[0] = ((PSP_TICK_STRUCT_PTR)(m_ptr))->TICKS[0] * (uint_32)(m)

/*
** Returns  1 if a >  b
** Returns -1 if a <  b
** Returns  0 if a == b
*/
#define PSP_CMP_TICKS(a_ptr, b_ptr) \
     ((((PSP_TICK_STRUCT_PTR)(a_ptr))->TICKS[0] > ((PSP_TICK_STRUCT_PTR)(b_ptr))->TICKS[0]) ?  1 : \
     (((PSP_TICK_STRUCT_PTR)(a_ptr))->TICKS[0] < ((PSP_TICK_STRUCT_PTR)(b_ptr))->TICKS[0]) ? -1 : \
     0 )

#define PSP_PRINT_TICKS(tick_ptr) \
   _psp_print_ticks((PSP_TICK_STRUCT_PTR)(tick_ptr))


#ifndef __ASM__

/*-----------------------------------------------------------------------*/
/*
** PSP TICK STRUCT
**
** The representation of the MQX tick struct from the PSP's
** view
**
*/
/*
** The _Packed keyword is specific to the MetaWare compiler. It will force the
** the PSP_TICK_STRUCT to be aligned on 32 bit boundaries instead of 64. This
** will allow casting between the PSP_TICK_STRUCT and the MQX_TICK_STRUCT
*/
typedef struct psp_tick_struct
{
    /* We need 64 bits for the ticks */
    uint_64    TICKS[PSP_NUMBER_OF_TICK_FIELDS];

    /* also need 32 bits for the hw ticks */
    uint_32    HW_TICKS[PSP_NUMBER_OF_HWTICK_FIELDS];

} PSP_TICK_STRUCT, _PTR_ PSP_TICK_STRUCT_PTR;

/*--------------------------------------------------------------------------*/
/*
**                  PROTOTYPES OF PSP FUNCTIONS
*/

#ifdef __cplusplus
extern "C" {
#endif

void      _psp_add_ticks(PSP_TICK_STRUCT_PTR, PSP_TICK_STRUCT_PTR,
   PSP_TICK_STRUCT_PTR);
void      _psp_subtract_ticks(PSP_TICK_STRUCT_PTR, PSP_TICK_STRUCT_PTR,
   PSP_TICK_STRUCT_PTR);
int_32    _psp_subtract_ticks_int32(PSP_TICK_STRUCT_PTR, PSP_TICK_STRUCT_PTR,
   boolean _PTR_);
void      _psp_mul_ticks_by_32(PSP_TICK_STRUCT_PTR, uint_32,
   PSP_TICK_STRUCT_PTR);

void      _psp_print_ticks(PSP_TICK_STRUCT_PTR);
void      _psp_normalize_ticks(PSP_TICK_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif /* __ASM__ */

#endif /*__psp_ticks_h__ */
/* EOF */
