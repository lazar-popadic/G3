/*
 * pwm.c
 *
 *  Created on: Nov 15, 2023
 *      Author: lazar
 */

#include "../pwm/pwm.h"

#include "stm32f4xx.h"
#include "../regulation/regulation.h"

static void
tim3_init ();
static void
tim4_init ();
static void
io_init ();
//static void
//interrupt_init ();

static uint8_t const TIM3_CH2 = 7;	//PC7
static uint8_t const TIM4_CH1 = 6;	//PB6

//extern ADC_HandleTypeDef hadc1;
//extern DMA_HandleTypeDef hdma_adc1;
//extern uint8_t adc_value[2];

void
pwm_init ()
{
  tim3_init();
  tim4_init ();
  io_init ();
//  interrupt_init ();
}

static void
tim3_init ()
{
  RCC->APB1ENR |= (0b1 << 1);

  // Željena frekvencija za DC motor: 21kHz
  TIM3->PSC = 0;
//  TIM3->ARR = 4000 - 1;
  TIM3->ARR = 1680 - 1;
  // Željena frekvencija za RC servo motor: 50Hz

  // Podešavanje "PWM mode 1" za sva 4 kanala
  TIM3->CCMR1 &= ~(0b111 << 12);
  TIM3->CCMR1 |= (0b110 << 12);

  // preload, da bi moglo u kodu da se menja, za sva 4 kanala
  TIM3->CCMR1 |= (0b1 << 11);
  TIM3->CR1 |= (0b1 << 7);

  // Uključujemo kanal PWM-a
  TIM3->CCER |= (0b1 << 4);

  TIM3->CR1 &= ~(0b1 << 1); 	// Dozvola događaja
  TIM3->CR1 &= ~(0b1 << 2); 	// Šta generiše događaj
  TIM3->EGR |= (0b1 << 0); 	// Reinicijalizacija tajmera
  while (	!(TIM3->SR & (0b1 << 0)))
    ;
  TIM3->SR &= ~(0b1 << 0);
  TIM3->CR1 |= (0b1 << 2);
}

static void
tim4_init ()
{
  RCC->APB1ENR |= (0b1 << 2);

  // Željena frekvencija za DC motor: 21kHz
  TIM4->PSC = 0;
//  TIM4->ARR = 4000 - 1;
  TIM4->ARR = 1680 - 1;

  // Podešavanje "PWM mode 1"
  TIM4->CCMR1 &= ~(0b111 << 4);
  TIM4->CCMR1 |= (0b110 << 4);

  // preload, da bi moglo u kodu da se menja, za sva 4 kanala
  TIM4->CCMR1 |= (0b1 << 3);
  TIM4->CR1 |= (0b1 << 7);

  // Uključujemo kanal PWM-a
  TIM4->CCER |= (0b1 << 0);

  TIM4->CR1 &= ~(0b1 << 1); 	// Dozvola događaja
  TIM4->CR1 &= ~(0b1 << 2); 	// Šta generiše događaj
  TIM4->EGR |= (0b1 << 0); 	// Reinicijalizacija tajmera
  while (!(TIM4->SR & (0b1 << 0)))
    ;
  TIM4->SR &= ~(0b1 << 0);
  TIM4->CR1 |= (0b1 << 2);
}

void
pwm_start ()
{
  // Uključivanje tajmera
  TIM3->CR1 |= (0b1 << 0);
  TIM4->CR1 |= (0b1 << 0);
}

static void
io_init ()
{
  RCC->AHB1ENR |= (0b1 << 1);
  RCC->AHB1ENR |= (0b1 << 2);

  GPIOC->MODER &= ~(0b11 << TIM3_CH2 * 2);
  GPIOB->MODER &= ~(0b11 << TIM4_CH1 * 2);
  GPIOC->MODER |= (0b10 << TIM3_CH2 * 2);
  GPIOB->MODER |= (0b10 << TIM4_CH1 * 2);

  uint8_t const AF = 2;

  GPIOC->AFR[TIM3_CH2 / 8] &= ~(0xF << (TIM3_CH2 % 8) * 4);
  GPIOB->AFR[TIM4_CH1 / 8] &= ~(0xF << (TIM4_CH1 % 8) * 4);
  GPIOC->AFR[TIM3_CH2 / 8] |= (AF << (TIM3_CH2 % 8) * 4);
  GPIOB->AFR[TIM4_CH1 / 8] |= (AF << (TIM4_CH1 % 8) * 4);
}

void
pwm_duty_cycle_right (uint16_t duty_cycle) // pre ovoga obavezno uradi saturaciju
{
  TIM3->CCR2 = duty_cycle;
}

void
pwm_duty_cycle_left (uint16_t duty_cycle)
{
  TIM4->CCR1 = duty_cycle;
}

//static void
//interrupt_init ()
//{
//  TIM3->DIER |= (0b1 << 0);	//dozvola prekida
//  uint8_t const TIM3_INTERRUPT = 29;
//  NVIC->ISER[0] |= (0b1 << TIM3_INTERRUPT);
//}
//
//void
//TIM3_IRQHandler ()
//{
//  if ((TIM3->SR & (0b1 << 0)) == (0b1 << 0))
//    {
//      TIM3->SR &= ~(0b1 << 0);	// da bi sledeci put mogli da detektujemo prekid
//      // ovde zovi adc start
//      HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&adc_value,2);
//    }
//}

/*
 * prag:
 * if (signal > max)
 * return true
 * else
 * return false
 */
