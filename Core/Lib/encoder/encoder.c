/*
 * encoder.c
 *
 *  Created on: Nov 18, 2023
 *      Author: lazar
 */

#include "encoder.h"
#include "stm32f4xx.h"
#include "../odometry/odometry.h"

static void
tim2_init ();
static void
tim5_init ();
static void
io_init ();

volatile int16_t state_enc_right_passive = 0;
volatile int16_t state_enc_left_passive = 0;

uint8_t const ENC1_A = 5;	//tim2 - PA5
uint8_t const ENC1_B = 3;	//	 PB3
uint8_t const ENC2_A = 0;	//tim5 - PA0
uint8_t const ENC2_B = 1;	//	 PA1

uint8_t const AF_TIM2 = 1;
uint8_t const AF_TIM5 = 2;


void
encoder_init ()
{
  tim2_init ();			//enkoder 1 - desni pasivni tocak
  tim5_init ();			//enkoder 2 - levi pasivni tocak
  io_init ();
}

int16_t
speed_of_encoder_right_passive ()
{
  int16_t speed = (int16_t) TIM2->CNT - state_enc_right_passive;
  state_enc_right_passive = (int16_t) TIM2->CNT;
  return speed;		//inkrementi, tj. impulsi
}

int16_t
speed_of_encoder_left_passive ()
{
  int16_t speed = (int16_t) TIM5->CNT - state_enc_left_passive;
  state_enc_left_passive = (int16_t) TIM5->CNT;
  return speed;
}

static void
tim2_init ()
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
tim5_init ()
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

static void
io_init ()
{
  RCC->AHB1ENR |= (0b1 << 0);

  GPIOA->MODER &= ~(0b11 << 2 * ENC1_A);
  GPIOA->MODER |= (0b10 << 2 * ENC1_A);
  GPIOA->AFR[ENC1_A / 8] &= ~(0b1111 << 4 * (ENC1_A % 8));
  GPIOA->AFR[ENC1_A / 8] |= (AF_TIM2 << 4 * (ENC1_A % 8));

  GPIOA->MODER &= ~(0b11 << 2 * ENC2_A);
  GPIOA->MODER |= (0b10 << 2 * ENC2_A);
  GPIOA->AFR[ENC2_A / 8] &= ~(0b1111 << 4 * (ENC2_A % 8));
  GPIOA->AFR[ENC2_A / 8] |= (AF_TIM5 << 4 * (ENC2_A % 8));

  GPIOA->MODER &= ~(0b11 << 2 * ENC2_B);
  GPIOA->MODER |= (0b10 << 2 * ENC2_B);
  GPIOA->AFR[ENC2_B / 8] &= ~(0b1111 << 4 * (ENC2_B % 8));
  GPIOA->AFR[ENC2_B / 8] |= (AF_TIM5 << 4 * (ENC2_B % 8));

  RCC->AHB1ENR |= (0b1 << 1);

  GPIOB->MODER &= ~(0b11 << 2 * ENC1_B);
  GPIOB->MODER |= (0b10 << 2 * ENC1_B);
  GPIOB->AFR[ENC1_B / 8] &= ~(0b1111 << 4 * (ENC1_B % 8));
  GPIOB->AFR[ENC1_B / 8] |= (AF_TIM2 << 4 * (ENC1_B % 8));
}
