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
  interrupt_init ();
}

static void
interrupt_init ()
{
  // infra0
  SYSCFG->EXTICR[0] &= ~(0b1111 << 0);	// da PC0 bude input
  SYSCFG->EXTICR[0] |= (0b0010 << 0);
  EXTI->RTSR |= (0b1 << infra0);	// na rastucu ivicu
  EXTI->IMR |= (0b1 << infra0);		// dozvoli interrupt request
  EXTI->EMR |= (0b1 << infra0);		// dozvoli interrupt event
  uint8_t EXTI0 = 6;
  NVIC->ISER[EXTI0 / 32] |= (0b1 << (EXTI0 % 32));

  // infra1
  SYSCFG->EXTICR[0] &= ~(0b1111 << 4);	// da PC1 bude input
  SYSCFG->EXTICR[0] |= (0b0010 << 4);
  EXTI->RTSR |= (0b1 << infra1);	// na rastucu ivicu
  EXTI->IMR |= (0b1 << infra1);		// dozvoli interrupt request
  EXTI->EMR |= (0b1 << infra1);		// dozvoli interrupt event
  uint8_t EXTI1 = 7;
  NVIC->ISER[EXTI1 / 32] |= (0b1 << (EXTI1 % 32));

  // infra2
  SYSCFG->EXTICR[0] &= ~(0b1111 << 8);	// da PC2 bude input
  SYSCFG->EXTICR[0] |= (0b0010 << 8);
  EXTI->RTSR |= (0b1 << infra2);	// na rastucu ivicu
  EXTI->IMR |= (0b1 << infra2);		// dozvoli interrupt request
  EXTI->EMR |= (0b1 << infra2);		// dozvoli interrupt event
  uint8_t EXTI2 = 8;
  NVIC->ISER[EXTI2 / 32] |= (0b1 << (EXTI2 % 32));

  // infra3
  SYSCFG->EXTICR[0] &= ~(0b1111 << 12);	// da PC3 bude input
  SYSCFG->EXTICR[0] |= (0b0010 << 12);
  EXTI->RTSR |= (0b1 << infra3);	// na rastucu ivicu
  EXTI->IMR |= (0b1 << infra3);		// dozvoli interrupt request
  EXTI->EMR |= (0b1 << infra3);		// dozvoli interrupt event
  uint8_t EXTI3 = 9;
  NVIC->ISER[EXTI3 / 32] |= (0b1 << (EXTI3 % 32));

  // infra4
  SYSCFG->EXTICR[1] &= ~(0b1111 << 0);	// da PC4 bude input
  SYSCFG->EXTICR[1] |= (0b0010 << 0);
  EXTI->RTSR |= (0b1 << infra4);	// na rastucu ivicu
  EXTI->IMR |= (0b1 << infra4);		// dozvoli interrupt request
  EXTI->EMR |= (0b1 << infra4);		// dozvoli interrupt event
  uint8_t EXTI4 = 10;
  NVIC->ISER[EXTI4 / 32] |= (0b1 << (EXTI4 % 32));

  // infra5
  SYSCFG->EXTICR[1] &= ~(0b1111 << 4);	// da PC5 bude input
  SYSCFG->EXTICR[1] |= (0b0010 << 4);
  EXTI->RTSR |= (0b1 << infra5);	// na rastucu ivicu
  EXTI->IMR |= (0b1 << infra5);		// dozvoli interrupt request
  EXTI->EMR |= (0b1 << infra5);		// dozvoli interrupt event
  uint8_t EXTI9_5 = 23;
  NVIC->ISER[ EXTI9_5 / 32] |= (0b1 << ( EXTI9_5 % 32));
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

void
EXTI0_IRQHandler ()
{
  if (EXTI->PR & (0b1 << infra0))
    {
      EXTI->PR &= ~(0b1 << infra0);
      io_led (false);
    }
}

void
EXTI1_IRQHandler ()
{
  if (EXTI->PR & (0b1 << infra1))
    {
      EXTI->PR &= ~(0b1 << infra1);
      io_led (false);
    }
}

void
EXTI2_IRQHandler ()
{
  if (EXTI->PR & (0b1 << infra2))
    {
      EXTI->PR &= ~(0b1 << infra2);
      io_led (false);
    }
}

void
EXTI3_IRQHandler ()
{
  if (EXTI->PR & (0b1 << infra3))
    {
      EXTI->PR &= ~(0b1 << infra3);
      io_led (false);
    }
}

void
EXTI4_IRQHandler ()
{
  if (EXTI->PR & (0b1 << infra4))
    {
      EXTI->PR &= ~(0b1 << infra4);
      io_led (false);
    }
}

void
EXTI9_5_IRQHandler ()
{
    if (EXTI->PR & (0b1 << infra5))
      {
        EXTI->PR &= ~(0b1 << infra5);
        io_led (false);
      }
}
