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

typedef struct
{
  float x;
  float y;
} target;

bool
task_go_home (target* home_array_pointer);
bool
task_pickup_plants (target *plant_array_pointer);
bool
task_solar_from_start (uint8_t side);
bool
positioning_solar_blue ();
bool
positioning_solar_yellow ();

#endif /* LIB_TACTICS_TASK_MODULES_H_ */
