/*
 * sensors.h
 *
 *  Created on: Dec 14, 2023
 *      Author: lazar
 */

#ifndef LIB_SENSORS_SENSORS_H_
#define LIB_SENSORS_SENSORS_H_

#include <stdbool.h>
#include <stdint.h>

uint8_t
switch_2 ();
bool
blue_side_selected ();
uint8_t
switch_1 ();
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
