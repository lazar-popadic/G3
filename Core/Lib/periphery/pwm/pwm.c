/*
 * pwm.c
 *
 *  Created on: Nov 15, 2023
 *      Author: lazar
 */

#include "../../periphery/pwm/pwm.h"

#include "stm32f4xx.h"
#include "../../modules/regulation/regulation.h"

static void
tim1_init ();

void
pwm_init ()
{
  tim1_init ();
}

static void
tim1_init ()
{
  RCC->APB2ENR |= (0b1 << 0);

  // Željena frekvencija za DC motor: 21kHz
  TIM1->PSC = 0;
  TIM1->ARR = 4000 - 1;
  // Željena frekvencija za RC servo motor: 50Hz

  // Podešavanje "PWM mode 1" za sva 4 kanala
  TIM1->CCMR1 &= ~(0b111 << 4);
  TIM1->CCMR1 |= (0b110 << 4);
  TIM1->CCMR1 &= ~(0b111 << 12);
  TIM1->CCMR1 |= (0b110 << 12);
  TIM1->CCMR2 &= ~(0b111 << 4);
  TIM1->CCMR2 |= (0b110 << 4);
  TIM1->CCMR2 &= ~(0b111 << 12);
  TIM1->CCMR2 |= (0b110 << 12);

  // preload, da bi moglo u kodu da se menja, za sva 4 kanala
  TIM1->CCMR1 |= (0b1 << 3);
  TIM1->CCMR1 |= (0b1 << 11);
  TIM1->CCMR2 |= (0b1 << 3);
  TIM1->CCMR2 |= (0b1 << 11);
  TIM1->CR1 |= (0b1 << 7);

  // Uključujemo kanal PWM-a
  TIM1->CCER |= (0b1 << 0);
  TIM1->CCER |= (0b1 << 4);
  TIM1->CCER |= (0b1 << 8);
  TIM1->CCER |= (0b1 << 12);

  // Uključivanje tajmera
  TIM1->CR1 |= (0b1 << 0);

  TIM1->CCR1 = 2000; //ide do ARR, pa je ovo 50%
  TIM1->CCR2 = 1212;
  TIM1->CCR3 = 606;
  TIM1->CCR4 = 4000;

  TIM1->CR1 &= ~(0b1 << 1); // Dozvola događaja
  TIM1->CR1 &= ~(0b1 << 2); // Šta generiše događaj
  TIM1->EGR |= (0b1 << 0); // Reinicijalizacija tajmera
  while (!(TIM1->SR & (0b1 << 0)))
    {
      __NOP();
    }
  TIM1->SR &= ~(0b1 << 0);
}

void
pwm_duty_cycle_out_right_maxon (uint16_t duty_cycle)// pre ovoga obavezno uradi saturaciju
{
  TIM1->CCR1 = duty_cycle;
}

void
pwm_duty_cycle_out_left_maxon (uint16_t duty_cycle)
{
  TIM1->CCR2 = duty_cycle;
}

/*
 * saturacija:
 * if (signal > max)
 * return max
 * if (signal < min)
 * return min
 * return signal
 *
 * prag:
 * if (signal > max)
 * return true
 * else
 * return false
 */
