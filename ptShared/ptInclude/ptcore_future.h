/*
 * core_future.h
 *
 *  Created on: 29 Nov 2018
 *      Author: Bruce Belson
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 */

#ifndef SOURCES_SCHEDULING_FUTURE_H_
#define SOURCES_SCHEDULING_FUTURE_H_

#pragma once

#ifdef USE_SIMULATOR
#include <atomic>
#include <assert.h>
#endif
#include "ptcore_scheduler.h"
#include <vector>

/**
 * Futures, promises and shared data
 *
 * This implementation is based on the the libuv adapter written by
 * Jim Springfield (https://github.com/jimspr/awaituv).
 *
 * 1) The code specific to libuv is excluded.
 *
 * 2) The scheduler is used for thread control.
 *    i)   Awaitable state includes the task id of the task that was active
 *         when the promise was created.
 *    ii)  When a future (or its underlying data) is awaited, the
 *    	   originating task is blocked.
 *    iii) Resumed tasks are posted to the scheduler by marking the owning
 *         task as unblocked, rather than by simply invoking the blocked
 *         coroutine.
 *
 * 3) std::atomic is excluded in the MCU version because it breaks the clang
 *    5.0 compiler.
 *
 * 4) TODO. Memory allocation is overideable, so that dynamic (heap) allocations
 *    can be avoided.
 *
 * 5) TODO. Exceptions are not directly used. Instead, a platform-specific
 *    equivalent is invoked.
 */

namespace ptp { namespace core {

	enum struct future_error
	{
		not_ready,			// get_value called when value not available
		already_acquired	// attempt to get another future
	};

	struct future_exception : std::exception
	{
		future_error _error;
		future_exception(future_error fe) : _error(fe)
		{
		}
	};

	struct awaitable_state_base
	{
		//std::function<void(void)> _coro;
		std::experimental::coroutine_handle<> _coro;
		bool _ready = false;
		bool _future_acquired = false;
		task_id_t _taskid = scheduler_t::bad_task_id();

		awaitable_state_base() : _taskid(task_t::getRunningTaskId()) { }
		awaitable_state_base(awaitable_state_base&&) = delete;
		awaitable_state_base(const awaitable_state_base&) = delete;

		//void set_coroutine_callback(std::function<void(void)> cb)
		void set_coroutine_callback(std::experimental::coroutine_handle<> cb)
		{
#ifdef assert
			// Test to make sure nothing else is waiting on this future.
			//assert(((cb == nullptr) || (_coro == nullptr)) && "This future is already being awaited.");
			assert(((!cb) || (!_coro)) && "This future is already being awaited.");
#endif
			_coro = cb;
		}

		void set_value()
		{
			// Set all members first as calling coroutine may reset stuff here.
			_ready = true;
			auto coro = _coro;
			_coro = nullptr;
			if (coro) {
				//coro();
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

		// functions that make this directly awaitable
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

	template <typename T>
	struct awaitable_state : public awaitable_state_base
	{
		T _value;

		void set_value(const T& t)
		{
			_value = t;
			awaitable_state_base::set_value();
		}

		void finalize_value()
		{
			awaitable_state_base::set_value();
		}

		auto get_value()
		{
			if (!_ready)
				throw future_exception{ future_error::not_ready };
			return _value;
		}

		void reset()
		{
			awaitable_state_base::reset();
			_value = T{};
		}

		// make this directly awaitable
		auto await_resume() const
		{
			return _value;
		}
	};

	// specialization of awaitable_state<void>
	template <>
	struct awaitable_state<void> : public awaitable_state_base
	{
		void get_value() const
		{
			if (!_ready)
				throw future_exception{ future_error::not_ready };
		}

		// make this directly awaitable
		void await_resume() const
		{
		}
	};

	// We need to be able to manage reference count of the state object in the callback.
	template <typename awaitable_state_t>
	struct counted_awaitable_state : public awaitable_state_t
	{
#ifdef USE_SIMULATOR
		std::atomic<int> _count{ 0 }; // tracks reference count of state object
#else
		volatile int _count;
#endif

		template <typename ...Args>
		counted_awaitable_state(Args&&... args) : _count{ 0 }, awaitable_state_t(std::forward<Args>(args)...)
		{
		}
		counted_awaitable_state(const counted_awaitable_state&) = delete;
		counted_awaitable_state(counted_awaitable_state&&) = delete;

		counted_awaitable_state* lock()
		{
			++_count;
			return this;
		}

		void unlock()
		{
			if (--_count == 0)
				delete this;
		}
	protected:
		~counted_awaitable_state() {}
	};

	// counted_ptr is similar to shared_ptr but allows explicit control
	//
	template <typename T>
	struct counted_ptr
	{
		counted_ptr() = default;
		counted_ptr(const counted_ptr& cp) : _p(cp._p)
		{
			_lock();
		}

		counted_ptr(counted_awaitable_state<T>* p) : _p(p)
		{
			_lock();
		}

		counted_ptr(counted_ptr&& cp)
		{
			std::swap(_p, cp._p);
		}

		counted_ptr& operator=(const counted_ptr& cp)
		{
			if (&cp != this)
			{
				_unlock();
				_lock(cp._p);
			}
			return *this;
		}

		counted_ptr& operator=(counted_ptr&& cp)
		{
			if (&cp != this)
				std::swap(_p, cp._p);
			return *this;
		}

		~counted_ptr()
		{
			_unlock();
		}

		counted_awaitable_state<T>* operator->() const
		{
			return _p;
		}

		counted_awaitable_state<T>* get() const
		{
			return _p;
		}

	protected:
		void _unlock()
		{
			if (_p != nullptr)
			{
				auto t = _p;
				_p = nullptr;
				t->unlock();
			}
		}
		void _lock(counted_awaitable_state<T>* p)
		{
			if (p != nullptr)
				p->lock();
			_p = p;
		}
		void _lock()
		{
			if (_p != nullptr)
				_p->lock();
		}
		counted_awaitable_state<T>* _p = nullptr;
	};

	template <typename T, typename... Args>
	counted_ptr<T> make_counted(Args&&... args)
	{
		return new counted_awaitable_state<T>{ std::forward<Args>(args)... };
	}

	// The awaitable_state class is good enough for most cases, however there are some cases
	// where a libuv callback returns more than one "value".  In that case, the function can
	// define its own state type that holds more information.
	template <typename T, typename state_t = awaitable_state<T>>
	struct promise_t;

	template <typename T, typename state_t = awaitable_state<T>>
	struct future_t
	{
		typedef T type;
		typedef state_t state_type;
		typedef promise_t<T, state_t> promise_type;
		counted_ptr<state_t> _state;

		future_t(const counted_ptr<state_t>& state) : _state(state)
		{
			_state->_taskid = task_t::getRunningTaskId();
			_state->_future_acquired = true;
		}

		// movable, but not copyable
		future_t(const future_t&) = delete;
		future_t& operator=(const future_t&) = delete;
		future_t(future_t&& f) = default;
		future_t& operator=(future_t&&) = default;

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

	template <typename T, typename state_t>
	struct promise_t
	{
		typedef future_t<T, state_t> future_type;
		typedef counted_awaitable_state<state_t> state_type;
		typedef counted_ptr<state_t> state_ptr_type;
		state_ptr_type _state;

		// movable not copyable
		template <typename ...Args>
		promise_t(Args&&... args) : _state(make_counted<state_t>(std::forward<Args>(args)...))
		{
		}
		promise_t(const promise_t&) = delete;
		promise_t(promise_t&&) = default;

		future_type get_future()
		{
			if (_state->_future_acquired)
				throw future_exception{ future_error::already_acquired };
			return future_type(_state);
		}

		// Most functions don't need this but timers and reads from streams
		// cause multiple callbacks.
		future_type next_future()
		{
			// reset and return another future
			if (_state->_future_acquired)
				_state->reset();
			return future_type(_state);
		}

		future_type get_return_object()
		{
			return future_type(_state);
		}

		std::experimental::suspend_never initial_suspend() const
		{
			return {};
		}

		std::experimental::suspend_never final_suspend() const
		{
			return {};
		}

		void return_value(const T& val)
		{
			_state->set_value(val);
		}

		[[noreturn]] void unhandled_exception()
		{
			std::terminate();
		}
		/*
		void* operator new(std::size_t sz) {
			trace("promise_t::new(%lu)\n", sz);
			return malloc(sz);
		}
		void operator delete(void* p) {
			trace("promise_t::delete()\n");
			if (p)
				free(p);
		}
		*/
	};

	template <typename state_t>
	struct promise_t<void, state_t>
	{
		typedef future_t<void, state_t> future_type;
		typedef counted_awaitable_state<state_t> state_type;
		typedef counted_ptr<state_t> state_ptr_type;
		state_ptr_type _state;

		// movable not copyable
		template <typename ...Args>
		promise_t(Args&&... args) : _state(make_counted<state_t>(std::forward<Args>(args)...))
		{
		}
		promise_t(const promise_t&) = delete;
		promise_t(promise_t&&) = default;

		future_type get_future()
		{
			return future_type(_state);
		}

		future_type get_return_object()
		{
			return future_type(_state);
		}

		std::experimental::suspend_never initial_suspend() const
		{
			return {};
		}

		std::experimental::suspend_never final_suspend() const
		{
			return {};
		}

		void return_void()
		{
			_state->set_value();
		}

		[[noreturn]] void unhandled_exception()
		{
			std::terminate();
		}
	};

	// future_of_all is pretty trivial as we can just await on each argument

	template <typename T>
	future_t<void> future_of_all(T& f)
	{
		co_await f;
	}

	template <typename T, typename... Rest>
	future_t<void> future_of_all(T& f, Rest&... args)
	{
		co_await f;
		co_await future_of_all(args...);
	}

	// future_of_all_range can take a vector/array of futures, although
	// they must be of the same time. It returns a vector of all the results.
	template <typename Iterator>
#ifdef _MSC_VER
	auto future_of_all_range(Iterator begin, Iterator end) -> future_t<std::vector<decltype(begin->await_resume())>>
	{
		std::vector<decltype(co_await *begin)> vec;
		while (begin != end)
		{
			vec.push_back(co_await *begin);
			++begin;
		}
		co_return vec;
	}
#else
	auto future_of_all_range(Iterator begin, Iterator end) -> future_t<std::vector<typename decltype(*begin)::type>>
	{
		std::vector<typename decltype(*begin)::type> vec;
		while (begin != end)
		{
			vec.push_back(co_await *begin);
			++begin;
		}
		co_return vec;
	}
#endif

	// Define some helper templates to iterate through each element
	// of the tuple
	template <typename tuple_t, size_t N>
	struct coro_helper_t
	{
		static void set(tuple_t& tuple, std::function<void(void)> cb)
		{
			std::get<N>(tuple)._state->set_coroutine_callback(cb);
			coro_helper_t<tuple_t, N - 1>::set(tuple, cb);
		}
	};
	// Specialization for last item
	template <typename tuple_t>
	struct coro_helper_t<tuple_t, 0>
	{
		static void set(tuple_t& tuple, std::function<void(void)> cb)
		{
			std::get<0>(tuple)._state->set_coroutine_callback(cb);
		}
	};

	template <typename tuple_t>
	void set_coro_helper(tuple_t& tuple, std::function<void(void)> cb)
	{
		coro_helper_t<tuple_t, std::tuple_size<tuple_t>::value - 1>::set(tuple, cb);
	}

	// allows waiting for just one future to complete
	template <typename... Rest>
	struct multi_awaitable_state : public awaitable_state<void>
	{
		// Store references to all the futures passed in.
		std::tuple<Rest&...> _futures;
		multi_awaitable_state(Rest&... args) : _futures(args...)
		{
		}

		void set_coroutine_callback(std::function<void(void)> cb)
		{
			set_coro_helper(_futures,
				[this]()
			{
				// reset callbacks on all futures to stop them
				set_coro_helper(_futures, nullptr);
				set_value();
			});
			awaitable_state<void>::set_coroutine_callback(cb);
		}
	};

	// future_of_any is pretty complicated
	// We have to create a new promise with a custom awaitable state object
	template <typename T, typename... Rest>
	future_t<void, multi_awaitable_state<T, Rest...>> future_of_any(T& f, Rest&... args)
	{
		promise_t<void, multi_awaitable_state<T, Rest...>> promise(f, args...);
		return promise.get_future();
	}

	// iterator_awaitable_state will track the index of which future completed
	template <typename Iterator>
	struct iterator_awaitable_state : public awaitable_state<Iterator>
	{
		Iterator _begin;
		Iterator _end;
		iterator_awaitable_state(Iterator begin, Iterator end) : _begin(begin), _end(end)
		{
		}

		// any_completed will be called by any future completing
		void any_completed(Iterator completed)
		{
			// stop any other callbacks from coming in
			for (Iterator c = _begin; c != _end; ++c)
				c->_state->set_coroutine_callback(nullptr);
			set_value(completed);
		}

		void set_coroutine_callback(std::function<void(void)> cb)
		{
			for (Iterator c = _begin; c != _end; ++c)
			{
				std::function<void(void)> func = std::bind(&iterator_awaitable_state::any_completed, this, c);
				c->_state->set_coroutine_callback(func);
			}
			awaitable_state<Iterator>::set_coroutine_callback(cb);
		}
	};

	// returns the index of the iterator that succeeded
	template <typename Iterator>
	future_t<Iterator, iterator_awaitable_state<Iterator>> future_of_any_range(Iterator begin, Iterator end)
	{
		promise_t<Iterator, iterator_awaitable_state<Iterator>> promise(begin, end);
		return promise.get_future();
	}


	template<typename T1, typename S1, typename T2, typename S2>
	auto operator||(future_t<T1, S1>& t1, future_t<T2, S2>& t2)
	{
		return future_of_any(t1, t2);
	}
	/*
	template<typename T1, typename S1, typename T2, typename S2>
	auto operator&&(future_t<T1, S1>& t1, future_t<T2, S2>& t2)
	{
		return future_of_all(t1, t2);
	}
	*/
} }

#endif /* SOURCES_SCHEDULING_FUTURE_H_ */

