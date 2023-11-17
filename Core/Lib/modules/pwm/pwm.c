/*
 * pwm.c
 *
 *  Created on: Nov 15, 2023
 *      Author: lazar
 */

#include "pwm.h"

static void
tim5_init ();

void
pwm_init ()
{
  tim5_init ();
}

static void
tim5_init ()					// PWM
{
  RCC->APB1ENR |= (0b1 << 3);	//dozvola takta za tim5 pa za gpio port A
  RCC->AHB1ENR |= (0b1 << 0);

  uint8_t const KANAL_1_PWM = 0;
  uint8_t const KANAL_2_PWM = 1;
  uint8_t const KANAL_3_PWM = 2;
  uint8_t const KANAL_4_PWM = 3;

  GPIOA->MODER &= ~(0b11 << KANAL_1_PWM * 2);
  GPIOA->MODER &= ~(0b11 << KANAL_2_PWM * 2);
  GPIOA->MODER &= ~(0b11 << KANAL_3_PWM * 2);
  GPIOA->MODER &= ~(0b11 << KANAL_4_PWM * 2);

  GPIOA->MODER |= (0b10 << KANAL_1_PWM * 2);
  GPIOA->MODER |= (0b10 << KANAL_2_PWM * 2);
  GPIOA->MODER |= (0b10 << KANAL_3_PWM * 2);
  GPIOA->MODER |= (0b10 << KANAL_4_PWM * 2);

  uint8_t const AF = 2;		//alternativna funkcija

  GPIOA->AFR[KANAL_1_PWM / 8] &= ~(0x1111 << (KANAL_1_PWM % 8) * 4);
  GPIOA->AFR[KANAL_2_PWM / 8] &= ~(0x1111 << (KANAL_2_PWM % 8) * 4);
  GPIOA->AFR[KANAL_3_PWM / 8] &= ~(0x1111 << (KANAL_3_PWM % 8) * 4);
  GPIOA->AFR[KANAL_4_PWM / 8] &= ~(0x1111 << (KANAL_4_PWM % 8) * 4);

  GPIOA->AFR[KANAL_1_PWM / 8] |= (AF << (KANAL_1_PWM % 8) * 4);
  GPIOA->AFR[KANAL_2_PWM / 8] |= (AF << (KANAL_2_PWM % 8) * 4);
  GPIOA->AFR[KANAL_3_PWM / 8] |= (AF << (KANAL_3_PWM % 8) * 4);
  GPIOA->AFR[KANAL_4_PWM / 8] |= (AF << (KANAL_4_PWM % 8) * 4);

  // podesavanje frekvencije PWM-a na 21kHz
  // 84M podeljeno sa 21k je 4000 sto je ARR
  // za rc servoe treba 50Hz, psc=84-1, arr 20000
  // 20000 : 20ms = x : n ms	=>	x = 1000*n	//faktor ispune!! 1000 = najmanji ugao, 2000 = najveci ugao ILI za kontinualne nije ugao nego brzina
  TIM5->PSC = 0;
  TIM5->ARR = 4000 - 1;

  // TODO: uradi i za ostale kanale, ukupno da budu 2
  // pwm mode 1
  TIM5->CCMR1 &= ~(0b111 << 4);
  TIM5->CCMR1 |= (0b110 << 4);
  //preload enable
  TIM5->CCMR1 |= (0b1 << 3);
  TIM5->CR1 |= (0b1 << 7);

  // podesavanje tajmera (kopirano od tim2)
  TIM5->CR1 &= ~((0b1 << 1) || (0b1 << 2)); //sta generise dogadjaj | dozvola dogadjaja ILI obrnuto
  TIM5->EGR |= (0b1 << 0);	// Reinicijalizacija timera
  while (!(TIM5->SR & (0b1 << 0)))
    ;		//cekanje da se izvrsi reinicijalizacija
  TIM5->SR &= ~(0b1 << 0);

  //ukljucen kanal 1 PWM-a
  TIM5->CCER |= (0b1 << 0);
  //ukljucen tajmer
  TIM5->CR1 |= (0b1 << 0);

  //FAKTOR ISPUNE
  //TIM5->CCR1 = 2000; //ide do ARR, pa je ovo 50%
}

void
pwm_duty_cycle (uint16_t duty_cycle)	//FAKTOR ISPUNE: ide od 0 do ARR. TODO: razmisli da stavis u procente
{
  TIM5->CCR1 = duty_cycle;
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
