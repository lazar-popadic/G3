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
tim3_init ();
static void
tim4_init ();
static void
io_init ();

volatile int16_t state_enc_right_passive = 0;
volatile int16_t state_enc_left_passive = 0;

uint8_t const ENC1_A = 4;	//tim3
uint8_t const ENC1_B = 5;
uint8_t const ENC2_A = 6;	//tim4
uint8_t const ENC2_B = 7;

uint8_t const AF_TIM3 = 2;
uint8_t const AF_TIM4 = 2;


void
encoder_init ()
{
  tim3_init ();			//enkoder 1 - desni pasivni tocak
  tim4_init ();			//enkoder 2 - levi pasivni tocak
  io_init ();
}

int16_t
speed_of_encoder_right_passive ()
{
  int16_t speed = (int16_t) TIM3->CNT - state_enc_right_passive;
  state_enc_right_passive = (int16_t) TIM3->CNT;
  return speed;		//inkrementi, tj. impulsi
}

int16_t
speed_of_encoder_left_passive ()
{
  int16_t speed = (int16_t) TIM4->CNT - state_enc_left_passive;
  state_enc_left_passive = (int16_t) TIM4->CNT;
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
io_init ()
{
  RCC->AHB1ENR |= (0b1 << 0);

  GPIOB->MODER &= ~(0b11 << 2 * ENC1_A);//podesavanje pinova da rade kao alternativna funkcija
  GPIOB->MODER &= ~(0b11 << 2 * ENC1_B);
  GPIOB->MODER |= (0b10 << 2 * ENC1_A);
  GPIOB->MODER |= (0b10 << 2 * ENC1_B);
  GPIOB->AFR[ENC1_A / 8] &= ~(0b1111 << 4 * (ENC1_A % 8));//podesavanje odabira alternativne funkcije
  GPIOB->AFR[ENC1_B / 8] &= ~(0b1111 << 4 * (ENC1_B % 8));
  GPIOB->AFR[ENC1_A / 8] |= (AF_TIM3 << 4 * (ENC1_A % 8));
  GPIOB->AFR[ENC1_B / 8] |= (AF_TIM3 << 4 * (ENC1_B % 8));

  GPIOB->MODER &= ~(0b11 << 2 * ENC2_A);//podesavanje pinova da rade kao alternativna funkcija
  GPIOB->MODER &= ~(0b11 << 2 * ENC2_B);
  GPIOB->MODER |= (0b10 << 2 * ENC2_A);
  GPIOB->MODER |= (0b10 << 2 * ENC2_B);
  GPIOB->AFR[ENC2_A / 8] &= ~(0b1111 << 4 * (ENC2_A % 8));//podesavanje odabira alternativne funkcije
  GPIOB->AFR[ENC2_B / 8] &= ~(0b1111 << 4 * (ENC2_B % 8));
  GPIOB->AFR[ENC2_A / 8] |= (AF_TIM4 << 4 * (ENC2_A % 8));
  GPIOB->AFR[ENC2_B / 8] |= (AF_TIM4 << 4 * (ENC2_B % 8));
}
