/*
 * task_modules.c
 *
 *  Created on: Feb 27, 2024
 *      Author: lazar
 */

#include "task_modules.h"

volatile int8_t task_status = TASK_IN_PROGRESS;
volatile bool task_init = false;
volatile uint8_t task_case = 0;
extern volatile bool interrupted;
volatile uint8_t task_counter = 0;
extern volatile uint8_t sensors_case_timer;
extern volatile float transition_factor;
extern volatile target solar_central;
extern volatile target planter_blue_y;
extern volatile target planter_blue_x_close;
extern volatile target planter_blue_x_far;
extern volatile target planter_yellow_y;
extern volatile target planter_yellow_x_close;
extern volatile target planter_yellow_x_far;
extern position robot_position;
volatile int8_t plants_done[3] =
  { -1, -1, -1 };
volatile uint8_t plant_counter = 0;

static int8_t
interrupted_func (uint8_t no_targets);

int8_t
task_go_home (target *home_array_pointer)
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH_AND_LOW;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
      turn_to_pos ((home_array_pointer + task_counter + plant_counter)->x,
		   (home_array_pointer + task_counter + plant_counter)->y,
		   WALL);
//      if (timer_delay_nonblocking (100))
//	{
//	  continue_movement ();
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
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
      move_to_xy ((home_array_pointer + task_counter + plant_counter)->x,
		  (home_array_pointer + task_counter + plant_counter)->y,
		  WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case = 100;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_case = 0;
	  if (interrupted_func (2))
	    task_case = 200;
	  else
	    {
	      task_counter++;
	      task_init = false;
	    }
	}
      break;
    case 100:
      reset_task ();
      task_status = TASK_SUCCESS;
      break;
    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_pickup_plants (target *plant_array_pointer, uint8_t number_of_retries)
{
  // TODO: sredi ovo
//  do
//    {
//      task_counter++;
//    }
//  while (task_counter == plants_done[0] || task_counter == plants_done[1]
//      || task_counter == plants_done[2]);
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_BACK;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
      turn_to_pos ((plant_array_pointer + task_counter + plant_counter)->x,
		   (plant_array_pointer + task_counter + plant_counter)->y,
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
      move_to_xy_offset (
	  (plant_array_pointer + task_counter + plant_counter)->x,
	  (plant_array_pointer + task_counter + plant_counter)->y, MECHANISM,
	  -300);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_case = 0;
	  if (interrupted_func (number_of_retries))
	    task_case = 200;
	  else
	    {
	      task_counter++;
	      task_init = false;
	    }
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
	  if (interrupted_func (number_of_retries))
	    task_case = 200;
	  else
	    {
	      task_counter++;
	      task_init = false;
	    }
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
      reset_task ();
      task_status = TASK_SUCCESS;
      plants_done[plant_counter] = task_counter;
      plant_counter++;
      break;

    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_dropoff_plants_x_close (uint8_t side)
{
  switch (task_case)
    {
    case 0:
      {
	sensors_case_timer = SENSORS_BACK;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      if (side == BLUE)
	move_full (150, 1000, 180, WALL);
      else
	move_full (3000 - 150, 1000, 0, WALL);
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
	  set_translation_speed_limit (0.25);
	}
      move_on_direction (250, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 2:
      if (side == BLUE)
	reset_x_coord_close ();
      else
	reset_x_coord_far ();
      reset_movement ();
      task_case++;
      break;
    case 3:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      move_on_direction (80, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;

    case 4:
      if (side == BLUE)
	move_full (planter_blue_x_close.x + 120, planter_blue_x_close.y, 0,
	MECHANISM);
      else
	move_full (planter_yellow_x_close.x + 120, planter_yellow_x_close.y,
		   180,
		   MECHANISM);
//      move_full (side * 3000 - (2 * side - 1) * 200,
//		 (planter_array_pointer + task_counter )->y, side * 180, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.4);
	}
      move_on_direction (150, MECHANISM);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      if (!task_init)
	{
	  task_init = true;
	  reset_movement ();
	}
      mechanism_half_down ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 7:
      mechanism_open_slow ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 8:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.5);
	}
      move_on_direction (150, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 9:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      mechanism_down ();
      if (timer_delay_nonblocking (100))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;
    case 100:
      reset_task ();
      task_status = TASK_SUCCESS;
      break;
    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_dropoff_plants_x_far (uint8_t side)
{
  switch (task_case)
    {
    case 0:
      {
	sensors_case_timer = SENSORS_BACK;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      if (side == YELLOW)
	move_full (250, 1000, 180, WALL);
      else
	move_full (3000 - 200, 1000, 0, WALL);
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
	  set_translation_speed_limit (0.25);
	}
      move_on_direction (250, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 2:
      if (side == YELLOW)
	reset_x_coord_close ();
      else
	reset_x_coord_far ();
      reset_movement ();
      task_case++;
      break;
    case 3:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      move_on_direction (80, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;

    case 4:
      if (side == BLUE)
	move_full (planter_blue_x_far.x + 120, planter_blue_x_far.y, 0,
	MECHANISM);
      else
	move_full (planter_yellow_x_far.x + 120, planter_yellow_x_far.y, 180,
	MECHANISM);
//      move_full (side * 3000 - (2 * side - 1) * 200,
//		 (planter_array_pointer + task_counter )->y, side * 180, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.4);
	}
      move_on_direction (150, MECHANISM);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      if (!task_init)
	{
	  task_init = true;
	  reset_movement ();
	}
      mechanism_half_down ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 7:
      mechanism_open_slow ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 8:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.5);
	}
      move_on_direction (150, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 9:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      mechanism_down ();
      if (timer_delay_nonblocking (100))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;
    case 100:
      reset_task ();
      task_status = TASK_SUCCESS;
      break;
    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_dropoff_plants_y (uint8_t side)
{
  switch (task_case)
    {
    case 0:
      {
	sensors_case_timer = SENSORS_BACK;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      if (side == BLUE)
	move_full (planter_blue_y.x, planter_blue_y.y - 200, 90, WALL);
      else
	move_full (planter_yellow_y.x, planter_yellow_y.y - 200, 90, WALL);
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
	  set_translation_speed_limit (0.25);
	}
      move_on_direction (250, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 2:
      reset_y_coord_far ();
      reset_movement ();
      task_case++;
      break;
    case 3:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      move_on_direction (80, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;

    case 4:
      if (side == BLUE)
	move_full (planter_blue_y.x, planter_blue_x_far.y - 120, -90,
	MECHANISM);
      else
	move_full (planter_yellow_y.x + 120, planter_yellow_x_far.y, -90,
	MECHANISM);
//      move_full (side * 3000 - (2 * side - 1) * 200,
//		 (planter_array_pointer + task_counter)->y, side * 180, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.4);
	}
      move_on_direction (150, MECHANISM);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      if (!task_init)
	{
	  task_init = true;
	  reset_movement ();
	}
      mechanism_half_down ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 7:
      mechanism_open_slow ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 8:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.5);
	}
      move_on_direction (150, WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 9:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      mechanism_down ();
      if (timer_delay_nonblocking (100))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;
    case 100:
      reset_task ();
      task_status = TASK_SUCCESS;
      break;
    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
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
	task_status = TASK_IN_PROGRESS;
      }
      move_full (solar_central.x - (2 * side - 1) * 250, solar_central.y + 20,
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
      if (timer_delay_nonblocking (1500))
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
      move_full (solar_central.x - (2 * side - 1) * 25, solar_central.y + 20,
		 side * 180,
		 MECHANISM);
//      move_on_direction (225, MECHANISM);
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
      if (timer_delay_nonblocking (1500))
	{
	  task_case++;
	}
      break;
    case 6:
      //      move_full (side * 3000 + (-2 * side + 1) * 740, 190, side * 180, WALL);
//      move_on_direction (225, MECHANISM);
      move_full (solar_central.x + (2 * side - 1) * 200, solar_central.y + 20,
		 side * 180,
		 MECHANISM);
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
      if (timer_delay_nonblocking (1500))
	{
	  task_case++;
	}
      break;
    case 9:
      move_to_angle (-90);
      if (movement_finished () && timer_delay_nonblocking (500))
	task_case++;
    case 10:
      move_on_direction (100, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (500))
	task_case = 100;

    case 100:
      reset_task ();
      task_status = TASK_SUCCESS;
      break;

    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_solar_from_start (uint8_t side) // treba da je vec na 265, 190, 180 za plavu
{					// i na 3000-265, 190, 0 za zutu
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_BACK;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	}
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (1000))
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
      move_full (side * 3000 + (-2 * side + 1) * 500, 210, side * 180, WALL);
//      move_on_direction (235, MECHANISM);      move_full (275, 210, 0, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
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
      if (timer_delay_nonblocking (1000))
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
      move_full (side * 3000 + (-2 * side + 1) * 500, 210, side * 180, WALL);
//      move_on_direction (235, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
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
      if (timer_delay_nonblocking (1000))
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
      reset_task ();
      task_status = TASK_SUCCESS;
      break;

    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
positioning_solar_blue ()	// u polje bilo gde, okrenut na 180
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
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
      move_full (280, robot_position.y_mm, -90, MECHANISM);
//      move_on_direction (150, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_on_direction (450, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	}
      break;
    case 4:
      reset_y_coord_close ();
      reset_movement ();
      set_translation_speed_limit (1.0);
      task_case++;
      break;
    case 5:
      move_to_xy (robot_position.x_mm, 200, MECHANISM);
//      move_on_direction (100, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      move_full (280, 210, 0, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;
    case 7:
      move_full (225, 210, 0, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;

    case 100:
      reset_task ();
      task_status = TASK_SUCCESS;
      break;

    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
positioning_solar_yellow ()	// u polje bilo gde, okrenut na 0
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
//	  continue_movement ();
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
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
      move_full (3000 - 280, robot_position.y_mm, -90, MECHANISM);
//      move_on_direction (150, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_on_direction (450, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	}
      break;
    case 4:
      reset_y_coord_close ();
      reset_movement ();
      set_translation_speed_limit (0.5);
      task_case++;
      break;
    case 5:
      move_to_xy (robot_position.x_mm, 200, MECHANISM);
//      move_on_direction (100, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      move_full (3000 - 280, 210, 180, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;
    case 7:
      move_full (3000 - 225, 210, 180, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;

    case 100:
      reset_task ();
      task_status = TASK_SUCCESS;
      break;

    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
positioning_up_blue (target first_plant)// u polje bilo gde, okrenut na 0
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
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
      move_full (200, robot_position.y_mm, 90, MECHANISM);
//      move_on_direction (150, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_on_direction (450, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	}
      break;
    case 4:
      reset_y_coord_far ();
      reset_movement ();
      set_translation_speed_limit (0.5);
      task_case++;
      break;
    case 5:
      move_full (robot_position.x_mm, 2000 - 200, 90, MECHANISM);
//      move_on_direction (100, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      turn_to_pos (first_plant.x, first_plant.y,
      MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;

    case 100:
      reset_task ();
      task_status = TASK_SUCCESS;
      break;

    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
positioning_up_yellow (target first_plant)// u polje bilo gde, okrenut na 90
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
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
      reset_y_coord_far ();
      reset_movement ();
      task_case++;
      break;
    case 2:
      move_full (robot_position.x_mm, 2000 - 200, 180, MECHANISM);
//      move_on_direction (150, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_on_direction (450, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	}
      break;
    case 4:
      reset_x_coord_far ();
      reset_movement ();
      set_translation_speed_limit (0.5);
      task_case++;
      break;
    case 5:
      move_full (3000 - 200, robot_position.y_mm, 90, MECHANISM);
//      move_on_direction (100, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      turn_to_pos (first_plant.x, first_plant.y,
      MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;

    case 100:
      reset_task ();
      task_status = TASK_SUCCESS;
      break;

    case 200:
      reset_task ();
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

static int8_t
interrupted_func (uint8_t no_targets)
{
  hold_position_with_reg ();
  task_init = false;
  reset_movement ();
  if (task_counter == no_targets)
    return 0;
  else
    return 1;
}

void
reset_task ()
{
  task_init = false;
  task_case = 0;
  task_counter = 0;
}
