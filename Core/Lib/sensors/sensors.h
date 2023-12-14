/*
 * sensors.h
 *
 *  Created on: Dec 14, 2023
 *      Author: lazar
 */

#ifndef LIB_SENSORS_SENSORS_H_
#define LIB_SENSORS_SENSORS_H_

#include <stdbool.h>

void
sensors_init ();
bool
sensors_low ();
bool
sensors_high ();
bool
sensors_back ();
bool
button_pressed ();

#endif /* LIB_SENSORS_SENSORS_H_ */
