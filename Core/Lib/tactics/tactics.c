/*
 * tactics.c
 *
 *  Created on: Nov 14, 2023
 *      Author: lazar
 */

#include "main.h"

uint8_t state = 0;
bool state_init = false;
bool tactic_finished;

enum direction
{
  FORWARDS = 1, STOP = 0, BACKWARDS = -1
};

uint16_t dc = 0;
enum direction wheel = STOP;


bool
ax_test2 ()
{
  switch (state)
    {
    case 0:
      if (!state_init)
	{
	  state_init = true;
	  tactic_finished = false;
	  io_led (false);
	}
      ax_move (4, 0, 0);
      HAL_Delay (10);
      ax_move (5, 0, 0);
      HAL_Delay (10);
      ax_move (6, 0, 0);
      HAL_Delay (10);
      ax_move (7, 0, 0);

      state++;
      state_init = false;
      break;
    case 1:
      if (timer_delay_nonblocking (1500))
	state++;
      break;
    case 2:
      //inicijalizacija
      //telo stanja
      ax_move (4, 1023, 1023);
      HAL_Delay (10);
      ax_move (5, 1023, 1023);
      HAL_Delay (10);
      ax_move (6, 1023, 1023);
      HAL_Delay (10);
      ax_move (7, 1023, 1023);
      //uslov prelaska
      state++;
      break;
    case 3:
      if (timer_delay_nonblocking (1500))
	state++;
      break;
    case 4:
      tactic_finished = true;
      return tactic_finished;
    }
  return tactic_finished;
}

bool
pwm_test ()
{
  switch (state)
    {
    case 0:
      if (!state_init)
	{
	  state_init = true;
	  tactic_finished = false;
	  io_led (false);

	}
      state++;
      state_init = false;
      break;
    case 1:
      if (wheel == STOP)
	stop_wheel_1 ();
      else if (wheel == FORWARDS)
	wheel_1_forwards ();
      else
	wheel_2_backwards ();
      pwm_duty_cycle_left(dc);

      break;
    }
  return tactic_finished;
}
