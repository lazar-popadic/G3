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
tim1_init ();

void
pwm_init ()
{
  tim1_init ();
}

static void
tim1_init ()					// PWM
{
  RCC->APB2ENR |= (0b1 << 0);	//dozvola takta za tim1 pa za gpio port A
  RCC->AHB1ENR |= (0b1 << 0);

  uint8_t const KANAL_1_PWM = 8;
  uint8_t const KANAL_2_PWM = 9;
  //uint8_t const KANAL_3_PWM = 10;
  //uint8_t const KANAL_4_PWM = 11;

  GPIOA->MODER &= ~(0b11 << KANAL_1_PWM * 2);
  GPIOA->MODER &= ~(0b11 << KANAL_2_PWM * 2);
  //GPIOA->MODER &= ~(0b11 << KANAL_3_PWM * 2);
  //GPIOA->MODER &= ~(0b11 << KANAL_4_PWM * 2);

  GPIOA->MODER |= (0b10 << KANAL_1_PWM * 2);
  GPIOA->MODER |= (0b10 << KANAL_2_PWM * 2);
  //GPIOA->MODER |= (0b10 << KANAL_3_PWM * 2);
  //GPIOA->MODER |= (0b10 << KANAL_4_PWM * 2);

  uint8_t const AF = 1;		//alternativna funkcija

  GPIOA->AFR[KANAL_1_PWM / 8] &= ~(0x1111 << (KANAL_1_PWM % 8) * 4);
  GPIOA->AFR[KANAL_2_PWM / 8] &= ~(0x1111 << (KANAL_2_PWM % 8) * 4);
  //GPIOA->AFR[KANAL_3_PWM / 8] &= ~(0x1111 << (KANAL_3_PWM % 8) * 4);
  //GPIOA->AFR[KANAL_4_PWM / 8] &= ~(0x1111 << (KANAL_4_PWM % 8) * 4);

  GPIOA->AFR[KANAL_1_PWM / 8] |= (AF << (KANAL_1_PWM % 8) * 4);
  GPIOA->AFR[KANAL_2_PWM / 8] |= (AF << (KANAL_2_PWM % 8) * 4);
  //GPIOA->AFR[KANAL_3_PWM / 8] |= (AF << (KANAL_3_PWM % 8) * 4);
  //GPIOA->AFR[KANAL_4_PWM / 8] |= (AF << (KANAL_4_PWM % 8) * 4);

  // podesavanje frekvencije PWM-a na 21kHz
  // 84M podeljeno sa 21k je 4000 sto je ARR
  // za rc servoe treba 50Hz, psc=84-1, arr 20000
  // 20000 : 20ms = x : n ms	=>	x = 1000*n	//faktor ispune!! 1000 = najmanji ugao, 2000 = najveci ugao ILI za kontinualne nije ugao nego brzina
  TIM1->PSC = 0;
  TIM1->ARR = 4000 - 1;

  // pwm mode 1
  TIM1->CCMR1 &= ~(0b111 << 4);
  TIM1->CCMR1 |= (0b110 << 4);
  //preload enable
  TIM1->CCMR1 |= (0b1 << 3);
  TIM1->CR1 |= (0b1 << 7);

  //DIR bit: upcounting
  TIM1->CR1 &= ~(0b1 << 4);

  // podesavanje tajmera (kopirano od tim2)
  TIM1->CR1 &= ~((0b1 << 1) || (0b1 << 2)); //sta generise dogadjaj | dozvola dogadjaja ILI obrnuto
  TIM1->EGR |= (0b1 << 0);	// Reinicijalizacija timera
  while (!(TIM1->SR & (0b1 << 0)))
    ;		//cekanje da se izvrsi reinicijalizacija
  TIM1->SR &= ~(0b1 << 0);

  //ukljuceni kanali 1 i 2 PWM-a
  TIM1->CCER |= (0b1 << 0);
  TIM1->CCER |= (0b1 << 4);
  //ukljucen tajmer
  TIM1->CR1 |= (0b1 << 0);

  //FAKTOR ISPUNE
  //TIM1->CCR1 = 2000; //ide do ARR, pa je ovo 50%
}

void
pwm_duty_cycle_out_right_maxon (uint16_t duty_cycle)	// pre ovoga obavezno uradi saturaciju
{								// TODO: razmisli sta ovde guras: int za registar, procenat, apsolutnu brzinu
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
