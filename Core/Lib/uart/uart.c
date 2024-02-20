/*
 * uart.c
 *
 *  Created on: Nov 2, 2023
 *      Author: lazar
 */

#include "uart.h"

#define MAX_BUFFER_SIZE 32

volatile static uint8_t input = 0;
volatile uint8_t uart6_buffer[MAX_BUFFER_SIZE];
volatile uint8_t uart2_buffer[MAX_BUFFER_SIZE];
volatile static uint8_t uart6_buffer_size = 0;
volatile static uint8_t uart2_buffer_size = 0;
volatile static uint8_t index6_write = 0;
volatile static uint8_t index6_read = 0;
volatile static uint8_t index2_write = 0;
volatile static uint8_t index2_read = 0;

static void
uart6_init ();
static void
uart2_init ();

void
uart_init ()
{
  uart6_init ();
  uart2_init ();
}

static void
uart6_init ()
{
  RCC->APB2ENR |= (0b1 << 5);		// takt na usart 6
  RCC->AHB1ENR |= (0b1 << 0);		// takt na gpioa

  uint8_t const TX_PIN = 11;
  uint8_t const RX_PIN = 12;

  GPIOA->MODER &= ~(0b11 << TX_PIN * 2);
  GPIOA->MODER &= ~(0b11 << RX_PIN * 2);
  GPIOA->MODER |= (0b10 << TX_PIN * 2);
  GPIOA->MODER |= (0b10 << RX_PIN * 2);

  //za half-duplex za ax
  GPIOA->OTYPER |= (0b1 << TX_PIN);
  GPIOA->PUPDR &= ~(0b11 << TX_PIN*2);
  GPIOA->PUPDR |= (0b01 << TX_PIN*2);

  uint8_t const Alt_function = 8;

  GPIOA->AFR[TX_PIN / 8] &= ~(0b1111 << (TX_PIN % 8) * 4);
  GPIOA->AFR[TX_PIN / 8] |= (Alt_function << (TX_PIN % 8) * 4);
  GPIOA->AFR[RX_PIN / 8] &= ~(0b1111 << (RX_PIN % 8) * 4);
  GPIOA->AFR[RX_PIN / 8] |= (Alt_function << (RX_PIN % 8) * 4);

  USART6->CR1 &= ~(0b1 << 12);		// 8 bita rec
  USART6->CR2 &= ~(0b11 << 12);		// 1 stop bit

  // baudrate na 9600
  // usartdiv => 519. strana formula => 546,875 => 546 u mantisu, 0,875 * 16 = 14 u frakcioni deo
  USART6->BRR = 0;
  USART6->BRR |= ((546 << 4) | (14 << 0));

  // ukljucivanje tx i rx pinova
  USART6->CR1 |= (0b11 << 2);

  // prekid kada dodje poruka
  USART6->CR1 |= (0b1 << 5);
  // ukljuci uart
  USART6->CR1 |= (0b1 << 13);
  // koji prekid
  uint8_t USART6_INTERRUPT = 71;
  NVIC->ISER[USART6_INTERRUPT / 32] |= (0b1 << USART6_INTERRUPT % 32);
  // za sad full duplex, za ax treba half duplex

  //half-duplex:
  USART6->CR2 &= ~(0b1 << 11);
  USART6->CR2 &= ~(0b1 << 14);
  USART6->CR3 &= ~(0b1 << 1);
  USART6->CR3 &= ~(0b1 << 5);
  USART6->CR3 |= (0b1 << 3);
}

static void
uart2_init ()
{
  RCC->APB1ENR |= (0b1 << 17);		// takt na usart 2
  RCC->AHB1ENR |= (0b1 << 0);		// takt na gpioa

  uint8_t const TX_PIN = 2;
  uint8_t const RX_PIN = 3;

  GPIOA->MODER &= ~(0b11 << TX_PIN * 2);
  GPIOA->MODER &= ~(0b11 << RX_PIN * 2);
  GPIOA->MODER |= (0b10 << TX_PIN * 2);
  GPIOA->MODER |= (0b10 << RX_PIN * 2);

  //za half-duplex za ax
  GPIOA->OTYPER |= (0b1 << TX_PIN);
  GPIOA->PUPDR &= ~(0b11 << TX_PIN*2);
  GPIOA->PUPDR |= (0b01 << TX_PIN*2);

  uint8_t const Alt_function = 7;

  GPIOA->AFR[TX_PIN / 8] &= ~(0b1111 << (TX_PIN % 8) * 4);
  GPIOA->AFR[TX_PIN / 8] |= (Alt_function << (TX_PIN % 8) * 4);
  GPIOA->AFR[RX_PIN / 8] &= ~(0b1111 << (RX_PIN % 8) * 4);
  GPIOA->AFR[RX_PIN / 8] |= (Alt_function << (RX_PIN % 8) * 4);

  USART2->CR1 &= ~(0b1 << 12);		// 8 bita rec
  USART2->CR2 &= ~(0b11 << 12);		// 1 stop bit

  // baudrate na 9600
  // usartdiv => 519. strana formula => 546,875 => 546 u mantisu, 0,875 * 16 = 14 u frakcioni deo
  USART2->BRR = 0;
  USART2->BRR |= ((546 << 4) | (14 << 0));

  // ukljucivanje tx i rx pinova
  USART2->CR1 |= (0b11 << 2);

  // prekid kada dodje poruka
  USART2->CR1 |= (0b1 << 5);
  // ukljuci uart
  USART2->CR1 |= (0b1 << 13);
  // koji prekid
  uint8_t USART2_INTERRUPT = 38;
  NVIC->ISER[USART2_INTERRUPT / 32] |= (0b1 << USART2_INTERRUPT % 32);
  // za sad full duplex, za ax treba half duplex

  //half-duplex:
  USART2->CR2 &= ~(0b1 << 11);
  USART2->CR2 &= ~(0b1 << 14);
  USART2->CR3 &= ~(0b1 << 1);
  USART2->CR3 &= ~(0b1 << 5);
  USART2->CR3 |= (0b1 << 3);
}


void
uart6_send_byte (uint8_t data)
{
  USART6->DR = data; //ovako se salje, kada je usart6->dr sa desne strane, onda se ponasa kao recieve registar

  while (!(USART6->SR & (0b1 << 6)))
    ;

  USART6->SR &= ~(0b1 << 6);
}

// pisi u bafer
void
uart6_write (uint8_t data)
{
  if (uart6_buffer_size != MAX_BUFFER_SIZE)
    {
      uart6_buffer[index6_write] = data;
      index6_write = (index6_write + 1) % MAX_BUFFER_SIZE;
      uart6_buffer_size++;
    }
  else
    {
      uart6_buffer[index6_write] = data;
      index6_write = (index6_write + 1) % MAX_BUFFER_SIZE;
      index6_read = (index6_read + 1) % MAX_BUFFER_SIZE;
    }
}

//citaj iz bafera
uint8_t
uart6_read ()
{
  uint8_t temp_data;

  if (uart6_buffer_size != 0)
    {
      temp_data = uart6_buffer[index6_read];
      index6_read = (index6_read + 1) % MAX_BUFFER_SIZE;
      uart6_buffer_size--;
    }
  return temp_data;
}

bool
uart6_is_empty ()
{
  if (uart6_buffer_size == 0)
    return true;
  else
    return false;
}

void
USART6_IRQHandler ()
{
  if (USART6->SR & (0b1 << 5))
    {
      uart6_write (USART6->DR);
    }
}

void
uart2_send_byte (uint8_t data)
{
  USART2->DR = data; //ovako se salje, kada je usart2->dr sa desne strane, onda se ponasa kao recieve registar

  while (!(USART2->SR & (0b1 << 6)))
    ;

  USART2->SR &= ~(0b1 << 6);
}

// pisi u bafer
void
uart2_write (uint8_t data)
{
  if (uart2_buffer_size != MAX_BUFFER_SIZE)
    {
      uart2_buffer[index2_write] = data;
      index2_write = (index2_write + 1) % MAX_BUFFER_SIZE;
      uart2_buffer_size++;
    }
  else
    {
      uart2_buffer[index2_write] = data;
      index2_write = (index2_write + 1) % MAX_BUFFER_SIZE;
      index2_read = (index2_read + 1) % MAX_BUFFER_SIZE;
    }
}

//citaj iz bafera
uint8_t
uart2_read ()
{
  uint8_t temp_data;

  if (uart2_buffer_size != 0)
    {
      temp_data = uart2_buffer[index2_read];
      index2_read = (index2_read + 1) % MAX_BUFFER_SIZE;
      uart2_buffer_size--;
    }
  return temp_data;
}

bool
uart2_is_empty ()
{
  if (uart2_buffer_size == 0)
    return true;
  else
    return false;
}

void
USART2_IRQHandler ()
{
  if (USART2->SR & (0b1 << 5))
    {
      uart2_write (USART2->DR);
    }
}

/*
 *
 * predlog poruke:
 *
 * s (kao start)
 * poruka (proveri da li je vec predefinisana)
 * (neki simbol za kraj)
 *
 */

/*
 *
 * uart za ax:
 * 1)half duplex
 * 2)baudrate na 9600
 * 3)ID			//0xFE je broadcast ID za sve ax
 *
 * poruka za ax:
 * 1)0xFF 0xFF - start
 * 2)ID
 * 3)duzina poruke
 * 4)instrukcija: slanje i primanje
 * 5)parametri
 * 6)checksum
 *
 */
