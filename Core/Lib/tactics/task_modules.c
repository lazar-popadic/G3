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
volatile uint8_t task_points = 0;

extern float distance;
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

volatile target plant_target_2 =
  { 0, 0 };

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
      solar_in_r ();
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
      solar_in_l ();
      move_to_xy (home.x, home.y, direction);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_points = 10;
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

      mechanism_down ();
      mechanism_open ();
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
	  set_translation_speed_limit (0.3);
	  transition_factor = 100.0;
	  plant_target_2.x = return_x_offset (plant_target.x, plant_target.y,
	  MECHANISM,
					      150 * offset_perc);
	  plant_target_2.y = return_y_offset (plant_target.x, plant_target.y,
	  MECHANISM,
					      150 * offset_perc);

	}
      move_to_xy_offset (plant_target_2.x, plant_target_2.y, MECHANISM,
			 110 * offset_perc);
      task_status = TASK_IN_PROGRESS;
      if (timer_delay_nonblocking (1050))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
	}
      break;
    case 3:
      transition_factor = 1.0;
      mechanism_half_up ();
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
	}
      break;
    case 4:
      mechanism_close ();
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
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
task_pickup_plants_safe (target plant_target, uint8_t side, float offset_perc)
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
	  transition_factor = 1.0;
	}
      if (side == BLUE)
	turn_to_pos (plant_target.x - 360, plant_target.y,
	MECHANISM);
      else
	turn_to_pos (plant_target.x + 360, plant_target.y,
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
	  transition_factor = 1.0;
	}

      mechanism_down ();
      mechanism_open ();
      if (side == BLUE)
	move_to_xy (plant_target.x - 360, plant_target.y,
	MECHANISM);
      else
	move_to_xy (plant_target.x + 360, plant_target.y,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 10;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_1;
	  task_case = RETURN_CASE;
	}
      break;
    case 10:
      if (!task_init)
	{
	  task_init = true;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (1.0);
	  transition_factor = 1.0;
	}

      mechanism_down ();
      mechanism_open ();
      if (side == BLUE)
	move_to_angle (180);
      else
	move_to_angle (0);

      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case = 2;
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
	  set_translation_speed_limit (0.3);
	  transition_factor = 100.0;
	  plant_target_2.x = return_x_offset (plant_target.x, plant_target.y,
	  MECHANISM,
					      150 * offset_perc);
	  plant_target_2.y = return_y_offset (plant_target.x, plant_target.y,
	  MECHANISM,
					      150 * offset_perc);

	}
      move_to_xy_offset (plant_target_2.x, plant_target_2.y, MECHANISM,
			 110 * offset_perc);
      task_status = TASK_IN_PROGRESS;
      if (timer_delay_nonblocking (1050))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
	}
      break;
    case 3:
      transition_factor = 1.0;
      mechanism_half_up ();
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
	}
      break;
    case 4:
      mechanism_close ();
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      if (interrupted)
	{
	  task_status = TASK_FAILED_2;
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
      mechanism_up ();
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
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_half_open ();
      if (timer_delay_nonblocking (400))
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
	  task_points = 5 * 4;
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
	set_rotation_speed_limit (0.4);
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
	set_rotation_speed_limit (0.4);
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
	  set_translation_speed_limit (0.5);
	  set_rotation_speed_limit (0.4);
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
      mechanism_up ();
      if (timer_delay_nonblocking (1500))
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
	  set_translation_speed_limit (1.0);
	}
      mechanism_half_down ();
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 7:
      task_status = TASK_IN_PROGRESS;
      mechanism_half_open ();
      if (timer_delay_nonblocking (400))
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
	  task_points = 5 * 4;
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
task_dropoff_y_2 (uint8_t side, uint8_t first_side)
{
  switch (task_case)
    {
    case 0:
      {
	if (first_side == WALL)
	  sensors_case_timer = SENSORS_HIGH;
	else
	  sensors_case_timer = SENSORS_MECHANISM;
	task_init = true;
	task_status = TASK_IN_PROGRESS;
	set_rotation_speed_limit (0.4);
	set_translation_speed_limit (1.0);
	transition_factor = 1.0;
      }
      if (side == BLUE)
	turn_to_pos (planter_blue_y.x, 2000 - 200, first_side);
      else
	turn_to_pos (planter_yellow_y.x, 2000 - 200, first_side);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	move_to_xy (planter_blue_y.x, 2000 - 300, first_side);
      else
	move_to_xy (planter_yellow_y.x, 2000 - 300, first_side);
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
	  set_rotation_speed_limit (0.4);
	}
      mechanism_up ();
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
      if (timer_delay_nonblocking (500))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      move_to_xy (robot_position.x_mm, 2000 - 60, MECHANISM);
      task_status = TASK_IN_PROGRESS;
      mechanism_half_open ();
      if (timer_delay_nonblocking (400))
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
	  task_points = 5 * 4;
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
task_solar (uint8_t side, uint8_t solar, float speed_limit, uint8_t first_side)
{
  switch (task_case)
    {
    case 0:
      if (!task_init)
	{
	  if (first_side == WALL)
	    sensors_case_timer = SENSORS_HIGH;
	  else
	    sensors_case_timer = SENSORS_MECHANISM;
	  task_init = true;
	  task_status = TASK_IN_PROGRESS;
	  set_rotation_speed_limit (1.0);
	  set_translation_speed_limit (speed_limit);
	}
      mechanism_up ();
      turn_to_pos (side * 3000 - (2 * side - 1) * (320 + solar * 1000), 225,
		   first_side);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      move_to_xy (side * 3000 - (2 * side - 1) * (320 + solar * 1000), 225,
		  first_side);
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
      move_to_xy (side * 3000 - (2 * side - 1) * (775 + solar * 1000), 225,
      MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	}
      if (interrupted)
	{
	  if (distance < 200)
	    task_points = 2 * 5;
	  else
	    task_points = 5;
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
	  if (solar == CENTRAL)
	    task_points = 2 * 5;
	  else
	    task_points = 3 * 5;
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
	  set_rotation_speed_limit (0.5);
	  set_translation_speed_limit (1.0);
	}
      if (side == BLUE)
	turn_to_pos (planter_blue_x_close.x + 550, 2000 - 620,
	MECHANISM);
      else
	turn_to_pos (planter_yellow_x_close.x - 550, 2000 - 620,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	move_to_xy (planter_blue_x_close.x + 550, 2000 - 620,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_close.x - 550, 2000 - 620,
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
	  move_on_direction_2 (328, MECHANISM);
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
	  mechanism_open_almost ();
	  mechanism_open_almost ();
	  mechanism_open_almost ();
	  mechanism_open_almost ();
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_open_almost ();
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
	  task_points = 2 * 4;
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
task_pot_solar (uint8_t side)
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
	turn_to_pos (1000, 350,
	MECHANISM);
      else
	turn_to_pos (2000, 350,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	move_to_xy (1000, 350,
	MECHANISM);
      else
	move_to_xy (2000, 350,
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
      move_to_angle ((1) * 90);
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
	  move_on_direction_2 (328 - 200, MECHANISM);
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
	  mechanism_open_almost ();
	  mechanism_open_almost ();
	  mechanism_open_almost ();
	  mechanism_open_almost ();
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_open_almost ();
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
	move_to_xy (robot_position.x_mm, 500,
	WALL);
      else
	move_to_xy (robot_position.x_mm, 500,
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
task_pot_far (uint8_t side)
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
	turn_to_pos (planter_blue_x_far.x - 550, 640,
	MECHANISM);
      else
	turn_to_pos (planter_yellow_x_far.x + 550, 640,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	move_to_xy (planter_blue_x_far.x - 550, 640,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_far.x + 550, 640,
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
	  move_on_direction_2 (328, MECHANISM);
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
	  mechanism_open_almost ();
	  mechanism_open_almost ();
	  mechanism_open_almost ();
	  mechanism_open_almost ();
	  task_case++;
	  task_init = false;
	}
      break;
    case 5:
      mechanism_open_almost ();
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
	move_to_xy (planter_blue_x_far.x - 500, robot_position.y_mm,
	WALL);
      else
	move_to_xy (planter_yellow_x_far.x + 500, robot_position.y_mm,
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
      mechanism_down ();
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
      mechanism_down ();

      if (side == BLUE)
	move_to_xy (planter_blue_x_close.x + 450, 2000 - 640,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_close.x - 450, 2000 - 640,
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
      mechanism_open ();

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
	  move_on_direction_2 (220, MECHANISM);
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
      move_to_xy (robot_position.x_mm, 2000 - 245,
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
	  task_points = 2 * 4;
	  task_case = RETURN;
	  task_status = TASK_SUCCESS;
	  task_init = false;
	}
      break;
    }
  return task_status;
}

int8_t
task_push_pots_far (uint8_t side)
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
	turn_to_pos (planter_blue_x_far.x - 750, 640,
	MECHANISM);
      else
	turn_to_pos (planter_yellow_x_far.x + 750, 640,
	MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 1:
      if (side == BLUE)
	move_to_xy (planter_blue_x_far.x - 750, 640,
	MECHANISM);
      else
	move_to_xy (planter_yellow_x_far.x + 750, 640,
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
	  move_on_direction_2 (520, MECHANISM);
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
    case 5:
      move_to_xy (robot_position.x_mm, 1000 - 250,
      MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  task_case++;
	  task_init = false;
	}
      break;
    case 6:
      move_to_xy (robot_position.x_mm, 1000 - 500,
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

void
just_set_task_case (uint8_t number)
{
  task_case = number;
}

uint8_t
get_and_reset_task_points ()
{
  uint8_t score = task_points;
  task_points = 0;
  return score;
}
