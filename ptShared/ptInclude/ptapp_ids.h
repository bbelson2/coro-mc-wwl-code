/*
 * ptapp_ids.h
 *
 *  Created on: 3 Dec 2018
 *      Author: Bruce Belson
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 */

// The event and task identifiers for this application only

#pragma once

#ifndef SOURCES_APP_IDS_H_
#define SOURCES_APP_IDS_H_

#define EVENT_ID_START_ADCX 			0x01
#define EVENT_ID_READ_ADCX  			0x02
#define EVENT_ID_START_ADCY 			0x03
#define EVENT_ID_READ_ADCY				0x04
#define EVENT_ID_TIMER						0x05
#define EVENT_ID_TRANSMIT_DATA		0x06
#define EVENT_ID_I2C_TRANSMIT			0x06
#define EVENT_ID_I2C_RECEIVE			0x07
#define EVENT_ID_I2C_RECEIVE			0x07

// 1st task ID is 1 (IDLE = 0)
#define TASK_ID_ADC					  		0x01
#define TASK_ID_TIMER					  	0x02
#define TASK_ID_I2C					  	  0x03
#define TASK_COUNT								4

#endif /* SOURCES_APP_IDS_H_ */
