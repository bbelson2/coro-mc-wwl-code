/*
 * core_resumable.h
 *
 *  Created on: 29 Nov 2018
 *      Author: Bruce Belson
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 */

/**
 * A minimal resumable object, used to instantiate the thread task
 * as a coroutine.
 */

#ifndef SOURCES_CORE_RESUMABLE_H_
#define SOURCES_CORE_RESUMABLE_H_

#pragma once

#if __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
#else
#include <experimental\resumable>
#include <new>
#include <stdlib.h>
#endif

#include "services.h"

namespace scp { namespace core {
	using namespace std::experimental;
	struct resumable {
		struct promise_type {
			resumable get_return_object() {
				return resumable(
					coroutine_handle < promise_type > ::from_promise(*this));
			}
			auto initial_suspend() {
				return suspend_never{ };
			}
			auto final_suspend() {
				return suspend_never{ };
			}
			void return_void() {
			}
			void unhandled_exception() {}
			/*
			template<int sz> 
			static void* my_alloc()
			{
				trace("resumable::new(%lu)\n", sz);
				return malloc(sz);
			}
			void* operator new(std::size_t sz) {
			// This fails because sz is not available as a constexpr 
			// when the coroutine is built
				return my_alloc<sz>();
			}
			*/
			void* operator new(std::size_t sz) {
#ifdef SOURCES_SERVICES_H_
				trace("resumable::new(%lu)\r\n", sz);
#endif
				return malloc(sz);
			}
			void operator delete(void* p) {
#ifdef SOURCES_SERVICES_H_
				trace("resumable::delete()\r\n");
#endif
				if (p)
					free(p);
			}
		};
		coroutine_handle<promise_type> _coroutine; // = nullptr;
		resumable() = default;
		resumable(resumable const&) = delete;
		resumable& operator=(resumable const&) = delete;
		resumable(resumable&& other) :
			_coroutine(other._coroutine) {
			other._coroutine = nullptr;
		}
		resumable& operator =(resumable&& other) {
			if (&other != this) {
				_coroutine = other._coroutine;
				other._coroutine = nullptr;
			}
			return *this;
		}
		explicit resumable(coroutine_handle<promise_type> coroutine) :
			_coroutine(coroutine) {
		}
		~resumable() {
			if (_coroutine) {
				_coroutine.destroy();
			}
		}
		void resume() {
			_coroutine.resume();
		}
		bool isempty() const { return !(bool)_coroutine; }
	};
} }

#endif /* SOURCES_CORE_RESUMABLE_H_ */

