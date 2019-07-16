/*
 * api_i2c.cpp
 *
 *  Created on: 12 Jan 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#include "core_resumable.h"
#include "core_scheduler.h"
#include "core_future.h"
#include "core_split_phase.h"
#include "app_ids.h"
#include "services.h"
#include "api_i2c.h"


#ifdef USE_SIMULATOR
 // Simulator utilities
#include "core_simulator.h"
// General purpose PE polyfill
#include "pe_polyfill.h"
// Simulated calls
byte I2C_SelectSlave(byte Slv) { return ERR_OK; }
byte I2C_SendBlock(void* Ptr, word Siz, word *Snt) { 
	if (Snt) { *Snt = Siz; } return ERR_OK; }
byte I2C_SendChar(byte Chr) { return ERR_OK; }
byte I2C_RecvBlock(void* Ptr, word Siz, word *Rcv) {
	if (Rcv) { *Rcv = Siz; } return ERR_OK; }
byte I2C_SendStop(void) { return ERR_OK; }
#else
#include "I2C.h"	// TODO wrap for simulator
#endif

namespace scp { namespace drivers {

using namespace scp::core;

/*
 * 	Primitives, each of which is a coroutine
 */

/*
 * Note that the buffer Ptr[] is NOT within the future's shared state,
 * but is expected to be maintained by the caller.
 * This is in line with the semantics of the future, and in particular
 * of shared_state::set_value(const T& t);
 * so the shared state is merely the return code (e.g. ERR_OK etc).
 */

scp::core::future_t<byte> I2C_SendBlock_async(void* Ptr, word Siz, word *Snt) {
	promise_t<byte> p;
	split_phase_event_t(EVENT_ID_I2C_TRANSMIT, [s = p._state]() {
		I2C_SendStop();
		s->set_value(ERR_OK);
	}).reg();
	byte rc = I2C_SendBlock(Ptr, Siz, Snt);
	if (rc != ERR_OK) {
		I2C_SendStop();
		p._state->set_value(rc);
	}
	return p.get_future();
}

scp::core::future_t<byte> I2C_SendChar_async(byte Chr) {
	promise_t<byte> p;
	split_phase_event_t(EVENT_ID_I2C_TRANSMIT, [s = p._state]() {
		s->set_value(0);
	}).reg();
	byte rc = I2C_SendChar(Chr);
	if (rc != ERR_OK) {
		I2C_SendStop();
		p._state->set_value(rc);
	}
	return p.get_future();
}

scp::core::future_t<byte> I2C_RecvBlock_async(void* Ptr, word Siz, word *Rcv) {
	promise_t<byte> p;
	split_phase_event_t(EVENT_ID_I2C_RECEIVE, [s = p._state]() {
		I2C_SendStop();
		s->set_value(ERR_OK);
	}).reg();
	byte rc = I2C_RecvBlock(Ptr, Siz, Rcv);
	if (rc != ERR_OK) {
		I2C_SendStop();
		p._state->set_value(rc);
	}
	return p.get_future();
}

/*
 *	Operations, composed of sync and async primitives
 */

#ifdef USE_STATIC_PTR_FOR_READ_I2C
future_t<byte, static_ptr<byte>> read_i2c(uint8_t slave_address, uint8_t reg, uint8_t* data, word len) {
#else
future_t<byte> read_i2c(uint8_t slave_address, uint8_t reg, uint8_t* data, word len) {
#endif
	byte rc = I2C_SelectSlave(slave_address);
	if (rc == ERR_OK) {
		rc = co_await I2C_SendChar_async(reg);
	}
	if (rc == ERR_OK) {
		word recv;
		rc = co_await I2C_RecvBlock_async(data, len, &recv);
	}
	I2C_SendStop();
	co_return rc;
}

future_t<byte> write_i2c(uint8_t slave_address, uint8_t reg, uint8_t data) {
	byte rc = I2C_SelectSlave(slave_address);
	if (rc == ERR_OK) {
		uint8_t msg [2] = {reg, data};
		word sent;
		rc = co_await I2C_SendBlock_async(msg, 2, &sent);
	}
	I2C_SendStop();
	co_return rc;
}

// This section applies the same structure using static state data,
// and the associated types sfuture_t<> and spromise_t<>.
// Points of difference:
// 1) future_t => sfuture_t; promise_t => spromise_t
// 2) Global buffers for awaitable state
// 3) Buffer is passed to future (and to promise) as template parameter.
// 4) get_future() => next_future()

scp::core::awaitable_state<byte> I2C_SendBlock_async_4_s;
scp::core::awaitable_state<byte> I2C_SendChar_async_4_s;
scp::core::awaitable_state<byte> I2C_RecvBlock_async_4_s;

scp::core::sfuture_t<byte, &I2C_SendBlock_async_4_s> I2C_SendBlock_async_4(void* Ptr, word Siz, word *Snt) {
	spromise_t<byte, &I2C_SendBlock_async_4_s> p;
	split_phase_event_t(EVENT_ID_I2C_TRANSMIT, [s = p._state]() {
		I2C_SendStop();
		s->set_value(ERR_OK);
	}).reg();
	byte rc = I2C_SendBlock(Ptr, Siz, Snt);
	if (rc != ERR_OK) {
		I2C_SendStop();
		p._state->set_value(rc);
	}
	return p.next_future();
}
scp::core::sfuture_t<byte, &I2C_SendChar_async_4_s> I2C_SendChar_async_4(byte Chr) {
	spromise_t<byte, &I2C_SendChar_async_4_s> p;
	split_phase_event_t(EVENT_ID_I2C_TRANSMIT, [s = p._state]() {
		s->set_value(0);
	}).reg();
	byte rc = I2C_SendChar(Chr);
	if (rc != ERR_OK) {
		I2C_SendStop();
		p._state->set_value(rc);
	}
	return p.next_future();
}
scp::core::sfuture_t<byte, &I2C_RecvBlock_async_4_s> I2C_RecvBlock_async_4(void* Ptr, word Siz, word *Rcv) {
	spromise_t<byte, &I2C_RecvBlock_async_4_s> p;
	split_phase_event_t(EVENT_ID_I2C_RECEIVE, [s = p._state]() {
		I2C_SendStop();
		s->set_value(ERR_OK);
	}).reg();
	byte rc = I2C_RecvBlock(Ptr, Siz, Rcv);
	if (rc != ERR_OK) {
		I2C_SendStop();
		p._state->set_value(rc);
	}
	return p.next_future();
}

awaitable_state<byte> read_i2c_4_s;
sfuture_t<byte, &read_i2c_4_s> read_i2c_4(uint8_t slave_address, uint8_t reg, uint8_t* data, word len) {
	byte rc = I2C_SelectSlave(slave_address);
	if (rc == ERR_OK) {
		rc = co_await I2C_SendChar_async_4(reg);
	}
	if (rc == ERR_OK) {
		word recv;
		rc = co_await I2C_RecvBlock_async_4(data, len, &recv);
	}
	I2C_SendStop();
	co_return rc;
}

awaitable_state<byte> write_i2c_4_s;
sfuture_t<byte, &write_i2c_4_s> write_i2c_4(uint8_t slave_address, uint8_t reg, uint8_t data) {
	byte rc = I2C_SelectSlave(slave_address);
	if (rc == ERR_OK) {
		uint8_t msg [2] = {reg, data};
		word sent;
		rc = co_await I2C_SendBlock_async_4(msg, 2, &sent);
	}
	I2C_SendStop();
	co_return rc;
}


} } // namespace scp::drivers
