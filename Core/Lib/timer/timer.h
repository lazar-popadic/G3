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

#define SENSORS_HIGH 		1
#define SENSORS_LOW 		2
#define SENSORS_MECHANISM 	3
#define SENSORS_HIGH_AND_LOW 	4
#define SENSORS_OFF 		0
#define HOME			199

void
timer_init ();
bool
timer_delay_nonblocking (uint32_t delay_ms);
bool
task_timeout (uint32_t delay_ms);
void
timer_start_sys_time ();
void
timer_stop_sys_time ();
void
reset_and_stop_timer ();
bool
timer_end ();
bool
timer_home ();

#endif /* LIB_PERIPHERY_TIMER_TIMER_H_ */
