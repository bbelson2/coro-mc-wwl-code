/*
 * cobuild.h
 *
 *  Created on: 1 Apr 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#ifndef SOURCES_INCLUDE_COBUILD_H_
#define SOURCES_INCLUDE_COBUILD_H_

// Add a definintion to Project Properties > Settings > C++ Compiler > Preprocessor:
// e.g.: COBUILD_APP_NAME=\"k22fawait2\"

#ifndef COBUILD_APP_NAME
#define COBUILD_APP_NAME "[coro_test_app]"
#endif
#define COBUILD_APP_NAME_DESC "AppName=" COBUILD_APP_NAME

#if defined(COBUILD_EXTERNAL_TIMER)

#define PRINT_NUMBER(x)
#define PRINT_STRING(x)

#define COBUILD_EXTERNAL_TIMER_DESCRIPTION "ExtTimer=true"

#else

#define INCLUDE_TERM
#define PRINT_NUMBER(x) print_number(x)
#define PRINT_STRING(x) print_string(x)

#define COBUILD_EXTERNAL_TIMER_DESCRIPTION "ExtTimer=false"

#endif

#if defined(COBUILD_NO_SCHEDULER)
#define COBUILD_NO_SCHEDULER_DESCRIPTION "NoScheduler=true"
#else
#define COBUILD_NO_SCHEDULER_DESCRIPTION "NoScheduler=false"
#endif

#if defined(COBUILD_MINIMAL)

#define COBUILD_MINIMAL_DESCRIPTION "Minimal=true"

#ifdef INCLUDE_IDLE_TASK
#undef INCLUDE_IDLE_TASK
#endif
#ifdef INCLUDE_ADC_TASK
#undef INCLUDE_ADC_TASK
#endif
#ifdef INCLUDE_TIMER_TASK
#undef INCLUDE_TIMER_TASK
#endif
#ifdef INCLUDE_I2C_TASK
#undef INCLUDE_I2C_TASK
#endif

#define INCLUDE_TEST_TASK

#else

#define COBUILD_MINIMAL_DESCRIPTION "Minimal=false"
/*
#ifdef INCLUDE_TEST_TASK
#undef INCLUDE_TEST_TASK
#endif

#ifndef INCLUDE_IDLE_TASK
#define INCLUDE_IDLE_TASK
#endif

#ifndef INCLUDE_ADC_TASK
#define INCLUDE_ADC_TASK
#endif

#ifndef INCLUDE_TIMER_TASK
#define INCLUDE_TIMER_TASK
#endif

#ifndef INCLUDE_I2C_TASK
#define INCLUDE_I2C_TASK
#endif
*/

#endif

#define COBUILD_DESCRIPTION \
	COBUILD_APP_NAME_DESC \
	" " COBUILD_EXTERNAL_TIMER_DESCRIPTION \
	" " COBUILD_NO_SCHEDULER_DESCRIPTION \
	" " COBUILD_MINIMAL_DESCRIPTION

#endif /* SOURCES_INCLUDE_COBUILD_H_ */
