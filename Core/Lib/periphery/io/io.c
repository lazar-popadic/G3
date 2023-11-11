/*
 * io.c
 *
 *  Created on: Oct 19, 2023
 *      Author: lazar
 */

#include "../../periphery/io/io.h"

#include "stm32f4xx.h"

static void
porta_init();

void
io ()
{
  porta_init ();
}

static void
porta_init ()		//dozvola takta A
{
  RCC->AHB1ENR |= (0b1 << 0);


  GPIOA->MODER &= ~(0b11 << 2 * 5);
  GPIOA->MODER |= (0b01 << 2 * 5);
  GPIOA->OTYPER &= ~(0b1 << 5);
  GPIOA->OSPEEDR |= (0b11 << 2 * 5);
  GPIOA->PUPDR &= ~(0b11 << 2 * 5);
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
