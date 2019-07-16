/*
 * api_i2c.h
 *
 *  Created on: 12 Jan 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#ifndef SHARED_INCLUDE_API_I2C_H_
#define SHARED_INCLUDE_API_I2C_H_

#include "core_future.h"
#include "pe_polyfill.h"

namespace scp { namespace drivers {

enum I2C_CHANNELS {
	I2C_CHANNEL_NONE,
	I2C_CHANNEL_0 = 1,
	I2C_CHANNEL_1
};

using namespace scp::core;

future_t<byte> write_i2c(uint8_t slave_address, uint8_t reg, uint8_t data);
#ifdef USE_STATIC_PTR_FOR_READ_I2C
future_t<byte, core::static_ptr<byte>> read_i2c(uint8_t slave_address, uint8_t reg, uint8_t* data, word len);
#else
future_t<byte> read_i2c(uint8_t slave_address, uint8_t reg, uint8_t* data, word len);
#endif

future_t<byte> I2C_SendBlock_async(void* Ptr, word Siz, word *Snt);
future_t<byte> I2C_SendChar_async(byte Chr);
future_t<byte> I2C_RecvBlock_async(void* Ptr, word Siz, word *Rcv);

extern awaitable_state<byte> write_i2c_4_s;
extern awaitable_state<byte> read_i2c_4_s;
extern awaitable_state<byte> I2C_SendBlock_async_4_s;
extern awaitable_state<byte> I2C_SendChar_async_4_s;
extern awaitable_state<byte> I2C_RecvBlock_async_4_s;

sfuture_t<byte, &write_i2c_4_s> write_i2c_4(uint8_t slave_address, uint8_t reg, uint8_t data);
sfuture_t<byte, &read_i2c_4_s> read_i2c_4(uint8_t slave_address, uint8_t reg, uint8_t* data, word len);

sfuture_t<byte, &I2C_SendBlock_async_4_s> I2C_SendBlock_async_4(void* Ptr, word Siz, word *Snt);
sfuture_t<byte, &I2C_SendChar_async_4_s> I2C_SendChar_async_4(byte Chr);
sfuture_t<byte, &I2C_RecvBlock_async_4_s> I2C_RecvBlock_async_4(void* Ptr, word Siz, word *Rcv);

} } // namespace scp::drivers

#endif /* SHARED_INCLUDE_API_I2C_H_ */
