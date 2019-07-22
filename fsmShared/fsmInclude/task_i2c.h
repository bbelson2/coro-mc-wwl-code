/*
 * task_i2c.h
 *
 *  Created on: 22 Jul 2019
 *      Author: Bruce Belson
 */

#ifndef FSMSHARED_FSMINCLUDE_TASK_I2C_H_
#define FSMSHARED_FSMINCLUDE_TASK_I2C_H_

#include "fsmcore_scheduler.h"

namespace fsm { namespace task {

using namespace fsm::core;

struct i2c_task_info_t {
	volatile uint8_t progress;
	uint8_t channel;
	volatile uint8_t whoami;
	volatile uint8_t buf7[7]; // 7-byte inbound message buffer
};

extern void i2cTaskFn(task_t*);

} } // namespace fsm::task




#endif /* FSMSHARED_FSMINCLUDE_TASK_I2C_H_ */
