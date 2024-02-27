/*
 * tactics.c
 *
 *  Created on: Nov 14, 2023
 *      Author: lazar
 */

#include "main.h"

#define AX_MAX_SPEED 528

uint8_t tactic_state = 0;
uint8_t previous_tactic_state = 0;
bool tactic_state_init = false;
bool tactic_finished;

int16_t dc = 0;
extern volatile uint8_t sensors_case_timer;
extern volatile bool sensors_state;
extern volatile position target_position;
extern volatile position robot_position;
volatile position previous_target =
  { 0, 0, 0 };

volatile uint8_t alternate_move = 0;
extern volatile uint8_t state_angle;

float V_max_perc = 1.0, w_max_perc = 1.0;

volatile target plant_blue1 =
  { 1000, 1300 };
volatile target plant_blue2 =
  { 1000, 700 };
volatile target plant_central1 =
  { 1500, 1500 };
volatile target plant_central2 =
  { 1500, 500 };
volatile target plant_yellow1 =
  { 2000, 1300 };
volatile target plant_yellow2 =
  { 2000, 700 };

volatile target planter_blue1 =
  { 600 + (325 / 2), 2000 - 80 };
volatile target planter_blue2 =
  { 80, 2000 - 450 - (325 / 2) };
volatile target planter_blue3 =
  { 3000 - 80, 450 + (325 / 2) };
volatile target planter_yellow1 =
  { 3000 - 600 - (325 / 2), 2000 - 80 };
volatile target planter_yellow2 =
  { 3000 - 80, 2000 - 450 - (325 / 2) };
volatile target planter_yellow3 =
  { 80, 450 + (325 / 2) };

volatile target home_blue1 =
  { 450, 2000 - 450 };
volatile target home_blue2 =
  { 450, 450 };
volatile target home_blue3 =
  { 3000 - 450, 1000 };
volatile target home_yellow1 =
  { 3000 - 450, 2000 - 450 };
volatile target home_yellow2 =
  { 3000 - 450, 450 };
volatile target home_yellow3 =
  { 450, 1000 };
// TODO: smisli za solare kako cemo

target homes[3];

bool
solar_test ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  set_starting_position (3000 - 450 + 80, 450 - 160, 180);
	  set_translation_speed_limit (0.2);
	  set_rotation_speed_limit (1.0);
	  tactic_finished = false;
	}
      move_full (3000 - 265, 160 + 30, 180, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (2000))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}

      break;

    case 1:
      solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 2:
      solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 3:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  set_translation_speed_limit (1.0);
	  set_rotation_speed_limit (1.0);
	}
      move_to_xy (3000 - 300 - 225, 160 + 30, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 4:
      solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 5:
      solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 6:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  set_translation_speed_limit (1.0);
	  set_rotation_speed_limit (1.0);
	}
      move_to_xy (3000 - 290 - 450, 160 + 30, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 7:
      solar_out_l ();
      if (timer_delay_nonblocking (2000))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 8:
      solar_in_l ();
      if (timer_delay_nonblocking (1000))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 9:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  set_translation_speed_limit (1.0);
	  set_rotation_speed_limit (0.2);
	}
      solar_in_r ();
      if (timer_delay_nonblocking (20))
	move_to_xy (3000 - 450, 2000 - 450, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state_init = false;
	  tactic_state = RETURN;
	}
      break;
    case RETURN:
      tactic_finished = true;
      break;
    }
  return tactic_finished;
}

bool
grabulja_test ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  set_starting_position (3000 - 450 + 80, 450 - 160, 0);
	  tactic_finished = false;
	}
      set_translation_speed_limit (1);
      move_to_xy (1000, 2000 - 450 + 80, WALL);
      ax_move (4, 312, 200);

      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 1:
      set_translation_speed_limit (0.2);
      move_to_xy (1000, 1200, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 2:
      ax_move (7, 512 - 50, 50);
      if (timer_delay_nonblocking (1000))
	tactic_state++;
      break;
    case 3:
      ax_move (4, 800, 200);
      if (timer_delay_nonblocking (1000))
	tactic_state++;
      break;
    case 4:
      ax_move (7, 512 + 100, 200);
      if (timer_delay_nonblocking (1000))
	tactic_state++;
      break;
    case 5:
      set_translation_speed_limit (1.0);
      move_to_xy (180, 2000 - 612.5, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	tactic_state++;
      break;
    case 6:
      set_translation_speed_limit (0.1);
      move_to_xy (80, 2000 - 612.5, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	tactic_state = RETURN;
      break;
    case RETURN:
      tactic_finished = true;
      break;
    }
  return tactic_finished;
}

bool
sensors_timer_test ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  tactic_finished = false;
	  io_led (false);
	  pwm_start ();
	}
      tactic_state++;
      tactic_state_init = false;
      break;
    case SENSORS_HIGH:
      sensors_case_timer = SENSORS_HIGH;
      break;
    case SENSORS_LOW:
      sensors_case_timer = SENSORS_LOW;
      break;
    case SENSORS_BACK:
      sensors_case_timer = SENSORS_BACK;
      break;
    }
  io_led (sensors_state);
  return tactic_finished;
}

bool
movement_test1 ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  set_starting_position (0, 2000 - 80, 0);
	  tactic_finished = false;
	}
      tactic_state++;
      tactic_state_init = false;
      break;

    case 1:
      set_rotation_speed_limit (0.1);
      set_translation_speed_limit (1.0);
      move_to_angle (10);
//      move_to_xy (0, 500, WALL);
      if (movement_finished () && timer_delay_nonblocking (5000))
	{
//	  tactic_state = RETURN;
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;

    case 2:
      set_rotation_speed_limit (1.0);
//      set_translation_speed_limit (1.0);
//      move_to_xy (0, 2000 - 80 - 200, MECHANISM);
      move_to_angle (90);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
//	  tactic_state++;
	  tactic_state = RETURN;
	  tactic_state_init = false;
	}
      break;

    case 3:
      move_to_xy (450, 2000 - 450, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state++;
	}
      break;

    case 4:
      move_to_xy (450, 1550, 0);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state = RETURN;
	  tactic_state_init = false;
	}
      break;

    case RETURN:
      tactic_finished = true;
      break;
    }
  return tactic_finished;
}

bool
go_home_test ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  set_starting_position (450-80, 1000, 0);
	  homes[0] = home_blue2;
	  homes[1] = home_blue1;
	  homes[2] = home_blue3;
	  tactic_finished = false;
	}
      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      if(task_go_home (*homes))
	{
	  tactic_state = RETURN;
	  tactic_state_init = false;
	}
      break;
    case RETURN:
      tactic_finished = true;
      break;

    }
return tactic_finished;
    }

//case BRAKE:
//if (!tactic_state_init)
//  {
//    tactic_state_init = true;
//    previous_target = target_position;
//  }
//target_position = robot_position;
//if (timer_delay_nonblocking(4000))
//  {
//    tactic_state_init = false;
//    tactic_state = alternate_move;
//  }
//if (timer_delay_nonblocking(1000) && !sensors_state)
//  {
//    tactic_state_init = false;
//    tactic_state = previous_tactic_state;
//  }
//break;

//uint8_t
//alternative_move (position brake, uint8_t current_case, uint8_t next_target, uint8_t next_task, uint8_t wait_time, uint8_t number_of_retries)
//{
//  target_position = brake;
//  if (timer_delay_nonblocking(wait_time))
//    {
//      tactic_state = alternate_move;
//    }
//}
