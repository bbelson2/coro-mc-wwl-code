/*
 * adctask.cpp
 *
 *  Created on: 7 Jan 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */


#include "task_adc.h"
#include "Bit1.h"
#include "AD1.h"
#include "AD2.h"
#include "app_ids.h"
#include "fsmapi_timer.h"

/***************************************************************************/
/* ADC task                                                                */
/***************************************************************************/

using namespace fsm::core;

extern volatile unsigned long __idle_count;
extern volatile unsigned long __timer_count;
extern volatile int16_t __accel_x;
extern volatile int16_t __accel_y;
extern volatile int16_t __accel_z;
extern volatile uint8_t __accel_whoami;
extern volatile int16_t __accel_count;

static int count = 0;

enum adc_task_steps {
	STEP_START = 0,
	STEP_CALIBRATE_ADC1_WAIT,
	STEP_CALIBRATE_ADC1_COMPLETE,
	STEP_CALIBRATE_ADC2_WAIT,
	STEP_CALIBRATE_ADC2_COMPLETE,
	STEP_MEASURE_ADC1_WAIT,
	STEP_MEASURE_ADC1_COMPLETE,
	STEP_MEASURE_ADC2_WAIT,
	STEP_MEASURE_ADC2_COMPLETE,
	STEP_TIMER_WAIT,
	STEP_TIMER_COMPLETE
};

static void task_adc_afterWait();

void fsm::task::adcTaskFn(fsm::core::task_t* task) {
	fsm::task::adc_task_info_t* task_info = (fsm::task::adc_task_info_t*)task->getTaskData();
	switch (task_info->progress)
	{
		case STEP_START:
			task_info->progress = STEP_CALIBRATE_ADC1_WAIT;
			task->block();
			AD1_Calibrate(false);
			break;
		case STEP_CALIBRATE_ADC1_COMPLETE:
			task_info->progress = STEP_CALIBRATE_ADC2_WAIT;
			task->block();
			AD2_Calibrate(false);
			break;
		case STEP_CALIBRATE_ADC2_COMPLETE:	// Start
			task_info->progress = STEP_MEASURE_ADC1_WAIT;
			task->block();
			AD1_Measure(false);
			break;
		case STEP_MEASURE_ADC1_COMPLETE:	// Start
			task_info->progress = STEP_MEASURE_ADC2_WAIT;
			task->block();
			AD2_Measure(false);
			break;
		case STEP_MEASURE_ADC2_COMPLETE:
			trace("x,y (accel) [t,i] : %d,%d (%d,%d,%d,%x,%d) [%lu,%lu]\r\n",
							task_info->measure1, task_info->measure2,
							__accel_x, __accel_y, __accel_z, __accel_whoami, __accel_count,
			//				__timer_count, __idle_count);
							0, __idle_count);
			Bit1_PutVal(++count % 2 == 0);

			task_info->progress = STEP_TIMER_WAIT;
			task->block();
			if (fsm::api::onTimer(10, task_adc_afterWait) != ERR_OK) {
				task->unblock();
				task_info->progress = STEP_CALIBRATE_ADC2_COMPLETE;
			}
			break;
		case STEP_TIMER_COMPLETE:
			task_info->progress = STEP_CALIBRATE_ADC2_COMPLETE;
			task->unblock();
			break;
	}
}

// Interrupt service routines

static fsm::task::task_t* getTask() {
	return fsm::core::scheduler_t::getInstance().findTaskPtrById(TASK_ID_ADC);
}

static fsm::task::adc_task_info_t* getTaskData(fsm::core::task_t* task) {
	return (fsm::task::adc_task_info_t*)task->getTaskData();
}

static fsm::task::adc_task_info_t* getTaskData() {
	fsm::core::task_t* task = getTask();
	return getTaskData(task);
}

extern "C"
void task_adc_ad1_OnCalibrationEnd()
{
	auto task = getTask();
	getTaskData(task)->progress = STEP_CALIBRATE_ADC1_COMPLETE;
	task->unblock();
}
extern "C"
void task_adc_ad1_OnEnd()
{
	auto task = getTask();
	fsm::task::adc_task_info_t* info = getTaskData(task);
	word result = 0;
	byte rc = AD1_GetValue16(&result);
	info->measure1 = result;
	info->progress = STEP_MEASURE_ADC1_COMPLETE;
	task->unblock();
}
extern "C"
void task_adc_ad2_OnCalibrationEnd()
{
	auto task = getTask();
	getTaskData(task)->progress = STEP_CALIBRATE_ADC2_COMPLETE;
	task->unblock();
}
extern "C"
void task_adc_ad2_OnEnd()
{
	auto task = getTask();
	fsm::task::adc_task_info_t* info = getTaskData(task);
	word result = 0;
	byte rc = AD2_GetValue16(&result);
	info->measure2 = result;
	info->progress = STEP_MEASURE_ADC2_COMPLETE;
	task->unblock();
}

void task_adc_afterWait() {
	auto task = getTask();
	auto info = getTaskData(task);
	switch (info->progress) {
	case STEP_TIMER_WAIT:
		info->progress = STEP_TIMER_COMPLETE;
		task->unblock();
		break;
	}
}
