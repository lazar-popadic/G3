/*
 * uart.h
 *
 *  Created on: Nov 2, 2023
 *      Author: lazar
 */

#ifndef LIB_PERIPHERY_UART_UART_H_
#define LIB_PERIPHERY_UART_UART_H_

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>

void
uart_init ();

void
uart6_send_byte (uint8_t data);
void
uart6_write (uint8_t data);
uint8_t
uart6_read ();
bool
uart6_is_empty ();

void
uart2_send_byte (uint8_t data);
void
uart2_write (uint8_t data);
uint8_t
uart2_read ();
bool
uart2_is_empty ();



#endif /* LIB_PERIPHERY_UART_UART_H_ */
