/*
 * services.cpp
 *
 *  Created on: 1 Dec 2018
 *      Author: Bruce Belson
 *
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 */

#include "services.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef USE_SIMULATOR

extern "C"
void trace(const char* tpl, ...) {
	va_list argptr;
	va_start(argptr, tpl);

	vprintf(tpl, argptr);

	va_end(argptr);
}

#else

#if __has_include("Term1.h")

extern "C" {
// Term1.h is a Processor Expert file created by a CriticalTerm component, named Term1
#include "Term1.h"
}

#define TRACE_BUF_SIZE 1024

extern "C"
void trace(const char* tpl, ...) {
	va_list argptr;
	va_start(argptr, tpl);

	char achBuf[TRACE_BUF_SIZE];
	vsnprintf(achBuf, sizeof(achBuf) / sizeof(char), tpl, argptr);

	va_end(argptr);

	Term1_SendStr(achBuf);
}
#else

extern "C"
void trace(const char* tpl, ...) {
}
#endif // __has_include("Term1.h")

#endif
