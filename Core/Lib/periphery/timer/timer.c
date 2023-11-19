/*
 * timer.c
 *
 *  Created on: Oct 19, 2023
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
tim9_init ();

volatile uint32_t sys_time_ms = 0; //volatile da kompajler ne vrsi optimizaciju
bool flag_delay = true;

void
timer_init ()
{
  tim9_init ();			// vreme
//  odometrija_init ();		// zasto sam ovo uradio???????????????????????
}

static void
tim9_init ()
{
  RCC->APB2ENR |= (0b1 << 16);
  // 84MHz -> 1kHz
  // 1) 84MHz -> 1MHz
  TIM9->PSC = 84 - 1;		// -1 jer brojimo od 0
  // 2) 1MHz -> 1kHz
  TIM9->ARR = 1000 - 1;

  TIM9->CR1 &= ~((0b1 << 1) || (0b1 << 2)); //sta generise dogadjaj | dozvola dogadjaja ILI obrnuto
  TIM9->EGR |= (0b1 << 0);	// Reinicijalizacija timera
  TIM9->DIER |= (0b1 << 0);	//dozvola prekida
  while (!(TIM9->SR & (0b1 << 0)))
    ;		//cekanje da se izvrsi reinicijalizacija

  TIM9->SR &= ~(0b1 << 0);
  TIM9->CR1 |= (0b1 << 2);

  //odabir prekidne rutine
  uint8_t const TIM9_PREKID = 24;
  NVIC->ISER[0] |= (0b1 << TIM9_PREKID);

  //ne bi trebalo da je jos ukljucen
}

void
timer_start_sys_time ()
{
  TIM9->CR1 |= (0b1 << 0);	//tek ga ovo ukljucuje
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
TIM1_BRK_TIM9_IRQHandler ()
{
  // poziva se svake milisekunde
  // proveri da li je stvarno TIM2 pozvao rutinu
  if ((TIM9->SR & (0b1 << 0)) == (0b1 << 0))
    {
      TIM9->SR &= ~(0b1 << 0);	// da bi sledeci put mogli da detektujemo prekid

      //if(sys_time_ms % 10)//svakih 10ms
      odometrija_robot ();//mozda probaj i brze od 1ms		TODO!!!!!!!!!!!!!!

      sys_time_ms++;
    }
}
