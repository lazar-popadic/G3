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


#endif /* IO_H_ */
