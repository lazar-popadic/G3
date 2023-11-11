/*
 * timer.h
 *
 *  Created on: Oct 19, 2023
 *      Author: lazar
 */

#ifndef LIB_PERIPHERY_TIMER_TIMER_H_
#define LIB_PERIPHERY_TIMER_TIMER_H_

#include <stdint.h>		//za tip uint
#include <stdbool.h>
#include "stm32f4xx.h"

void
timer_init ();
bool
timer_delay_nonblocking (uint32_t delay_ms);
int16_t
timer_speed_of_encoder ();

#endif /* LIB_PERIPHERY_TIMER_TIMER_H_ */
