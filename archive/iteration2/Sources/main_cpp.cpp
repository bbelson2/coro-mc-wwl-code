/*
 * main_cpp.cpp
 *
 *  Created on: 2 Dec 2018
 *      Author: Bruce Belson
 */

#include "cobuild.h" // macros based on build flags

#ifndef COBUILD_NO_SCHEDULER

//#undef UNUSED_TASK_COUNT
//#define UNUSED_TASK_COUNT 2

#include "services.h"
#include "core_resumable.h"
#include "app_ids.h"

//#define TEST_BUILD

//////////////////////////////////////
// Dependencies - include minimum set
//////////////////////////////////////

#if (defined(INCLUDE_ADC_TASK) || defined(INCLUDE_TIMER_TASK) || defined(INCLUDE_I2C_TASK))
#include "core_split_phase.h"
#include "core_future.h"
#endif

#if (defined(INCLUDE_TEST_TASK) || defined(INCLUDE_TEST_ALT_TASK) || defined(INCLUDE_UNUSED_TASK))
#include "core_types.h"
#include "core_scheduler.h"
#endif

//////////////////////////////////////
// Each task
//////////////////////////////////////

#if (defined(INCLUDE_ADC_TASK) || defined(INCLUDE_TIMER_TASK) || defined(INCLUDE_I2C_TASK) || defined(INCLUDE_TEST_TASK) || defined(INCLUDE_TEST_ALT_TASK))
using namespace scp::core;
#endif

#ifdef INCLUDE_ADC_TASK
// ADC task, defined elsewhere
extern resumable adcTaskFn(uint8_t pin);
extern void adcInit();
#endif

#ifdef INCLUDE_TIMER_TASK
// Timer task, defined elsewhere
extern resumable timerTaskFn(uint8_t timer);
#endif

#ifdef INCLUDE_I2C_TASK
// I2C task, defined elsewhere
extern resumable i2cTaskFn(uint8_t channel);
#endif

#if (defined(INCLUDE_TEST_TASK) || defined(INCLUDE_UNUSED_TASK))
// Test task, defined elsewhere
extern resumable testTaskFn(uint8_t value);
// TODO - remove unused tasks from build
#endif

#ifdef INCLUDE_TEST_ALT_TASK
// Test task, defined elsewhere
extern resumable testTaskAltFn(uint8_t value);
// TODO - remove unused tasks from build
#endif


extern "C"
void main_cpp()
{
	PRINT_STRING("\r\nco_main_cpp (" COBUILD_DESCRIPTION ")\r\n");

#ifdef INCLUDE_ADC_TASK
	// Instantiate and initialise the application-specific tasks
	resumable adcTaskFn_ = adcTaskFn(12);
	task_t adcTask(TASK_ID_ADC, task_t::task_state_t::Ready, adcTaskFn_._coroutine);
#endif

#ifdef INCLUDE_TIMER_TASK
	resumable timerTaskFn_ = timerTaskFn(0);
	task_t timerTask(TASK_ID_TIMER, task_t::task_state_t::Ready, timerTaskFn_._coroutine);
#endif

#ifdef INCLUDE_I2C_TASK
	resumable i2cTaskFn_ = i2cTaskFn(0);
	task_t i2cTask(TASK_ID_I2C, task_t::task_state_t::Ready, i2cTaskFn_._coroutine);
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
	adcInit();
#endif

	// Run the program main thread
	scheduler_t::getInstance().run();
}

#endif // COBUILD_NO_SCHEDULER

#ifdef COBUILD_NO_SCHEDULER

#include "services.h"
#include "core_resumable.h"
#include "core_types.h"
#include "core_scheduler.h"
using namespace scp::core;

// Test task, defined elsewhere
extern resumable testTaskFn(uint8_t value);

extern "C"
void main_cpp()
{
	PRINT_STRING("co_main_cpp (" COBUILD_DESCRIPTION ")\r\n");
	resumable testTaskFnOff_ = testTaskFn(0);
	resumable testTaskFnOn_ = testTaskFn(1);
	task_t testTaskOff(1, task_t::task_state_t::Ready, testTaskFnOff_._coroutine);
	task_t testTaskOn(2, task_t::task_state_t::Ready, testTaskFnOn_._coroutine);
	for (;;) {
		testTaskOn.resume();
		testTaskOff.resume();
	}
}
#endif // COBUILD_NO_SCHEDULER
