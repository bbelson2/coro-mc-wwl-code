/*
 * pttask_test.h
 *
 *  Created on: 13 Mar 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#ifndef SOURCES_PTINCLUDE_PTTASK_TEST_H_
#define SOURCES_PTINCLUDE_PTTASK_TEST_H_

#include "ptcore_scheduler.h"

using namespace ptp::core;

namespace ptp { namespace task {
	class TestTask : public task_t {
	public:
		TestTask(task_id_t taskid, int arg) : task_t(taskid, task_t::task_state_t::Ready), arg_(arg) {}
		virtual bool Run();
	private:
		int arg_;
	};
} }


#endif /* SOURCES_PTINCLUDE_PTTASK_TEST_H_ */
