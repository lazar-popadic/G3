/*
 * io.c
 *
 *  Created on: Oct 19, 2023
 *      Author: lazar
 */

#include "../io/io.h"
#include "stm32f4xx.h"

static void
porta_init ();
static void
portb_init ();
static void
portc_init ();

uint8_t const INT_LED = 5, CINC = 6;
uint8_t const H1_DIR1 = 14, H1_DIR2 = 15, H2_DIR1 = 2, H2_DIR2 = 3;
uint8_t const ENC1_KANAL_A = 4;
uint8_t const ENC1_KANAL_B = 5;
uint8_t const ENC2_KANAL_A = 6;
uint8_t const ENC2_KANAL_B = 7;
uint8_t const ENC3_KANAL_A = 15;
uint8_t const ENC3_KANAL_B = 3;
uint8_t const ENC4_KANAL_A = 0;
uint8_t const ENC4_KANAL_B = 1;
uint8_t const AF_TIM1 = 1;
uint8_t const AF_TIM2 = 1;
uint8_t const AF_TIM3 = 2;
uint8_t const AF_TIM4 = 2;
uint8_t const AF_TIM5 = 2;

void
io_init ()
{
  porta_init ();
  portb_init ();
  portc_init ();
}

static void
porta_init ()
{
  RCC->AHB1ENR |= (0b1 << 0);

  GPIOA->MODER &= ~(0b11 << 2 * INT_LED);
  GPIOA->MODER |= (0b01 << 2 * INT_LED);
  GPIOA->OTYPER &= ~(0b1 << INT_LED);
  GPIOA->OSPEEDR |= (0b11 << 2 * INT_LED);
  GPIOA->PUPDR &= ~(0b11 << 2 * INT_LED);

  GPIOA->MODER &= ~(0b11 << 2 * CINC);
  GPIOA->OSPEEDR &= ~(0b11 << 2 * CINC);	//low speed
  GPIOA->PUPDR &= ~(0b11 << 2 * CINC);		//reset
  GPIOA->PUPDR |= (0b10 << 2 * CINC);		//pull-down

  GPIOA->MODER &= ~(0b11 << 2 * ENC3_KANAL_A);
  GPIOA->MODER |= (0b10 << 2 * ENC3_KANAL_A);
  GPIOA->AFR[ENC3_KANAL_A / 8] &= ~(0b1111 << 4 * (ENC3_KANAL_A % 8));
  GPIOA->AFR[ENC3_KANAL_A / 8] |= (AF_TIM2 << 4 * (ENC3_KANAL_A % 8));

  GPIOA->MODER &= ~(0b11 << 2 * ENC4_KANAL_A);
  GPIOA->MODER |= (0b10 << 2 * ENC4_KANAL_A);
  GPIOA->AFR[ENC4_KANAL_A / 8] &= ~(0b1111 << 4 * (ENC4_KANAL_A % 8));
  GPIOA->AFR[ENC4_KANAL_A / 8] |= ( AF_TIM5 << 4 * (ENC4_KANAL_A % 8));

  GPIOA->MODER &= ~(0b11 << 2 * ENC4_KANAL_B);
  GPIOA->MODER |= (0b10 << 2 * ENC4_KANAL_B);
  GPIOA->AFR[ENC4_KANAL_B / 8] &= ~(0b1111 << 4 * (ENC4_KANAL_B % 8));
  GPIOA->AFR[ENC4_KANAL_B / 8] |= ( AF_TIM5 << 4 * (ENC4_KANAL_B % 8));
}

static void
portb_init ()
{
  RCC->AHB1ENR |= (0b1 << 1);

  GPIOB->MODER &= ~(0b11 << 2 * ENC3_KANAL_B);
  GPIOB->MODER |= (0b10 << 2 * ENC3_KANAL_B);
  GPIOB->AFR[ENC3_KANAL_B / 8] &= ~(0b1111 << 4 * (ENC3_KANAL_B % 8));
  GPIOB->AFR[ENC3_KANAL_B / 8] |= ( AF_TIM2 << 4 * (ENC3_KANAL_B % 8));

  GPIOB->MODER &= ~(0b11 << 2 * ENC1_KANAL_A);//podesavanje pinova da rade kao alternativna funkcija
  GPIOB->MODER &= ~(0b11 << 2 * ENC1_KANAL_B);
  GPIOB->MODER |= (0b10 << 2 * ENC1_KANAL_A);
  GPIOB->MODER |= (0b10 << 2 * ENC1_KANAL_B);
  GPIOB->AFR[ENC1_KANAL_A / 8] &= ~(0b1111 << 4 * (ENC1_KANAL_A % 8));//podesavanje odabira alternativne funkcije
  GPIOB->AFR[ENC1_KANAL_B / 8] &= ~(0b1111 << 4 * (ENC1_KANAL_B % 8));
  GPIOB->AFR[ENC1_KANAL_A / 8] |= ( AF_TIM3 << 4 * (ENC1_KANAL_A % 8));
  GPIOB->AFR[ENC1_KANAL_B / 8] |= ( AF_TIM3 << 4 * (ENC1_KANAL_B % 8));

  GPIOB->MODER &= ~(0b11 << 2 * ENC2_KANAL_A);//podesavanje pinova da rade kao alternativna funkcija
  GPIOB->MODER &= ~(0b11 << 2 * ENC2_KANAL_B);
  GPIOB->MODER |= (0b10 << 2 * ENC2_KANAL_A);
  GPIOB->MODER |= (0b10 << 2 * ENC2_KANAL_B);
  GPIOB->AFR[ENC2_KANAL_A / 8] &= ~(0b1111 << 4 * (ENC2_KANAL_A % 8));//podesavanje odabira alternativne funkcije
  GPIOB->AFR[ENC2_KANAL_B / 8] &= ~(0b1111 << 4 * (ENC2_KANAL_B % 8));
  GPIOB->AFR[ENC2_KANAL_A / 8] |= ( AF_TIM4 << 4 * (ENC2_KANAL_A % 8));
  GPIOB->AFR[ENC2_KANAL_B / 8] |= ( AF_TIM4 << 4 * (ENC2_KANAL_B % 8));
}

static void
portc_init ()
{

  RCC->AHB1ENR |= (0b1 << 2);

  GPIOC->MODER &= ~(0b11 << 2 * H1_DIR1);
  GPIOC->MODER &= ~(0b11 << 2 * H1_DIR2);
  GPIOC->MODER |= (0b01 << 2 * H1_DIR1);
  GPIOC->MODER |= (0b01 << 2 * H1_DIR2);

  //h-most 1 stop
  GPIOC->ODR &= ~(0b1 << H1_DIR1);
  GPIOC->ODR &= ~(0b1 << H1_DIR2);

  GPIOC->MODER &= ~(0b11 << 2 * H2_DIR1);
  GPIOC->MODER &= ~(0b11 << 2 * H2_DIR2);
  GPIOC->MODER |= (0b01 << 2 * H2_DIR1);
  GPIOC->MODER |= (0b01 << 2 * H2_DIR2);

  //h-most 1 stop
  GPIOC->ODR &= ~(0b1 << H2_DIR1);
  GPIOC->ODR &= ~(0b1 << H2_DIR2);
}

void
io_led (bool status)
{
  if (true == status)
    {
      GPIOA->ODR |= (0b1 << 5);
    }
  else
    GPIOA->ODR &= ~(0b1 << 5);

}

void
io_cinc_loop ()
{
  for (uint8_t i = 0; i < 6; i++)
    {
      while ((GPIOA->IDR & (0b1 << 6)))
	;
      HAL_Delay (1);
    }
}

void
stop_wheel_1 ()
{
  GPIOC->ODR &= ~(0b1 << H1_DIR1);
  GPIOC->ODR &= ~(0b1 << H1_DIR2);
}

void
stop_wheel_2 ()
{
  GPIOC->ODR &= ~(0b1 << H2_DIR1);
  GPIOC->ODR &= ~(0b1 << H2_DIR2);
}

void
set_direction_1_wheel_1 ()
{
  GPIOC->ODR &= ~(0b1 << H1_DIR2);
  GPIOC->ODR |= (0b1 << H1_DIR1);
}

void
set_direction_2_wheel_1 ()
{
  GPIOC->ODR &= ~(0b1 << H1_DIR1);
  GPIOC->ODR |= (0b1 << H1_DIR2);
}

void
set_direction_1_wheel_2 ()
{
  GPIOC->ODR &= ~(0b1 << H2_DIR2);
  GPIOC->ODR |= (0b1 << H2_DIR1);
}
void
set_direction_2_wheel_2 ()
{
  GPIOC->ODR &= ~(0b1 << H2_DIR1);
  GPIOC->ODR |= (0b1 << H2_DIR2);
}
