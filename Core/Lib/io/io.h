/*
 * io.h
 *
 *  Created on: Oct 19, 2023
 *      Author: lazar
 */

#ifndef IO_H_
#define IO_H_

#include <stdbool.h>

void
io_init ();
void
io_led (bool status);
void
io_cinc_loop ();
void
stop_wheel_1 ();
void
stop_wheel_2 ();
void
set_direction_1_wheel_1();
void
set_direction_2_wheel_1();
void
set_direction_1_wheel_2();
void
set_direction_2_wheel_2();


#endif /* IO_H_ */
