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
speed_of_encoder_right_passive ();
int16_t
speed_of_encoder_left_passive ();
void
encoder_init ();

#endif /* LIB_PERIPHERY_ENCODER_ENCODER_H_ */
