/**HEADER*********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2008 Embedded Access Inc.;
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
* $FileName: asm_mac.h$
* $Version : 3.8.1.0$
* $Date    : Aug-30-2011$
*
* Comments:
*   This file contains macros used by the CodeWarrior assembler
*
*END*************************************************************************/

#ifndef __asm_mac_h__
#define __asm_mac_h__   1

#define ASM_PREFIX(x)   x

#define ASM_EXTERN(label)       .extern ASM_PREFIX(label)
#define ASM_PUBLIC(label)       .global ASM_PREFIX(label)

#define ASM_LABEL(label)        label:
#define ASM_EQUATE(label,value) .equ label, value

#define ASM_ALIGN(value)        .balign value

#define ASM_CONST16(value)      .word value
#define ASM_CONST32(value)      .long value

#define ASM_LABEL_CONST32(label,value) label .long value

#define ASM_CODE_SECTION(name)  .section name, "ax"
#define ASM_DATA_SECTION(name)  TBD /* .section name,16,r */

#define ASM_END                 .end

#define ASM_COMP_SPECIFIC_DIRECTIVES .syntax unified

#define ASM_PUBLIC_BEGIN(name) .thumb_func
#define ASM_PUBLIC_FUNC(name)
#define ASM_PUBLIC_END(name)

#endif /* __asm_mac_h__ */
