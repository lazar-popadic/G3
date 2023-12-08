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
}

static void
portb_init ()
{
  RCC->AHB1ENR |= (0b1 << 1);
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

  //h-most 2 stop
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
wheel_1_forwards ()
{
  GPIOC->ODR &= ~(0b1 << H1_DIR2);
  GPIOC->ODR |= (0b1 << H1_DIR1);
}

void
wheel_1_backwards ()
{
  GPIOC->ODR &= ~(0b1 << H1_DIR1);
  GPIOC->ODR |= (0b1 << H1_DIR2);
}

void
wheel_2_forwards ()
{
  GPIOC->ODR &= ~(0b1 << H2_DIR2);
  GPIOC->ODR |= (0b1 << H2_DIR1);
}
void
wheel_2_backwards ()
{
  GPIOC->ODR &= ~(0b1 << H2_DIR1);
  GPIOC->ODR |= (0b1 << H2_DIR2);
}
