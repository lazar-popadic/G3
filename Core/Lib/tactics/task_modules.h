/*
 * task_modules.h
 *
 *  Created on: Feb 27, 2024
 *      Author: lazar
 */

#ifndef LIB_TACTICS_TASK_MODULES_H_
#define LIB_TACTICS_TASK_MODULES_H_

#include "../../Inc/main.h"
#define INTERRUPTED 200
#define BLUE 0
#define YELLOW 1

#define TASK_IN_PROGRESS	0
#define TASK_SUCCESS		1
#define TASK_FAILED		-1

typedef struct
{
  float x;
  float y;
} target;

int8_t
task_go_home (target *home_array_pointer);
int8_t
task_pickup_plants (target *plant_array_pointer, uint8_t number_of_retries);
int8_t
task_dropoff_plants_x_close (uint8_t side);
int8_t
task_dropoff_plants_x_far (uint8_t side);
int8_t
task_dropoff_plants_y (uint8_t side);
int8_t
task_solar_from_start (uint8_t side);
int8_t
task_central_solar_without (uint8_t side);
int8_t
positioning_solar_blue ();
int8_t
positioning_solar_yellow ();

void
reset_task ();

#endif /* LIB_TACTICS_TASK_MODULES_H_ */
