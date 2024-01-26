/*
 * regulation.h
 *
 *  Created on: Nov 16, 2023
 *      Author: lazar
 */

#ifndef LIB_MODULES_REGULATION_REGULATION_H_
#define LIB_MODULES_REGULATION_REGULATION_H_

#include <stdint.h>
#include "position/position.h"
#include "speed/speed.h"

#define V_REF_LIMIT_DEFAULT		1	// m/s
#define W_REF_LIMIT_DEFAULT		12.6	// rad/s

void
regulation_init ();
float
float_saturation (float signal, float MAX, float MIN);
float
float_ramp (float signal, float desired_value, float slope);
int32_t
int_saturation (int32_t signal, int32_t MAX, int32_t MIN);
int32_t
int_ramp_simple (int32_t signal, int32_t desired_value, int8_t slope);
int32_t
int_ramp_advanced (int32_t signal, int32_t desired_value, int8_t slope, uint8_t prescaler);
int8_t
sign (int32_t signal);
#endif /* LIB_MODULES_REGULATION_REGULATION_H_ */
