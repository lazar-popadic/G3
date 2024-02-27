/*
 * task_modules.c
 *
 *  Created on: Feb 27, 2024
 *      Author: lazar
 */

#include "task_modules.h"

volatile bool task_finished = false;
volatile bool task_init = false;
volatile uint8_t task_case = 0;
extern volatile bool interrupted;
volatile uint8_t task_counter = 1;
extern volatile uint8_t sensors_case_timer;

bool
task_go_home (target** home_array_pointer)
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  task_init = true;
	  task_finished = false;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  sensors_case_timer = SENSORS_OFF;
	}
      turn_to_pos ((*home_array_pointer + task_counter * sizeof(target))->x,
		   (*home_array_pointer + task_counter * sizeof(target))->y,
		   WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_finished = false;
	}
      if (interrupted)
	{
	  task_case = INTERRUPTED;
	  task_init = false;
	}
      break;
    case 1:
      if (!task_init)
	{
	  task_init = true;
	  task_finished = false;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  sensors_case_timer = SENSORS_HIGH_AND_LOW;
	}
      move_to_xy ((*home_array_pointer + task_counter * sizeof(target))->x,
		  (*home_array_pointer + task_counter * sizeof(target))->y,
		  WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case = 100;
	  task_finished = false;
	}
      if (interrupted)
	{
	  task_case = INTERRUPTED;
	  task_init = false;
	}
      break;
    case INTERRUPTED:
      break;
    case 100:
      task_finished = true;
      break;
    }
  return task_finished;
}
