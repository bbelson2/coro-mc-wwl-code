/*
 * report_cycles.c
 *
 *  Created on: 26 Mar 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#if !defined(COBUILD_EXTERNAL_TIMER) || defined(SET_GLOBAL_COUNTER)
unsigned long __co_g_cycles = 0;
#endif

#ifndef COBUILD_EXTERNAL_TIMER
#include "Term1.h"

unsigned long __co_g_cycles = 0;
volatile unsigned long __co_idle_count = 0;

void report_cycles() {
	Term1_SendStr((void*)"Cycles in timer period: ");
	Term1_SendNum(__co_g_cycles);
	Term1_SendStr((void*)". Idle cycles: ");
	Term1_SendNum(__co_idle_count);
	Term1_SendStr((void*)"\r\n");
	__co_g_cycles = 0;
}

#else

void report_cycles() {
}

#endif
