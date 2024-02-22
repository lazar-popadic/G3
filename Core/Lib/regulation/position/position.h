/*
 * position.h
 *
 *  Created on: Jan 3, 2024
 *      Author: lazar
 */

#ifndef LIB_REGULATION_POSITION_POSITION_H_
#define LIB_REGULATION_POSITION_POSITION_H_

#include <stdint.h>

#define EPSILON_THETA_SMALL	0.5	*0.0175		// 1.5 stepena
#define EPSILON_THETA_MEDIUM	1.5	*0.0175		// 10 stepeni
#define EPSILON_THETA_BIG	15	*0.0175		// 20 stepeni
#define EPSILON_DISTANCE	20			// 20 mm
#define EPSILON_DISTANCE_ROT	150			// 100 mm

#define ROT_TO_ANGLE		0
#define ROT_TO_POS		1
#define TRAN_WITH_ROT		2
#define TRAN_WITHOUT_ROT	3

void
regulation_position ();

void
regulation_rotation (float theta_er, float factor, float limit_factor);
void
regulation_translation (float distance_er);

void
regulation_rotation_finished ();
void
regulation_translation_finished ();
#endif /* LIB_REGULATION_POSITION_POSITION_H_ */
