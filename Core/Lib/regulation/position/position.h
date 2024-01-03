/*
 * position.h
 *
 *  Created on: Jan 3, 2024
 *      Author: lazar
 */

#ifndef LIB_REGULATION_POSITION_POSITION_H_
#define LIB_REGULATION_POSITION_POSITION_H_

#include <stdint.h>

void
regulation_position ();
void
regulation_single_wheel (int16_t referent_position, int16_t measured_position);

void
regulation_rotation (int32_t theta_er, float faktor);
void
regulation_translation (int32_t distance_er);

void
regulation_rotation_finished ();
void
regulation_translation_finished ();
#endif /* LIB_REGULATION_POSITION_POSITION_H_ */
