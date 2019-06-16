/*
 * pttask_test.cpp
 *
 *  Created on: 13 Mar 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#ifndef CONFIG_LLVM_NS

#include "ptcore_scheduler.h"
#include "Bit1.h"
#include "pttask_test.h"


/***************************************************************************/
/* Test task                                                                */
/***************************************************************************/

#define TEST_VERSION 0

#if TEST_VERSION == 0

using namespace ptp::core;

#if !defined(PTBUILD_EXTERNAL_TIMER) || defined(SET_GLOBAL_COUNTER)
extern unsigned long __pt_g_cycles;
#endif

namespace ptp { namespace task {
	bool TestTask::Run() {
		PT_BEGIN();

		for (;;) {
			Bit1_PutVal(!!arg_);
#if !defined(PTBUILD_EXTERNAL_TIMER) || defined(SET_GLOBAL_COUNTER)
			__pt_g_cycles++;
#endif
			PT_YIELD();
		}
		PT_END();
	}
} }

#endif // TEST_VERSION == 0

#endif
