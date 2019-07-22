/*
 * main_cpp.cpp
 *
 *  Created on: 2 Dec 2018
 *      Author: Bruce Belson
 */

#include "fsmbuild.h" // macros based on build flags

#ifndef FSMBUILD_NO_SCHEDULER

//#undef UNUSED_TASK_COUNT
//#define UNUSED_TASK_COUNT 2

#include "services.h"
#include "app_ids.h"

//#define TEST_BUILD

//#undef INCLUDE_IDLE_TASK
#undef INCLUDE_TIMER_TASK
//#undef INCLUDE_I2C_TASK
#undef INCLUDE_TEST_TASK
#undef INCLUDE_TEST_ALT_TASK
#undef INCLUDE_UNUSED_TASK

//////////////////////////////////////
// Dependencies - include minimum set
//////////////////////////////////////

#define INCLUDE_ANY_TASK (defined(INCLUDE_ADC_TASK) || defined(INCLUDE_TIMER_TASK) || defined(INCLUDE_I2C_TASK) || defined(INCLUDE_TEST_TASK) || defined(INCLUDE_TEST_ALT_TASK) || defined(INCLUDE_UNUSED_TASK))

#if (defined(INCLUDE_ANY_TASK))
#include "core_types.h"
#include "fsmcore_scheduler.h"
#endif

//////////////////////////////////////
// Each task
//////////////////////////////////////

#if (defined(INCLUDE_ANY_TASK))
using namespace fsm::core;
#endif

#ifdef INCLUDE_ADC_TASK
// ADC task, defined elsewhere
#include "task_adc.h"
#endif

#ifdef INCLUDE_TIMER_TASK
// Timer task, defined elsewhere
extern task_fn_t timerTaskFn;
#endif

#ifdef INCLUDE_I2C_TASK
// I2C task, defined elsewhere
#include "task_i2c.h"
#endif

#if (defined(INCLUDE_TEST_TASK) || defined(INCLUDE_UNUSED_TASK))
// Test task, defined elsewhere
extern task_fn_t testTaskFn;
// TODO - remove unused tasks from build
#endif

#ifdef INCLUDE_TEST_ALT_TASK
// Test task, defined elsewhere
extern task_fn_t testTaskAltFn;
// TODO - remove unused tasks from build
#endif


extern "C"
void main_cpp()
{
	PRINT_STRING("\r\nfsm_main_cpp (" FSMBUILD_DESCRIPTION ")\r\n");

#ifdef INCLUDE_ADC_TASK
	// Instantiate and initialise the application-specific tasks
	fsm::task::adc_task_info_t adc_task_info = { 0, 0 };
	task_t adcTask(TASK_ID_ADC, task_t::task_state_t::Ready, fsm::task::adcTaskFn, (task_data_t)&adc_task_info);
#endif

#ifdef INCLUDE_TIMER_TASK
	resumable timerTaskFn_ = timerTaskFn(0);
	task_t timerTask(TASK_ID_TIMER, task_t::task_state_t::Ready, timerTaskFn_._coroutine);
#endif

#ifdef INCLUDE_I2C_TASK
	fsm::task::i2c_task_info_t i2c_task_info = { 0, 0, 0, 0, 0 };
	task_t i2cTask(TASK_ID_I2C, task_t::task_state_t::Ready, fsm::task::i2cTaskFn, (task_data_t)&i2c_task_info);
#endif

#ifdef INCLUDE_TEST_TASK
	resumable testTaskFnOff_ = testTaskFn(0);
	resumable testTaskFnOn_ = testTaskFn(1);
	task_t testTaskOff(1, task_t::task_state_t::Ready, testTaskFnOff_._coroutine);
	task_t testTaskOn(2, task_t::task_state_t::Ready, testTaskFnOn_._coroutine);
#endif

#if (UNUSED_TASK_COUNT > 0)
	PRINT_STRING("UNUSED_TASK_COUNT > 0\r\n");
	resumable testTaskFnUnused_ = testTaskFn(0);
	task_t testTaskUnused0(3, task_t::task_state_t::Ready, testTaskFnUnused_._coroutine);
	testTaskUnused0.setPriority(0);
#if (UNUSED_TASK_COUNT > 1)
	task_t testTaskUnused1(4, task_t::task_state_t::Ready, testTaskFnUnused_._coroutine);
	testTaskUnused0.setPriority(0);
#endif
	testTaskOff.setPriority(10);
	testTaskOn.setPriority(10);
#endif

#ifdef INCLUDE_TEST_ALT_TASK
	resumable testTaskFnAlt_ = testTaskAltFn(0);
	task_t testTaskAlt(1, task_t::task_state_t::Ready, testTaskFnAlt_._coroutine);
#endif

	// Register tasks
#ifdef INCLUDE_IDLE_TASK
	scheduler_t::getInstance().registerIdleTask();
#endif
#ifdef INCLUDE_ADC_TASK
	scheduler_t::getInstance().registerTask(&adcTask);
#endif
#ifdef INCLUDE_TIMER_TASK
	scheduler_t::getInstance().registerTask(&timerTask);
#endif
#ifdef INCLUDE_I2C_TASK
	scheduler_t::getInstance().registerTask(&i2cTask);
#endif
#ifdef INCLUDE_TEST_TASK
	scheduler_t::getInstance().registerTask(&testTaskOn);
	scheduler_t::getInstance().registerTask(&testTaskOff);
#endif
#ifdef INCLUDE_TEST_ALT_TASK
	scheduler_t::getInstance().registerTask(&testTaskAlt);
#endif
#if (UNUSED_TASK_COUNT > 0)
	scheduler_t::getInstance().registerTask(&testTaskUnused0);
#if (UNUSED_TASK_COUNT > 1)
	scheduler_t::getInstance().registerTask(&testTaskUnused1);
#endif
#endif

	// Other setup
#ifdef INCLUDE_ADC_TASK
	//adcInit();
#endif

	// Run the program main thread
	scheduler_t::getInstance().run();
}

#endif // FSMBUILD_NO_SCHEDULER

