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

future_t<byte> write_i2c1(uint8_t slave_address, uint8_t reg, uint8_t data) {
	promise_t<byte> p;
	split_phase_event_t(EVENT_ID_I2C_TRANSMIT, [s = p._state]() {
		I2C_SendStop();
		s->set_value(ERR_OK);
	}).reg();
	byte rc = I2C_SelectSlave(slave_address);
	if (rc == ERR_OK) {
		uint8_t msg [2] = {reg, data};
		word sent;
		rc = I2C_SendBlock(msg, 2, &sent);
	}
	if (rc != ERR_OK) {
		I2C_SendStop();
		p._state->set_value(rc);
	}
	return p.get_future();
}

future_t<byte> write_i2c(uint8_t slave_address, uint8_t reg, uint8_t data) {
	promise_t<byte> p;
	byte rc = ERR_OK;
	split_phase_event_t::reg(
			EVENT_ID_I2C_TRANSMIT,
			[slave_address, reg, data, s = p._state]() {
				byte rc = I2C_SelectSlave(slave_address);
				if (rc == ERR_OK) {
					uint8_t msg [2] = {reg, data};
					word sent;
					rc = I2C_SendBlock(msg, 2, &sent);
				}
				if (rc != ERR_OK) {
					I2C_SendStop();
					s->set_value(rc);
				}
			},
			[s = p._state]() {
				I2C_SendStop();
				s->set_value(ERR_OK);
			});
	return p.get_future();
}

/*
 * Note that the buffer is NOT within the future's shared state,
 * but is expected to be maintained by the caller.
 * This is in line with the semantics of the future, and in particular
 * of shared_state::set_value(const T& t);
 * so the shared state is merely the return code (e.g. ERR_OK etc).
 */

future_t<byte> read_i2c(uint8_t slave_address, uint8_t reg, uint8_t* data, word len) {
	promise_t<byte> p;
	split_phase_event_t(EVENT_ID_I2C_TRANSMIT, [s = p._state, data, len]() {
		split_phase_event_t(EVENT_ID_I2C_RECEIVE, [s]() {
			I2C_SendStop();
			s->set_value(ERR_OK);
		}).reg();
		word recv;
		byte rc = I2C_RecvBlock(data, len, &recv);
		if (rc != ERR_OK) {
			I2C_SendStop();
			s->set_value(rc);
		}
	}).reg();
	byte rc = I2C_SelectSlave(slave_address);
	if (rc == ERR_OK) {
		rc = I2C_SendChar(reg);
	}
	if (rc != ERR_OK) {
		I2C_SendStop();
		p._state->set_value(rc);
	}
	return p.get_future();
}

} } // namespace scp::drivers
