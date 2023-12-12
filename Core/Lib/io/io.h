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
io_protocinc ();
bool
io_cinc ();
void
stop_wheel_1 ();
void
stop_wheel_2 ();
void
wheel_1_forwards ();
void
wheel_1_backwards ();
void
wheel_2_forwards ();
void
wheel_2_backwards ();


#endif /* IO_H_ */
