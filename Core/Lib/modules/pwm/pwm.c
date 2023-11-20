/*
 * pwm.c
 *
 *  Created on: Nov 15, 2023
 *      Author: lazar
 */

#include "pwm.h"
#include "stm32f4xx.h"
#include "../regulation/regulation.h"

static void
tim5_init ();

void
pwm_init ()
{
  tim5_init ();
}

static void
tim5_init ()
{
  RCC->AHB1ENR |= (0b1 << 0);
  RCC->APB1ENR |= (0b1 << 3);

  uint8_t const KANAL1 = 0; // PA0
  uint8_t const KANAL2 = 1; // PA1

  GPIOA->MODER &= ~(0b11 << KANAL1 * 2);
  GPIOA->MODER &= ~(0b11 << KANAL2 * 2);
  GPIOA->MODER |= (0b10 << KANAL1 * 2);
  GPIOA->MODER |= (0b10 << KANAL2 * 2);

  uint8_t const AF = 2;

  GPIOA->AFR[KANAL1 / 8] &= ~(0xF << (KANAL1 % 8) * 4);
  GPIOA->AFR[KANAL2 / 8] &= ~(0xF << (KANAL2 % 8) * 4);
  GPIOA->AFR[KANAL1 / 8] |= (AF << (KANAL1 % 8) * 4);
  GPIOA->AFR[KANAL2 / 8] |= (AF << (KANAL2 % 8) * 4);

  // Željena frekvencija za DC motor: 21kHz
  TIM5->PSC = 0;
  TIM5->ARR = 4000 - 1;
  // Željena frekvencija za RC servo motor: 50Hz

  // Podešavanje "PWM mode 1"
  TIM5->CCMR1 &= ~(0b111 << 4);
  TIM5->CCMR1 |= (0b110 << 4);
  TIM5->CCMR1 &= ~(0b111 << 12);
  TIM5->CCMR1 |= (0b110 << 12);

  TIM5->CCMR1 |= (0b1 << 3);
  TIM5->CR1 |= (0b1 << 7);

  TIM5->CR1 &= ~(0b1 << 1); // Dozvola događaja
  TIM5->CR1 &= ~(0b1 << 2); // Šta generiše događaj
  TIM5->EGR |= (0b1 << 0); // Reinicijalizacija tajmera
  while (!(TIM5->SR & (0b1 << 0)))
    {
      __NOP();
    }
  TIM5->SR &= ~(0b1 << 0);

  // Uključujemo kanal PWM-a
  TIM5->CCER |= (0b1 << 0);
  TIM5->CCER |= (0b1 << 4);

  // Uključivanje tajmera
  TIM5->CR1 |= (0b1 << 0);

  //TIM5->CCR1 = 2000; //ide do ARR, pa je ovo 50%
}

void
pwm_duty_cycle_out_right_maxon (uint16_t duty_cycle)// pre ovoga obavezno uradi saturaciju
{// TODO: razmisli sta ovde guras: int za registar, procenat, apsolutnu brzinu
  TIM5->CCR1 = duty_cycle;
}

void
pwm_duty_cycle_out_left_maxon (uint16_t duty_cycle)
{
  TIM5->CCR2 = duty_cycle;
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
