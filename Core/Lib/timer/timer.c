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

#define END_TIME 100*2*1000	// 100 * 2 * 0.5 * 1 000ms = 100s

static void
tim10_init ();

volatile uint32_t sys_time_half_ms = 0;
bool flag_delay = true;
int16_t speed_right = 0, speed_left = 0;
volatile uint8_t sensors_case_timer = 0;
volatile bool sensors_state = false;
extern uint8_t previous_tactic_state;
extern uint8_t tactic_state;

extern volatile int16_t ref_speed_left;
extern volatile int16_t ref_speed_right;

volatile int16_t ref_test = 0;
extern volatile int16_t wheel_position_inc;
volatile float wheel_position_degrees = 0;

volatile int16_t left_buffer[10] =
  { };
volatile int16_t right_buffer[10] =
  { };
volatile uint8_t graph_counter = 0;
volatile float left_average_speed = 0;
volatile float right_average_speed = 0;

extern int16_t Vd_inc;

void
timer_init ()
{
  tim10_init ();			// vreme
  //odometry_init ();		// zasto sam ovo uradio? Mozda jer se odometry zove u prekidu tajmera
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

bool
timer_end ()
{
  if (sys_time_half_ms == END_TIME)
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

void
TIM1_UP_TIM10_IRQHandler ()
{
  // poziva se svakih 0.5ms
  // proveri da li je stvarno TIM10 pozvao rutinu
  if ((TIM10->SR & (0b1 << 0)) == (0b1 << 0))
    {
      TIM10->SR &= ~(0b1 << 0);	// da bi sledeci put mogli da detektujemo prekid

      sys_time_half_ms++;

      if (!(sys_time_half_ms % (10))) //svakih 5ms
	{
	  odometry_robot ();
//TODO:	  prvo regulaciju brzine namesti
//	  ref_speed_left = ref_test;

//TODO:	  pa onda regulaciju pozicije jednog maxona / tocka
//	  left_wheel_position ();
//	  wheel_position_degrees = wheel_position_inc * 360 / 4096;
//	  regulation_single_wheel (ref_test, wheel_position_degrees);

//TODO:	  pa tek onda celu regulaciju pozicije
//	  regulation_position();
	}
//      speed_right = speed_of_encoder_right_maxon ();
//      speed_left = speed_of_encoder_left_maxon ();

//      left_buffer[9] = left_buffer[8];
//      left_buffer[8] = left_buffer[7];
//      left_buffer[7] = left_buffer[6];
//      left_buffer[6] = left_buffer[5];
//      left_buffer[5] = left_buffer[4];
//      left_buffer[4] = left_buffer[3];
//      left_buffer[3] = left_buffer[2];
//      left_buffer[2] = left_buffer[1];
//      left_buffer[1] = left_buffer[0];
//      left_buffer[0] = speed_left;
//      left_average_speed = (left_buffer[0] + left_buffer[1] + left_buffer[2]
//	  + left_buffer[3] + left_buffer[4] + left_buffer[5] + left_buffer[6]
//	  + left_buffer[7] + left_buffer[8] + left_buffer[9]) * 0.1;
//
//      right_buffer[9] = right_buffer[8];
//      right_buffer[8] = right_buffer[7];
//      right_buffer[7] = right_buffer[6];
//      right_buffer[6] = right_buffer[5];
//      right_buffer[5] = right_buffer[4];
//      right_buffer[4] = right_buffer[3];
//      right_buffer[3] = right_buffer[2];
//      right_buffer[2] = right_buffer[1];
//      right_buffer[1] = right_buffer[0];
//      right_buffer[0] = speed_right;
//      right_average_speed = (right_buffer[0] + right_buffer[1] + right_buffer[2]
//	  + right_buffer[3] + right_buffer[4] + right_buffer[5]
//	  + right_buffer[6] + right_buffer[7] + right_buffer[8]
//	  + right_buffer[9]) * 0.1;

//      regulation_speed (right_average_speed, left_average_speed);
//      switch (sensors_case_timer)
//	{
//	case SENSORS_HIGH:
//	  sensors_state = sensors_high ();
//	  break;
//	case SENSORS_LOW:
//	  sensors_state = sensors_low ();
//	  break;
//	case SENSORS_BACK:
//	  sensors_state = sensors_back ();
//	  break;
//	case SENSORS_OFF:
//	  sensors_state = false;
//	  break;
//	default:
//	  sensors_state = false;
//	  break;
//	}
//
//      if (sensors_state)
//	{
//	  previous_tactic_state = tactic_state;
//	  tactic_state = BRAKE;
//	}
    }
}
