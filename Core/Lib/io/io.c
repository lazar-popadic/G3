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
volatile uint16_t counter = 0;

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
//  RCC->AHB1ENR |= (0b1 << 0);
//
//  GPIOA->MODER &= ~(0b11 << 2 * INT_LED);
//  GPIOA->MODER |= (0b01 << 2 * INT_LED);
//  GPIOA->OTYPER &= ~(0b1 << INT_LED);
//  GPIOA->OSPEEDR |= (0b11 << 2 * INT_LED);
//  GPIOA->PUPDR &= ~(0b11 << 2 * INT_LED);

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
}

//void
//io_led (bool status)
//{
//  if (true == status)
//    {
//      GPIOA->ODR |= (0b1 << 5);
//    }
//  else
//    GPIOA->ODR &= ~(0b1 << 5);
//}

void
io_protocinc ()
{
  for (uint8_t i = 0; i < 6; i++)
    {
      while ((GPIOA->IDR & (0b1 << 6)))
	;
      HAL_Delay (1);
    }
}

bool
io_cinc ()
{
  if (!(GPIOA->IDR & (0b1 << 6)))
    counter++;
  else
    counter = 0;
  if (counter > 4)
    return true;
  return false;
}

