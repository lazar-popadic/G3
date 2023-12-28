/*
 * tactics.c
 *
 *  Created on: Nov 14, 2023
 *      Author: lazar
 */

#include "main.h"

#define AX_MAX_SPEED 528

uint8_t state = 0;
bool state_init = false;
bool tactic_finished;

enum direction
{
  FORWARDS = 1, STOP = 0, BACKWARDS = -1
};

int16_t dc = 0;
enum direction wheel_direction = STOP;
extern volatile uint8_t sensors_case_timer;
extern volatile bool sensors_state;

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
      ax_move (5, 0, AX_MAX_SPEED);

      state++;
      state_init = false;
      break;
    case 1:
      if (timer_delay_nonblocking (2000))
	state++;
      break;
    case 2:
      //inicijalizacija
      //telo stanja
      ax_move (5, 1023, AX_MAX_SPEED);
      //uslov prelaska
      state++;
      break;
    case 3:
      if (timer_delay_nonblocking (2000))
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
	  pwm_start ();
	}
      state++;
      state_init = false;
      break;
    case 1:
      dc = saturation (dc, 2600, 0);
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
	  pwm_start ();
	}
      state++;
      state_init = false;
      break;
    case 1:
      dc += timer_speed_of_encoder_right_passive ();
      dc = saturation (dc, 2500, -2500);

      if (dc > 500)
	{
	  wheel_1_forwards ();
	}
      else if (dc < -500)
	{
	  wheel_1_backwards ();
	}

      pwm_duty_cycle_right (abs (dc));
      pwm_duty_cycle_left (abs (2500 - dc));

      break;
    }
  return tactic_finished;
}

bool
sensors_timer_test ()
{
  switch (state)
    {
    case 0:
      if (!state_init)
	{
	  state_init = true;
	  tactic_finished = false;
	  io_led (false);
	  pwm_start ();
	}
      state++;
      state_init = false;
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
