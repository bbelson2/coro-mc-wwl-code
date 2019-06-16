/*
 * ptmain_cpp.cpp
 *
 *  Created on: 29 Jan 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#include "ptbuild.h" // macros based on build flags

#ifndef PTBUILD_NO_SCHEDULER

//#undef UNUSED_TASK_COUNT
//#define UNUSED_TASK_COUNT 1

#include "ptcore_scheduler.h"

#include "services.h"
#include "ptcore_split_phase.h"
//#include "ptcore_future.h"
#include "app_ids.h"
#include "pttask_test.h"
#include "services.h"

using namespace ptp::core;
using namespace ptp::task;

// ADC task, defined elsewhere
//#include "pt_task_adc.h"
//extern resumable adcTaskFn(uint8_t pin);
//extern void adcInit();

// Timer task, defined elsewhere
//#include "pt_task_timer.h"
//extern resumable timerTaskFn(uint8_t timer);

// I2C task, defined elsewhere
//#include "pt_task_i2c.h"
//extern resumable i2cTaskFn(uint8_t channel);

extern "C"
void pt_main_cpp()
{
	PRINT_STRING("pt_main_cpp (" PTBUILD_DESCRIPTION ")\r\n");
	// Instantiate and initialise the application-specific tasks
	TestTask testTaskOn(1, 1);
	TestTask testTaskOff(2, 0);

#if (UNUSED_TASK_COUNT > 0)
	PRINT_STRING("UNUSED_TASK_COUNT > 0\r\n");
	TestTask testTaskUnused0(3, 0);
#if (UNUSED_TASK_COUNT > 1)
	PRINT_STRING("UNUSED_TASK_COUNT > 1\r\n");
	TestTask testTaskUnused1(4, 1);
#endif
#endif

	#if UNUSED_TASK_COUNT > 0
	testTaskOn.setPriority(10);
	testTaskOff.setPriority(10);
	#endif

#ifdef INCLUDE_IDLE_TASK
	scheduler_t::getInstance().registerIdleTask();
#endif
#ifdef INCLUDE_TEST_TASK
	scheduler_t::getInstance().registerTask(&testTaskOn);
	scheduler_t::getInstance().registerTask(&testTaskOff);
#if (UNUSED_TASK_COUNT > 0)
	scheduler_t::getInstance().registerTask(&testTaskUnused0);
#if (UNUSED_TASK_COUNT > 1)
	scheduler_t::getInstance().registerTask(&testTaskUnused1);
#endif
#endif
#endif

	// Other setup
	//adcInit();

	// Run the program main thread
	scheduler_t::getInstance().run();
}

#endif

#ifdef PTBUILD_NO_SCHEDULER

#include "Protothread.h"
#include "Bit1.h"

extern unsigned long __pt_g_cycles;

class TestTask : public Protothread
{
public:
		TestTask(bool arg) : arg_(arg) {}
    virtual bool Run();
private:
    bool arg_;
};

bool TestTask::Run() {
	PT_BEGIN();

	for (;;) {
		Bit1_PutVal(!!arg_);
#ifndef PTBUILD_EXTERNAL_TIMER
		__pt_g_cycles++;
#endif
		PT_YIELD();
	}
	PT_END();
}

extern "C"
void pt_main_cpp()
{
	//PRINT_STRING("pt_main_cpp (" PTBUILD_DESCRIPTION ")\r\n");
	TestTask testTaskOn(true);
	TestTask testTaskOff(false);
	for (;;) {
		testTaskOn.Run();
		testTaskOff.Run();
	}
}

#endif
