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
task_pickup_plants (target plant_target);
int8_t
task_dropoff_x_close_2 (uint8_t side);
int8_t
task_dropoff_x_far_2 (uint8_t side);
int8_t
task_dropoff_y_2 (uint8_t side);
int8_t
task_solar (uint8_t side, uint8_t solar);

void
reset_task ();
void
set_task_case (uint8_t number);

#endif /* LIB_TACTICS_TASK_MODULES_H_ */
