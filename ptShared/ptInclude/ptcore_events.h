/*
 * core_events.h
 *
 *  Created on: 3 Dec 2018
 *      Author: Bruce Belson
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 */

/*
 * Contains the API for event handlers to call back into the scheduler.
 * This header must be C/C++ portable.
 * It is included by events.c, and is therefore minimal.
 * Note: the events IDs themselves will be in an application-specific file, e.g. app_ids.h.
 */

#ifndef SOURCES_SCHEDULING_EVENTS_H_
#define SOURCES_SCHEDULING_EVENTS_H_

#pragma once

#include "core_types.h"

#ifdef USE_SIMULATOR

//#error This file does not yet fit in to the simulator framework.

#else

#ifdef __cplusplus
extern "C" {
#endif

void pt_handle_async_event(event_id_t event_id);

#ifdef __cplusplus
}
#endif

#endif // !USE_SIMULATOR

#endif /* SOURCES_SCHEDULING_EVENTS_H_ */
