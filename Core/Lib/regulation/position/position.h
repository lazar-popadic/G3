/*
 * position.h
 *
 *  Created on: Jan 3, 2024
 *      Author: lazar
 */

#ifndef LIB_REGULATION_POSITION_POSITION_H_
#define LIB_REGULATION_POSITION_POSITION_H_

#include <stdint.h>

#define EPSILON_THETA_SMALL	2	*0.0175		// 2 stepena
#define EPSILON_THETA_BIG	15	*0.0175		// 15 stepeni
#define EPSILON_DISTANCE	10			// 10 mm
#define EPSILON_DISTANCE_ROT	100			// 100 mm

void
regulation_position ();
void
regulation_single_wheel (float referent_position, float measured_position);

void
regulation_rotation (float theta_er, float faktor);
void
regulation_translation (float distance_er);

void
regulation_rotation_finished ();
void
regulation_translation_finished ();
#endif /* LIB_REGULATION_POSITION_POSITION_H_ */
