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
extern volatile target solar_central;

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
//      if (timer_delay_nonblocking (100))
//	{
//	  continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_finished = false;
	  task_init = false;
	}
//	}
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
//      if (timer_delay_nonblocking (100))
//      {
//	continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
//    }
      break;
    case 1:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 2000.0;
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
	  set_translation_speed_limit (0.3);
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
      if (timer_delay_nonblocking (200))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
      mechanism_close ();
      if (timer_delay_nonblocking (200))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_up ();
      if (timer_delay_nonblocking (200))
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
task_dropoff_plants_x (target *planter_array_pointer, uint8_t side)
{
  switch (task_case)
    {
    case 0:
      {
	sensors_case_timer = SENSORS_BACK;
	task_init = true;
	task_finished = false;
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      move_full (side * 3000 - (2 * side - 1) * 200, 1000, (1 - side) * 180,
      WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      {
	sensors_case_timer = SENSORS_BACK;
	task_init = true;
	task_finished = false;
	set_rotation_speed_limit (0.4);
	set_translation_speed_limit (1.0);
      }
      move_on_direction (200, WALL);
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 2:
      reset_x_coord_far ();	//TODO ovde ne moze samo far, odvoj nekako
      reset_movement ();
      task_case++;
      break;
    case 3:
      move_on_direction (100, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;

    case 4:
      {
	sensors_case_timer = SENSORS_BACK;
	task_init = true;
	task_finished = false;
	set_rotation_speed_limit (0.4);
	set_translation_speed_limit (1.0);
      }
      move_to_xy ((planter_array_pointer + task_counter)->x, 100, WALL);
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
task_central_solar_without (uint8_t side)
{
  switch (task_case)
    {
    case 0:
      {
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	sensors_case_timer = SENSORS_HIGH;
	task_init = true;
	task_finished = false;
      }
      move_full (solar_central.x - (2 * side - 1) * 225, solar_central.y + 20,
		 side * 180,
		 WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_BACK;
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
    case 2:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	}
      break;
    case 3:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
      //      move_full (side * 3000 + (-2 * side + 1) * 525, 190, side * 180, WALL);
      move_on_direction (225, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      //      if (interrupted)
      //	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 4:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 5:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	}
      break;
    case 6:
      //      move_full (side * 3000 + (-2 * side + 1) * 740, 190, side * 180, WALL);
      move_on_direction (225, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	}
      //      if (interrupted)
      //	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 7:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 8:
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
task_central_solar (uint8_t side)
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
      move_to_xy (solar_central.x - (2 * side - 1) * 130, solar_central.y + 50,
      WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  //TODO: smisli ovde sta ce
	}
      break;
    case 1:
      move_to_angle (-90);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 2:
      {
	sensors_case_timer = SENSORS_BACK;
	task_init = true;
	task_finished = false;
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (0.4);
      }
      move_on_direction (250, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      reset_y_coord_close ();
      reset_movement ();
      task_case++;
      break;
    case 4:
      {
	sensors_case_timer = SENSORS_BACK;
	task_init = true;
	task_finished = false;
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
      }
      move_on_direction (120, MECHANISM);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
      break;
    case 5:
      {
	sensors_case_timer = SENSORS_HIGH;
	task_init = true;
	task_finished = false;
      }
      move_full (solar_central.x - (2 * side - 1) * 130, solar_central.y, 0,
		 side);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_BACK;
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
    case 7:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	}
      break;
    case 8:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
//      move_full (side * 3000 + (-2 * side + 1) * 525, 190, side * 180, WALL);
      move_on_direction (225, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
//      if (interrupted)
//	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 9:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 10:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	}
      break;
    case 11:
//      move_full (side * 3000 + (-2 * side + 1) * 740, 190, side * 180, WALL);
      move_on_direction (225, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	}
      //      if (interrupted)
      //	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 12:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 13:
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
task_solar_from_start (uint8_t side) // treba da je vec na 265, 190, 180 za plavu
{					// i na 3000-265, 190, 0 za zutu
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_BACK;
	  task_init = true;
	  task_finished = false;
	}
      if (side == BLUE)
	solar_out_l ();
      else
	solar_out_r ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	solar_in_l ();
      else
	solar_in_r ();
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
//      move_full (side * 3000 + (-2 * side + 1) * 525, 190, side * 180, WALL);
      move_on_direction (225, MECHANISM);
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
	solar_out_l ();
      else
	solar_out_r ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 4:
      if (side == BLUE)
	solar_in_l ();
      else
	solar_in_r ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	}
      break;
    case 5:
//      move_full (side * 3000 + (-2 * side + 1) * 740, 190, side * 180, WALL);
      move_on_direction (225, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	}
      //      if (interrupted)
      //	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 6:
      if (side == BLUE)
	solar_out_l ();
      else
	solar_out_r ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 7:
      if (side == BLUE)
	solar_in_l ();
      else
	solar_in_r ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case = 100;
	}
      break;
//    case 8:
//      move_to_angle (-90);
//      if (movement_finished () && timer_delay_nonblocking (100))
//	{
//	  task_case++;
//	}
//      break;
//    case 9:
//      move_on_direction(100, MECHANISM);
//      if (movement_finished () && timer_delay_nonblocking (100))
//	{
//	  task_case = 100;
//	}
//      break;

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
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (0.4);
	  transition_factor = 1.0;
	}
      move_on_direction (450, WALL);

      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      reset_x_coord_close ();
      reset_movement ();
      task_case++;
      break;
    case 2:
      move_on_direction (150, MECHANISM);
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
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	}
      break;
    case 5:
      reset_y_coord_close ();
      reset_movement ();
      set_translation_speed_limit (1.0);
      task_case++;
      break;
    case 6:
      move_on_direction (100, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 7:
      move_full (280, 210, 180, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
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
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (0.4);
	  transition_factor = 1.0;
	}
      move_on_direction (450, WALL);

      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      reset_x_coord_far ();
      reset_movement ();
      task_case++;
      break;
    case 2:
      move_on_direction (150, MECHANISM);
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
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	}
      break;
    case 5:
      reset_y_coord_close ();
      reset_movement ();
      set_translation_speed_limit (0.5);
      task_case++;
      break;
    case 6:
      move_on_direction (100, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 7:
      move_full (3000 - 280, 210, 0, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
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

static void
interrupted_func (uint8_t no_targets)
{
  hold_position_with_reg ();
  task_counter++;
  task_counter %= no_targets;
  task_init = false;
  reset_movement ();
}
