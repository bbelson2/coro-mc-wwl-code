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

#ifdef USE_SIMULATOR

extern "C"
void print_number(int32_t val) {
	trace("%ld", val);
}

extern "C"
void print_string(const char* val) {
	trace("%s", val);
}

#else

#if __has_include("Term1.h")

extern "C" {
// Term1.h is a Processor Expert file created by a Term component, named Term1
#include "Term1.h"
}

extern "C"
void print_number(int32_t val) {
	Term1_SendNum(val);
}

extern "C"
void print_string(const char* val) {
	Term1_SendStr((void*)val);
}
#else

extern "C"
void print_number(int32_t val) {
}

extern "C"
void print_string(const char* val) {
}

#endif // __has_include("Term1.h")

#endif
