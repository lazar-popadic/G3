/*
 * io.c
 *
 *  Created on: Oct 19, 2023
 *      Author: lazar
 */

#include "../../periphery/io/io.h"

#include "stm32f4xx.h"

static void
porta_init ();

static void
MX_GPIO_Init ();

void
io_init ()
{
  MX_GPIO_Init ();
  porta_init ();
}

static void
MX_GPIO_Init ()
{
  GPIO_InitTypeDef GPIO_InitStruct =
    { 0 };
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init (GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

static void
porta_init ()
{

  uint8_t LED = 5, CINC = 6;
  RCC->AHB1ENR |= (0b1 << 0);

  GPIOA->MODER &= ~(0b11 << 2 * LED);
  GPIOA->MODER |= (0b01 << 2 * LED);
  GPIOA->OTYPER &= ~(0b1 << LED);
  GPIOA->OSPEEDR |= (0b11 << 2 * LED);
  GPIOA->PUPDR &= ~(0b11 << 2 * LED);

  GPIOA->MODER &= ~(0b11 << 2 * CINC);
  GPIOA->OSPEEDR &= ~(0b11 << 2 * CINC);	//low speed
  GPIOA->PUPDR &= ~(0b11 << 2 * CINC);		//reset
  GPIOA->PUPDR |= (0b10 << 2 * CINC);		//pull-down
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

void
cinc_loop ()
{
  for (uint8_t i = 0; i < 6; i++)
    {
      while (GPIOA->IDR & (0b1 << 6))
	;
      HAL_Delay (1);
    }
}
