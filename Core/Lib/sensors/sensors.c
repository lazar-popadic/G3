/*
 * sensors.c
 *
 *  Created on: Dec 14, 2023
 *      Author: lazar
 */

#include "sensors.h"
#include "stm32f4xx.h"
#include "../io/io.h"

static void
sensors_io_init ();
static void
interrupt_init ();

//PortB

//PortC
uint8_t infra0 = 0;
uint8_t infra1 = 1;
uint8_t infra2 = 2;
uint8_t infra3 = 3;
uint8_t infra4 = 4;
uint8_t infra5 = 5;
uint8_t button = 13;

void
sensors_init ()
{
  sensors_io_init ();
}

void
sensors_io_init ()
{
//  RCC->AHB1ENR |= (0b1 << 1);		//PortB

  RCC->AHB1ENR |= (0b1 << 2);		//PortC

  GPIOC->MODER &= ~(0b11 << 2 * infra0);
  GPIOC->MODER &= ~(0b11 << 2 * infra1);
  GPIOC->MODER &= ~(0b11 << 2 * infra2);
  GPIOC->MODER &= ~(0b11 << 2 * infra3);
  GPIOC->MODER &= ~(0b11 << 2 * infra4);
  GPIOC->MODER &= ~(0b11 << 2 * infra5);

  GPIOC->MODER &= ~(0b11 << 2 * button);
}

bool
sensors_low ()
{
  if (GPIOC->IDR & ((0b1 << infra4) | (0b1 << infra5)))
    return true;
  return false;
}

bool
sensors_high ()
{
  if (GPIOC->IDR & ((0b1 << infra0) | (0b1 << infra1)))
    return true;
  return false;
}

bool
sensors_back ()
{
  if (GPIOC->IDR & ((0b1 << infra2) | (0b1 << infra3)))
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
