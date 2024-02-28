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
volatile uint8_t task_counter = 0;
extern volatile uint8_t sensors_case_timer;
extern volatile float transition_factor;

static void
interrupted_func (uint8_t no_targets);

bool
task_go_home (target *home_array_pointer)
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
      turn_to_pos ((home_array_pointer + task_counter)->x,
		   (home_array_pointer + task_counter)->y,
		   WALL);
      if (timer_delay_nonblocking (4000))
	continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_finished = false;
	  task_init = false;
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
      move_to_xy ((home_array_pointer + task_counter)->x,
		  (home_array_pointer + task_counter)->y,
		  WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case = 100;
	  task_finished = false;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_case = INTERRUPTED;
	  task_init = false;
	}
      break;
    case INTERRUPTED:
      hold_position ();
      task_counter++;
      task_counter %= 2;
      task_case = 0;
      break;
    case 100:
      task_counter = 0;
      task_case = 0;
      task_init = false;
      task_finished = true;
      break;
    }
  return task_finished;
}

bool
task_pickup_plants (target *plant_array_pointer)
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_BACK;
	  task_init = true;
	  task_finished = false;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	}
      turn_to_pos ((plant_array_pointer + task_counter)->x,
		   (plant_array_pointer + task_counter)->y,
		   MECHANISM);
      if (timer_delay_nonblocking (100))
	continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_finished = false;
	  task_init = false;
	}
      break;
    case 1:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (0.25);
	  transition_factor = 10.0;
	}
      move_to_xy_offset ((plant_array_pointer + task_counter)->x,
			 (plant_array_pointer + task_counter)->y, MECHANISM,
			 -300);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_case = 0;
	  interrupted_func (6);
	}
      break;
    case 2:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (0.1);
	  transition_factor = 1.0;
	}
      move_on_direction (400, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case = 100;
	}
      if (interrupted)
	{
	  task_case = 0;
	  interrupted_func (6);
	}
      break;

    case 100:
      task_counter = 0;
      task_case = 0;
      task_init = false;
      task_finished = true;
      break;
    }
  return task_finished;
}

static void
interrupted_func (uint8_t no_targets)
{
  hold_position ();
//  hold_position_with_reg ();
  task_counter++;
  task_counter %= no_targets;
  task_init = false;
  reset_movement ();
}
