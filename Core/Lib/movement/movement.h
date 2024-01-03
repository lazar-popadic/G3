/*
 * movement.h
 *
 *  Created on: Jan 2, 2024
 *      Author: lazar
 */

#ifndef LIB_MOVEMENT_MOVEMENT_H_
#define LIB_MOVEMENT_MOVEMENT_H_

#include <stdbool.h>

void
calculate_movement ();
bool
no_movement ();
void
set_starting_position (float starting_x, float starting_y, float starting_theta);

void
move_full (float desired_x, float desired_y, float desired_theta);
void
move_xy (float desired_x, float desired_y);
void
move_theta (float desired_theta);
void
move_relative_rotate (float angle);
void
move_translate (float distance);

#endif /* LIB_MOVEMENT_MOVEMENT_H_ */
