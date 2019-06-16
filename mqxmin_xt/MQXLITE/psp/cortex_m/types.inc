/*HEADER**********************************************************************
*
* Copyright (c) 2010-2011 Freescale Semiconductor
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
* $FileName: types.inc$
*
* Comments:
*   This file contains the assembler offsets calculated by the
*   program KRNL_OFF.C.  These offsets are to be included in
*   any assembler program that wishes to access kernel data
*   structures.
*
*END*************************************************************************/

#include <asm_mac.h>


/**
 * 'TD_STRUCT'
 */
ASM_EQUATE(TD_TD_NEXT,                              0)
ASM_EQUATE(TD_TD_PREV,                              4)
ASM_EQUATE(TD_STATE,                                8)
ASM_EQUATE(TD_TASK_ID,                             12)
ASM_EQUATE(TD_STACK_BASE,                          16)
ASM_EQUATE(TD_STACK_PTR,                           20)
ASM_EQUATE(TD_STACK_LIMIT,                         24)
ASM_EQUATE(TD_MY_QUEUE,                            28)
ASM_EQUATE(TD_HOME_QUEUE,                          32)
ASM_EQUATE(TD_TASK_SR,                             36)
ASM_EQUATE(TD_RESERVED_1,                          38)
ASM_EQUATE(TD_FLAGS,                               40)
ASM_EQUATE(TD_FLOAT_CONTEXT_PTR,                   52)

/**
 * 'READY_Q_STRUCT'
 */
ASM_EQUATE(RQ_HEAD_READY_Q,                         0)
ASM_EQUATE(RQ_TAIL_READY_Q,                         4)
ASM_EQUATE(RQ_NEXT_Q,                               8)
ASM_EQUATE(RQ_ENABLE_SR,                           12)
ASM_EQUATE(RQ_PRIORITY,                            14)

/**
 * 'INTERRUPT_TABLE_STRUCT'
 */
ASM_EQUATE(IT_APP_ISR,                              0)
ASM_EQUATE(IT_APP_ISR_EXCEPTION_HANDLER,            4)
ASM_EQUATE(IT_APP_ISR_DATA,                         8)

ASM_EQUATE(HASH_ISR_NUM,                            0)
ASM_EQUATE(HASH_ISR_ADDR,                           4)
ASM_EQUATE(HASH_ISR_EXCEPT_HNDL,                    8)
ASM_EQUATE(HASH_ISR_DATA,                           12)
ASM_EQUATE(HASH_ISR_NEXT,                          16)

/**
 * 'KERNEL_DATA_STRUCT'
 */
ASM_EQUATE(KD_ADDRESSING_CAPABILITY,                0)  //  0x0
ASM_EQUATE(KD_ENDIANESS,                            4)  //  0x4
ASM_EQUATE(KD_CPU_TYPE,                             8)  //  0x8
ASM_EQUATE(KD_PSP_CFG_MEMORY_ALIGNMENT,            10)  //  0xa
ASM_EQUATE(KD_PSP_CFG_STACK_ALIGNMENT,             12)  //  0xc
ASM_EQUATE(KD_PSP_CFG_MEM_STOREBLOCK_ALIGNMENT,    14)  //  0xe
ASM_EQUATE(KD_CONFIG1,                             16)  //  0x10
ASM_EQUATE(KD_CONFIG2,                             18)  //  0x12
ASM_EQUATE(KD_FLAGS,                               20)  //  0x14
ASM_EQUATE(KD_DISABLE_SR,                          22)  //  0x16
ASM_EQUATE(KD_IN_ISR,                              24)  //  0x18
ASM_EQUATE(KD_ACTIVE_SR,                           26)  //  0x1a
ASM_EQUATE(KD_ACTIVE_PTR,                          28)  //  0x1c
ASM_EQUATE(KD_READY_Q_LIST,                        32)  //  0x20
ASM_EQUATE(KD_CURRENT_READY_Q,                     36)  //  0x24
ASM_EQUATE(KD_DEFAULT_ISR,                         40)  //  0x28
ASM_EQUATE(KD_FIRST_USER_ISR_VECTOR,               44)  //  0x2c
ASM_EQUATE(KD_LAST_USER_ISR_VECTOR,                48)  //  0x30
ASM_EQUATE(KD_INTERRUPT_CONTEXT_PTR,               52)  //  0x34
ASM_EQUATE(KD_INTERRUPT_TABLE_PTR,                 56)  //  0x38
ASM_EQUATE(KD_INTERRUPT_STACK_PTR,                 60)  //  0x3c

ASM_EQUATE(KD_LOG_CONTROL,                         64)  //  0x40
ASM_EQUATE(KD_LOG_OLD_TD,                          68)  //  0x44
ASM_EQUATE(KD_FP_ACTIVE_PTR,                       72)  //  0x48
ASM_EQUATE(KD_DSP_ACTIVE_PTR,                      76)  //  0x4c
ASM_EQUATE(KD_SYSTEM_TD,                           80)  //  0x50

