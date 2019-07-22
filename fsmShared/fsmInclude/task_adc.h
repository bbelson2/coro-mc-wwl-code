/*
 * task_adc.h
 *
 *  Created on: 22 Jul 2019
 *      Author: Bruce Belson
 */

#ifndef FSMSHARED_FSMINCLUDE_TASK_ADC_H_
#define FSMSHARED_FSMINCLUDE_TASK_ADC_H_

#include "fsmcore_scheduler.h"

namespace fsm { namespace task {

using namespace fsm::core;

struct adc_task_info_t {
	volatile uint8_t progress;
	uint8_t pin;
	volatile word measure1;
	volatile word measure2;
};

extern void adcTaskFn(task_t*);
extern void adcInit();

} } // namespace fsm::task

#endif /* FSMSHARED_FSMINCLUDE_TASK_ADC_H_ */
