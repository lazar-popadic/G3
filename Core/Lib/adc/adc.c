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

  // kontinualan rezim: za sad iskljucen jer hocu automatski da ga palim u pwm prekidu
  ADC1->CR2 &= ~(0b1 << 1);

  // ukljucen scan mode, da radi vise konverzija
  ADC1->CR1 |= (0b1 << 8);

  // 2 konverzije
  ADC1->SQR1 &= ~(0b1111 << 20);
  ADC1->SQR1 |= (0b0001 << 20);

  // enable flag za kraj konverzije, tek kad obe zavrsi
  ADC1->CR2 &= ~(1 << 10);

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

void
adc_on ()
{
  ADC1->CR2 |= (0b1 << 0);
  //mzd treba delay ovde neki, tamo su stavili 10ms
}

void
adc_off ()
{
  ADC1->CR2 &= ~(0b1 << 0);
}

void
adc_start ()
{
  // Provera da li je ADC isključen
  if ((ADC1->CR2 & (1 << 0)) != (1 << 0))

    {
      // Uključivanje ADC-a - ako zelim da ga palim/gasim stalno
      // ADC1->CR2 |= (1 << 0);
      //mzd treba delay ovde neki, tamo su stavili 10ms
      // iskljucena zastavica za kraj konverzije
      ADC1->SR &= ~(0b1 << 1);
      // iskljucena zastavica za pocetak konverzije
      ADC1->SR &= ~(0b1 << 4);
      // Započinjanje konverzije
      ADC1->CR2 |= (1 << 30);
    }
}
