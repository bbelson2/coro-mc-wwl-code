/*
 * fsmapi_timer.h
 *
 *  Created on: 23 Jul 2019
 *      Author: Bruce Belson
 */

#ifndef FSMSHARED_FSMINCLUDE_FSMAPI_TIMER_H_
#define FSMSHARED_FSMINCLUDE_FSMAPI_TIMER_H_

typedef unsigned long tick_count_t;

namespace fsm { namespace api {
	typedef void (*timer_fn_t)();
	byte onTimer(tick_count_t ticks, timer_fn_t fn);
} }

#endif /* FSMSHARED_FSMINCLUDE_FSMAPI_TIMER_H_ */
