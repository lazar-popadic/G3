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
	  sensors_case_timer = SENSORS_HIGH_AND_LOW;
	  task_init = true;
	  task_finished = false;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
      turn_to_pos ((home_array_pointer + task_counter)->x,
		   (home_array_pointer + task_counter)->y,
		   WALL);
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
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
      move_to_xy ((home_array_pointer + task_counter)->x,
		  (home_array_pointer + task_counter)->y,
		  WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case = 100;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_case = 0;
	  interrupted_func (2);
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
	  transition_factor = 1.0;
	}
      turn_to_pos ((plant_array_pointer + task_counter)->x,
		   (plant_array_pointer + task_counter)->y,
		   MECHANISM);
      if (timer_delay_nonblocking (100))
	continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
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
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_case = 0;
	  interrupted_func (6);
	}
      break;
    case 3:
      mechanism_half_up ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
      mechanism_close ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_up ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case = 100;
	  task_init = false;
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

bool
task_solar_from_start (uint8_t side) // treba da je vec na 265, 190, 0 za plavu
{					// i na 3000-265, 190, M_PI za zutu
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_finished = false;
	}
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	}
      break;
    case 2:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
      move_full (side * 3000 + (-2 * side + 1) * 525, 190, side * 180, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
//      if (interrupted)
//	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 3:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 4:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	}
      break;
    case 5:
      move_full (side * 3000 + (-2 * side + 1) * 740, 190, side * 180, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	}
      //      if (interrupted)
      //	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 6:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 7:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case = 100;
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

bool
positioning_solar_blue ()	// u polje bilo gde, okrenut na 180
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  continue_movement ();
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_finished = false;
	  set_rotation_speed_limit (0.1);
	  set_translation_speed_limit (0.1);
	  transition_factor = 1.0;
	}
      move_on_direction (450, WALL);

      if (timer_delay_nonblocking (5000))
	{
	  hold_position ();
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      reset_x_coord_close ();
      task_case++;
      break;
    case 2:
      move_on_direction (265, MECHANISM);
      if (!task_init)
	continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_to_angle (-90);
      if (movement_finished () && timer_delay_nonblocking (100))
	task_case++;
      break;
    case 4:
      move_on_direction (450, WALL);
      if (timer_delay_nonblocking (5000))
	{
	  hold_position ();
	  task_case++;
	}
      break;
    case 5:
      reset_y_coord_close ();
      task_case++;
      break;
    case 6:
      move_full (265, 190, 0, MECHANISM);
      if (!task_init)
	continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
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

bool
positioning_solar_yellow ()	// u polje bilo gde, okrenut na 0
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  continue_movement ();
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_finished = false;
	  set_rotation_speed_limit (0.1);
	  set_translation_speed_limit (0.1);
	  transition_factor = 1.0;
	}
      move_on_direction (450, WALL);

      if (timer_delay_nonblocking (5000))
	{
	  hold_position ();
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      reset_x_coord_far ();
      task_case++;
      break;
    case 2:
      move_on_direction (265, MECHANISM);
      if (!task_init)
	continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_to_angle (-90);
      if (movement_finished () && timer_delay_nonblocking (100))
	task_case++;
      break;
    case 4:
      move_on_direction (450, WALL);
      if (timer_delay_nonblocking (5000))
	{
	  hold_position ();
	  task_case++;
	}
      break;
    case 5:
      reset_y_coord_close ();
      task_case++;
      break;
    case 6:
      move_full (3000-265, 190, 180, MECHANISM);
      if (!task_init)
	continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
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
  task_counter++;
  task_counter %= no_targets;
  task_init = false;
  reset_movement ();
}
