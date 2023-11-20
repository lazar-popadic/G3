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
tim9_init ();

void
pwm_init ()
{
  tim9_init ();
}

static void
tim9_init ()
{
  RCC->AHB1ENR |= (0b1 << 0);
  RCC->APB2ENR |= (0b1 << 16);

  uint8_t const KANAL1 = 2;
  uint8_t const KANAL2 = 3;

  GPIOA->MODER &= ~(0b11 << KANAL1 * 2);
  GPIOA->MODER |= (0b10 << KANAL1 * 2);
  GPIOA->MODER &= ~(0b11 << KANAL2 * 2);
  GPIOA->MODER |= (0b10 << KANAL2 * 2);

  uint8_t const AF = 3;

  GPIOA->AFR[KANAL1 / 8] &= ~(0xF << (KANAL1 % 8) * 4);
  GPIOA->AFR[KANAL1 / 8] |= (AF << (KANAL1 % 8) * 4);
  GPIOA->AFR[KANAL2 / 8] &= ~(0xF << (KANAL2 % 8) * 4);
  GPIOA->AFR[KANAL2 / 8] |= (AF << (KANAL2 % 8) * 4);

  // Željena frekvencija za DC motor: 21kHz
  TIM9->PSC = 0;
  TIM9->ARR = 4000 - 1;
  // Željena frekvencija za RC servo motor: 50Hz

  // Podešavanje "PWM mode 1"
  TIM9->CCMR1 &= ~(0b111 << 4);
  TIM9->CCMR1 |= (0b110 << 4);
  TIM9->CCMR1 &= ~(0b111 << 12);
  TIM9->CCMR1 |= (0b110 << 12);

  // preload, da bi moglo u kodu da se menja
  TIM9->CCMR1 |= (0b1 << 3);
  TIM9->CCMR1 |= (0b1 << 11);
  TIM9->CR1 |= (0b1 << 7);

  TIM9->CR1 &= ~(0b1 << 1); // Dozvola događaja
  TIM9->CR1 &= ~(0b1 << 2); // Šta generiše događaj
  TIM9->EGR |= (0b1 << 0); // Reinicijalizacija tajmera
  while (!(TIM9->SR & (0b1 << 0)))
    {
      __NOP();
    }
  TIM9->SR &= ~(0b1 << 0);

  // Uključujemo kanal PWM-a
  TIM9->CCER |= (0b1 << 0);
  TIM9->CCER |= (0b1 << 4);

  // Uključivanje tajmera
  TIM9->CR1 |= (0b1 << 0);

  //TIM9->CCR1 = 2000; //ide do ARR, pa je ovo 50%
}

void
pwm_duty_cycle_out_right_maxon (uint16_t duty_cycle)// pre ovoga obavezno uradi saturaciju
{
  TIM9->CCR1 = duty_cycle;
}

void
pwm_duty_cycle_out_left_maxon (uint16_t duty_cycle)
{
  TIM9->CCR2 = duty_cycle;
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
