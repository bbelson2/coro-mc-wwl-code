/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: psp_tipr.c$
* $Version : 3.0.2.0$
* $Date    : Aug-19-2008$
*
* Comments:
*
*   This file contains the function for printing the ticks portion
*   of the PSP_TICK_STRUCT
*
*END************************************************************************/

#include "mqx_inc.h"
#include <stdio.h>

typedef union psp_64_bit_union
{
   uint_64  LLW;
   uint_32  LW[2];
   uint_16  W[4];
   uint_8   B[8];
} PSP_64_BIT_UNION, _PTR_ PSP_64_BIT_UNION_PTR;


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name    : _psp_print_ticks
* Returned Value   : none
* Comments         :
*    Prints ticks in hex notation
*
*END*----------------------------------------------------------------------*/

void _psp_print_ticks
   (
      PSP_TICK_STRUCT_PTR tick_ptr
   )
{ /* Body */
   PSP_64_BIT_UNION  tmp;
   int_32            i;

   printf("0x");
   tmp.LLW = tick_ptr->TICKS[0];
#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
   for (i = 7; i >= 0; i--) {
      printf("%X", tmp.B[i]);
   } /* Endfor */
#else
   for (i = 0; i <= 7; i++) {
      printf("%X", tmp.B[i]);
   } /* Endfor */
#endif
   printf(":%04lX", tick_ptr->HW_TICKS[0]);

} /* Endbody */

/* EOF */
