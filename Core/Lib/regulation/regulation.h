/*
 * regulation.h
 *
 *  Created on: Nov 16, 2023
 *      Author: lazar
 */

#ifndef LIB_MODULES_REGULATION_REGULATION_H_
#define LIB_MODULES_REGULATION_REGULATION_H_

#include <stdint.h>

void
regulation_init ();
void
regulation_position ();
void
regulation_speed (int16_t speed_right, int16_t speed_left);
float
float_saturation (float signal, float MAX, float MIN);
int32_t
int_saturation (int32_t signal, int32_t MAX, int32_t MIN);
int32_t
int_ramp_simple (int32_t signal, int32_t desired_value, int8_t slope);
int32_t
int_ramp_advanced (int32_t signal, int32_t desired_value, int8_t slope, uint8_t prescaler);

#endif /* LIB_MODULES_REGULATION_REGULATION_H_ */
