/*
 * h-bridge.c
 *
 *  Created on: Dec 14, 2023
 *      Author: lazar
 */

#include "h-bridge.h"
#include "stm32f4xx.h"

static void
io_init ();

void
h_bridge_init ()
{
  io_init ();
}

uint8_t const M2DIR = 9, M2SLP = 5;
uint8_t const M1DIR = 8, M1SLP = 10;

void
io_init ()
{
  RCC->AHB1ENR |= (0b1 << 0);

  GPIOA->MODER &= ~(0b11 << 2 * M1DIR);
  GPIOA->MODER &= ~(0b11 << 2 * M1SLP);
  GPIOA->MODER |= (0b01 << 2 * M1DIR);
  GPIOA->MODER |= (0b01 << 2 * M1SLP);

  //h-most 1 stop
  GPIOA->ODR &= ~(0b1 << M1SLP);

  GPIOA->MODER &= ~(0b11 << 2 * M2DIR);
  GPIOA->MODER |= (0b01 << 2 * M2DIR);

  RCC->AHB1ENR |= (0b1 << 1);

  GPIOB->MODER &= ~(0b11 << 2 * M2SLP);
  GPIOB->MODER |= (0b01 << 2 * M2SLP);

  //h-most 2 stop
  GPIOB->ODR &= ~(0b1 << M2SLP);
}

void
stop_right_wheel ()
{
  GPIOA->ODR &= ~(0b1 << M1SLP);
}

void
stop_left_wheel ()
{
  GPIOB->ODR &= ~(0b1 << M2SLP);
}

void
right_wheel_forwards ()
{
  GPIOA->ODR |= (0b1 << M1DIR);
  GPIOA->ODR |= (0b1 << M1SLP);
}

void
right_wheel_backwards ()
{
  GPIOA->ODR &= ~(0b1 << M1DIR);
  GPIOA->ODR |= (0b1 << M1SLP);
}

void
left_wheel_forwards ()
{
  GPIOA->ODR |= (0b1 << M2DIR);
  GPIOB->ODR |= (0b1 << M2SLP);
}
void
left_wheel_backwards ()
{
  GPIOA->ODR &= ~(0b1 << M2DIR);
  GPIOB->ODR |= (0b1 << M2SLP);
}
