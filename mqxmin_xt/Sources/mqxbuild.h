/*
 * mqxbuild.h
 *
 *  Created on: 22 Apr 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#ifndef SOURCES_MQXBUILD_H_
#define SOURCES_MQXBUILD_H_

#if defined(MQXBUILD_EXTERNAL_TIMER)

#define PRINT_NUMBER(x)
#define PRINT_STRING(x)

#define MQXBUILD_EXTERNAL_TIMER_DESCRIPTION "ExtTimer=true"

#else

#define INCLUDE_TERM
#define PRINT_NUMBER(x) print_number(x)
#define PRINT_STRING(x) print_string(x)

#define MQXBUILD_EXTERNAL_TIMER_DESCRIPTION "ExtTimer=false"

#endif

#if defined(MQXBUILD_NO_SCHEDULER)
#define MQXBUILD_NO_SCHEDULER_DESCRIPTION "NoScheduler=true"
#else
#define MQXBUILD_NO_SCHEDULER_DESCRIPTION "NoScheduler=false"
#endif

#if defined(MQXBUILD_MINIMAL)

#define MQXTBUILD_MINIMAL_DESCRIPTION "Minimal=true"

#ifdef INCLUDE_IDLE_TASK
#undef INCLUDE_IDLE_TASK
#endif

#define INCLUDE_TEST_TASK

#else

#define MQXBUILD_MINIMAL_DESCRIPTION "Minimal=false"

#ifdef INCLUDE_TEST_TASK
#undef INCLUDE_TEST_TASK
#endif

#define INCLUDE_IDLE_TASK

#endif

#define MQXBUILD_DESCRIPTION MQXBUILD_EXTERNAL_TIMER_DESCRIPTION " " MQXBUILD_NO_SCHEDULER_DESCRIPTION " " MQXBUILD_MINIMAL_DESCRIPTION

#endif /* SOURCES_MQXBUILD_H_ */
