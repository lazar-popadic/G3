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

int8_t
task_go_home (target home, uint8_t direction)
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  if (direction == WALL)
	    sensors_case_timer = SENSORS_HIGH;
	  else
	    sensors_case_timer = SENSORS_MECHANISM;
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
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  task_status = TASK_SUCCESS;
	  task_case = RETURN_CASE;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case RETURN_CASE:
      break;
    }
  return task_status;
}

int8_t
task_pickup_plants (target plant_target, float offset_perc)
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_MECHANISM;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 2.5;
	}
      turn_to_pos (plant_target.x, plant_target.y,
      MECHANISM);
      mechanism_down ();
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
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case 2:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (0.24);
	  transition_factor = 4.0;
	  move_to_xy_offset (plant_target.x, plant_target.y, MECHANISM,
			     125 * offset_perc);
	}
      if (timer_delay_nonblocking (1750))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case 3:
      transition_factor = 1.0;
      mechanism_half_up ();
      if (timer_delay_nonblocking (666))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
      mechanism_close ();
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_up ();
      if (timer_delay_nonblocking (500))
	{
	  task_status = TASK_SUCCESS;
	  task_case = RETURN_CASE;
	  task_init = false;
	}
      break;

    case RETURN_CASE:
      break;
    }
  return task_status;
}

int8_t
task_dropoff_x_alt (uint8_t side, uint8_t planter)
{
  switch (task_case)
    {
    case 0:
      {
	sensors_case_timer = SENSORS_HIGH;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      if (planter == CLOSE)
	{
	  if (side == BLUE)
	    turn_to_pos (planter_blue_x_close.x + 750, planter_blue_x_close.y,
	    WALL);
	  else
	    turn_to_pos (planter_yellow_x_close.x - 750,
			 planter_yellow_x_close.y,
			 WALL);
	}
      else
	{
	  if (side == BLUE)
	    turn_to_pos (planter_blue_x_far.x - 750, planter_blue_x_far.y,
	    WALL);
	  else
	    turn_to_pos (planter_yellow_x_far.x + 750, planter_yellow_x_far.y,
	    WALL);
	}
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	}
      if (planter == CLOSE)
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_close.x + 750, planter_blue_x_close.y,
	    WALL);
	  else
	    move_to_xy (planter_yellow_x_close.x - 750,
			planter_yellow_x_close.y,
			WALL);
	}
      else
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_far.x - 750, planter_blue_x_far.y,
	    WALL);
	  else
	    move_to_xy (planter_yellow_x_far.x + 750, planter_yellow_x_far.y,
	    WALL);
	}
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case 2:
      if ((side == BLUE && planter == CLOSE)
	  || (side != BLUE && planter != CLOSE))
	move_to_angle (0);
      else
	move_to_angle (180);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 3:
      if (!task_init)
	{
	  task_status = TASK_IN_PROGRESS;
	  task_init = true;
	  set_translation_speed_limit (0.32);
	  transition_factor = 1.0;
	  move_on_direction (700, MECHANISM);
	}
      if (timer_delay_nonblocking (4000))
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
	  sensors_case_timer = SENSORS_HIGH;
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_translation_speed_limit (0.25);
	}
      if (planter == CLOSE)
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_close.x + 300, robot_position.y_mm,
	    WALL);
	  else
	    move_to_xy (planter_yellow_x_close.x - 300, robot_position.y_mm,
	    WALL);
	}
      else
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_far.x - 300, robot_position.y_mm,
	    WALL);
	  else
	    move_to_xy (planter_yellow_x_far.x + 300, robot_position.y_mm,
	    WALL);
	}
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
	  task_case = RETURN_CASE;
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
	  task_status = TASK_SUCCESS;
	  task_case = RETURN_CASE;
	  task_init = false;
	}
      break;
    case RETURN_CASE:
      break;
    }
  return task_status;
}

int8_t
task_dropoff_x (uint8_t side, uint8_t planter)
{
  switch (task_case)
    {
    case 0:
      {
	sensors_case_timer = SENSORS_HIGH;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      if (planter == CLOSE)
	{
	  if (side == BLUE)
	    turn_to_pos (planter_blue_x_close.x + 200, 1000,
	    WALL);
	  else
	    turn_to_pos (planter_yellow_x_close.x - 200, 1000,
	    WALL);
	}
      else
	{
	  if (side == BLUE)
	    turn_to_pos (planter_blue_x_far.x - 200, 1000,
	    WALL);
	  else
	    turn_to_pos (planter_yellow_x_far.x + 200, 1000,
	    WALL);
	}

      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      {
	sensors_case_timer = SENSORS_HIGH;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
	set_rotation_speed_limit (1.0);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      if (planter == CLOSE)
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_close.x + 200, 1000,
	    WALL);
	  else
	    move_to_xy (planter_yellow_x_close.x - 200, 1000,
	    WALL);
	}
      else
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_far.x - 200, 1000,
	    WALL);
	  else
	    move_to_xy (planter_yellow_x_far.x + 200, 1000,
	    WALL);
	}
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case 2:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_translation_speed_limit (0.42);
	  set_rotation_speed_limit (0.5);
	}
      if (planter == CLOSE)
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_close.x + 200,
			planter_blue_x_close.y + 150,
			WALL);
	  else
	    move_to_xy (planter_yellow_x_close.x - 200,
			planter_yellow_x_close.y + 150,
			WALL);
	}
      else
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_far.x - 200, planter_blue_x_far.y - 150,
	    WALL);
	  else
	    move_to_xy (planter_yellow_x_far.x + 200,
			planter_yellow_x_far.y - 150,
			WALL);
	}
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	  sensors_case_timer = SENSORS_MECHANISM;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
	  task_case = RETURN_CASE;
	}
      break;
    case 3:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_MECHANISM;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	}
      if (planter == CLOSE)
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_close.x + 200, planter_blue_x_close.y,
	    MECHANISM);
	  else
	    move_to_xy (planter_yellow_x_close.x - 200,
			planter_yellow_x_close.y,
			MECHANISM);
	}
      else
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_far.x - 200, planter_blue_x_far.y,
	    MECHANISM);
	  else
	    move_to_xy (planter_yellow_x_far.x + 200, planter_yellow_x_far.y,
	    MECHANISM);
	}
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_3;
	  task_case = RETURN_CASE;
	}
      break;
    case 4:
      if ((side == BLUE && planter == CLOSE)
	  || (side != BLUE && planter != CLOSE))
	move_to_angle (0);
      else
	move_to_angle (180);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      if (!task_init)
	{
	  task_status = TASK_IN_PROGRESS;
	  task_init = true;
	  set_translation_speed_limit (0.32);
	  transition_factor = 1.0;
	  move_on_direction (150, MECHANISM);
	}
      if (timer_delay_nonblocking (2000))
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
	  set_rotation_speed_limit (1.0);
	}
      mechanism_half_down ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 7:
      mechanism_half_open ();
      if (timer_delay_nonblocking (500))
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_case++;
	  task_init = false;
	}
      break;
    case 8:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_translation_speed_limit (0.25);
	}
      if (planter == CLOSE)
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_close.x + 300, robot_position.y_mm,
	    WALL);
	  else
	    move_to_xy (planter_yellow_x_close.x - 300, robot_position.y_mm,
	    WALL);
	}
      else
	{
	  if (side == BLUE)
	    move_to_xy (planter_blue_x_far.x - 300, robot_position.y_mm,
	    WALL);
	  else
	    move_to_xy (planter_yellow_x_far.x + 300, robot_position.y_mm,
	    WALL);
	}
      if (movement_finished () && timer_delay_nonblocking (100))
	{
	  task_case++;
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_4;
	  task_case = RETURN_CASE;
	}
      break;
    case 9:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (1.0);
	}
      mechanism_down ();
      if (timer_delay_nonblocking (20))
	{
	  task_status = TASK_SUCCESS;
	  task_case = RETURN_CASE;
	  task_init = false;
	}
      break;
    case RETURN_CASE:
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
	sensors_case_timer = SENSORS_MECHANISM;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
	set_rotation_speed_limit (0.5);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      if (side == BLUE)
	turn_to_pos (planter_blue_y.x, 2000 - 200, MECHANISM);
      else
	turn_to_pos (planter_yellow_y.x, 2000 - 200, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	move_to_xy (planter_blue_y.x, 2000 - 200, MECHANISM);
      else
	move_to_xy (planter_yellow_y.x, 2000 - 200, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case 2:
      move_to_angle_2 (-90);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	}
      break;
    case 3:
      if (!task_init)
	{
	  task_init = true;
	  set_translation_speed_limit (0.24);
	  set_rotation_speed_limit (0.5);
	}
      move_to_xy (robot_position.x_mm, 2000 - 60, MECHANISM);
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
	}
      mechanism_half_down ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      task_status = TASK_IN_PROGRESS;
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
	  sensors_case_timer = SENSORS_HIGH;
	  task_status = TASK_IN_PROGRESS;
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
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
	  task_case = RETURN_CASE;
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
	  task_status = TASK_SUCCESS;
	  task_case = RETURN_CASE;
	  task_init = false;
	}
      break;
    case RETURN_CASE:
      break;
    }
  return task_status;
}

int8_t
task_solar (uint8_t side, uint8_t solar, float speed_limit)
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
	  set_translation_speed_limit (speed_limit);
	}
      mechanism_up ();
      turn_to_pos (side * 3000 - (2 * side - 1) * (325 + solar * 1000), 220,
      WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      move_to_xy (side * 3000 - (2 * side - 1) * (325 + solar * 1000), 220,
      WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case 2:
      if (!task_init)
	{
	  set_translation_speed_limit (0.24);
	  sensors_case_timer = SENSORS_MECHANISM;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	}
      move_to_angle ((1 - side) * 180);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  set_translation_speed_limit (1.0);
	  task_case++;
	}
      break;
    case 3:
      if (side == BLUE)
	solar_out_l ();
      else
	solar_out_r ();
      if (timer_delay_nonblocking (1000))
	{
	  task_case++;
	}
      break;
    case 4:
      task_status = TASK_IN_PROGRESS;
      set_translation_speed_limit (0.24);
      task_case++;
      break;
    case 5:
      if (!task_init)
	{
	  set_translation_speed_limit (0.24);
	  sensors_case_timer = SENSORS_MECHANISM;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	}
      move_to_xy (side * 3000 - (2 * side - 1) * (325 + 450 + solar * 1000),
		  220,
		  MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
//	  task_case = RETURN_CASE;
	}
      break;
    case 6:
      set_translation_speed_limit (1.0);
      if (side == BLUE)
	solar_in_l ();
      else
	solar_in_r ();
      if (timer_delay_nonblocking (1000))
	{
	  task_status = TASK_SUCCESS;
	  task_case = RETURN_CASE;
	  task_init = false;
	}
      break;

    case RETURN_CASE:
      break;
    }
  return task_status;
}

int8_t
task_pot_reserved (uint8_t side)
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_MECHANISM;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	}
      if (side == BLUE)
	turn_to_pos (planter_blue_x_close.x + 550, 2000 - 640,
	MECHANISM);
      else
	turn_to_pos (planter_yellow_x_close.x - 550, 2000 - 640,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	move_to_xy (planter_blue_x_close.x + 550, 2000 - 640,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_close.x - 550, 2000 - 640,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case 2:
      move_to_angle (side * 180);
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
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (0.5);
	  set_translation_speed_limit (0.5);
	  move_on_direction_2 (325, MECHANISM);
	}
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
      task_status = TASK_IN_PROGRESS;
      mechanism_down_pot ();
      if (timer_delay_nonblocking (500))
	{
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_open ();
      task_case++;
      break;
    case 6:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_HIGH;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (0.25);
	}
      if (side == BLUE)
	move_to_xy (planter_blue_x_close.x + 500, robot_position.y_mm,
	WALL);
      else
	move_to_xy (planter_yellow_x_close.x - 500, robot_position.y_mm,
	WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  set_translation_speed_limit (1.0);
	  task_status = TASK_SUCCESS;
	  task_case = RETURN_CASE;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
	  task_case = RETURN_CASE;
	}
      break;

    case RETURN_CASE:
      break;
    }
  return task_status;
}

int8_t
task_push_pots (uint8_t side)
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  sensors_case_timer = SENSORS_MECHANISM;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	}
      if (side == BLUE)
	turn_to_pos (planter_blue_x_close.x + 750, 2000 - 640,
	MECHANISM);
      else
	turn_to_pos (planter_yellow_x_close.x - 750, 2000 - 640,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	move_to_xy (planter_blue_x_close.x + 750, 2000 - 640,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_close.x - 750, 2000 - 640,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case 2:
      move_to_angle (side * 180);
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
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (0.25);
	  set_translation_speed_limit (0.25);
	  move_on_direction_2 (525, MECHANISM);
	}
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 4:
      move_to_angle (side * 180 + (2 * side - 1) * 60);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      move_to_xy (robot_position.x_mm, 2000 - 250,
      MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      move_to_xy (robot_position.x_mm, 2000 - 500,
      WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  task_case = RETURN;
	  task_status = TASK_SUCCESS;
	  task_init = false;
	}
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

void
set_task_case (uint8_t number)
{
  task_init = false;
  task_case = number;
}
