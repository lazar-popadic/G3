/*
 * timer.h
 *
 *  Created on: Oct 19, 2023
 *      Author: lazar
 */

#ifndef LIB_PERIPHERY_TIMER_TIMER_H_
#define LIB_PERIPHERY_TIMER_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

void
timer_init ();
bool
timer_delay_nonblocking (uint32_t delay_ms);
void
timer_start_sys_time ();
bool
timer_end ();

#endif /* LIB_PERIPHERY_TIMER_TIMER_H_ */
