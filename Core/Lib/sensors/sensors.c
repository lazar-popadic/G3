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
uint8_t s1 = 8;
uint8_t s2 = 9;

uint8_t s5 = 14;
uint8_t s6 = 15;
//PortC
uint8_t s3 = 0;
uint8_t s4 = 1;
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

  GPIOB->MODER &= ~(0b11 << 2 * s1);
  GPIOB->MODER &= ~(0b11 << 2 * s2);
  GPIOB->MODER &= ~(0b11 << 2 * s3);
  GPIOB->MODER &= ~(0b11 << 2 * s4);
  GPIOB->MODER &= ~(0b11 << 2 * s5);
  GPIOB->MODER &= ~(0b11 << 2 * s6);

  RCC->AHB1ENR |= (0b1 << 2);		//PortC

  GPIOB->MODER &= ~(0b11 << 2 * button);
}

bool
sensors_low ()
{
  if (!(GPIOB->IDR & ((0b1 << s1) | (0b1 << s2))))
      return true;
  return false;
}

bool
sensors_high ()
{
  if (!(GPIOB->IDR & ((0b1 << s3) | (0b1 << s4))))
      return true;
  return false;
}

bool
sensors_back ()
{
  if (!(GPIOB->IDR & ((0b1 << s5) | (0b1 << s6))))
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
