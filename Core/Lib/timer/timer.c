/*
 * timer.c
 *
 *  Created on: Oct 19, 2023
 *      Author: lazar
 */

#include "timer.h"
#include "stm32f4xx.h"

#include "../odometry/odometry.h"
#include "../regulation/regulation.h"
#include "../encoder/encoder.h"
#include "../sensors/sensors.h"
#include "../tactics/tactics.h"
#include "../pwm/pwm.h"
#include "../h-bridge/h-bridge.h"
#include <math.h>
#include "../tactics/task_modules.h"
#include "../movement/movement.h"

#define END_TIME 100*2*1000	// 100 * 2 * 0.5 * 1 000ms = 100s
#define HOME_TIME 95*2*1000

#define W_LIMIT		0.5
#define V_LIMIT		0.25

static void
tim10_init ();

extern volatile float V_m_s;
extern volatile float w_rad_s;
extern volatile float V_ref;
extern volatile float w_ref;
extern volatile float transition_factor;

volatile uint16_t sys_time_s = 0;
volatile uint32_t sys_time_half_ms = 0;
bool flag_delay = true;
bool flag_timeout = true;
int16_t speed_right = 0, speed_left = 0;
volatile uint8_t sensors_case_timer = 0;
volatile bool interrupted = false;

volatile bool regulation_on;
const static uint8_t position_loop_freq = 20, speed_loop_freq = 2;	// [ms]
static uint8_t position_loop_cnt = 0, speed_loop_cnt = 0;
volatile bool robot_moving = false;

volatile uint8_t brake = 0;
extern volatile bool movement_init;
extern position robot_position;
extern position target_position;
extern uint8_t regulation_phase;
extern uint8_t state_main;
extern uint8_t tactic_state;

void
timer_init ()
{
  tim10_init ();			// vreme
  position_loop_cnt = position_loop_freq * 2;
  speed_loop_cnt = speed_loop_freq * 2;
}

static void
tim10_init ()
{
  RCC->APB2ENR |= (0b1 << 17);
  // 84MHz -> 2kHz
  // 1) 84MHz -> 1MHz
  TIM10->PSC = 84 - 1;		// -1 jer brojimo od 0
  // 2) 1MHz -> 2kHz
  TIM10->ARR = 500 - 1;

  TIM10->CR1 &= ~((0b1 << 1) || (0b1 << 2)); //sta generise dogadjaj | dozvola dogadjaja ILI obrnuto
  TIM10->EGR |= (0b1 << 0);	// Reinicijalizacija timera
  TIM10->DIER |= (0b1 << 0);	//dozvola prekida
  while (!(TIM10->SR & (0b1 << 0)))
    ;		//cekanje da se izvrsi reinicijalizacija

  TIM10->SR &= ~(0b1 << 0);
  TIM10->CR1 |= (0b1 << 2);

  //odabir prekidne rutine
  uint8_t const TIM10_INTERRUPT = 25;
  NVIC->ISER[0] |= (0b1 << TIM10_INTERRUPT);
}

void
timer_start_sys_time ()
{
  TIM10->CR1 |= (0b1 << 0);	//tek ga ovo ukljucuje
}

void
timer_stop_sys_time ()
{
  TIM10->CR1 &= ~(0b1 << 0);
}

void
reset_and_stop_timer ()
{
  TIM10->CR1 &= ~(0b1 << 0);
  sys_time_half_ms = 0;
  sys_time_s = 0;
}

bool
timer_end ()
{
  if (sys_time_half_ms == END_TIME)
    return true;
  return false;
}

bool
timer_home ()
{
  if (sys_time_half_ms == HOME_TIME)
    return true;
  return false;
}

bool
timer_delay_nonblocking (uint32_t delay_ms)
{
  static uint32_t start_sys_time_half_ms;
  static uint32_t delay_half_ms;
  if (flag_delay == true)				//da samo jednom udje
    {
      start_sys_time_half_ms = sys_time_half_ms;
      delay_half_ms = delay_ms * 2;
      flag_delay = false;
    }

  if (sys_time_half_ms <= start_sys_time_half_ms + delay_half_ms)
    return false;
  flag_delay = true;
  return true;
}



bool
task_timeout (uint32_t delay_ms)
{
  static uint32_t start_sys_time_half_ms;
  static uint32_t delay_half_ms;
  if (flag_timeout == true)				//da samo jednom udje
    {
      start_sys_time_half_ms = sys_time_half_ms;
      delay_half_ms = delay_ms * 2;
      flag_timeout = false;
    }

  if (sys_time_half_ms <= start_sys_time_half_ms + delay_half_ms)
    return false;
  flag_timeout = true;
  return true;
}

void
TIM1_UP_TIM10_IRQHandler ()
{
  // poziva se svake 2ms
  // proveri da li je stvarno TIM10 pozvao rutinu
  if ((TIM10->SR & (0b1 << 0)) == (0b1 << 0))
    {
      TIM10->SR &= ~(0b1 << 0);	// da bi sledeci put mogli da detektujemo prekid

      sys_time_half_ms++;
      sys_time_s = sys_time_half_ms * 0.0005;

      if (!(sys_time_half_ms % position_loop_cnt))
	{
	  odometry_robot ();
	  switch (brake)
	    {
	    case 0:
	      regulation_position ();
	      break;
	    case 1:
//	      V_ref = float_ramp_brake (V_ref, 100);
//	      w_ref = float_ramp_brake (w_ref, 200);
	      V_ref = 0;
	      w_ref = 0;
	      if (!robot_moving)
		{
		  brake = 0;
		  target_position.x_mm = robot_position.x_mm;
		  target_position.y_mm = robot_position.y_mm;
		  target_position.theta_rad = robot_position.theta_rad;
		  regulation_phase = ROT_TO_ANGLE;
		  movement_init = false;
		}
	      break;
	    }
	}

      if (regulation_on)
	{
	  if (!(sys_time_half_ms % speed_loop_cnt))
	    regulation_speed ();
	}
      else
	{
	  stop_right_wheel ();
	  stop_left_wheel ();
	  pwm_duty_cycle_left (0);
	  pwm_duty_cycle_right (0);
	}

      if (fabs (w_rad_s) < W_LIMIT * transition_factor
	  && fabs (V_m_s) < V_LIMIT * transition_factor)
	robot_moving = false;
      else
	robot_moving = true;

      switch (sensors_case_timer)
	{
	case SENSORS_HIGH:
	  interrupted = sensors_high ();
	  break;
	case SENSORS_LOW:
	  interrupted = sensors_low ();
	  break;
	case SENSORS_MECHANISM:
	  interrupted = sensors_back ();
	  break;
	case SENSORS_HIGH_AND_LOW:
	  interrupted = sensors_high () | sensors_low ();
	  break;
	case SENSORS_OFF:
	  interrupted = false;
	  break;
	default:
	  interrupted = false;
	  break;
	}
      if (timer_end ())
	{
	  reset_movement ();
	  reset_task ();
	  state_main = END;
	}

      if (timer_home ())
	{
	  add_points(get_and_reset_task_points());
	  reset_movement ();
	  reset_task ();
	  tactic_state = HOME;
	}

    }
}
