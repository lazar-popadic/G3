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

#endif /* LIB_MODULES_REGULATION_REGULATION_H_ */
