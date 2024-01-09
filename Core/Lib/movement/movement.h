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
bool
movement_finished ();
void
set_starting_position (float starting_x, float starting_y, float starting_theta);

void
move_full (float x, float y, float theta);
void
move_to_xy (float x, float y);
void
move_to_angle (float theta);
void
move_on_direction (float distance);
void
move_relative_angle (float angle_degrees);

#endif /* LIB_MOVEMENT_MOVEMENT_H_ */
