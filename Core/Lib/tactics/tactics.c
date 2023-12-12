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

int16_t dc = 0;
enum direction wheel_direction = STOP;

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
      ax_move (4, 511, 0);
      HAL_Delay (10);
      ax_move (5, 511, 0);
      HAL_Delay (10);
      ax_move (6, 511, 0);
      HAL_Delay (10);
      ax_move (7, 511, 0);

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
      if (wheel_direction == BACKWARDS)
	wheel_2_backwards ();
      else if (wheel_direction == FORWARDS)
	wheel_2_forwards ();
      else
	stop_wheel_1 ();
      pwm_duty_cycle_left (abs (dc));
      break;
    }
  return tactic_finished;
}

bool
pwm_test2 ()
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
      dc += timer_speed_of_encoder_left_passive();
      dc = saturation (dc, 2824, -2824);


      if (dc > 500)
      {
    	  //wheel_1_forwards ();
    	  wheel_2_forwards ();
      }
      else if (dc < -500)
      {
    	  //wheel_1_backwards ();
    	  wheel_2_backwards ();
      }

      pwm_duty_cycle_right(abs (dc));
      pwm_duty_cycle_left(abs (2824 - dc));

      break;
    }
  return tactic_finished;
}
