/*
 * task_idle.cpp
 *
 *  Created on: 22 Jul 2019
 *      Author: Bruce Belson
 */

#include "fsmcore_scheduler.h"

/**
 * Task and scheduler implementation
 */

volatile unsigned long __idle_count = 0;

namespace fsm { namespace core {

	void idleTaskFn(task_t*) {
	#ifdef USE_SIMULATOR
	#else
	//	__asm ("wfi");
	#endif
		__idle_count++;
	}

	task_t idleTask(TASK_ID_IDLE, task_t::task_state_t::Ready,
		idleTaskFn, nullptr);

	void scheduler_t::registerIdleTask() {
		registerTask(&idleTask);
	}

} }



