/*
 * adc.cpp
 *
 *  Created on: 02 Dec 2018
 *      Author: Bruce Belson
 *
 *  This file contains code for the adc task - for simulator and live.
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
#include "api_adc.h"

#ifdef USE_SIMULATOR
// Simulator utilities
#include "core_simulator.h"
// General purpose PE polyfill
#include "pe_polyfill.h"
// Simulated data
simulated_data_t<word> adcReadData1;
simulated_data_t<word> adcReadData2;
simulated_data_t<uint16_t> adcTransmitData;
// Simulated calls
byte AD1_GetCalibrationStatus() { return ERR_OK; }
byte AD1_Calibrate(bool WaitForResult) { return ERR_OK; }
byte AD1_Measure(bool WaitForResult) { return ERR_OK; }
byte AD1_GetValue16(word *Values) {
	adcReadData1.pop(*Values);
	return ERR_OK;
}
byte AD2_GetCalibrationStatus() { return ERR_OK; }
byte AD2_Calibrate(bool WaitForResult) { return ERR_OK; }
byte AD2_Measure(bool WaitForResult) { return ERR_OK; }
byte AD2_GetValue16(word *Values) {
	adcReadData2.pop(*Values);
	return ERR_OK;
}
#else
extern "C" {
#include "AD1.h"
#include "AD2.h"
}
#endif

/*
 * ADC simulated data
 */

#ifdef USE_SIMULATOR

void adcCreateData() {
	word adc_values1[] = {
		10, 20, 30, 40, 50, 60, 70, 80
	};
	adcReadData1.add(adc_values1, adc_values1 + (sizeof(adc_values1) / sizeof(word)));
	adcReadData1.save();

	word adc_values2[] = {
		5, 10, 15, 20, 25, 30, 35, 40
	};
	adcReadData2.add(adc_values2, adc_values2 + (sizeof(adc_values2) / sizeof(word)));
	adcReadData2.save();

	uint16_t transmit_values[] = {
		1, 1, 1, 0, 1, 1, 1, 0
	};
	adcTransmitData.add(transmit_values, transmit_values + (sizeof(transmit_values) / sizeof(uint16_t)));
	adcTransmitData.save();
}

#endif

/*
 * ADC initialisation
 */

void adcInit() {
#ifdef USE_SIMULATOR
	adcCreateData();
#endif
}

/*
 * ADC wrappers
 */

typedef byte (*adc_Calibrate_t)(bool WaitForResult);
typedef byte (*adc_GetCalibrationStatus_t)(void);
typedef byte (*adc_Measure_t)(bool WaitForResult);
typedef byte (*adc_GetValue16_t)(word *Values);

struct adc_wrapper_t {
	uint8_t 										channelId;
	event_id_t									calibrateEventId;
	event_id_t									measureEventId;
	adc_Calibrate_t 						calibrate;
	adc_GetCalibrationStatus_t	getCalibrationStatus;
	adc_Measure_t								measure;
	adc_GetValue16_t						getValue16;
};

adc_wrapper_t adc_wrappers[] = {
	{
		ADC_CHANNEL_X,
		EVENT_ID_START_ADCX,
		EVENT_ID_READ_ADCX,
		AD1_Calibrate,
		AD1_GetCalibrationStatus,
		AD1_Measure,
		AD1_GetValue16
	},
	{
		ADC_CHANNEL_Y,
		EVENT_ID_START_ADCY,
		EVENT_ID_READ_ADCY,
		AD2_Calibrate,
		AD2_GetCalibrationStatus,
		AD2_Measure,
		AD2_GetValue16
	},
	{
		ADC_CHANNEL_NONE
	}
};

namespace scp { namespace drivers {

using namespace scp::core;

const adc_wrapper_t * findAdc(uint8_t channelId) {
	for (auto w = adc_wrappers; w->channelId != ADC_CHANNEL_NONE; w++) {
		if (w->channelId == channelId) {
			return w;
		}
	}
	return nullptr;
}

/*
 * ADC component promises
 */

using namespace scp::core;

future_t<byte> start_adc(uint8_t channelId) {
	auto w = findAdc(channelId);
	promise_t<byte> p;
	split_phase_event_t(w->calibrateEventId, [w, s = p._state]() {
		auto result = w->getCalibrationStatus();
		s->set_value(result);
	}).reg();
	w->calibrate(false);
	trace("leaving start_adc\r\n");
	return p.get_future();
}

// Reusable stream model

promise_t<word> read_adc_promise;

future_t<word> read_adc2(uint8_t channelId) {
	auto w = findAdc(channelId);
	split_phase_event_t(w->measureEventId, [w](void) {
		word result = 0;
		auto rc = w->getValue16(&result);
		// TODO - handle error
		read_adc_promise.return_value(result);
	}).reg();
	w->measure(false);
	return read_adc_promise.next_future();
}

// One-hit future model

future_t<word> read_adc(uint8_t channelId) {
	auto w = findAdc(channelId);
	promise_t<word> p;
	split_phase_event_t(w->measureEventId, 
		[w, s = p._state]() {
			word result = 0;
			byte rc = w->getValue16(&result);
			s->set_value(result);
		}).reg();
	w->measure(false);
	return p.get_future();
}

future_t<word> read_adc3(uint8_t channelId) {
	auto w = findAdc(channelId);
	int i = 0;
	promise_t<word> p;
	split_phase_event_t::reg(w->measureEventId,
	  [w]() { w->measure(false); },
		[w, s = p._state]() {
			word result = 0;
			byte rc = w->getValue16(&result);
			s->set_value(result); }
	  );
	i++;
	return p.get_future();
}

future_t<bool> transmit_data(uint16_t value) {
	// TODO - send the data
	promise_t<bool> p;
	split_phase_event_t(EVENT_ID_TRANSMIT_DATA, [s = p._state]() {
		bool result = true;
		s->set_value(result);
	}).reg();
	return p.next_future();
}

} } // namespace scp::drivers

