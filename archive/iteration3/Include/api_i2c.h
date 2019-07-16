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

scp::core::future_t<byte> write_i2c(uint8_t slave_address, uint8_t reg, uint8_t data);
#ifdef USE_STATIC_PTR_FOR_READ_I2C
scp::core::future_t<byte, core::static_ptr<byte>> read_i2c(uint8_t slave_address, uint8_t reg, uint8_t* data, word len);
#else
scp::core::future_t<byte> read_i2c(uint8_t slave_address, uint8_t reg, uint8_t* data, word len);
#endif

scp::core::future_t<byte> I2C_SendBlock_async(void* Ptr, word Siz, word *Snt);
scp::core::future_t<byte> I2C_SendChar_async(byte Chr);
scp::core::future_t<byte> I2C_RecvBlock_async(void* Ptr, word Siz, word *Rcv);

} } // namespace scp::drivers

#endif /* SHARED_INCLUDE_API_I2C_H_ */
