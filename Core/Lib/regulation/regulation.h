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
saturation (float signal, float MAX, float MIN);
void
regulation_speed (int16_t speed_right, int16_t speed_left);


#endif /* LIB_MODULES_REGULATION_REGULATION_H_ */
