/*
 * i2ctask.cpp
 *
 *  Created on: 14 Jan 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#include "api_i2c.h"
#include "api_timer.h"
#include "core_resumable.h"
#include "services.h"

/***************************************************************************/
/* I2C task                                                                */
/***************************************************************************/

#define I2C_VERSION 0

#if I2C_VERSION == 0

using namespace scp::core;
using namespace scp::drivers;

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

resumable i2cTaskFn(uint8_t channel /* ignored */) {
	co_await suspend_always{};

	// Check that the accelerometer is alive
	uint8_t whoami;
	byte rc = co_await read_i2c(ACCEL_ADDRESS, 0x0D, &whoami, 1);
	__accel_whoami = whoami;
	if (!rc && (whoami != 0xC7)) {
		rc = ERR_COMMON;
		trace("Accelerometer is not responding.\r\n");
	}

	if (!rc) {
		rc = co_await write_i2c(ACCEL_ADDRESS, 0x2A, 0);
	}

	if (!rc) {
		rc = co_await write_i2c(ACCEL_ADDRESS, 0x2A, 1);
	}

	for (;;) {
		if (!rc) {
			uint8_t buf[7] = { 0, 0, 0, 0, 0, 0, 0 };
			rc = co_await read_i2c(ACCEL_ADDRESS, 0x00, buf, sizeof(buf));
			if (!rc) {
				int16_t x = 0, y = 0, z = 0;
				decodeCoordsFromBuffer(buf, x, y, z);
			  __accel_x = x;
			  __accel_y = y;
			  __accel_z = z;
			  __accel_count++;
			}
		}
		co_await wait_on_ticks(100);
	}
}

#endif // I2C_VERSION == 0


