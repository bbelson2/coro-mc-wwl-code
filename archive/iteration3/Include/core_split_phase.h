/*
 * core_split_phase.h
 *
 *  Created on: 29 Nov 2018
 *      Author: Bruce Belson
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 */

#ifndef SOURCES_SCHEDULING_SPLIT_PHASE_H_
#define SOURCES_SCHEDULING_SPLIT_PHASE_H_

// TODO - support move semantics, to improve performance when pushing
// and popping

#pragma once

#include <functional>
#include "core_events.h"

namespace scp { namespace core {

struct split_phase_event_t {
	event_id_t event_id;
	std::function<void(void)> callback;
	split_phase_event_t(event_id_t id, std::function<void(void)> cb) {
		event_id = id;
		callback = cb;
	}
	split_phase_event_t() {
		event_id = 0;
		callback = nullptr;
	}
	
	static void reg(event_id_t id,
			std::function<void(void)> immediate,
			std::function<void(void)> deferred) {
		split_phase_event_t(id, deferred).reg();
		immediate();
	}
	void reg();
};

} } // namespace scp::core

#endif /* SOURCES_SCHEDULING_SPLIT_PHASE_H_ */

