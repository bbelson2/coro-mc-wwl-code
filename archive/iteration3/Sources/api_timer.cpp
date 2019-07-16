/*
 * timer_api.cpp
 *
 *  Created on: 19 Dec 2018
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#include "api_timer.h"
#include "core_split_phase.h"

#include "app_ids.h"

/*
 * Timer component promises
 */

namespace scp { namespace drivers {

using namespace scp::core;

future_t<void> wait_on_timer(uint8_t channelId) {
	//auto w = findAdc(channelId);
	promise_t<void> p;
	split_phase_event_t(EVENT_ID_TIMER, [s = p._state]() {
		s->set_value();
	}).reg();
	return p.get_future();
}

#ifdef USE_SIMULATOR
void sim_start_timer();
void sim_trigger_tick_event();
#endif

#include "core_crit_sec.h"

struct timer_item_t {
	promise_t<void>::state_ptr_type state;
	tick_count_t ticks_wait;
	tick_count_t ticks_start;
};

class timer_item_list {
public:
	timer_item_list() : global_ticks_(0) {}
	void push(timer_item_t&& t);
	void handle_event();
private:
	tick_count_t global_ticks_;
	std::vector<timer_item_t> timer_items_;
	scp::core::mutex_t mutex_;
};

timer_item_list theList;

void timer_item_list::handle_event() {
	thread_lock_t l(mutex_);
	global_ticks_++;
	for (size_t i = 0; i < timer_items_.size(); i++) {
		timer_item_t& item = timer_items_[i];
		if (item.ticks_start == (tick_count_t)-1) {
			item.ticks_start = global_ticks_;
		}
		if ((item.ticks_start + item.ticks_wait) <= global_ticks_) {
			auto s = item.state;
			timer_items_.erase(timer_items_.begin() + i);
			i--;
			s->set_value();
		}
	}
}

void timer_item_list::push(timer_item_t&& t) {
	thread_lock_t l(mutex_);
	timer_items_.push_back(t);
}

// Public tick API

future_t<void> wait_on_ticks(tick_count_t ticks) {
#ifdef USE_SIMULATOR
	sim_start_timer();
#endif
	promise_t<void> p;
	theList.push(timer_item_t {
		p._state,
		ticks,
		(tick_count_t)-1
	});
	return p.get_future();
}

extern "C" void handle_tick_event() {
	theList.handle_event();
}

#ifdef USE_SIMULATOR
#include <thread>
bool sim_timer_started = false;
std::thread sim_timer_thread;
void sim_timer_thread_fn();
void sim_start_timer() {
	if (!sim_timer_started) {
		sim_timer_thread = std::thread(sim_timer_thread_fn);
		sim_timer_started = true;
	}
}
void sim_timer_thread_fn() {
	while (scheduling::scheduler_t::getInstance().shouldRun()) {
		Sleep(50);
		sim_trigger_tick_event();
	}
}
void sim_trigger_tick_event() {
	if (scheduling::scheduler_t::getInstance().shouldRun()) {
		handle_tick_event();
	}
}
#endif

} } // namespace scp::drivers
