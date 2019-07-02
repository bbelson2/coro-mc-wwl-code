/*
 * main_cpp.cpp
 *
 *  Created on: 2 Dec 2018
 *      Author: Bruce Belson
 */

//#include "cobuild.h" // macros based on build flags


//#undef UNUSED_TASK_COUNT
//#define UNUSED_TASK_COUNT 2

#include "cobuild.h"
#include "services.h"
#include "core_resumable.h"
#include "core_scheduler.h"
#include "core_future2.h"
#include "app_ids.h"
#include "api2_adc.h"

//#define TEST_BUILD

//////////////////////////////////////
// Dependencies - include minimum set
//////////////////////////////////////


//////////////////////////////////////
// Each task
//////////////////////////////////////

#define INCLUDES_ANY_TASK \
		defined(INCLUDE_ADC_TASK) \
		|| defined(INCLUDE_TIMER_TASK) \
		|| defined(INCLUDE_I2C_TASK) \
		|| defined(INCLUDE_TEST_TASK) \
		|| defined(INCLUDE_TEST_ALT_TASK)
#if (INCLUDES_ANY_TASK)
using namespace scp::core;
#endif

extern "C"
void main_cpp()
{
	PRINT_STRING("\r\n" COBUILD_DESCRIPTION "\r\n");

	// Register tasks
#ifdef INCLUDE_IDLE_TASK
	scheduler_t::getInstance().registerIdleTask();
#endif

	// Other setup
	// Run the program main thread
	scheduler_t::getInstance().run();
}


