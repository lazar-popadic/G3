/*
 * tactics.c
 *
 *  Created on: Nov 14, 2023
 *      Author: lazar
 */

#include "main.h"

  uint8_t state = 0;
  bool flag_fsm = true;
  uint32_t wait = 0;
  bool tactic_finished;

bool
ax_test()
{
  switch (state)
  	{
  	case 0:
  	  //inicijalizacija
  	  tactic_finished = false;
  	  io_led(false);
  	  //telo stanja
  	  ax_move (4, 1023, 21);
  	  ax_move (5, 203, 554);
  	  ax_move (6, 674, 825);
  	  ax_move (7, 511, 100);
  	  //uslov prelaska
  	  state++;
  	  break;
  	case 1:
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
  	      state++;
  	      flag_fsm = true;
  	    }
  	  break;
  	case 2:
  	  //inicijalizacija
  	  //telo stanja
  	  ax_move (4, 1023, 21);
  	  ax_move (5, 203, 554);
  	  ax_move (6, 674, 825);
  	  ax_move (7, 511, 100);
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
  	      state++;
  	      flag_fsm = true;
  	    }
  	  break;
  	case 4:
  	  tactic_finished = true;
  	  return tactic_finished;
  	}
  return tactic_finished;
}
