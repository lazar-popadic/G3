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

//PortB

//PortC
static uint8_t infra0 = 0;
static uint8_t infra1 = 1;
static uint8_t infra2 = 2;
static uint8_t infra3 = 3;
static uint8_t infra4 = 4;
static uint8_t infra5 = 5;
static uint8_t pos_switch = 10;
static uint8_t side_switch = 11;
static uint8_t tactic_switch = 12;

static uint8_t button = 13;

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

  GPIOC->MODER &= ~(0b11 << 2 * pos_switch);
  GPIOC->MODER &= ~(0b11 << 2 * side_switch);
  GPIOC->MODER &= ~(0b11 << 2 * tactic_switch);

  GPIOC->MODER &= ~(0b11 << 2 * button);
}

uint8_t
switch_2 ()
{
  if (GPIOC->IDR & (0b1 << pos_switch))
    return 2;
  return 0;
}

bool
blue_side_selected ()
{
  if (GPIOC->IDR & (0b1 << side_switch))
    return true;
  return false;
}

uint8_t
switch_1 ()
{
  if (GPIOC->IDR & (0b1 << tactic_switch))
    return 1;
  return 0;
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
  if (GPIOC->IDR & ((0b1 << infra0) | (0b1 << infra2)))
    return true;
  return false;
}

bool
sensors_back ()
{
  if (GPIOC->IDR & ((0b1 << infra1) | (0b1 << infra3)))
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
