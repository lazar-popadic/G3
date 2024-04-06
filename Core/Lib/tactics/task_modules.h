/*
 * task_modules.h
 *
 *  Created on: Feb 27, 2024
 *      Author: lazar
 */

#ifndef LIB_TACTICS_TASK_MODULES_H_
#define LIB_TACTICS_TASK_MODULES_H_

#include "../../Inc/main.h"
#define BLUE		0
#define YELLOW		1
#define RESERVED	0
#define CENTRAL		1
#define CLOSE		0
#define FAR		1

#define RETURN_CASE		100
#define TASK_IN_PROGRESS	0
#define TASK_SUCCESS		1
#define TASK_FAILED_1		-11
#define TASK_FAILED_2		-12
#define TASK_FAILED_3		-13
#define TASK_FAILED_4		-14

typedef struct
{
  float x;
  float y;
} target;

int8_t
task_go_home (target home_target, uint8_t direction);
int8_t
task_pickup_plants (target plant_target, float offset_perc);
int8_t
task_pickup_plants_safe (target plant_target, uint8_t side, float offset_perc);
int8_t
task_dropoff_x_close_2 (uint8_t side);
int8_t
task_dropoff_x_alt (uint8_t side, uint8_t planter);
int8_t
task_dropoff_x (uint8_t side, uint8_t planter);
int8_t
task_dropoff_x_far_2 (uint8_t side);
int8_t
task_dropoff_y_2 (uint8_t side, uint8_t first_side);
int8_t
task_solar (uint8_t side, uint8_t solar, float speed_limit, uint8_t first_side);
int8_t
task_pot_reserved (uint8_t side);
int8_t
task_pot_solar (uint8_t side);
int8_t
task_pot_far (uint8_t side);
int8_t
task_push_pots (uint8_t side);
int8_t
task_push_pots_far (uint8_t side);

uint8_t
get_and_reset_task_points ();
void
reset_task ();
void
set_task_case (uint8_t number);
void
just_set_task_case (uint8_t number);
void
swap_plants (uint8_t number);

#endif /* LIB_TACTICS_TASK_MODULES_H_ */
