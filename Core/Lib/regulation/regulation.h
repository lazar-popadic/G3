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

#define V_REF_LIMIT_DEFAULT		25.0
#define W_REF_LIMIT_DEFAULT		50.0

void
regulation_init ();
float
float_saturation (float signal, float MAX, float MIN);
float
float_saturation2 (float signal, float MAX, float MIN, float limit);
float
float_ramp (float signal, float desired_value, float slope);
float
float_ramp2 (float signal, float desired_value, float slope_acceleration,
	     float slope_deceleration);
float
float_ramp_acc (float signal, float desired_value, float slope_acceleration);
float
float_ramp_brake (float signal, float deceleration);
int32_t
int_saturation (int32_t signal, int32_t MAX, int32_t MIN);
int8_t
sign (float signal);
#endif /* LIB_MODULES_REGULATION_REGULATION_H_ */
