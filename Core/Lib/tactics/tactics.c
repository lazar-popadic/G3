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

bool
grabulja_test ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  tactic_finished = false;
	}
      ax_move (7, 512 + 200, 200);

      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      if (timer_delay_nonblocking (2500))
	tactic_state++;
      break;
    case 2:
      ax_move (4, 850, 250);
      tactic_state++;
      break;
    case 3:
      if (timer_delay_nonblocking (2500))
	tactic_state++;
      break;
    case 4:
      ax_move (7, 512 - 200, 200);
      tactic_state++;
      break;
    case 5:
      if (timer_delay_nonblocking (2500))
	tactic_state++;
      break;
    case 6:
      ax_move (4, 1023, 250);
      tactic_state++;
      break;
    case 7:
      if (timer_delay_nonblocking (2500))
	tactic_state++;
      break;
    case 8:
      xl_angle_move (2, 1023);
      tactic_state++;
      break;
    case 9:
      if (timer_delay_nonblocking (2500))
	tactic_state++;
      break;
    case 10:
      xl_angle_move (2, 0);
      tactic_state++;
      break;
    case 11:
      if (timer_delay_nonblocking (2500))
	tactic_state++;
      break;
    case 12:
      xl_angle_move (3, 1023);
      tactic_state++;
      break;
    case 13:
      if (timer_delay_nonblocking (2500))
	tactic_state++;
      break;
    case 14:
      xl_angle_move (3, 0);
      tactic_state = 0;
      break;
    case 20:
      tactic_finished = true;
      return tactic_finished;
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
