/*
 * services.h
 *
 *  Created on: 22 Apr 2019
 *      Author: Bruce Belson
 * 
 *  This file is subject to the terms and conditions defined in
 *  file 'LICENSE.txt', which is part of this source code package.
 *
 */

#ifndef SOURCES_SERVICES_H_
#define SOURCES_SERVICES_H_


#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Full service, using <stdio.h>
void trace(const char* tpl, ...);

// Minimal service, using "Term1.h" or simulated equivalent
void print_number(int32_t);
void print_string(const char*);

#ifdef __cplusplus
} // extern "C"
#endif


#endif /* SOURCES_SERVICES_H_ */
