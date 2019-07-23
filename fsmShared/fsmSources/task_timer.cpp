/*
 * task_timer.cpp
 *
 *  Created on: 23 Jul 2019
 *      Author: Bruce Belson
 */

#include "core_crit_sec.h"
#include "task_timer.h"
#include "fsmapi_timer.h"
#include "TU1.h"
#include "FC1.h"
#include <string.h>
#include "services.h"

#ifndef EVENT_MAX_COUNT
#define EVENT_MAX_COUNT 64
#endif

namespace fsm { namespace api {

	struct timer_item_t {
		timer_fn_t fn;
		tick_count_t ticks_wait;
		tick_count_t ticks_start;
		task_id_t task_id;
	};

	struct timer_item_list {
		timer_item_list() : global_ticks_(0), item_count_(0), task_(nullptr) {
			memset(timer_items_, 0, sizeof(timer_items_));
		}
		byte push(tick_count_t ticks, timer_fn_t fn, task_id_t task_id) {
			scp::core::thread_lock_t l(mutex_);
			if (item_count_ >= EVENT_MAX_COUNT) {
				return ERR_OVERFLOW;
			}
			timer_item_t* p = timer_items_ + item_count_;
			p->fn = fn;
			p->ticks_wait = ticks;
			p->ticks_start = (tick_count_t)-1;
			p->task_id = task_id;
			item_count_++;
			//trace("timer_item_list pushed from task %u (count=%u)\r\n", task_id, item_count_);
			return ERR_OK;
		}
		void handle_event() {
			scp::core::thread_lock_t l(mutex_);
			global_ticks_++;
			for (size_t i = 0; i < item_count_; i++) {
				if (timer_items_[i].ticks_start == (tick_count_t)-1) {
					timer_items_[i].ticks_start = global_ticks_;
				}
			}
			if ((task_ != nullptr) && (item_count_ > 0)) {
				task_->unblock();
			}
		}
		bool pop(timer_fn_t& fn, task_id_t& task_id) {
			scp::core::thread_lock_t l(mutex_);
			for (size_t i = 0; i < item_count_; i++) {
				timer_item_t& t = timer_items_[i];
				if (t.ticks_start + t.ticks_wait <= global_ticks_) {
					fn = t.fn;
					task_id = t.task_id;
					for (size_t j = i + 1; j < item_count_; j++) {
						timer_items_[j - 1] = timer_items_[j];
					}
					item_count_--;
					return true;
				}
			}
			return false;
		}
		void setTask(fsm::core::task_t* task) {
			task_ = task;
		}
		void pop_and_run() {
			timer_fn_t fn;
			task_id_t task_id;
			while (pop(fn, task_id)) {
				//trace("timerTaskFn popped for task %d\r\n", task_id);
				(*fn)();
			}
		}
	private:
		tick_count_t global_ticks_;
		timer_item_t timer_items_[EVENT_MAX_COUNT];
		uint16_t item_count_;
		scp::core::mutex_t mutex_;
		fsm::core::task_t* task_;
	};

	static timer_item_list theTimerList;

	byte onTimer(tick_count_t ticks, timer_fn_t fn) {
		auto task = fsm::core::scheduler_t::getInstance().getRunningTask();
		return theTimerList.push(ticks, fn, task.getId());
	}

} }

void fsm::task::timerTaskFn(fsm::core::task_t* task) {
	//trace("timerTaskFn begins\r\n");
	fsm::api::theTimerList.setTask(task); // Unnecessary after the first time
	task->block(); // until timer unblocks us
}

extern "C"
void handle_tick_event() {
	fsm::api::theTimerList.handle_event();
	fsm::api::theTimerList.pop_and_run();
}

