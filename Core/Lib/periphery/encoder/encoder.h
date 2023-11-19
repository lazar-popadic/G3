/*
 * encoder.h
 *
 *  Created on: Nov 18, 2023
 *      Author: lazar
 */

#ifndef LIB_PERIPHERY_ENCODER_ENCODER_H_
#define LIB_PERIPHERY_ENCODER_ENCODER_H_

#include <stdint.h>

int16_t
timer_speed_of_encoder_right_passive ();
int16_t
timer_speed_of_encoder_left_passive ();
int16_t
timer_speed_of_encoder_right_maxon ();
int16_t
timer_speed_of_encoder_left_maxon ();
void
encoder_init ();

#endif /* LIB_PERIPHERY_ENCODER_ENCODER_H_ */
