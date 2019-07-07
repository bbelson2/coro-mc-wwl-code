/*
 * api_timer.h
 *
 *  Created on: 19 Dec 2018
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#ifndef SHARED_INCLUDE_API_TIMER_H_
#define SHARED_INCLUDE_API_TIMER_H_

#ifdef USE_SIMULATOR
// Simulator utilities
#include "core_simulator.h"
// General purpose PE polyfill
#include "pe_polyfill.h"
// Simulated calls
#else
extern "C" {
#include "TU1.h"
#include "FC1.h"
}
#endif

#include "core_future.h"

typedef unsigned long tick_count_t;
extern "C" void handle_tick_event();

namespace scp { namespace drivers {

scp::core::future_t<void> wait_on_timer(uint8_t channelId);
scp::core::future_t<void> wait_on_ticks(tick_count_t ticks);

} } // namespace scp::drivers


#endif /* SHARED_INCLUDE_API_TIMER_H_ */
