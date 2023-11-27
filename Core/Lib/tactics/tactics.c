/*
 * tactics.c
 *
 *  Created on: Nov 14, 2023
 *      Author: lazar
 */

#include "main.h"

#include "../pwm/pwm.h"

uint8_t state = 0;
bool flag_fsm = true;
uint32_t wait = 0;
bool tactic_finished;

bool
ax_test ()
{
  switch (state)
    {
    case 0:
      //inicijalizacija
      tactic_finished = false;
      io_led (false);
      //telo stanja
      ax_move (4, 1023, 1023);
      ax_move (5, 203, 1023);
      ax_move (6, 674, 1023);
      ax_move (7, 511, 1023);
      //uslov prelaska
      state++;
      break;
    case 1:
      //inicijalizacija
      if (flag_fsm == true)	//da samo jednom udje ovde i zada vrednost wait
	{				//mozda moze ovo samo da se preskoci
	  wait = 3000;//i da dole ide samo: if (timer_delay_nonblocking (3000))
	  flag_fsm = false;
	}
      //telo stanja
      //uslov prelaska
      if (timer_delay_nonblocking (wait))
	{
	  state++;
	  flag_fsm = true;
	}
      break;
    case 2:
      //inicijalizacija
      //telo stanja
      ax_move (4, 0, 1023);
      ax_move (5, 0, 1023);
      ax_move (6, 0, 1023);
      ax_move (7, 0, 1023);
      //uslov prelaska
      state++;
      break;
    case 3:
      //inicijalizacija
      if (flag_fsm == true)
	{
	  wait = 3000;
	  flag_fsm = false;
	}
      //telo stanja
      //uslov prelaska
      if (timer_delay_nonblocking (wait))
	{
	  state ++;
	  flag_fsm = true;
	}
      break;
    case 4:
      tactic_finished = true;
      state = 0;
      return tactic_finished;
    }
  return tactic_finished;
}

bool
ax_test_kraci_fsm ()
{
  switch (state)
    {
    case 0:
      //inicijalizacija
      tactic_finished = false;
      io_led (false);
      //telo stanja

      ax_move (4, 0, 0);
      HAL_Delay(10);
      ax_move (5, 0, 0);
      HAL_Delay(10);
      ax_move (6, 0, 0);
      HAL_Delay(10);
      ax_move (7, 0, 0);
      //uslov prelaska
      state++;
      break;
    case 1:
      //inicijalizacija
      //telo stanja
      //uslov prelaska
      if (timer_delay_nonblocking (1500))
	state++;
      break;
    case 2:
      //inicijalizacija
      //telo stanja
      ax_move (4, 1023, 1023);
      HAL_Delay(10);
      ax_move (5, 1023, 1023);
      HAL_Delay(10);
      ax_move (6, 1023, 1023);
      HAL_Delay(10);
      ax_move (7, 1023, 1023);
      //uslov prelaska
      state++;
      break;
    case 3:
      //inicijalizacija
      //telo stanja
      //uslov prelaska
      if (timer_delay_nonblocking (1500))
	//state++;
	state = 4;
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
        //inicijalizacija
        tactic_finished = false;
        //telo stanja
        pwm_duty_cycle_out_right_maxon(4000);
        pwm_duty_cycle_out_left_maxon(400);
        //uslov prelaska
        state++;
        break;
      case 1:
        //inicijalizacija
        //telo stanja
        //uslov prelaska
        if (timer_delay_nonblocking (25000))
  	state++;
        break;
      case 2:
        //inicijalizacija
        //telo stanja
        pwm_duty_cycle_out_right_maxon(400);
        pwm_duty_cycle_out_left_maxon(4000);
        //uslov prelaska
        state++;
        break;
      case 3:
        tactic_finished = true;
        return tactic_finished;
      }
    return tactic_finished;
}
