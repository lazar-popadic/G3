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
uart_send_byte (uint8_t data);

void
uart_write (uint8_t data);

uint8_t
uart_read ();

bool
uart_is_empty ();



#endif /* LIB_PERIPHERY_UART_UART_H_ */
