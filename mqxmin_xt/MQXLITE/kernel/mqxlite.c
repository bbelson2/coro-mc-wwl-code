/*HEADER**********************************************************************
*
* Copyright (c) 2008-2010 Freescale Semiconductor;
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
* $FileName: mqx.c$
* $Version : 3.8.31.3$
* $Date    : Feb-24-2012$
*
* Comments:
*
*   This file contains the source for the main MQX function _mqx().
*
*END*************************************************************************/

#ifndef __NO_SETJMP
#include <setjmp.h>
#endif

#include "mqx_inc.h"

/*
 * A global zero initialized MQX tick structure.
 * It is used by various MQX functions that need
 * to zero initialize local tick structures. An
 * extern to it is provided in MQX.H so applications
 * can use it as well.
 */
const MQX_TICK_STRUCT _mqx_zero_tick_struct;

/* Version/vendor identification also used by TAD to determine paths to its context files */
const uint_32      _mqx_version_number      = 0x04000000;
const char * const _mqx_version             = "4.0.0";
const char * const _mqx_vendor              = "Freescale/Freescale MQX";
const char * const _mqx_path                = MQX_PATH;
/* Version strings */
const char * const _mqx_generic_revision    = REAL_NUM_TO_STR(GEN_REVISION);
const char * const _mqx_psp_revision        = REAL_NUM_TO_STR(PSP_REVISION);
const char * const _mqx_copyright           = "(c) 2011 Freescale Semiconductor. All rights reserved.";
const char * const _mqx_date                = __DATE__ " at " __TIME__;

/* Identify MQX Lite version number and version to source files (generated in user_config.h) */
const uint_32      _mqxlite_version_number  = MQX_LITE_VERSION_NUMBER;
const char * const _mqxlite_version         = MQX_LITE_VERSION;


/* A global pointer to the location of the kernel data structure */
KERNEL_ACCESS struct   kernel_data_struct * _mqx_kernel_data = (pointer)-1;
KERNEL_ACCESS volatile pointer              _mqx_system_stack;

/* Error return jump buffer for kernel errors */
#if MQX_EXIT_ENABLED || MQX_CRIPPLED_EVALUATION
jmp_buf _mqx_exit_jump_buffer_internal;
#endif


/*!
 * \brief Initializes MQXLite on the processor.
 *
 * The function does the following:
 * \n - Initializes kernel data.
 * \n - Creates the interrupt stack.
 * \n - Creates the ready queues.
 * \n - Creates a lightweight semaphore for task creation/destruction.
 * \n - Initializes interrupts.
 * \n - Initializes system timer.
 *
 * \param[in] mqx_init Pointer to the MQXLITE initialization structure for the
 * processor.
 *
 * \return MQX_OK
 * \return Initialization error code
 *
 * \warning Must be called exactly once per processor.
 *
 * \see _mqxlite
 * \see _mqx_exit
 * \see MQXLITE_INITIALIZATION_STRUCT
 */
_mqx_uint _mqxlite_init
(
    MQXLITE_INITIALIZATION_STRUCT const * mqx_init
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    pointer                 stack_ptr;
    _mqx_uint               result = MQX_OK;

    /*
     * The kernel data structure starts at the start of kernel memory,
     * as specified in the initialization structure. Make sure address
     * specified is aligned
     */
    kernel_data = (KERNEL_DATA_STRUCT_PTR) (mqx_init->START_OF_KERNEL_MEMORY);

    /* Set the global pointer to the kernel data structure */
    _SET_KERNEL_DATA(kernel_data);

    /* The following assignments are done to force the linker to include
     * the symbols, which are required by TAD.
     * Note that we should use address of the variable so it is not optimized
     * as direct constant assignment when optimization level is high.
     * Note that counter will be immediately reset to zero on the subsequent
     * _mem_zero call. */
    {
        MQX_INITIALIZATION_STRUCT  * MQX_init_struct_ptr;

        *(volatile pointer*) kernel_data = (pointer) & _mqx_version_number;
        *(volatile pointer*) kernel_data = (pointer) & _mqx_vendor;
        *(volatile pointer*) kernel_data = (pointer) & _mqx_path;
        *(volatile pointer*) kernel_data = (pointer) & _mqxlite_version_number;
        *(volatile pointer*) kernel_data = (pointer) & MQX_init_struct_ptr;
    }
    /* Initialize the kernel data to zero. */
    _mem_zero((pointer) kernel_data, (_mem_size) sizeof(KERNEL_DATA_STRUCT));

#if MQX_CHECK_ERRORS && MQX_VERIFY_KERNEL_DATA
    /* Verify that kernel data can be read and written correctly without
     * errors.  This is necessary during BSP development to validate the
     * DRAM controller is initialized properly.
     */
    if (PSP_KERNEL_DATA_VERIFY_ENABLE) {
        /* This memory check is dangerous, because can destroy boot stack
         * stack which is used !!! -> MQX will failed !
         * Set PSP_KERNEL_DATA_VERIFY_ENABLE to 1
         * only if your boot stack is out of MQX memory heap
         */

        result = _mem_verify((uchar_ptr)kernel_data + sizeof(KERNEL_DATA_STRUCT),
                        mqx_init->END_OF_KERNEL_MEMORY);
        if ( result != MQX_OK ) {
            return (result); /* RETURN TO USER */
        }
    }
#endif /* MQX_CHECK_ERRORS && MQX_VERIFY_KERNEL_DATA */
    /* Copy the MQX initialization structure into kernel data. */
    kernel_data->INIT = *mqx_init;

    /* init kernel data structures */
    _mqx_init_kernel_data_internal();

#if MQX_USE_LWMEM == 1
    /**
     * Initialize lightweight memory pool for dynamic memory allocation
     * Function is generated in Generated_Code\MQX1.c module
     */
    mqx_initialize_heap();
#endif


#if MQX_USE_INTERRUPTS

    /* Now obtain the interrupt stack */
    if (kernel_data->INIT.INTERRUPT_STACK_LOCATION) {
        stack_ptr = kernel_data->INIT.INTERRUPT_STACK_LOCATION;
        result = kernel_data->INIT.INTERRUPT_STACK_SIZE;
    }
    else
    {
        return (MQX_INVALID_PARAMETER);
    } /* Endif */

#if MQX_MONITOR_STACK
    _task_fill_stack_internal((_mqx_uint_ptr)stack_ptr, result);
#endif

    kernel_data->INTERRUPT_STACK_PTR = _GET_STACK_BASE(stack_ptr, result);

#endif /* MQX_USE_INTERRUPTS */

#if MQX_USE_IDLE_TASK == 0
    {
        /*
         * Set the stack for the system TD, in case the idle task gets blocked
         * by an exception or if idle task is not used.
         */
        TD_STRUCT_PTR   td_ptr;
        uchar_ptr       stack_base_ptr;

        stack_base_ptr = (uchar_ptr) _GET_STACK_BASE(mqx_system_stack, PSP_MINSTACKSIZE);
        td_ptr = SYSTEM_TD_PTR(kernel_data);
        td_ptr->STACK_PTR   = (pointer)(stack_base_ptr - sizeof(PSP_STACK_START_STRUCT));
        td_ptr->STACK_BASE  = stack_base_ptr;
#if MQX_TD_HAS_STACK_LIMIT
        td_ptr->STACK_LIMIT = _GET_STACK_LIMIT(mqx_system_stack, PSP_MINSTACKSIZE);
#endif
        _mqx_system_stack   = td_ptr->STACK_PTR;
    }
#endif /* MQX_USE_IDLE_TASK */

    /* Build the MQX ready to run queues */
    result = _psp_init_readyqs();
#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
    if ( result != MQX_OK ) {
        return (result); /* RETURN TO USER */
    } /* Endif */
#endif

#if MQX_USE_COMPONENTS
    /* Create a light wait semaphore for component creation */
    _lwsem_create((LWSEM_STRUCT_PTR)&kernel_data->COMPONENT_CREATE_LWSEM, 1);
#endif

    /* Create a light wait semaphore for task creation/destruction creation */
    _lwsem_create((LWSEM_STRUCT_PTR) & kernel_data->TASK_CREATE_LWSEM, 1);

    /* Set the CPU type */
    _mqx_set_cpu_type(MQX_CPU);

    result = _psp_int_init(FIRST_INTERRUPT_VECTOR_USED, LAST_INTERRUPT_VECTOR_USED);
    if (result != MQX_OK) {
        return(result); /* RETURN TO USER */
    }

    /* set possible new interrupt vector table
     * if MQX_ROM_VECTORS = 0 switch to ram interrupt table which
     * was initialized in _psp_int_init)
     */
    _int_set_vector_table((uint32_t)(&__vect_table));

    /*
     * Initialize System Timer and Ticks parameters in kernel_data structure
     */
    system_timer_init(NULL);

    return MQX_OK; /* To satisfy lint */

} /* Endbody */

/*!
 * \brief Starts MQXLite on the processor.
 *
 * The function does the following:
 * \n - Starts system timer.
 * \n - Starts MQX tasks.
 * \n - Starts autostart application tasks.
 *
 * \return Does not return (Success.)
 * \return If application calls _mqx_exit(), error code that it passed to _mqx_exit().
 *
 * \warning Must be called exactly once per processor.
 *
 * \see _mqxlite_init
 * \see _mqx_exit
 */
_mqx_uint _mqxlite(void)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR              kernel_data;
    TD_STRUCT_PTR                       td_ptr;

#if MQX_EXIT_ENABLED || MQX_CRIPPLED_EVALUATION
    /* Setup a longjmp buffer using setjmp, so that if an error occurs
     * in mqx initialization, we can perform a longjmp to this location.
     *
     * Also _mqx_exit will use this jumpbuffer to longjmp to here in order
     * to cleanly exit MQX.
     */
    if ( MQX_SETJMP( _mqx_exit_jump_buffer_internal ) ) {
        _GET_KERNEL_DATA(kernel_data);
        _int_set_vector_table(kernel_data->USERS_VBR);
        return kernel_data->USERS_ERROR;
    } /* Endif */
#endif


    _GET_KERNEL_DATA(kernel_data);


    /* Create the idle task */
#if MQX_USE_IDLE_TASK
    td_ptr = _task_init_internal((TASK_TEMPLATE_STRUCT_PTR)&kernel_data->IDLE_TASK_TEMPLATE,
                                kernel_data->ACTIVE_PTR->TASK_ID,
                                #if MQX_ENABLE_LOW_POWER
                                    (uint_32)1,
                                #else
                                    (uint_32)0,
                                #endif
                                 FALSE,
                                 kernel_data->INIT.IDLE_TASK_STACK_LOCATION,
                                 kernel_data->IDLE_TASK_TEMPLATE.TASK_STACKSIZE);
    #if MQX_CHECK_ERRORS
        if (td_ptr == NULL) {
            _mqx_exit(MQX_OUT_OF_MEMORY);
        } /* Endif */
    #endif
    _task_ready_internal(td_ptr);
#endif /* MQX_USE_IDLE_TASK */

    /* Check here for auto-create tasks, and create them here */
    {
        TASK_TEMPLATE_STRUCT_PTR    template_ptr;
        int                         task_index = 0;

        template_ptr = kernel_data->INIT.TASK_TEMPLATE_LIST;

        while (template_ptr->TASK_TEMPLATE_INDEX) {
            if (template_ptr->TASK_ATTRIBUTES & MQX_AUTO_START_TASK) {
                td_ptr = _task_init_internal(template_ptr,
                                             kernel_data->ACTIVE_PTR->TASK_ID,
                                             template_ptr->CREATION_PARAMETER,
                                             FALSE,
                                             (pointer)mqx_task_stack_pointers[task_index],
                                             (_mem_size)template_ptr->TASK_STACKSIZE);
    #if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
                if (td_ptr == NULL) {
                    _mqx_exit(MQX_OUT_OF_MEMORY);
                } /* Endif */
    #endif
                _task_ready_internal(td_ptr);
            } /* Endif */
            ++template_ptr;
            ++task_index;
        } /* Endwhile */
    }

    /* System timer start */
    system_timer_start(NULL);

    _sched_start_internal(); /* WILL NEVER RETURN FROM HERE */

    return MQX_OK; /* To satisfy lint */
} /* Endbody */

/*!
 * \brief Terminate the MQX application and return to the environment that started
 * the application.
 *
 * The function returns back to the environment that called _mqxlite(). If the
 * application has installed the MQX exit handler (_mqx_set_exit_handler()),
 * _mqx_exit() calls the MQX exit handler before it exits. By default,
 * _bsp_exit_handler() is installed as the MQX exit handler in each BSP.
 *
 * \note
 * It is important to ensure that the environment (boot call stack) the MQX is
 * returning to is in the consistent state. This is not provided by distributed
 * MQX BSPs, because the boot stack is reused (rewritten) by MQX Kernel data. Set
 * the boot stack outside of Kernel data section to support correct _mqx_exit
 * functionality.
 *
 * \param[in] error Error code to return to the function that called _mqxlite_init()
 * or _mqxlite().
 *
 * \warning Behavior depends on the BSP.
 *
 * \see mqxlite_init
 * \see mqxlite
 */
void _mqx_exit
(
    _mqx_uint error
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _int_disable();

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_mqx_exit, error);

#if MQX_EXIT_ENABLED
    kernel_data->USERS_ERROR = error;
    if (kernel_data->EXIT_HANDLER) {
        (*kernel_data->EXIT_HANDLER)();
    }/* Endif */
    MQX_LONGJMP( _mqx_exit_jump_buffer_internal, 1 );
#else
    while (TRUE) {
    } /* Endwhile */
#endif /* MQX_EXIT_ENABLED */
} /* Endbody */

/* EOF */
