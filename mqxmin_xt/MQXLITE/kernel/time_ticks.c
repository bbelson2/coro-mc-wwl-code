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
 *****************************************************************************
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
 *****************************************************************************
 *
 * $FileName: time.c$
 * $Version : 3.8.0.3$
 * $Date    : Mar-14-2012$
 *
 * Comments:
 *
 *END************************************************************************/

#include "mqx_inc.h"

//#include "timer.h"
//#include "timer_prv.h"
/*
 *  CR171 & CR172
 *
 *  For a time delay queues we insert the task into the queue after
 *  all tasks of the same delay period.  We do not need to do this, we
 *  can just insert the task into the queue before the task waiting
 *  for the same time, as all tasks will be activated anyways.
 *  Running the queues takes a long time, and is done with interrupts
 *  disabled.  There will be a slight behavioural difference in that
 *  two tasks that delay for the same time at the same priority level,
 *  will not run in the order that they called delay.
 *
 *  Set the following define to 1 if you want the original FIFO
 *  behavior where tasks are activated from the time delay queue into
 *  the ready queue in FIFO order (longer enqueue search with
 *  interrupts disabled).
 */
#ifndef MQX_DELAY_ENQUEUE_FIFO_ORDER
#define MQX_DELAY_ENQUEUE_FIFO_ORDER    0  /* 0=LIFO, 1=FIFO */
#endif
#if MQX_DELAY_ENQUEUE_FIFO_ORDER
#define MQX_DELAY_ENQUEUE_POLICY(result)  (result > 0)  /* FIFO original */
#else
#define MQX_DELAY_ENQUEUE_POLICY(result)  (result >= 0) /* LIFO faster */
#endif

#if MQX_HAS_TICK
/*!
 * \brief Suspends the active task for the number of ticks (in tick time).
 *
 * The functions put the active task in the timeout queue for the specified time.
 * \n Before the time expires, any task can remove the task from the timeout queue
 * by calling _time_dequeue().
 *
 * \param[in] ticks Pointer to the minimum number of ticks to suspend the task.
 *
 * \warning Blocks the calling task.
 *
 * \see _time_delay
 * \see _time_delay_ticks
 * \see _time_delay_until
 * \see _time_dequeue
 * \see MQX_TICK_STRUCT
 */
void _time_delay_for
(
    register MQX_TICK_STRUCT_PTR ticks
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TD_STRUCT_PTR td_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_time_delay_for, ticks);

#if MQX_CHECK_ERRORS
    if (ticks == NULL) {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_time_delay_for, MQX_INVALID_PARAMETER);
        return;
    } /* Endif */
#endif

    td_ptr = kernel_data->ACTIVE_PTR;

    _INT_DISABLE();

    /* Calculate time to wake up the task */
    PSP_ADD_TICKS(ticks, &kernel_data->TIME, &td_ptr->TIMEOUT);

    _time_delay_internal(td_ptr);

    _INT_ENABLE();

    _KLOGX1( KLOG_time_delay_for);

} /* Endbody */

#endif /* MQX_HAS_TICK */

#if MQX_HAS_TICK
/*!
 * \private
 *
 * \brief Suspends the active task.
 *
 * This function puts a task on the timeout queue for the specified number of
 * ticks, or until removed by another task. Must be called int disabled.
 *
 * \param[in] td_ptr Pointer to the task to delay.
 *
 * \see TD_STRUCT
 */
void _time_delay_internal
   (
      register TD_STRUCT_PTR td_ptr
   )
{ /* Body */
   register KERNEL_DATA_STRUCT_PTR  kernel_data;
   register TD_STRUCT_PTR           td2_ptr;
   register TD_STRUCT_PTR           tdprev_ptr;
   register _mqx_uint               count;
   register _mqx_int                result;

   _GET_KERNEL_DATA(kernel_data);

   /* Remove task from ready to run queue */
   tdprev_ptr = (TD_STRUCT_PTR)((pointer)&kernel_data->TIMEOUT_QUEUE);
   if ( _QUEUE_GET_SIZE(&kernel_data->TIMEOUT_QUEUE) ) {

      /* Perform insertion sort by time */
      td2_ptr    = (TD_STRUCT_PTR)((pointer)kernel_data->TIMEOUT_QUEUE.NEXT);

      /* SPR P171-0023-01 use pre-decrement on while loop */
      count      = _QUEUE_GET_SIZE(&kernel_data->TIMEOUT_QUEUE) + 1;
      while ( --count ) {
      /* END SPR */
         result = PSP_CMP_TICKS(&td2_ptr->TIMEOUT, &td_ptr->TIMEOUT);
         if (MQX_DELAY_ENQUEUE_POLICY(result)) {
            /* Enqueue before td2_ptr */
            break;
         } /* Endif */

         tdprev_ptr = td2_ptr;
         td2_ptr    = td2_ptr->TD_NEXT;
      } /* Endwhile */

   } /* Endif */

   /* Remove from ready queue */
   _QUEUE_UNLINK(td_ptr);

   /* Insert into timeout queue */
   _QUEUE_INSERT(&kernel_data->TIMEOUT_QUEUE,tdprev_ptr,td_ptr);

   td_ptr->STATE |= IS_ON_TIMEOUT_Q;

   _sched_execute_scheduler_internal();

} /* Endbody */
#endif /* MQX_HAS_TICK */

#if MQX_HAS_TICK
/*!
 * \brief Suspends the active task for the number of ticks.
 *
 * The functions put the active task in the timeout queue for the specified time.
 * \n Before the time expires, any task can remove the task from the timeout queue
 * by calling _time_dequeue().
 *
 * \param[in] time_in_ticks Minimum number of ticks to suspend the task.
 *
 * \warning Blocks the calling task.
 *
 * \see _time_delay
 * \see _time_delay_for
 * \see _time_delay_until
 * \see _time_dequeue
 */
void _time_delay_ticks
(
    register _mqx_uint time_in_ticks
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TD_STRUCT_PTR td_ptr;

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE) {
        _usr_time_delay_ticks(time_in_ticks);
        return;
    }
#endif

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_time_delay_ticks, time_in_ticks);

    if ( !time_in_ticks ) {
        _KLOGX1(KLOG_time_delay_ticks);
        return;
    } /* Endif */

    td_ptr = kernel_data->ACTIVE_PTR;

    _INT_DISABLE();

    PSP_ADD_TICKS_TO_TICK_STRUCT(&kernel_data->TIME, time_in_ticks, &td_ptr->TIMEOUT);

    _time_delay_internal(td_ptr);

    _INT_ENABLE();
    _KLOGX1(KLOG_time_delay_ticks);

}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Suspends the active task for the number of ticks.
 *
 * This function is an equivalent to the _time_delay_ticks() API call but it can
 * be executed from within the User task or other code running in the CPU User
 * mode. Parameters passed to this function by pointer are required to meet the
 * memory protection requirements as described in the parameter list below.
 *
 * \param[in] time_in_ticks Minimum number of ticks to suspend the task.
 *
 * \warning Blocks the calling task.
 *
 * \see _time_delay_ticks
 * \see _usr_time_delay
 * \see _usr_time_get_elapsed_ticks
 */
void _usr_time_delay_ticks
(
    register _mqx_uint time_in_ticks
)
{
    MQX_API_CALL_PARAMS params = {(uint_32)time_in_ticks, 0, 0, 0, 0};
    _mqx_api_call(MQX_API_TIME_DELAY_TICKS, &params);
}

#endif /* MQX_ENABLE_USER_MODE */
#endif /* MQX_HAS_TICK */

#if MQX_HAS_TICK
/*!
 * \brief Suspends the active task until the specified time (in tick time).
 *
 * The functions put the active task in the timeout queue until the specified tick
 * count is reached.
 * \n Before the time expires, any task can remove the task from the timeout queue
 * by calling _time_dequeue().
 *
 * \param[in] ticks Pointer to the time (in tick time) until which to suspend the task.
 *
 * \warning Blocks the calling task.
 *
 * \see _time_delay
 * \see _time_delay_for
 * \see _time_delay_ticks
 * \see _time_dequeue
 * \see MQX_TICK_STRUCT
 */
void _time_delay_until
(
    register MQX_TICK_STRUCT_PTR ticks
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TD_STRUCT_PTR td_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_time_delay_until, ticks);

#if MQX_CHECK_ERRORS
    if (ticks == NULL) {
        _task_set_error(MQX_INVALID_PARAMETER);
        _KLOGX2(KLOG_time_delay_until, MQX_INVALID_PARAMETER);
        return;
    } /* Endif */
#endif

    td_ptr = kernel_data->ACTIVE_PTR;

    /* Calculate time to wake up the task */
    td_ptr->TIMEOUT = *ticks;

    _INT_DISABLE();

    _time_delay_internal(td_ptr);

    _INT_ENABLE();

    _KLOGX1(KLOG_time_delay_until);

} /* Endbody */

#endif /* MQX_HAS_TICK */

/*!
 * \brief Get the difference between two tick times.
 *
 * \param[in]  end_tick_ptr   Pointer to the normalized end time, which must be
 * greater than the start time.
 * \param[in]  start_tick_ptr Pointer to the normalized start time in tick time.
 * \param[out] diff_tick_ptr  Pointer to the time difference (the time is normalized).
 *
 * \return MQX_OK
 * \return MQX_INVALID_PARAMETER (One or more pointers are NULL.)
 *
 * \see _time_diff
 * \see _time_diff_days
 * \see _time_diff_hours
 * \see _time_diff_minutes
 * \see _time_diff_seconds
 * \see _time_diff_milliseconds
 * \see _time_diff_microseconds
 * \see _time_diff_nanoseconds
 * \see _time_diff_picoseconds
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_set
 * \see _time_set_ticks
 * \see MQX_TICK_STRUCT
 */
_mqx_uint _time_diff_ticks
(
    MQX_TICK_STRUCT_PTR end_tick_ptr,
    MQX_TICK_STRUCT_PTR start_tick_ptr,
    MQX_TICK_STRUCT_PTR diff_tick_ptr
)
{ /* Body */

#if MQX_CHECK_ERRORS
    if ((end_tick_ptr == NULL) || (start_tick_ptr == NULL) ||
                    (diff_tick_ptr == NULL))
    {
        return MQX_INVALID_PARAMETER;
    } /* Endif */
#endif

    PSP_SUB_TICKS(end_tick_ptr, start_tick_ptr, diff_tick_ptr);

    return MQX_OK;

} /* Endbody */

/*!
 * \brief Get the difference between two tick times and clamps result into int_32 interval.
 *
 * \param[in]  end_tick_ptr   Pointer to the normalized end time (in ticks), which must be
 * greater than the start time.
 * \param[in]  start_tick_ptr Pointer to the normalized start time (in ticks).
 * \param[out] overflow_ptr   Set to TRUE if overflow occurs.
 *
 * \return Difference between the times as int_32 (<-(MAX_INT_32 + 1), MAX_INT_32>).
 *
 * \see _time_diff_ticks
 * \see _time_diff
 * \see _time_diff_days
 * \see _time_diff_hours
 * \see _time_diff_minutes
 * \see _time_diff_seconds
 * \see _time_diff_milliseconds
 * \see _time_diff_microseconds
 * \see _time_diff_nanoseconds
 * \see _time_diff_picoseconds
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_set
 * \see _time_set_ticks
 * \see MQX_TICK_STRUCT
 */
int_32 _time_diff_ticks_int32
(
    MQX_TICK_STRUCT_PTR end_tick_ptr,
    MQX_TICK_STRUCT_PTR start_tick_ptr,
    boolean _PTR_ overflow_ptr
)
{ /* Body */

#if MQX_CHECK_ERRORS
    if ((end_tick_ptr == NULL) || (start_tick_ptr == NULL))
    {
        if (overflow_ptr != NULL) *overflow_ptr = TRUE;
        return MAX_INT_32;
    } /* Endif */
#endif

    return (PSP_SUB_TICKS_INT32(end_tick_ptr, start_tick_ptr, overflow_ptr));
} /* Endbody */

/*!
 * \brief Removes the task (specified by task ID) from the timeout queue.
 *
 * The function removes from the timeout queue a task that has put itself there
 * for a period of time (_time_delay()).
 * \n If tid is invalid or represents a task that is on another processor, the
 * function does nothing.
 * \n A task that calls the function must subsequently put the task in the task's
 * ready queue with _task_ready().
 *
 * \param[in] tid Task ID of the task to be removed from the timeout queue.
 *
 * \warning Removes the task from the timeout queue, but does not put it in the
 * task's ready queue.
 *
 * \see _task_ready
 * \see _time_delay
 * \see _time_delay_for
 * \see _time_delay_ticks
 * \see _time_delay_until
 * \see _time_dequeue_td
 */
void _time_dequeue
(
    _task_id tid
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR td_ptr;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_time_dequeue, tid);

    td_ptr = (TD_STRUCT_PTR) _task_get_td(tid);
    if (td_ptr == NULL) {
        _KLOGX1( KLOG_time_dequeue);
        return;
    }/* Endif */

    _int_disable();
    _TIME_DEQUEUE(td_ptr, kernel_data);
    _int_enable();

    _KLOGX1( KLOG_time_dequeue);

} /* Endbody */

/*!
 * \brief Removes the task (specified by task descriptor) from the timeout queue.
 *
 * \param[in] td Pointer to the task descriptor of the task to be removed from
 * the timeout queue.
 *
 * \warning Removes the task from the timeout queue; does not put it in the task's
 * ready queue.
 *
 * \see _task_ready
 * \see _time_delay
 * \see _time_delay_for
 * \see _time_delay_ticks
 * \see _time_delay_until
 * \see _time_dequeue
 */
void _time_dequeue_td
(
    pointer td
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    TD_STRUCT_PTR td_ptr = (TD_STRUCT_PTR) td;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_time_dequeue_td, td);

    _int_disable();
    _TIME_DEQUEUE(td_ptr, kernel_data);
    _int_enable();

    _KLOGX1( KLOG_time_dequeue_td);

} /* Endbody */

/*!
 * \brief Get the time elapsed since MQX started in tick time.
 *
 * The function always returns elapsed time; it is not affected by _time_set() or
 * _time_set_ticks().
 *
 * \param[in,out] tick_ptr Where to store the elapsed tick time.
 *
 * \see _time_get_elapsed
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_set
 * \see _time_set_ticks
 * \see _time_get_elapsed_ticks_fast
 * \see MQX_TICK_STRUCT
 */
void _time_get_elapsed_ticks
(
    MQX_TICK_STRUCT_PTR tick_ptr
)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;

#if MQX_CHECK_ERRORS
    if ( tick_ptr == NULL ) {
        return;
    }
#endif

#if MQX_ENABLE_USER_MODE && MQX_ENABLE_USER_STDAPI
    if (MQX_RUN_IN_USER_MODE) {
        _usr_time_get_elapsed_ticks(tick_ptr);
        return;
    }
#endif

    _GET_KERNEL_DATA(kernel_data);

    _INT_DISABLE();

    *tick_ptr = kernel_data->TIME;

    if (kernel_data->GET_HWTICKS) {
        /* The hardware clock may have counted passed it's reference
         * and have an interrupt pending.  Thus, HW_TICKS may exceed
         * kernel_data->HW_TICKS_PER_TICK and this tick_ptr may need
         * normalizing.  This is done in a moment.
         */
        tick_ptr->HW_TICKS = (*kernel_data->GET_HWTICKS)(kernel_data->GET_HWTICKS_PARAM);
    } /* Endif */

    /* The timer ISR may go off and increment kernel_data->TIME */
    _INT_ENABLE();

    /* The tick_ptr->HW_TICKS value might exceed the
     * kernel_data->HW_TICKS_PER_TICK and need to be
     * normalized for the PSP.
     */
    PSP_NORMALIZE_TICKS(tick_ptr);

}

#if MQX_ENABLE_USER_MODE
/*!
 * \brief Get the time elapsed since MQX started in tick time.
 *
 * This function is an equivalent to the _time_get_elapsed_ticks() API call but
 * it can be executed from within the User task or other code running in the CPU
 * User mode. Parameters passed to this function by pointer are required to meet
 * the memory protection requirements as described in the parameter list below.
 *
 * \param[in,out] tick_ptr Read/write. Where to store the elapsed tick time.
 *
 * \see _time_get_elapsed_ticks
 * \see _usr_time_delay
 * \see _usr_time_delay_ticks
 * \see MQX_TICK_STRUCT
 */
void _usr_time_get_elapsed_ticks
(
    MQX_TICK_STRUCT_PTR tick_ptr
)
{
    MQX_API_CALL_PARAMS params = {(uint_32)tick_ptr, 0, 0, 0, 0};
    _mqx_api_call(MQX_API_TIME_GET_ELAPSED_TICKS, &params);
}

#endif /* MQX_ENABLE_USER_MODE */

#if MQX_HAS_TICK
/*!
 * \brief Get the time elapsed since MQX started in tick time.
 *
 * The function always returns elapsed time; it is not affected by _time_set() or
 * _time_set_ticks().
 * \n The only difference between _time_get_elapsed_ticks_fast and
 * _time_get_elapsed_ticks is that this one is supposed to be called from code
 * with interrupts DISABLED. Do not use this function with interrupts ENABLED.
 *
 * \param[in,out] tick_ptr Where to store the elapsed tick time.
 *
 * \see _time_get_elapsed_ticks
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_set
 * \see _time_set_ticks
 * \see MQX_TICK_STRUCT
 */
void _time_get_elapsed_ticks_fast
(
    MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

#if MQX_CHECK_ERRORS
    if ( tick_ptr == NULL ) {
        return;
    } /* Endif */
#endif

    _GET_KERNEL_DATA(kernel_data);

    *tick_ptr = kernel_data->TIME;

    if (kernel_data->GET_HWTICKS) {
        /* The hardware clock may have counted passed it's reference
         * and have an interrupt pending.  Thus, HW_TICKS may exceed
         * kernel_data->HW_TICKS_PER_TICK and this tick_ptr may need
         * normalizing.  This is done in a moment.
         */
        tick_ptr->HW_TICKS = (*kernel_data->GET_HWTICKS)(kernel_data->GET_HWTICKS_PARAM);
    } /* Endif */

    /* The tick_ptr->HW_TICKS value might exceed the
     * kernel_data->HW_TICKS_PER_TICK and need to be
     * normalized for the PSP.
     */
    PSP_NORMALIZE_TICKS(tick_ptr);

} /* Endbody */
#endif /* MQX_HAS_TICK */

#if MQX_HAS_HW_TICKS
/*!
 * \brief Gets the number of hardware ticks since the last tick.
 *
 * \return Number of hardware ticks since the last tick.
 *
 * \see _time_get_hwticks_per_tick
 * \see _time_set_hwticks_per_tick
 */
uint_32 _time_get_hwticks
(
    void
)
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    if (kernel_data->GET_HWTICKS) {
        return (*kernel_data->GET_HWTICKS)(kernel_data->GET_HWTICKS_PARAM);
    } /* Endif */

    return 0;

} /* Endbody */
#endif /* MQX_HAS_HW_TICKS */

#if MQX_HAS_TICK
/*!
 * \brief Get the absolute time in tick time.
 *
 * If the application changed the absolute time with _time_set_ticks(),
 * _time_get_ticks() returns the time that was set plus the number of  ticks since
 * the time was set.
 * \n If the application has not changed the absolute time with _time_set_ticks(),
 * _time_get_ticks() returns the same as _time_get_elapsed_ticks(), which is the
 * number of ticks since MQX started.
 *
 * \param[in,out] tick_ptr Where to store the absolute time in tick time.
 *
 * \see _time_get
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _time_set
 * \see _time_set_ticks
 * \see MQX_TICK_STRUCT
 */
void _time_get_ticks
(
    register MQX_TICK_STRUCT_PTR tick_ptr
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

#if MQX_CHECK_ERRORS
    if ( tick_ptr == NULL ) {
        return;
    } /* Endif */
#endif

    _GET_KERNEL_DATA(kernel_data);

    _INT_DISABLE();

    *tick_ptr = kernel_data->TIME;

    /* The hardware clock keeps counting... */

    if (kernel_data->GET_HWTICKS) {
        /* The hardware clock may have counted passed it's reference
         * and have an interrupt pending.  Thus, HW_TICKS may exceed
         * kernel_data->HW_TICKS_PER_TICK and this tick_ptr may need
         * normalizing.  This is done in a moment.
         */
        tick_ptr->HW_TICKS = (*kernel_data->GET_HWTICKS)(kernel_data->GET_HWTICKS_PARAM);
    } /* Endif */

    PSP_ADD_TICKS(tick_ptr, &kernel_data->TIME_OFFSET, tick_ptr);

    /* The timer ISR may go off and increment kernel_data->TIME */
    _INT_ENABLE();

    /* The tick_ptr->HW_TICKS value might exceed the
     * kernel_data->HW_TICKS_PER_TICK and need to be
     * normalized for the PSP.
     */
    PSP_NORMALIZE_TICKS(tick_ptr);

} /* Endbody */
#endif /* MQX_HAS_TICK */

#if MQX_HAS_HW_TICKS
/*!
 * \brief Gets the number of hardware ticks per tick.
 *
 * \return Number of hardware ticks per tick.
 *
 * \see _time_set_hwticks_per_tick
 * \see _time_get_hwticks
 */
uint_32 _time_get_hwticks_per_tick
(
    void
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    return kernel_data->HW_TICKS_PER_TICK;

} /* Endbody */
#endif /* MQX_HAS_HW_TICKS */

/*!
 * \brief Initializes a tick time structure with the number of ticks.
 *
 * \param[in,out] tick_ptr Pointer to the tick time structure to initialize.
 * \param[in]     ticks    Number of ticks with which to initialize the structure.
 *
 * \return MQX_OK
 * \return MQX_INVALID_PARAMETER (Tick_ptr is NULL.)
 *
 * \see _time_ticks_to_xdate
 * \see _time_set
 * \see _time_set_ticks
 * \see MQX_TICK_STRUCT
 */
_mqx_uint _time_init_ticks
(
    MQX_TICK_STRUCT_PTR tick_ptr,
    _mqx_uint           ticks
)
{ /* Body */

#if MQX_CHECK_ERRORS
    if (tick_ptr == NULL) {
        return MQX_INVALID_PARAMETER;
    } /* Endif */
#endif

    if (ticks) {
        tick_ptr->HW_TICKS = 0;
        PSP_ADD_TICKS_TO_TICK_STRUCT(&_mqx_zero_tick_struct, ticks, tick_ptr);
    }
    else {
        *tick_ptr = _mqx_zero_tick_struct;
    } /* Endif */

    return MQX_OK;

} /* Endbody */

#if MQX_HAS_TICK
/*!
 * \brief The BSP periodic timer ISR calls the function when a periodic timer
 * interrupt occurs.
 *
 * The BSP installs an ISR for the periodic timer interrupt. The ISR calls
 * _time_notify_kernel(), which does the following:
 * \n - Increments kernel time.
 * \n - If the active task is a time slice task whose time slice has expired,
 * puts it at the end of the task's ready queue.
 * \n - If the timeout has expired for tasks on the timeout queue, puts them in
 * their ready queues.
 * \n If the BSP does not have periodic timer interrupts, MQX components that use
 * time will not operate.
 *
 * \warning See description.
 *
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_set
 * \see _time_set_ticks
 */
void _time_notify_kernel
(
    void
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;
    register TD_STRUCT_PTR td_ptr;
    register TD_STRUCT_PTR next_td_ptr;
    register _mqx_uint count;
    register _mqx_int result;

    _GET_KERNEL_DATA(kernel_data);

    /*
     * Update the current time.
     */
    PSP_INC_TICKS(&kernel_data->TIME);

    _INT_DISABLE();

    if (kernel_data->GET_HWTICKS) {
        /* The hardware clock may have counted passed it's reference
         * and have an interrupt pending.  Thus, HW_TICKS may exceed
         * kernel_data->HW_TICKS_PER_TICK and this tick_ptr may need
         * normalizing.  This is done in a moment.
         */
        kernel_data->TIME.HW_TICKS = (*kernel_data->GET_HWTICKS)(kernel_data->GET_HWTICKS_PARAM);
    } /* Endif */

    /* The tick_ptr->HW_TICKS value might exceed the
     * kernel_data->HW_TICKS_PER_TICK and need to be
     * normalized for the PSP.
     */
    PSP_NORMALIZE_TICKS(&kernel_data->TIME);

    /*
     * Check for tasks on the timeout queue, and wake the appropriate
     * ones up.  The timeout queue is a time-priority queue.
     */
    count = _QUEUE_GET_SIZE(&kernel_data->TIMEOUT_QUEUE);
    if (count) {
        td_ptr = (TD_STRUCT_PTR)((pointer) kernel_data->TIMEOUT_QUEUE.NEXT);
        ++count;
        while (--count) {
            next_td_ptr = td_ptr->TD_NEXT;
            result = PSP_CMP_TICKS(&kernel_data->TIME, &td_ptr->TIMEOUT);
            if (result >= 0) {
                --kernel_data->TIMEOUT_QUEUE.SIZE;
                _QUEUE_UNLINK(td_ptr);
                td_ptr->STATE &= ~IS_ON_TIMEOUT_Q;
                if (td_ptr->STATE & TD_IS_ON_AUX_QUEUE) {
                    td_ptr->STATE &= ~TD_IS_ON_AUX_QUEUE;
                    _QUEUE_REMOVE(td_ptr->INFO, &td_ptr->AUX_QUEUE);
                } /* Endif */
                _TASK_READY(td_ptr, kernel_data);
            }
            else {
                break; /* No more to do */
            } /* Endif */
            td_ptr = next_td_ptr;
        } /* Endwhile */
    } /* Endif */

#if MQX_HAS_TIME_SLICE
    /*
     * Check if the currently running task is a time slice task
     * and if its time has expired, put it at the end of its queue
     */
    td_ptr = kernel_data->ACTIVE_PTR;
    if ( td_ptr->FLAGS & MQX_TIME_SLICE_TASK ) {
        PSP_INC_TICKS(&td_ptr->CURRENT_TIME_SLICE);
        if (! (td_ptr->FLAGS & TASK_PREEMPTION_DISABLED) ) {
            result = PSP_CMP_TICKS(&td_ptr->CURRENT_TIME_SLICE, &td_ptr->TIME_SLICE);
            if ( result >= 0 ) {
                _QUEUE_UNLINK(td_ptr);
                _TASK_READY(td_ptr,kernel_data);
            } /* Endif */
        } /* Endif */
    } /* Endif */
#endif

    _INT_ENABLE();
#if MQX_USE_TIMER
    /* If the timer component needs servicing, call its ISR function */
    if (kernel_data->TIMER_COMPONENT_ISR != NULL) {
        (*kernel_data->TIMER_COMPONENT_ISR)();
    }/* Endif */
#endif

#if MQX_USE_LWTIMER
    /* If the lwtimer needs servicing, call its ISR function */
    if (kernel_data->LWTIMER_ISR != NULL) {
        (*kernel_data->LWTIMER_ISR)();
    }/* Endif */
#endif

} /* Endbody */

#endif /* MQX_HAS_TICK */

#if MQX_HAS_TICK
/*!
 * \brief Set the absolute time in tick time.
 *
 * The function affects _time_get_ticks(), but does not affect _time_get_elapsed_ticks().
 *
 * \param[in] ticks Pointer to the structure that contains the new time in tick time.
 *
 * \see _time_set
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_get_elapsed
 * \see _time_get_elapsed_ticks
 * \see _time_to_date
 * \see _time_init_ticks
 * \see _time_to_ticks
 * \see _time_from_date
 * \see MQX_TICK_STRUCT
 */
void _time_set_ticks
(
    register MQX_TICK_STRUCT_PTR ticks
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    _KLOGE2(KLOG_time_set_ticks, ticks);

    _INT_DISABLE();

    PSP_SUB_TICKS(ticks, &kernel_data->TIME, &kernel_data->TIME_OFFSET);

    _INT_ENABLE();

    _KLOGX1(KLOG_time_set_ticks);

} /* Endbody */
#endif /* MQX_HAS_TICK */

#if MQX_HAS_TICK
/*!
 * \brief Sets the periodic timer interrupt vector number that MQX uses.
 *
 * The BSP should call the function during initialization.
 *
 * \param[in] vector Periodic timer interrupt vector to use.
 *
 * \see _time_get
 * \see _time_get_ticks
 * \see _time_get_resolution
 * \see _time_set_resolution
 */
void _time_set_timer_vector
(
    _mqx_uint vector
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    kernel_data->SYSTEM_CLOCK_INT_NUMBER = vector;

} /* Endbody */
#endif /* MQX_HAS_TICK */

#if MQX_HAS_HW_TICKS
/*!
 * \brief Set the fields in kernel data to get the hardware ticks.
 *
 * \param[in] hwtick_function_ptr Pointer to the function that returns hw tick,
 * to be executed by the kernel.
 * \param[in] parameter           Parameter of the function that returns hw tick.
 *
 * \see _time_set_hwticks_per_tick
 * \see _time_get_hwticks
 */
void _time_set_hwtick_function
(
    MQX_GET_HWTICKS_FPTR hwtick_function_ptr,
    pointer              parameter
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    kernel_data->GET_HWTICKS = hwtick_function_ptr;
    kernel_data->GET_HWTICKS_PARAM = parameter;

} /* Endbody */
#endif /* MQX_HAS_HW_TICKS */

#if MQX_HAS_HW_TICKS
/*!
 * \brief Sets the number of hardware ticks per tick.
 *
 * \param[in] new_val New number of hardware ticks per tick.
 *
 * \see _time_get_hwticks_per_tick
 * \see _time_get_hwticks
 */
void _time_set_hwticks_per_tick
(
    uint_32 new_val
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    kernel_data->HW_TICKS_PER_TICK = new_val;

} /* Endbody */

#endif /* MQX_HAS_HW_TICKS */

#if MQX_HAS_TICK
/*!
 * \brief Gets the timer frequency (in ticks per second) that MQX uses.
 *
 * \warning If the timer frequency does not correspond with the interrupt period
 * that was programmed at the hardware level, some timing functions will give
 * incorrect results.
 *
 * \return Period of clock interrupt in ticks per second.
 *
 * \see _time_set_ticks_per_sec
 */
_mqx_uint _time_get_ticks_per_sec
(
    void
)
{ /* Body */
    register KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    return( kernel_data->TICKS_PER_SECOND );

} /* Endbody */
#endif /* MQX_HAS_TICK */


/* EOF */
