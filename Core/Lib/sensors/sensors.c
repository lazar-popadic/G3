/*
 * sensors.c
 *
 *  Created on: Dec 14, 2023
 *      Author: lazar
 */

#include "sensors.h"
#include "stm32f4xx.h"

static void
io_init ();

//PortB
uint8_t infra1 = 8;
uint8_t infra2 = 9;

//PortC
uint8_t infra3 = 0;
uint8_t infra4 = 1;
uint8_t infra5 = 10;
uint8_t infra6 = 11;
uint8_t button = 13;

void
sensors_init ()
{
  io_init ();
}

void
io_init ()
{
  RCC->AHB1ENR |= (0b1 << 1);		//PortB

  GPIOB->MODER &= ~(0b11 << 2 * infra1);
  GPIOB->MODER &= ~(0b11 << 2 * infra2);
  GPIOB->MODER &= ~(0b11 << 2 * infra3);
  GPIOB->MODER &= ~(0b11 << 2 * infra4);
  GPIOB->MODER &= ~(0b11 << 2 * infra5);
  GPIOB->MODER &= ~(0b11 << 2 * infra6);

  RCC->AHB1ENR |= (0b1 << 2);		//PortC

  GPIOB->MODER &= ~(0b11 << 2 * button);
}

bool
sensors_low ()
{
  if (!(GPIOB->IDR & ((0b1 << infra1) | (0b1 << infra2))))
      return true;
  return false;
}

bool
sensors_high ()
{
  if (!(GPIOC->IDR & ((0b1 << infra3) | (0b1 << infra4))))
      return true;
  return false;
}

bool
sensors_back ()
{
  if (!(GPIOC->IDR & ((0b1 << infra5) | (0b1 << infra6))))
      return true;
  return false;
}

bool
button_pressed ()
{
  if (!(GPIOC->IDR & (0b1 << button)))
      return true;
  return false;
}
