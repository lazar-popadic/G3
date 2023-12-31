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
float
float_saturation (float signal, float MAX, float MIN);
uint32_t
int_saturation (uint32_t signal, uint32_t MAX, uint32_t MIN);
void
regulation_speed (int16_t speed_right, int16_t speed_left);
uint32_t
int_ramp (uint32_t signal, uint32_t desired_value, uint32_t slope);

#endif /* LIB_MODULES_REGULATION_REGULATION_H_ */
