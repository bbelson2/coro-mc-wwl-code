/*
 * task_test.cpp
 *
 *  Created on: 20 Feb 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#include "cobuild.h" // macros based on build flags

#include "core_resumable.h"
#include "Bit1.h"

#ifdef TEST_SIZEOF
int voidfn() { return 321; }
typedef int (*pfn_t)(void);
pfn_t  pfn = voidfn;

extern "C"
void offline_test() {
	auto l = [](){ return 123; };
	auto sizeof_l = sizeof(l);
	auto sizeof_v = sizeof(pfn);
	trace("sizeof_l=%d, sizeof_v=%d\r\n", sizeof_l, sizeof_v);
	trace("l()=%d, pfn()=%d\r\n", l(), pfn());
}
#endif

/***************************************************************************/
/* Timer task                                                                */
/***************************************************************************/

#define TEST_VERSION 0

#if TEST_VERSION == 0

using namespace scp::core;

#if !defined(COBUILD_EXTERNAL_TIMER) || defined(SET_GLOBAL_COUNTER)
extern unsigned long __co_g_cycles;
#endif

#ifdef INCLUDE_TEST_TASK
resumable testTaskFn(uint8_t value) {
	bool bitValue = !!value;
	co_await suspend_always{};

	for (;;) {

		Bit1_PutVal(bitValue);
#if !defined(COBUILD_EXTERNAL_TIMER) || defined(SET_GLOBAL_COUNTER)
		__co_g_cycles++;
#endif
		co_await suspend_always{};
	}
}
#endif // INCLUDE_TEST_TASK

#ifdef INCLUDE_TEST_ALT_TASK
resumable testTaskAltFn(uint8_t value) {
	bool bitValue = !!value;
	co_await suspend_always{};

	for (;;) {

		Bit1_PutVal(bitValue);
		bitValue = !bitValue;
#ifndef COBUILD_EXTERNAL_TIMER
		__co_g_cycles++;
#endif
		co_await suspend_always{};
	}
}
#endif // INCLUDE_TEST_ALT_TASK

#endif // TEST_VERSION == 0
