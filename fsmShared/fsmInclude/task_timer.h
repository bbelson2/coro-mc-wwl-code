/*
 * task_timer.h
 *
 *  Created on: 23 Jul 2019
 *      Author: Bruce Belson
 */

#ifndef FSMSHARED_FSMINCLUDE_TASK_TIMER_H_
#define FSMSHARED_FSMINCLUDE_TASK_TIMER_H_


#include "fsmcore_scheduler.h"

namespace fsm { namespace task {

	void timerTaskFn(fsm::core::task_t*);

} } // namespace fsm::task

#endif /* FSMSHARED_FSMINCLUDE_TASK_TIMER_H_ */
