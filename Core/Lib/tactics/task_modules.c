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
volatile uint8_t plant_counter = 0;

int8_t
task_go_home (target home, uint8_t direction, uint8_t sensors)
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = sensors;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
      turn_to_pos (home.x, home.y, direction);
      if (movement_finished () && timer_delay_nonblocking (20))
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
      move_to_xy (home.x, home.y, direction);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 100;
	}
      if (interrupted)
	{
	  task_case = 200;
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
task_pickup_plants (target plant_target)
{
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
	  transition_factor = 2.5;
	}
      turn_to_pos (plant_target.x, plant_target.y,
      MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
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
      move_to_xy_offset (plant_target.x, plant_target.y, MECHANISM, -320);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
//      if (interrupted)
//	{
//	  task_case = 200;
//	}
      break;
    case 2:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (0.16);
	  transition_factor = 4.0;
	  move_to_xy_offset (plant_target.x, plant_target.y, MECHANISM, 75);
	}
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
//      if (interrupted)
//	{
//	  task_case = 200;
//	}
      break;
    case 3:
      transition_factor = 1.0;
      mechanism_half_up ();
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
      mechanism_close ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_up ();
      if (timer_delay_nonblocking (500))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;

    case 100:
      task_status = TASK_SUCCESS;
      plant_counter++;
      break;

    case 200:
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_dropoff_x_close_2 (uint8_t side)
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
	move_to_xy (planter_blue_x_close.x + 110, 1000,
	WALL);
      else
	move_to_xy (planter_yellow_x_close.x - 110, 1000,
	WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.3);
	  set_rotation_speed_limit (0.25);
	}
      if (side == BLUE)
	move_to_xy (planter_blue_x_close.x + 110, planter_blue_x_close.y + 150,
	WALL);
      else
	move_to_xy (planter_yellow_x_close.x - 110,
		    planter_yellow_x_close.y + 150,
		    WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 2:
      if (side == BLUE)
	move_to_xy (planter_blue_x_close.x + 110, planter_blue_x_close.y,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_close.x - 110, planter_yellow_x_close.y,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.16);
	}
      if (side == BLUE)
	move_to_xy (planter_blue_x_close.x, robot_position.y_mm,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_close.x, robot_position.y_mm,
	MECHANISM);
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
      if (!task_init)
	{
	  task_init = true;
	  reset_movement ();
	  set_rotation_speed_limit (1.0);
	}
      mechanism_half_down ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_half_open ();
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.25);
	}
      if (side == BLUE)
	move_to_xy (planter_blue_x_close.x + 300, robot_position.y_mm, WALL);
      else
	move_to_xy (planter_yellow_x_close.x - 300, robot_position.y_mm, WALL);

      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  task_init = false;
	}
      break;
    case 7:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      mechanism_down ();
      if (timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;
    case 100:
      task_status = TASK_SUCCESS;
      break;
    case 200:
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_dropoff_x_far_2 (uint8_t side)
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
	move_to_xy (planter_blue_x_far.x - 110, 1000,
	WALL);
      else
	move_to_xy (planter_yellow_x_far.x + 110, 1000,
	WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.3);
	  set_rotation_speed_limit (0.25);
	}
      if (side == BLUE)
	move_to_xy (planter_blue_x_far.x - 110, planter_blue_x_far.y - 150,
	WALL);
      else
	move_to_xy (planter_yellow_x_far.x + 110,
		    planter_yellow_x_far.y - 150,
		    WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 2:
      if (side == BLUE)
	move_to_xy (planter_blue_x_far.x - 110, planter_blue_x_far.y,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_far.x + 110, planter_yellow_x_far.y,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.16);
	}
      if (side == BLUE)
	move_to_xy (planter_blue_x_far.x, robot_position.y_mm,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_far.x, robot_position.y_mm,
	MECHANISM);
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
      if (!task_init)
	{
	  task_init = true;
	  reset_movement ();
	  set_rotation_speed_limit (1.0);
	}
      mechanism_half_down ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_half_open ();
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.25);
	}
      if (side == BLUE)
	move_to_xy (planter_blue_x_far.x - 300, robot_position.y_mm, WALL);
      else
	move_to_xy (planter_yellow_x_far.x + 300, robot_position.y_mm, WALL);

      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  task_init = false;
	}
      break;
    case 7:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      mechanism_down ();
      if (timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;
    case 100:
      task_status = TASK_SUCCESS;
      break;
    case 200:
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_dropoff_y_2 (uint8_t side)
{
  switch (task_case)
    {
    case 0:
      {
	sensors_case_timer = SENSORS_BACK;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
	set_rotation_speed_limit (0.25);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      if (side == BLUE)
	move_to_xy (planter_blue_y.x, 2000 - 200, WALL);
      else
	move_to_xy (planter_yellow_y.x, 2000 - 200, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      move_to_angle_2 (-90);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	}
      break;

    case 2:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.16);
	  set_rotation_speed_limit (0.25);
	}
      move_to_xy (robot_position.x_mm, 2000 - 70, MECHANISM);
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      if (!task_init)
	{
	  task_init = true;
	  reset_movement ();
	}
      mechanism_half_down ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
      mechanism_half_open ();
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.25);
	  set_rotation_speed_limit (1.0);
	}
      move_to_xy (robot_position.x_mm, 2000 - 300, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  task_init = false;
	}
      break;
    case 6:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      mechanism_down ();
      if (timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;
    case 100:
      task_status = TASK_SUCCESS;
      break;
    case 200:
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_central_solar_long (uint8_t side, uint8_t first_dir)
{
  switch (task_case)
    {
    case 0:
      {
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	set_transition_factor (2.0);
	if (first_dir == MECHANISM)
	  sensors_case_timer = SENSORS_BACK;
	else
	  sensors_case_timer = SENSORS_HIGH;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
      }
      move_to_xy (solar_central.x - (2 * side - 1) * 250, solar_central.y + 25,
		  first_dir);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      move_to_angle_2 (side * 180);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	}
      break;
    case 2:
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
    case 3:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1500))
	{
	  task_case++;
	}
      break;
    case 4:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
      move_full (solar_central.x - (2 * side - 1) * 25, solar_central.y + 25,
		 side * 180,
		 MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      //      if (interrupted)
      //	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 5:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 6:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1500))
	{
	  task_case++;
	}
      break;
    case 7:
      move_full (solar_central.x + (2 * side - 1) * 200, solar_central.y + 25,
		 side * 180,
		 MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	}
      //      if (interrupted)
      //	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 8:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 9:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1500))
	{
	  task_case = 100;
	}
      break;

    case 100:
      task_status = TASK_SUCCESS;
      break;

    case 200:
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
task_central_solar_short (uint8_t side, uint8_t first_dir)
{
  switch (task_case)
    {
    case 0:
      {
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	set_transition_factor (2.0);
	if (first_dir == MECHANISM)
	  sensors_case_timer = SENSORS_BACK;
	else
	  sensors_case_timer = SENSORS_HIGH;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
      }
      move_to_xy (solar_central.x + (2 * side - 1) * 230, solar_central.y + 25,
		  first_dir);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      move_to_angle_2 (side * 180);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	}
      break;
    case 2:
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
    case 3:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1500))
	{
	  task_case++;
	}
      break;
    case 4:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}
      move_full (solar_central.x - (2 * side - 1) * 45, solar_central.y + 25,
		 side * 180,
		 WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  task_init = false;
	}
      //      if (interrupted)
      //	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 5:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 6:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1500))
	{
	  task_case++;
	}
      break;
    case 7:
      move_full (solar_central.x - (2 * side - 1) * 270, solar_central.y + 25,
		 side * 180,
		 WALL);
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	}
      //      if (interrupted)
      //	ako bas treba uradi da odmah vrati task_failed i da predje na sledece
      break;
    case 8:
      if (side == BLUE)
	solar_out_r ();
      else
	solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  task_case++;
	}
      break;
    case 9:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1500))
	{
	  task_case = 100;
	}
      break;

    case 100:
      task_status = TASK_SUCCESS;
      break;

    case 200:
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

    case 100:
      task_status = TASK_SUCCESS;
      break;

    case 200:
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

int8_t
reserved_solar (uint8_t side)
{
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
	}
      move_full (side * 3000 - (2 * side - 1) * 725, 200, (1 - side) * 180,
      WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
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
    case 2:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_to_xy (side * 3000 - (2 * side - 1) * 500, 200, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
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
    case 5:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      move_to_xy (side * 3000 - (2 * side - 1) * 275, 200, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 7:
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
    case 8:
      if (side == BLUE)
	solar_in_r ();
      else
	solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;

    case 100:
      task_status = TASK_SUCCESS;
      break;

    case 200:
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
      move_to_xy (robot_position.x_mm + 450, robot_position.y_mm, WALL);

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
      move_to_xy (280, robot_position.y_mm, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_to_xy (robot_position.x_mm, robot_position.y_mm - 450, WALL);
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
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      move_to_xy (280, 210, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 100;
	  task_init = false;
	}
      break;

    case 100:
      task_status = TASK_SUCCESS;
      break;

    case 200:
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
      move_to_xy (robot_position.x_mm + 450, robot_position.y_mm, WALL);
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
      move_to_xy (3000 - 280, robot_position.y_mm, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_to_xy (robot_position.x_mm, robot_position.y_mm - 450, WALL);
      if (timer_delay_nonblocking (4000))
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

    case 100:
      task_status = TASK_SUCCESS;
      break;

    case 200:
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
	  set_translation_speed_limit (0.2);
	  transition_factor = 1.0;
	}
      move_to_xy (robot_position.x_mm + 450, robot_position.y_mm, WALL);

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
      move_to_xy (200, robot_position.y_mm, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_to_xy (robot_position.x_mm, robot_position.y_mm + 450, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	}
      break;
    case 4:
      reset_y_coord_far ();
      reset_movement ();
      set_translation_speed_limit (0.2);
      task_case++;
      break;
    case 5:
      move_to_xy (robot_position.x_mm, 2000 - 200, MECHANISM);
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
      task_status = TASK_SUCCESS;
      break;

    case 200:
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
	  set_translation_speed_limit (0.2);
	  transition_factor = 1.0;
	}
      move_to_xy (robot_position.x_mm + 450, robot_position.y_mm, WALL);

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
      move_to_xy (3000 - 300, robot_position.y_mm, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      move_to_xy (robot_position.x_mm, robot_position.y_mm + 450, WALL);
      if (timer_delay_nonblocking (3000))
	{
	  task_case++;
	}
      break;
    case 4:
      reset_y_coord_far ();
      reset_movement ();
      set_translation_speed_limit (0.2);
      task_case++;
      break;
    case 5:
      move_to_xy (robot_position.x_mm, 2000 - 200, MECHANISM);
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
      task_status = TASK_SUCCESS;
      break;

    case 200:
      task_status = TASK_FAILED;
      break;
    }
  return task_status;
}

void
reset_task ()
{
  task_init = false;
  task_case = 0;
}
