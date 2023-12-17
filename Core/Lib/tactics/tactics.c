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
      ax_move (6, 0, 0);

      state++;
      state_init = false;
      break;
    case 1:
      if (timer_delay_nonblocking (1000))
	state++;
      break;
    case 2:
      //inicijalizacija
      //telo stanja
      ax_move (6, 1023, 0);
      //uslov prelaska
      state++;
      break;
    case 3:
      if (timer_delay_nonblocking (1000))
	state = 0;
      break;
    case 4:
      tactic_finished = true;
      return tactic_finished;
    }
  return tactic_finished;
}

bool
grabulja_test ()
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
      ax_move (7, 364, 100);

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
      ax_move (7, 1023, 100);
      //uslov prelaska
      state++;
      break;
    case 3:
      if (timer_delay_nonblocking (1500))
	state = 0;
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
	  dc = saturation(dc, 2800, 0);
	  pwm_duty_cycle_right (dc);
	  pwm_duty_cycle_left (dc / 2);

	  if (button_pressed ())
	    {
	      io_led (true);
	      wheel_1_forwards ();
	    }
	  else
	    {
	      io_led (false);
	      wheel_1_backwards ();
	    }
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
      dc += timer_speed_of_encoder_right_passive();
      dc = saturation (dc, 2500, -2500);


      if (dc > 500)
      {
    	  wheel_1_forwards ();
      }
      else if (dc < -500)
      {
    	  wheel_1_backwards ();
      }

      pwm_duty_cycle_right(abs (dc));
      pwm_duty_cycle_left(abs (2824 - dc));

      break;
    }
  return tactic_finished;
}
