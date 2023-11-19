/*
 * encoder.c
 *
 *  Created on: Nov 18, 2023
 *      Author: lazar
 */

#include "encoder.h"
#include "stm32f4xx.h"

static void
tim3_init ();
static void
tim4_init ();
static void
tim2_init ();
static void
tim5_init ();

volatile int16_t state_enc_right_passive = 0;
volatile int16_t state_enc_left_passive = 0;
volatile int16_t state_enc_right_maxon = 0;
volatile int16_t state_enc_left_maxon = 0;

void
encoder_init ()
{
  tim3_init ();			//enkoder 1 - desni pasivni tocak
  tim4_init ();			//enkoder 2 - levi pasivni tocak
  tim2_init ();			//enkoder 3 - desni maxon
  tim5_init ();			//enkoder 4 - levi maxon
}

int16_t
timer_speed_of_encoder_right_passive ()
{
  int16_t speed = (int16_t) TIM3->CNT - (int16_t) state_enc_right_passive;
  state_enc_right_passive = (int16_t) TIM3->CNT;
  return speed;		//inkrementi, tj. impulsi
}

int16_t
timer_speed_of_encoder_left_passive ()
{
  int16_t speed = (int16_t) TIM4->CNT - (int16_t) state_enc_left_passive;
  state_enc_left_passive = (int16_t) TIM4->CNT;
  return speed;
}

int16_t
timer_speed_of_encoder_right_maxon ()
{
  int16_t speed = (int16_t) TIM2->CNT - (int16_t) state_enc_right_maxon;
  state_enc_right_maxon = (int16_t) TIM2->CNT;
  return speed;		//inkrementi, tj. impulsi
}

int16_t
timer_speed_of_encoder_left_maxon ()
{
  int16_t speed = (int16_t) TIM5->CNT - (int16_t) state_enc_left_maxon;
  state_enc_left_maxon = (int16_t) TIM5->CNT;
  return speed;
}

static void
tim3_init ()					//ENKODER
{
  RCC->APB1ENR |= (0b1 << 1);	//dozvola takta za tim3 pa za gpio port B

  TIM3->PSC = 0;				// zbog max rezolucije
  TIM3->ARR = 0xFFFF;// bitno je da najveci bit bude 1 zbog minusa i negativne brzine

  TIM3->SMCR &= ~(0b111 << 0);
  TIM3->SMCR |= (0b011 << 0);

  TIM3->CCMR1 &= ~(0b11 << 0);//povezujemo kanale enkodera timera sa kanalom samog timera
  TIM3->CCMR1 |= (0b01 << 0);
  TIM3->CCMR1 &= ~(0b11 << 8);
  TIM3->CCMR1 |= (0b01 << 8);

  TIM3->CCER &= ~(0b101 << 1);			//neinvertovan kanal A
  TIM3->CCER &= ~(0b101 << 5);			//neinvertovan kanal B

  TIM3->CR1 |= (0b1 << 0);			//ukljucivanje timera
}

static void
tim4_init ()					//ENKODER
{
  RCC->APB1ENR |= (0b1 << 2);	//dozvola takta za tim4 pa za gpio port B

  TIM4->PSC = 0;				// zbog max rezolucije
  TIM4->ARR = 0xFFFF;// bitno je da najveci bit bude 1 zbog minusa i negativne brzine

  TIM4->SMCR &= ~(0b111 << 0);
  TIM4->SMCR |= (0b011 << 0);

  TIM4->CCMR1 &= ~(0b11 << 0);//povezujemo kanale enkodera timera sa kanalom samog timera
  TIM4->CCMR1 |= (0b01 << 0);
  TIM4->CCMR1 &= ~(0b11 << 8);
  TIM4->CCMR1 |= (0b01 << 8);

  TIM4->CCER &= ~(0b100 << 1);		//invertovan kanal A	0b xxxx 0x1x
  TIM4->CCER |= (0b001 << 1);
  //  TIM4->CCER &= ~(0b101 << 1);	//neinvertovan kanal A
  TIM4->CCER &= ~(0b101 << 5);		//neinvertovan kanal B	0b xxxx 0x0x

  TIM4->CR1 |= (0b1 << 0);			//ukljucivanje timera
}

static void
tim2_init ()					//ENKODER
{
  RCC->APB1ENR |= (0b1 << 0);

  TIM2->PSC = 0;
  TIM2->ARR = 0xFFFF;

  TIM2->SMCR &= ~(0b111 << 0);
  TIM2->SMCR |= (0b011 << 0);

  TIM2->CCMR1 &= ~(0b11 << 0);//povezujemo kanale enkodera timera sa kanalom samog timera
  TIM2->CCMR1 |= (0b01 << 0);
  TIM2->CCMR1 &= ~(0b11 << 8);
  TIM2->CCMR1 |= (0b01 << 8);

  TIM2->CCER &= ~(0b101 << 1);		//neinvertovan kanal A
  TIM2->CCER &= ~(0b101 << 5);		//neinvertovan kanal B

  TIM2->CR1 |= (0b1 << 0);			//ukljucivanje timera
}

static void
tim5_init ()					//ENKODER
{
  RCC->APB1ENR |= (0b1 << 3);

  TIM5->PSC = 0;
  TIM5->ARR = 0xFFFF;

  TIM5->SMCR &= ~(0b111 << 0);
  TIM5->SMCR |= (0b011 << 0);

  TIM5->CCMR1 &= ~(0b11 << 0);//povezujemo kanale enkodera timera sa kanalom samog timera
  TIM5->CCMR1 |= (0b01 << 0);
  TIM5->CCMR1 &= ~(0b11 << 8);
  TIM5->CCMR1 |= (0b01 << 8);

  TIM5->CCER &= ~(0b100 << 1);		//invertovan kanal A	0b xxxx 0x1x
  TIM5->CCER |= (0b001 << 1);
  TIM5->CCER &= ~(0b101 << 5);		//neinvertovan kanal B	0b xxxx 0x0x

  TIM5->CR1 |= (0b1 << 0);			//ukljucivanje timera
}
