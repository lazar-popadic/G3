/*
 * dma.c
 *
 *  Created on: Dec 28, 2023
 *      Author: lazar
 */

#include "stm32f4xx.h"
#include <stdint.h>

#define BUFF_SIZE 64
#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

static volatile uint8_t uart_dma_rx[BUFF_SIZE];

void
dma_init ()
{
  RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_DMA1EN);
  RCC->APB1ENR |= (RCC_APB1ENR_USART2EN);

  // pinovi za uart2

  GPIOA->MODER &= ~(0b11 << 2 * 2 | 0b11 << 3 * 2);
  GPIOA->MODER |= (0b10 << 2 * 2 | 0b10 << 3 * 2);

  GPIOA->AFR[0] &= ~(0xF << 2 * 4 | 0xF << 3 * 4);
  GPIOA->AFR[1] |= (7 << 2 * 4 | 7 << 3 * 4);

  // Init DMA1, Rx
  // Disable DMA, pre konfiga
  DMA1_Stream5->CR &= ~(0b1 << 0);

  while ((DMA1_Stream5->CR & (0b1 << 0)) == (0b1 << 0))
    ;

  // stream selection
  DMA1_Stream5->CR &= ~(0b111 << 25);
  DMA1_Stream5->CR |= (0b100 << 25);	//ch4

  // data direction: iz periferije u memoriju
  DMA1_Stream5->CR &= ~(0b11 << 6);

  // circular mode: yes
  DMA1_Stream5->CR |= (0b1 << 8);

  // inkrementiranje memorijske lokacije: true
  DMA1_Stream5->CR |= (0b1 << 10);

  // inkrementiranje periferijske lokacije: false, mozda treba za adc zbog 2 kanala
  DMA1_Stream5->CR &= ~(0b1 << 9);

  // data transfer size: byte
  DMA1_Stream5->CR &= ~(0b11 << 11 | 0b11 << 13);

  // disable fifo
  DMA1_Stream5->FCR &= ~(0b1 << 2);

  // memory address
  DMA1_Stream5->M0AR = (uint32_t) uart_dma_rx;
  //periphery address
  DMA1_Stream5->PAR = (uint32_t) &USART2->DR;
  // number of data to transfer
  DMA1_Stream5->NDTR = (uint32_t) ARRAY_LEN(uart_dma_rx);

  // pre enable za periferiju mora da se enabluje dma: DMA1_Stream5 |= (0b1 << 0);
  // u adc mora da se stavi enable za dma
  // i kakav interrupt ide, za uart smo stavili idle
  // memcpy funkcija
}

void
USART2_IRQHandler ()
{
  // provera da li je idle
  if ((USART2->SR & (0b1 << 4)) == (0b1 << 4))
    {
      // poziv funkcije za obradu podatka

      // clear idle flag
      __IO uint32_t temp;
      temp = USART2->SR;
      (void)temp;
      temp = USART2->DR;
      (void)temp;
    }
}
