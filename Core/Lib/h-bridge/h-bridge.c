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

uint8_t const H_LEFT_DIR1 = 10, H_LEFT_DIR2 = 11;
uint8_t const H_RIGHT_DIR1 = 11, H_RIGHT_DIR2 = 12;

void
io_init ()
{
  RCC->AHB1ENR |= (0b1 << 0);

  GPIOA->MODER &= ~(0b11 << 2 * H_RIGHT_DIR1);
  GPIOA->MODER &= ~(0b11 << 2 * H_RIGHT_DIR2);
  GPIOA->MODER |= (0b01 << 2 * H_RIGHT_DIR1);
  GPIOA->MODER |= (0b01 << 2 * H_RIGHT_DIR2);

  //h-most 1 stop
  GPIOA->ODR &= ~(0b1 << H_RIGHT_DIR1);
  GPIOA->ODR &= ~(0b1 << H_RIGHT_DIR2);

  RCC->AHB1ENR |= (0b1 << 2);

  GPIOC->MODER &= ~(0b11 << 2 * H_LEFT_DIR1);
  GPIOC->MODER &= ~(0b11 << 2 * H_LEFT_DIR2);
  GPIOC->MODER |= (0b01 << 2 * H_LEFT_DIR1);
  GPIOC->MODER |= (0b01 << 2 * H_LEFT_DIR2);

  //h-most 2 stop
  GPIOC->ODR &= ~(0b1 << H_LEFT_DIR1);
  GPIOC->ODR &= ~(0b1 << H_LEFT_DIR2);
}

void
stop_right_wheel ()
{
  GPIOC->ODR &= ~(0b1 << H_RIGHT_DIR1);
  GPIOC->ODR &= ~(0b1 << H_RIGHT_DIR2);
}

void
stop_left_wheel ()
{
  GPIOC->ODR &= ~(0b1 << H_LEFT_DIR1);
  GPIOC->ODR &= ~(0b1 << H_LEFT_DIR2);
}

void
right_wheel_forwards ()
{
  GPIOA->ODR &= ~(0b1 << H_RIGHT_DIR2);
  GPIOA->ODR |= (0b1 << H_RIGHT_DIR1);
}

void
right_wheel_backwards ()
{
  GPIOA->ODR &= ~(0b1 << H_RIGHT_DIR1);
  GPIOA->ODR |= (0b1 << H_RIGHT_DIR2);
}

void
left_wheel_forwards ()
{
  GPIOC->ODR &= ~(0b1 << H_LEFT_DIR2);
  GPIOC->ODR |= (0b1 << H_LEFT_DIR1);
}
void
left_wheel_backwards ()
{
  GPIOC->ODR &= ~(0b1 << H_LEFT_DIR1);
  GPIOC->ODR |= (0b1 << H_LEFT_DIR2);
}
