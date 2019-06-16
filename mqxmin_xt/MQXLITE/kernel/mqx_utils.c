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

/*!
 * \brief Gets a pointer to kernel data.
 *
 * The address of kernel data corresponds to START_OF_KERNEL_MEMORY in the MQX
 * initialization structure that the application used to start MQX on the processor.
 *
 * \return Pointer to kernel data.
 *
 * \see MQX_INITIALIZATION_STRUCT
 */
pointer _mqx_get_kernel_data
(
    void
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    return (pointer) kernel_data;

} /* Endbody */

/*!
 * \brief Indicates that an error occurred that is so severe that MQX or the
 * application can no longer function.
 *
 * The function logs an error in kernel log (if it has been created and configured
 * to log errors) and calls _mqx_exit().
 * \n MQX calls _mqx_fatal_error() if it detects an unhandled interrupt while it
 * is in _int_exception_isr().
 * \n If an application calls _mqx_fatal_error() when it detects a serious error,
 * you can use this to help you debug by setting a breakpoint in the function.
 *
 * \param[in] error Error code.
 *
 * \warning Terminates the application by calling _mqx_exit().
 *
 * \see _mqx_exit
 * \see _int_exception_isr
 */
void _mqx_fatal_error
(
    _mqx_uint error
)
{ /* Body */
    _KLOGM(KERNEL_DATA_STRUCT_PTR kernel_data;)

    _KLOGM(_GET_KERNEL_DATA(kernel_data);)
    _KLOGE2(KLOG_mqx_fatal_error, error);
    _mqx_exit(error);
    _KLOGX1( KLOG_mqx_fatal_error);

} /* Endbody */

#if MQX_KD_HAS_COUNTER
/*!
 * \brief Gets a unique number.
 *
 * This function increments the counter and then returns value of the counter.
 * \n This provides a unique number for whoever requires it.
 *
 * \note The unique number will never be 0.
 *
 * \return 16-bit number for 16-bit processors or a 32-bit number for 32-bit
 * processors (unique for the processor and never 0).
 */
_mqx_uint _mqx_get_counter
(
    void
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    _mqx_uint return_value;

    _GET_KERNEL_DATA(kernel_data);
    _INT_DISABLE();

    /*
     * Increment counter, and ensure it is not zero.
     * If it is zero, set it to one.
     */
    if ( ++kernel_data->COUNTER == 0 ) {
        kernel_data->COUNTER = 1;
    } /* Endif */
    return_value = kernel_data->COUNTER;
    _INT_ENABLE();
    return (return_value);

} /* Endbody */
#endif /* MQX_KD_HAS_COUNTER */

/*!
 * \brief Gets the CPU type.
 *
 * CPU types begin with PSP_CPU_TYPE_ and are defined in
 * "source\psp\<cpu_family>\<cpu_family.h>".
 *
 * \return CPU_TYPE field of kernel data.
 *
 * \see _mqx_set_cpu_type
 */
_mqx_uint _mqx_get_cpu_type
(
    void
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    return kernel_data->CPU_TYPE;
} /* Endbody */

/*!
 * \brief Sets the CPU type.
 *
 * The function sets CPU_TYPE in kernel data. The MQX Host Tools family of products
 * uses CPU type. CPU types begin with PSP_CPU_TYPE_ and are defined in
 * source\psp\cpu_family\cpu_family.h.
 *
 * \param[in] cpu_type The value representing the kernel CPU type.
 *
 * \warning Does not verify that cpu_type is valid.
 *
 * \see _mqx_get_cpu_type
 */
void _mqx_set_cpu_type
(
    _mqx_uint cpu_type
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    kernel_data->CPU_TYPE = cpu_type;

} /* Endbody */

/*!
 * \brief Gets a pointer to the MQX initialization structure for this processor.
 *
 * \return Pointer to the MQX initialization structure in kernel data.
 *
 * \see _mqxlite_init
 * \see MQX_INITIALIZATION_STRUCT
 */
MQX_INITIALIZATION_STRUCT_PTR _mqx_get_initialization
(
    void
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    return ((MQX_INITIALIZATION_STRUCT_PTR) & kernel_data->INIT);

} /* Endbody */

/*!
 * \brief Gets a pointer to the MQX exit handler function called when MQX exits.
 *
 * \return Pointer to the MQX exit handler.
 *
 * \see _mqx_exit
 * \see _mqx_set_exit_handler
 */
MQX_EXIT_FPTR _mqx_get_exit_handler (void)
{ /* Body */
#if MQX_EXIT_ENABLED
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    return (kernel_data->EXIT_HANDLER);

#else
    return NULL;
#endif /* MQX_EXIT_ENABLED */
} /* Endbody */

/*!
 * \brief Sets a pointer to the MQX exit handler function called when MQX exits.
 *
 * \param[in] entry Pointer to the exit handler.
 *
 * \see _mqx_exit
 * \see _mqx_get_exit_handler
 */
void _mqx_set_exit_handler
(
    MQX_EXIT_FPTR entry
)
{ /* Body */
#if MQX_EXIT_ENABLED
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    _KLOGE2(KLOG_mqx_set_exit_handler, entry);
    kernel_data->EXIT_HANDLER = entry;
    _KLOGX1(KLOG_mqx_set_exit_handler);

#endif /* MQX_EXIT_ENABLED */
} /* Endbody */

/*!
 * \brief Gets the task ID of the System Task.
 *
 * System resources are owned by System Task.
 *
 * \return TASK_ID Task ID of System Task.
 *
 * \see _mem_transfer
 */
_task_id _mqx_get_system_task_id
(
    void
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    return(kernel_data->SYSTEM_TD.TASK_ID);

} /* Endbody */

/*!
 * \private
 *
 * \brief Initializes the static parts of the kernel data structure.
 *
 * Care has to be taken when calling functions within this file, as the kernel is
 * not running yet. Specifically, functions which rely on _mqx_get_kernel_data
 * can not be called.
 */
void _mqx_init_kernel_data_internal
(
    void
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    TASK_TEMPLATE_STRUCT_PTR    task_template_ptr;
    TD_STRUCT_PTR               td_ptr;
    _mqx_uint                   priority_levels;
    _mqx_uint                   i;

    _GET_KERNEL_DATA(kernel_data);

    /* Store the configuration used when the kernel was compiled */
    kernel_data->CONFIG1 = MQX_CNFG1;
    kernel_data->CONFIG2 = MQX_CNFG2;

    /* Store the addressability of the processor. How many bits in a byte. */
    kernel_data->ADDRESSING_CAPABILITY = PSP_MEMORY_ADDRESSING_CAPABILITY;

    /* Indicate the endianess of the target */
    kernel_data->ENDIANESS = PSP_ENDIAN;

    /* Store PSP memory alignment information */

#if PSP_MEM_STOREBLOCK_ALIGNMENT != 0
    kernel_data->PSP_CFG_MEM_STOREBLOCK_ALIGNMENT = PSP_MEM_STOREBLOCK_ALIGNMENT;
#endif

    kernel_data->PSP_CFG_MEMORY_ALIGNMENT = PSP_MEMORY_ALIGNMENT;
    kernel_data->PSP_CFG_STACK_ALIGNMENT = PSP_STACK_ALIGNMENT;

    /* Set IPC id for compatibility */
#if MQX_USE_IPC
    kernel_data->MY_IPC_ID = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, 1);
#endif

    /* Store location of current interrupt vector table */
#if MQX_EXIT_ENABLED
    kernel_data->USERS_VBR = (_mqx_max_type)_int_get_vector_table();
#endif

#if MQX_CHECK_ERRORS
    if (kernel_data->INIT.TASK_TEMPLATE_LIST == NULL) {
        _mqx_exit(MQX_INVALID_POINTER);
    } /* Endif */
#endif

#if MQX_HAS_TIME_SLICE
    /* Set the default scheduling policy for created tasks */
    kernel_data->SCHED_POLICY = MQX_SCHED_FIFO;
#endif

#if MQX_KD_HAS_COUNTER
    /* Initialize the kernel counter. */
    kernel_data->COUNTER = 1U;
#endif

    /* Set up the disable and enable priority levels */
    _psp_set_kernel_disable_level();

    /*
     * Initialize the system task so that functions which update the
     * task error code can be called.
     * The system task never runs, but it's TD is used for error codes
     * during initialization, and for storage of memory blocks assigned
     * to the system.
     */
    td_ptr = (TD_STRUCT_PTR) & kernel_data->SYSTEM_TD;
    kernel_data->ACTIVE_PTR = td_ptr;
    kernel_data->ACTIVE_SR = kernel_data->DISABLE_SR;
    td_ptr->TASK_SR = kernel_data->DISABLE_SR;
    td_ptr->TASK_ID = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, SYSTEM_TASK_NUMBER);
    td_ptr->STATE = BLOCKED;

    /* Initialize the light weight semaphores queue */
    _QUEUE_INIT(&kernel_data->LWSEM, 0);

#if MQX_ENABLE_USER_MODE
    _QUEUE_INIT(&kernel_data->USR_LWSEM, 0);
#endif

#if MQX_HAS_TICK
    /* Set up the timeout queue */
    _QUEUE_INIT(&kernel_data->TIMEOUT_QUEUE, 0);
#endif

    /*
     * Compute the number of MQX priority levels needed. This is done
     * by determining the task that has the lowest priority (highest number)
     */
    priority_levels = 0;

    task_template_ptr = kernel_data->INIT.TASK_TEMPLATE_LIST;

    for (i = 0; task_template_ptr->TASK_TEMPLATE_INDEX && (i < MQX_MAXIMUM_NUMBER_OF_TASK_TEMPLATES); ++i, ++task_template_ptr) {
        if (priority_levels < task_template_ptr->TASK_PRIORITY) {
            priority_levels = task_template_ptr->TASK_PRIORITY;
        } /* Endif */
    } /* Endfor */
    kernel_data->LOWEST_TASK_PRIORITY = priority_levels;

#if MQX_USE_IDLE_TASK
    /*
     * Initialize the task template for the IDLE Task.
     * NOTE that the idle task runs at 1 level lower than any user task.
     */
    task_template_ptr = (TASK_TEMPLATE_STRUCT_PTR)&kernel_data->IDLE_TASK_TEMPLATE;
    task_template_ptr->TASK_TEMPLATE_INDEX  = IDLE_TASK;
    task_template_ptr->TASK_STACKSIZE       = kernel_data->INIT.IDLE_TASK_STACK_SIZE;
    task_template_ptr->TASK_NAME            = "_mqx_idle_task";
    task_template_ptr->TASK_ADDRESS         = _mqx_idle_task;
    task_template_ptr->TASK_PRIORITY        = priority_levels + 1;
#endif

    /*
     * Initialize the linked list of all TDs in the system.
     * Initially zero. Not including system TD
     */
    _QUEUE_INIT(&kernel_data->TD_LIST, 0);

    /* Set the TD counter */
    kernel_data->TASK_NUMBER = 1;

} /* Endbody */


/* EOF */
