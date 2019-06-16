/*
 * ptcore_scheduler.cpp
 *
 *  Created on: 29 Jan 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

/**
 * Task and scheduler implementation
 */


#ifndef CONFIG_LLVM_NS

#include "ptcore_scheduler.h"

extern volatile unsigned long __pt_idle_count;

namespace ptp { namespace core {

IdleTask theIdleTask;

void scheduler_t::registerIdleTask() {
	registerTask(&theIdleTask);
}


bool IdleTask::Run() {
	PT_BEGIN();

	while (true) {
		__pt_idle_count++;
		PT_YIELD();
	}

	PT_END();
}

} }

#endif
