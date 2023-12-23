/*
 * adc.c
 *
 *  Created on: Dec 22, 2023
 *      Author: lazar
 */

#include "adc.h"
#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>

static void
io_init ();

uint8_t prvi = 4, drugi = 7;

void
adc_init ()
{
  io_init ();

  RCC->APB2ENR |= (0b1 << 8);

  // rezolucija: 8 bita
  ADC1->CR1 &= ~(0b11 << 24);
  ADC1->CR1 |= (0b10 << 24);

  // poravnavanje: desno
  ADC1->CR2 &= ~(0b1 << 11);

  // kontinualan rezim
  ADC1->CR2 |= (0b1 << 1);

  // 2 konverzije
  ADC1->SQR1 &= ~(0b1111 << 20);
  ADC1->SQR1 |= (0b0001 << 20);

  // flag za kraj konverzije
  ADC1->CR2 |= (1 << 10);

  // redosled konverzije: prvo 4, posle 7
  ADC1->SQR3 |= (prvi << 0);
  ADC1->SQR3 |= (drugi << 5);
}

static void
io_init ()
{
  RCC->AHB1ENR |= (0b1 << 0);

  GPIOA->MODER |= (0b11 << 2 * prvi);
  GPIOA->MODER |= (0b11 << 2 * drugi);
}
