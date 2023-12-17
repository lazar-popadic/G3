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
  sensors_io_init ();
  interrupt_init ();
}

static void
interrupt_init ()
{
  SYSCFG->EXTICR[3] &= ~(0b1111 << 4);
  SYSCFG->EXTICR[3] |= (0b0010 << 4);	// da PC13 bude input
  EXTI->FTSR |= (0b1 << button);	// na opadajucu ivicu
  EXTI->IMR |= (0b1 << button);		// dozvoli interrupt request
  EXTI->EMR |= (0b1 << button);		// dozvoli interrupt event
  uint8_t EXTI15_10 = 40;
  NVIC->ISER[EXTI15_10 / 32] |= (0b1 << (EXTI15_10 % 32));

  SYSCFG->EXTICR[0] &= ~(0b1111 << 4);	// da PC1 bude input
  SYSCFG->EXTICR[0] |= (0b0010 << 4);
  EXTI->RTSR |= (0b1 << infra4);	// na rastucu ivicu
  EXTI->IMR |= (0b1 << infra4);		// dozvoli interrupt request
  EXTI->EMR |= (0b1 << infra4);		// dozvoli interrupt event
  uint8_t EXTI1 = 7;
  NVIC->ISER[EXTI1 / 32] |= (0b1 << (EXTI1 % 32));
}

void
sensors_io_init ()
{
  RCC->AHB1ENR |= (0b1 << 1);		//PortB

  GPIOB->MODER &= ~(0b11 << 2 * infra1);
  GPIOB->MODER &= ~(0b11 << 2 * infra2);

  RCC->AHB1ENR |= (0b1 << 2);		//PortC

  GPIOC->MODER &= ~(0b11 << 2 * infra3);
  GPIOC->MODER &= ~(0b11 << 2 * infra4);
  GPIOC->MODER &= ~(0b11 << 2 * infra5);
  GPIOC->MODER &= ~(0b11 << 2 * infra6);
  GPIOB->MODER &= ~(0b11 << 2 * button);
}

bool
sensors_low ()
{
  if (GPIOB->IDR & ((0b1 << infra1) | (0b1 << infra2)))
    return true;
  return false;
}

bool
sensors_high ()
{
  if (GPIOC->IDR & ((0b1 << infra3) | (0b1 << infra4)))
    return true;
  return false;
}

bool
sensors_back ()
{
  if (GPIOC->IDR & ((0b1 << infra5) | (0b1 << infra6)))
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

void
EXTI15_10_IRQHandler ()
{
  if (EXTI->PR & (0b1 << button))
    {
      EXTI->PR &= ~(0b1 << button);
      io_led (true);
    }
}

void
EXTI1_IRQHandler ()
{
  if (EXTI->PR & (0b1 << infra4))
    {
      EXTI->PR &= ~(0b1 << infra4);
      io_led (false);
    }
}
