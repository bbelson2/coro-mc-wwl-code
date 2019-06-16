/*
 * core_crit_sec.h
 *
 *  Created on: 3 Dec 2018
 *      Author: Bruce Belson
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 */

#ifndef SOURCES_CORE_CRIT_SEC_H_
#define SOURCES_CORE_CRIT_SEC_H_

#pragma once

#ifdef USE_SIMULATOR

#include <windows.h>

namespace scheduling {

	class mutex_t {
	public:
		mutex_t() {
			InitializeCriticalSection(&criticalSection_);
		}
		~mutex_t() {
			DeleteCriticalSection(&criticalSection_);
		}
	public:
		void lock() {
			EnterCriticalSection(&criticalSection_);
		}
		void unlock() {
			LeaveCriticalSection(&criticalSection_);
		}
	private:
		CRITICAL_SECTION criticalSection_;
	};

	class thread_lock_t {
	public:
		thread_lock_t(mutex_t& m) : m_(m) {
			m_.lock();
		}
		~thread_lock_t() {
			m_.unlock();
		}
	private:
		mutex_t& m_;
	};
}

#else

#if __has_include("cs1.h")
extern "C" {
// CS1.h is a Processor Expert file created by a CriticalSection component, named CS1
#include "CS1.h"
}

namespace ptp { namespace core {

class critical_section_t {
public:
	critical_section_t() {
		lock();
	}
	~critical_section_t() {
		unlock();
	}
protected:
	CS1_CriticalVariable()
	void lock() {
		CS1_EnterCritical();
	}
	void unlock() {
		CS1_ExitCritical();
	}
};

class mutex_t {
public:
	mutex_t() {
	}
	~mutex_t() {
	}
public:
	void lock() {
		CS1_EnterCritical();
	}
	void unlock() {
		CS1_ExitCritical();
	}
private:
	CS1_CriticalVariable()
};

class thread_lock_t {
public:
	thread_lock_t(mutex_t& m) : m_(m) {
		m_.lock();
	}
	~thread_lock_t() {
		m_.unlock();
	}
private:
	mutex_t& m_;
};

} } // namespace ptp::core

#endif

#endif

#endif /* SOURCES_CORE_CRIT_SEC_H_ */
