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
tim9_init ();
static void
io_init ();
static void
interrupt_init ();

static uint8_t const PWM_KANAL1 = 2;
static uint8_t const PWM_KANAL2 = 3;
volatile uint32_t irq_counter = 0;

void
pwm_init ()
{
  tim9_init ();
  io_init ();
  interrupt_init ();
}

static void
tim9_init ()
{
  RCC->APB2ENR |= (0b1 << 16);

  // Željena frekvencija za DC motor: 21kHz
  TIM9->PSC = 0;
  TIM9->ARR = 4000 - 1;
  // Željena frekvencija za RC servo motor: 50Hz

  // Podešavanje "PWM mode 1" za sva 4 kanala
  TIM9->CCMR1 &= ~(0b111 << 4);
  TIM9->CCMR1 |= (0b110 << 4);
  TIM9->CCMR1 &= ~(0b111 << 12);
  TIM9->CCMR1 |= (0b110 << 12);

  // preload, da bi moglo u kodu da se menja, za sva 4 kanala
  TIM9->CCMR1 |= (0b1 << 3);
  TIM9->CCMR1 |= (0b1 << 11);
  TIM9->CR1 |= (0b1 << 7);

  // Uključujemo kanal PWM-a
  TIM9->CCER |= (0b1 << 0);
  TIM9->CCER |= (0b1 << 4);

  TIM9->CR1 &= ~(0b1 << 1); 	// Dozvola događaja
  TIM9->CR1 &= ~(0b1 << 2); 	// Šta generiše događaj
  TIM9->EGR |= (0b1 << 0); 	// Reinicijalizacija tajmera
  while (!(TIM9->SR & (0b1 << 0)))
    ;
  TIM9->SR &= ~(0b1 << 0);
  TIM9->CR1 |= (0b1 << 2);
}

void
pwm_start ()
{
  // Uključivanje tajmera
  TIM9->CR1 |= (0b1 << 0);
}

static void
io_init ()	// tim5
{
  RCC->AHB1ENR |= (0b1 << 0);

  GPIOA->MODER &= ~(0b11 << PWM_KANAL1 * 2);
  GPIOA->MODER &= ~(0b11 << PWM_KANAL2 * 2);
  GPIOA->MODER |= (0b10 << PWM_KANAL1 * 2);
  GPIOA->MODER |= (0b10 << PWM_KANAL2 * 2);

  uint8_t const AF = 3;

  GPIOA->AFR[PWM_KANAL1 / 8] &= ~(0xF << (PWM_KANAL1 % 8) * 4);
  GPIOA->AFR[PWM_KANAL2 / 8] &= ~(0xF << (PWM_KANAL2 % 8) * 4);
  GPIOA->AFR[PWM_KANAL1 / 8] |= (AF << (PWM_KANAL1 % 8) * 4);
  GPIOA->AFR[PWM_KANAL2 / 8] |= (AF << (PWM_KANAL2 % 8) * 4);
}

void
pwm_duty_cycle_right (uint16_t duty_cycle) // pre ovoga obavezno uradi saturaciju
{
  TIM9->CCR1 = duty_cycle;
}

void
pwm_duty_cycle_left (uint16_t duty_cycle)
{
  TIM9->CCR2 = duty_cycle;
}

static void
interrupt_init ()
{
//  SYSCFG->EXTICR[2] &= ~(0b1111 << 12);
//  SYSCFG->EXTICR[2] |= (0b0010 << 12);
//  EXTI->IMR |= (0b1 << 11);		// dozvoli interrupt request
//  EXTI->EMR |= (0b1 << 11);		// dozvoli interrupt event
//  uint8_t EXTI15_10 = 40;
//  NVIC->ISER[EXTI15_10 / 32] |= (0b1 << (EXTI15_10 % 32));

  TIM9->DIER |= (0b1 << 0);	//dozvola prekida
  uint8_t const TIM9_INTERRUPT = 24;
  NVIC->ISER[0] |= (0b1 << TIM9_INTERRUPT);
}

void
TIM1_BRK_TIM9_IRQHandler ()
{
  if ((TIM9->SR & (0b1 << 0)) == (0b1 << 0))
    {
      TIM9->SR &= ~(0b1 << 0);	// da bi sledeci put mogli da detektujemo prekid
      //EXTI->SWIER |= (0b1 << 11);
      // ovde zovi adc start
      // testiranje za diodu
      irq_counter++;
      if (irq_counter >= 84000)	//valjda svake 4 sekunde: 84000
	{
	  irq_counter = 0;
	  GPIOA->ODR |= (0b1 << 5); // io_led(true);
	}
    }
}

void
EXTI15_10_IRQHandler ()
{
  if (EXTI->PR & (0b1 << 11))
    {
      EXTI->PR &= ~(0b1 << 11);
      // ovde pisi interrupt za pocetak adc
      // testiranje za diodu
      irq_counter++;
      if (irq_counter >= 84000)	//valjda svake 4 sekunde: 84000
	{
	  irq_counter = 0;
	  GPIOA->ODR |= (0b1 << 5); // io_led(true);
	}

    }
}

/*
 * prag:
 * if (signal > max)
 * return true
 * else
 * return false
 */
