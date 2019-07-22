/*
 * fsmbuild.h
 *
 *  Created on: 1 Apr 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#ifndef SOURCES_FSMINCLUDE_FSMBUILD_H_
#define SOURCES_FSMINCLUDE_FSMBUILD_H_

#if defined(FSMBUILD_EXTERNAL_TIMER)

#define PRINT_NUMBER(x)
#define PRINT_STRING(x)

#define FSMBUILD_EXTERNAL_TIMER_DESCRIPTION "ExtTimer=true"

#else

#define INCLUDE_TERM
#define PRINT_NUMBER(x) print_number(x)
#define PRINT_STRING(x) print_string(x)

#define FSMBUILD_EXTERNAL_TIMER_DESCRIPTION "ExtTimer=false"

#endif

#if defined(FSMBUILD_NO_SCHEDULER)
#define FSMBUILD_NO_SCHEDULER_DESCRIPTION "NoScheduler=true"
#else
#define FSMBUILD_NO_SCHEDULER_DESCRIPTION "NoScheduler=false"
#endif

#if defined(FSMBUILD_MINIMAL)

#define FSMBUILD_MINIMAL_DESCRIPTION "Minimal=true"

#ifdef INCLUDE_IDLE_TASK
#undef INCLUDE_IDLE_TASK
#endif

#define INCLUDE_TEST_TASK

#else

#define FSMBUILD_MINIMAL_DESCRIPTION "Minimal=false"

#ifdef INCLUDE_TEST_TASK
#undef INCLUDE_TEST_TASK
#endif

#define INCLUDE_IDLE_TASK

#endif

#define FSMBUILD_DESCRIPTION FSMBUILD_EXTERNAL_TIMER_DESCRIPTION " " FSMBUILD_NO_SCHEDULER_DESCRIPTION " " FSMBUILD_MINIMAL_DESCRIPTION

#endif /* SOURCES_FSMINCLUDE_FSMTBUILD_H_ */
