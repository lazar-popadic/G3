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

#define END_TIME 100*2*1000	// 100 * 2 * 0.5 * 1 000ms = 100s

static void
tim10_init ();

volatile uint32_t sys_time_half_ms = 0;
bool flag_delay = true;
int16_t speed_right = 0, speed_left = 0;

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

      if (!(sys_time_half_ms % 10)) //svakih 5ms
	{
	  odometry_robot ();
	  speed_right = timer_speed_of_encoder_left_maxon();
	  speed_left = timer_speed_of_encoder_left_maxon();
	  regulation_speed (speed_right, speed_left);
	}

      sys_time_half_ms++;
//      sys_time_ms = 0.5 * sys_time_half_ms;
    }
}
