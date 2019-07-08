/*
 * timertask.cpp
 *
 *  Created on: 19 Dec 2018
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#include "api_timer.h"
#include "core_resumable.h"
#include "Bit1.h"

/***************************************************************************/
/* Timer task                                                                */
/***************************************************************************/

#define TIMER_VERSION 0

#if TIMER_VERSION == 0

using namespace scp::core;
using namespace scp::drivers;

volatile unsigned long __timer_count = 0;

resumable timerTaskFn(uint8_t pin) {
	unsigned long count = 0;
	co_await suspend_always{};

	for (;;) {
		co_await wait_on_timer(0);

		//trace("Timer: %lu\r\n", ++count);
		__timer_count = ++count;
		//Bit1_PutVal(count % 2 == 0);
	}
}

#endif // TIMER_VERSION == 0

