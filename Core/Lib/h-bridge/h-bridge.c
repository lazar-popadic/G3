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

uint8_t const H2_DIR1 = 10, H2_DIR2 = 11;
uint8_t const H1_DIR1 = 11, H1_DIR2 = 12;

void
io_init ()
{
  RCC->AHB1ENR |= (0b1 << 0);

  GPIOA->MODER &= ~(0b11 << 2 * H1_DIR1);
  GPIOA->MODER &= ~(0b11 << 2 * H1_DIR2);
  GPIOA->MODER |= (0b01 << 2 * H1_DIR1);
  GPIOA->MODER |= (0b01 << 2 * H1_DIR2);

  //h-most 1 stop
  GPIOA->ODR &= ~(0b1 << H1_DIR1);
  GPIOA->ODR &= ~(0b1 << H1_DIR2);

  RCC->AHB1ENR |= (0b1 << 2);

  GPIOC->MODER &= ~(0b11 << 2 * H2_DIR1);
  GPIOC->MODER &= ~(0b11 << 2 * H2_DIR2);
  GPIOC->MODER |= (0b01 << 2 * H2_DIR1);
  GPIOC->MODER |= (0b01 << 2 * H2_DIR2);

  //h-most 2 stop
  GPIOC->ODR &= ~(0b1 << H2_DIR1);
  GPIOC->ODR &= ~(0b1 << H2_DIR2);
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
  GPIOA->ODR &= ~(0b1 << H1_DIR2);
  GPIOA->ODR |= (0b1 << H1_DIR1);
}

void
wheel_1_backwards ()
{
  GPIOA->ODR &= ~(0b1 << H1_DIR1);
  GPIOA->ODR |= (0b1 << H1_DIR2);
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
