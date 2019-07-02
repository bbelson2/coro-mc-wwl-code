/*
 * core_future2.h
 *
 *  Created on: 15 May 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#ifndef SHARED_INCLUDE_CORE_FUTURE2_H_
#define SHARED_INCLUDE_CORE_FUTURE2_H_

#include "core_scheduler.h"

/*
 * This implementation of futures is designed for a no-heap,
 * no-exception environment, where the same promise produces
 * an endless stream of futures, one at a time.
 */

namespace scp { namespace core {

	struct persistent_awaitable_state_base {
		std::experimental::coroutine_handle<> _coro;
		bool _ready = false;
		bool _future_acquired = false;
		task_id_t _taskid = scheduler_t::bad_task_id();

		persistent_awaitable_state_base() {}
		persistent_awaitable_state_base(persistent_awaitable_state_base&&) = delete;
		persistent_awaitable_state_base(const persistent_awaitable_state_base&) = delete;

		void set_coroutine_callback(std::experimental::coroutine_handle<> cb)
		{
			#ifdef assert
			// Test to make sure nothing else is waiting on this future.
			//assert(((cb == nullptr) || (_coro == nullptr)) && "This future is already being awaited.");
			assert(((!cb) || (!_coro)) && "This future is already being awaited.");
			#endif
			_coro = cb;
		}
		void after_set_value()
		{
			_ready = true;
			auto coro = _coro;
			_coro = nullptr;
			if (coro) {
				// Push coro on to queue for scheduler, where it will be matched against a task
				// and then resumed.
				scp::core::scheduler_t::getInstance().unblockTask(_taskid, coro);
			}
		}
		bool ready() const
		{
			return _ready;
		}

		void reset()
		{
			_coro = nullptr;
			_ready = false;
			_future_acquired = false;
		}

		bool await_ready() const
		{
			return _ready;
		}

		void await_suspend(std::experimental::coroutine_handle<> resume_cb)
		{
			task_t::blockRunningTask();
			set_coroutine_callback(resume_cb);
		}
	};

	template<typename T>
	struct persistent_awaitable_state_t : public persistent_awaitable_state_base {
		T _value;

		void set_value(const T& t) {
			_value = t;
			persistent_awaitable_state_base::after_set_value();
		}
		void finalize_value() {
			persistent_awaitable_state_base::after_set_value();
		}
		auto get_value() {
			#ifdef assert
			assert(_ready && "This future is not ready");
			#endif
			return _value;
		}
		void reset() {
			persistent_awaitable_state_base::reset();
			_value = T();
		}
		// make this directly awaitable
		auto await_resume() const
		{
			return _value;
		}
	};

	// specialization of persistent_awaitable_state_t<void>
	template <>
	struct persistent_awaitable_state_t<void> : public persistent_awaitable_state_base
	{
		void get_value() const {
			#ifdef assert
			assert(_ready && "This future is not ready");
			#endif
		}

		// make this directly awaitable
		void await_resume() const {
		}
	};

	template<typename T>
	struct persistent_promise_t;

	template<typename T>
	struct future2_t {
		typedef T type;
		typedef persistent_awaitable_state_t<T> state_t;
		state_t* _state;

		future2_t(state_t& state) : _state(&state) {
			#ifdef assert
			assert((!_state->_future_acquired) && "This state already has a future2");
			#endif
			_state->_taskid = task_t::getRunningTaskId();
			_state->_future_acquired = true;
		}

		// movable, but not copyable
		future2_t(const future2_t&) = delete;
		future2_t& operator=(const future2_t&) = delete;
		future2_t(future2_t&& f) = default;
		future2_t& operator=(future2_t&&) = default;

		auto await_resume() const
		{
			return _state->get_value();
		}

		bool await_ready() const
		{
			return _state->_ready;
		}

		void await_suspend(std::experimental::coroutine_handle<> resume_cb)
		{
			task_t::blockRunningTask();
			_state->set_coroutine_callback(resume_cb);
		}

		bool ready() const
		{
			return _state->_ready;
		}

		auto get_value() const
		{
			return _state->get_value();
		}
	};

	template<typename T>
	struct persistent_promise_t {
		typedef future2_t<T> future_type;
		typedef persistent_awaitable_state_t<T> state_type;
		state_type _state;

		// movable not copyable
		template <typename ...Args>
		persistent_promise_t(Args&&... args) : _state(std::forward<Args>(args)...)
		{
		}
		persistent_promise_t(const persistent_promise_t&) = delete;
		persistent_promise_t(persistent_promise_t&&) = default;

		future2_type get() {
#ifdef assert
			assert (!_state._future_acquired && "A future on this promise is already in place");
#endif
			return future_type(_state);
		}

		void put(const T& t) {
			_state.set_value(t);
		}
	};

}}


#endif /* SHARED_INCLUDE_CORE_FUTURE2_H_ */
