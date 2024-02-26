/*
 * movement.h
 *
 *  Created on: Jan 2, 2024
 *      Author: lazar
 */

#ifndef LIB_MOVEMENT_MOVEMENT_H_
#define LIB_MOVEMENT_MOVEMENT_H_

#include <stdbool.h>
#include <stdint.h>

#define MECHANISM	1
#define WALL		0

void
calculate_movement ();
bool
no_movement ();
bool
movement_finished ();
void
set_starting_position (float starting_x, float starting_y, float starting_theta);

void
set_translation_speed_limit (float V_max);
void
set_rotation_speed_limit (float w_max);
void
set_transition_factor (float factor);

void
move_full (float x, float y, float theta, int8_t translation_direction);
void
move_to_xy (float x, float y, int8_t translation_direction);
void
move_to_angle (float theta);
void
move_on_direction (float distance, int8_t direction);

#endif /* LIB_MOVEMENT_MOVEMENT_H_ */
