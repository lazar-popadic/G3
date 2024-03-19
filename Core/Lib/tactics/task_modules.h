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
task_go_home (target home_target, uint8_t direction, uint8_t sensors);
int8_t
task_pickup_plants (target plant_target);
int8_t
task_dropoff_x_close_2 (uint8_t side);
int8_t
task_dropoff_plants_x_close (uint8_t side);
int8_t
task_dropoff_plants_x_far (uint8_t side);
int8_t
task_dropoff_plants_y (uint8_t side);
int8_t
task_solar_from_start (uint8_t side);
int8_t
task_central_solar_long (uint8_t side, uint8_t first_dir);
int8_t
task_central_solar_short (uint8_t side, uint8_t first_dir);
int8_t
positioning_solar_blue ();
int8_t
positioning_solar_yellow ();
int8_t
positioning_up_blue (target first_plant);
int8_t
positioning_up_yellow (target first_plant);
int8_t
reserved_solar (uint8_t side);

void
reset_task ();

#endif /* LIB_TACTICS_TASK_MODULES_H_ */
