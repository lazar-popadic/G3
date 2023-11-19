/*
 * timer.c
 *
 *  Created on: Oct 110, 2023
 *      Author: lazar
 */
/*
 TIM2 - za vreme
 TIM3 - enkoder 1
 TIM4 - enkoder 2
 TIM5 - pwm
 */

#include "timer.h"
#include "../../modules/odometrija/odometrija.h"
#include "stm32f4xx.h"

#define END_TIME 20*1000

static void
tim10_init ();

volatile uint32_t sys_time_ms = 0; //volatile da kompajler ne vrsi optimizaciju
bool flag_delay = true;

void
timer_init ()
{
  tim10_init ();			// vreme
//  odometrija_init ();		// zasto sam ovo uradio???????????????????????
}

static void
tim10_init ()
{
  RCC->APB2ENR |= (0b1 << 17);
  // 84MHz -> 1kHz
  // 1) 84MHz -> 1MHz
  TIM10->PSC = 84 - 1;		// -1 jer brojimo od 0
  // 2) 1MHz -> 1kHz
  TIM10->ARR = 1000 - 1;

  TIM10->CR1 &= ~((0b1 << 1) || (0b1 << 2)); //sta generise dogadjaj | dozvola dogadjaja ILI obrnuto
  TIM10->EGR |= (0b1 << 0);	// Reinicijalizacija timera
  TIM10->DIER |= (0b1 << 0);	//dozvola prekida
  while (!(TIM10->SR & (0b1 << 0)))
    ;		//cekanje da se izvrsi reinicijalizacija

  TIM10->SR &= ~(0b1 << 0);
  TIM10->CR1 |= (0b1 << 2);

  //odabir prekidne rutine
  uint8_t const TIM10_PREKID = 25;
  NVIC->ISER[0] |= (0b1 << TIM10_PREKID);

  //ne bi trebalo da je jos ukljucen
}

void
timer_start_sys_time ()
{
  TIM10->CR1 |= (0b1 << 0);	//tek ga ovo ukljucuje
}

bool
timer_end ()
{
  if (sys_time_ms == END_TIME)
    return true;
  return false;
}

bool
timer_delay_nonblocking (uint32_t delay_ms)
{
  static uint32_t start_sys_time_ms;
  if (flag_delay == true)				//da samo jednom udje
    {
      start_sys_time_ms = sys_time_ms;
      flag_delay = false;
    }

  if (sys_time_ms <= start_sys_time_ms + delay_ms)
    return false;
  flag_delay = true;
  return true;
}

void
TIM1_UP_TIM10_IRQHandler ()
{
  // poziva se svake milisekunde
  // proveri da li je stvarno TIM2 pozvao rutinu
  if ((TIM10->SR & (0b1 << 0)) == (0b1 << 0))
    {
      TIM10->SR &= ~(0b1 << 0);	// da bi sledeci put mogli da detektujemo prekid

      //if(sys_time_ms % 10)//svakih 10ms
      odometrija_robot ();//mozda probaj i brze od 1ms		TODO!!!!!!!!!!!!!!

      sys_time_ms++;
    }
}
