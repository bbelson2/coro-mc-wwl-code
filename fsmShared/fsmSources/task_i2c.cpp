/*
 * task_i2c.cpp
 *
 *  Created on: 22 Jul 2019
 *      Author: Bruce Belson
 */

#include "task_i2c.h"
#include "fsmapi_timer.h"
#include "I2C.h"
#include "app_ids.h"

/***************************************************************************/
/* I2C task                                                                */
/***************************************************************************/

#define ACCEL_ADDRESS 0x1C

volatile int16_t __accel_x = 0;
volatile int16_t __accel_y = 0;
volatile int16_t __accel_z = 0;
volatile uint8_t __accel_whoami = 0;
volatile int16_t __accel_count = 0;

void decodeCoordsFromBuffer(const uint8_t* buf, int16_t& x, int16_t& y, int16_t& z) {
  x = (int16_t)(((buf[1] << 8) | buf[2])) >> 2;
  y = (int16_t)(((buf[3] << 8) | buf[4])) >> 2;
  z = (int16_t)(((buf[5] << 8) | buf[6])) >> 2;
}

enum i2c_task_steps {
	STEP_START = 0,
	STEP_READ_WHOAMI_SENDREG_WAIT,
	STEP_READ_WHOAMI_SENDREG_COMPLETE,
	STEP_READ_WHOAMI_RCVBYTE_WAIT,
	STEP_READ_WHOAMI_RCVBYTE_COMPLETE,
	STEP_INITIALISATION_COMPLETE,
	STEP_WRITE_REG1_0_WAIT,
	STEP_WRITE_REG1_0_COMPLETE,
	STEP_WRITE_REG1_1_WAIT,
	STEP_WRITE_REG1_1_COMPLETE,
	STEP_READ_DATA_SENDREG_WAIT,
	STEP_READ_DATA_SENDREG_COMPLETE,
	STEP_READ_DATA_RCVBYTES_WAIT,
	STEP_READ_DATA_RCVBYTES_COMPLETE,
	STEP_TIMER_WAIT,
	STEP_TIMER_COMPLETE,  // ??
};

static void task_i2c_afterWait();

void fsm::task::i2cTaskFn(fsm::core::task_t* task) {
	fsm::task::i2c_task_info_t* task_info = (fsm::task::i2c_task_info_t*)task->getTaskData();
	byte rc;
	word recv, sent;
	uint8_t msg2[2]; // 2-byte outbound message buffer

	switch (task_info->progress) {
	case STEP_START:
		rc = I2C_SelectSlave(ACCEL_ADDRESS);
		if (rc != ERR_OK) {
			// Try again
			break;
		}
		task_info->progress = STEP_READ_WHOAMI_SENDREG_WAIT;
		task->block();
		rc = I2C_SendChar(0x0D);
		if (rc != ERR_OK) {
			// Try again
			I2C_SendStop();
			task->unblock();
			task_info->progress = STEP_START;
		}
		break;
	case STEP_READ_WHOAMI_SENDREG_COMPLETE:
		task_info->progress = STEP_READ_WHOAMI_RCVBYTE_WAIT;
		task->block();
		rc = I2C_RecvBlock((void*)&task_info->whoami, 1, &recv);
		if (rc != ERR_OK) {
			// Try again
			I2C_SendStop();
			task->unblock();
			task_info->progress = STEP_READ_WHOAMI_SENDREG_COMPLETE;
		}
		break;
	case STEP_READ_WHOAMI_RCVBYTE_COMPLETE:
		I2C_SendStop();
		__accel_whoami = task_info->whoami;
		if (task_info->whoami != 0xC7) {
			rc = ERR_COMMON;
			trace("Accelerometer is not responding.\r\n");
			task_info->progress = STEP_START;
			break;
		}
		task_info->progress = STEP_INITIALISATION_COMPLETE;
		break;
	case STEP_INITIALISATION_COMPLETE:
		// Next - write 0 to Reg1 (ACCEL_ADDRESS, 0x2A)
		rc = I2C_SelectSlave(ACCEL_ADDRESS);
		if (rc != ERR_OK) {
			// Try again
			break;
		}
		task_info->progress = STEP_WRITE_REG1_0_WAIT;
		task->block();
		msg2[0] = 0x2A;
		msg2[1] = 0;
		rc = I2C_SendBlock(msg2, 2, &sent);
		if (rc != ERR_OK) {
			// Try again
			I2C_SendStop();
			task->unblock();
			task_info->progress = STEP_READ_WHOAMI_RCVBYTE_COMPLETE;
		}
		break;
	case STEP_WRITE_REG1_0_COMPLETE:
		// Write 1 to Reg1 (ACCEL_ADDRESS, 0x2A)
		rc = I2C_SelectSlave(ACCEL_ADDRESS);
		if (rc != ERR_OK) {
			// Try again
			break;
		}
		task_info->progress = STEP_WRITE_REG1_1_WAIT;
		task->block();
		msg2[0] = 0x2A;
		msg2[1] = 1;
		rc = I2C_SendBlock(msg2, 2, &sent);
		if (rc != ERR_OK) {
			// Try again
			I2C_SendStop();
			task->unblock();
			task_info->progress = STEP_WRITE_REG1_0_COMPLETE;
		}
		break;
	case STEP_WRITE_REG1_1_COMPLETE:
		// read buffer part 1: send register 0x00
		rc = I2C_SelectSlave(ACCEL_ADDRESS);
		if (rc != ERR_OK) {
			// Try again
			break;
		}
		task_info->progress = STEP_READ_DATA_SENDREG_WAIT;
		task->block();
		rc = I2C_SendChar(0x00);
		if (rc != ERR_OK) {
			// Try again
			I2C_SendStop();
			task->unblock();
			task_info->progress = STEP_WRITE_REG1_1_COMPLETE;
		}
		break;
	case STEP_READ_DATA_SENDREG_COMPLETE:
		task_info->progress = STEP_READ_DATA_RCVBYTES_WAIT;
		task->block();
		rc = I2C_RecvBlock((void*)task_info->buf7, 7, &recv);
		if (rc != ERR_OK) {
			// Try again
			I2C_SendStop();
			task->unblock();
			task_info->progress = STEP_READ_DATA_SENDREG_COMPLETE;
		}
		break;
	case STEP_READ_DATA_RCVBYTES_COMPLETE:
		I2C_SendStop();
		{
			int16_t x = 0, y = 0, z = 0;
			decodeCoordsFromBuffer((const uint8_t*)task_info->buf7, x, y, z);
			__accel_x = x;
			__accel_y = y;
			__accel_z = z;
			__accel_count++;
		}
		task_info->progress = STEP_TIMER_WAIT;
		task->block();
		if (fsm::api::onTimer(100, task_i2c_afterWait) != ERR_OK) {
			task->unblock();
			task_info->progress = STEP_READ_WHOAMI_RCVBYTE_COMPLETE;
		}
		break;
	case STEP_TIMER_COMPLETE:
		task_info->progress = STEP_INITIALISATION_COMPLETE;
		task->unblock();
		break;
	}

}

// Interrupt service routines

static fsm::task::task_t* getTask() {
	return fsm::core::scheduler_t::getInstance().findTaskPtrById(TASK_ID_I2C);
}

static fsm::task::i2c_task_info_t* getTaskData(fsm::core::task_t* task) {
	return (fsm::task::i2c_task_info_t*)task->getTaskData();
}

static fsm::task::i2c_task_info_t* getTaskData() {
	fsm::core::task_t* task = getTask();
	return getTaskData(task);
}

extern "C"
void task_i2c_OnReceiveData() {
	auto task = getTask();
	auto info = getTaskData(task);
	switch (info->progress) {
	case STEP_READ_WHOAMI_RCVBYTE_WAIT:
		info->progress = STEP_READ_WHOAMI_RCVBYTE_COMPLETE;
		task->unblock();
		break;
	case STEP_READ_DATA_RCVBYTES_WAIT:
		info->progress = STEP_READ_DATA_RCVBYTES_COMPLETE;
		task->unblock();
		break;
	}
}

extern "C"
void task_i2c_OnTransmitData() {
	auto task = getTask();
	auto info = getTaskData(task);
	switch (info->progress) {
	case STEP_READ_WHOAMI_SENDREG_WAIT:
		info->progress = STEP_READ_WHOAMI_SENDREG_COMPLETE;
		task->unblock();
		break;
	case STEP_WRITE_REG1_0_WAIT:
		info->progress = STEP_WRITE_REG1_0_COMPLETE;
		task->unblock();
		break;
	case STEP_WRITE_REG1_1_WAIT:
		info->progress = STEP_WRITE_REG1_1_COMPLETE;
		task->unblock();
		break;
	case STEP_READ_DATA_SENDREG_WAIT:
		info->progress = STEP_READ_DATA_SENDREG_COMPLETE;
		task->unblock();
		break;
	}
}

void task_i2c_afterWait() {
	auto task = getTask();
	auto info = getTaskData(task);
	switch (info->progress) {
	case STEP_TIMER_WAIT:
		info->progress = STEP_TIMER_COMPLETE;
		task->unblock();
		break;
	}
}
