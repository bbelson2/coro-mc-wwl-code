/*
 * scheduling_events.cpp
 *
 *  Created on: 2 Dec 2018
 *      Author: Bruce Belson
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

#include "core_split_phase.h"
#include "core_events.h"
#include "core_crit_sec.h"

//#define FULL_TRACE

#include <vector>
#ifdef FULL_TRACE
#include "services.h"
#endif

namespace scp { namespace core {

 // TODO
 // 1) Separate queue for each event
 // 2) No dynamic containers - use a fixed size rotating buffer

class event_queue_t {
private:
	event_queue_t() { }
public:
	static event_queue_t& getInstance();
	void pushEvent(const split_phase_event_t& e);
	bool popEvent(event_id_t id, split_phase_event_t& e);
#ifdef USE_SIMULATOR
	bool popNextEvent(split_phase_event_t& e);
#endif
private:
	std::vector<split_phase_event_t> event_q_;
	mutex_t mutex_;
};

/* static */ event_queue_t& event_queue_t::getInstance() {
	static event_queue_t theInstance;
	return theInstance;
}

void event_queue_t::pushEvent(const split_phase_event_t& e) {
	thread_lock_t l(mutex_);
	event_q_.push_back(e);
}

bool event_queue_t::popEvent(event_id_t event_id, split_phase_event_t& e) {
	thread_lock_t l(mutex_);
	for (auto it = event_q_.begin(); it != event_q_.end(); it++) {
		if (it->event_id == event_id) {
			e = *it;
			event_q_.erase(it);
			return true;
		}
	}
	return false;
}

#ifdef USE_SIMULATOR
bool event_queue_t::popNextEvent(split_phase_event_t& e) {
	thread_lock_t l(mutex_);
	if (event_q_.size() != 0) {
		e = event_q_.front();
		event_q_.erase(event_q_.begin());
		return true;
	}
	return false;
}
#endif

void split_phase_event_t::reg() {
#ifdef FULL_TRACE
	trace("reg(%u)\r\n", this->event_id);
#endif
	event_queue_t::getInstance().pushEvent(*this);

} } // namespace scp::core
}

using namespace scp::core;

#ifdef USE_SIMULATOR

// Public facade

extern "C"
bool pop_async_event(split_phase_event_t& e) {
	return event_queue_t::getInstance().popNextEvent(e);
}

#else

extern "C"
void handle_async_event(event_id_t event_id) {
#ifdef FULL_TRACE
	trace("handle_async_event(%u)\r\n", event_id);
#endif
	split_phase_event_t e;
	if (event_queue_t::getInstance().popEvent(event_id, e)) {
#ifdef FULL_TRACE
		trace("Found callback\r\n");
#endif
		e.callback();
	}
	else {
#ifdef FULL_TRACE
		trace("No callback found\r\n");
#endif
	}
}

#endif



