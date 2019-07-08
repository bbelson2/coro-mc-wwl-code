/*
 * scheduling_scheduler.cpp
 *
 *  Created on: 29 Nov 2018
 *      Author: Bruce Belson
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 */

#include "core_scheduler.h"

/**
 * Task and scheduler implementation
 */

volatile unsigned long __idle_count = 0;

namespace scp { namespace core {

	resumable idleTaskFn() {
		co_await suspend_always{};
		for (;;) {
	#ifdef USE_SIMULATOR
	#else
	//		__asm ("wfi");
	#endif
			__idle_count++;
			co_await suspend_always{};
		}
	}

	auto idleTaskFn_ = idleTaskFn();
	task_t idleTask(TASK_ID_IDLE, task_t::task_state_t::Ready,
		idleTaskFn_._coroutine);

	void scheduler_t::registerIdleTask() {
		registerTask(&idleTask);
	}

} }
