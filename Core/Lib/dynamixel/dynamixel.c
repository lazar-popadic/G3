/*
 * ax12a.c
 *
 *  Created on: Nov 11, 2023
 *      Author: lazar
 */

#include "dynamixel.h"

#include "../uart/uart.h"

/*
 * ugao: (OD 0 DO 1023 => 0D 0 stepeni DO 300 stepeni)
 * 0 = 0
 * 0x1ff = 511 = 150 stepeni
 * 0x3ff = 1023 = 300 stepeni
 *
 * brzina: (OD 1 DO 1023 => OD minimalne DO maksimalne)
 * 1 = minimum
 * 528 = maximum za ax12a
 * ? = maximum za xl320
 * 0x3ff = 1023 = maximum (114 rpm, ali pise da je max zapravo oko 59 rpm.
 */

void
ax_move (uint8_t id, uint16_t angle, uint16_t speed)
{
  uint8_t angle_low, angle_high, speed_low, speed_high;
  angle_low = angle & 0xff;
  angle_high = angle >> 8;
  speed_low = speed & 0xff;
  speed_high = speed >> 8;

  uint16_t checksum_local = id + 7 + 3 + 0x1e + angle_low + angle_high
      + speed_low + speed_high;
  uint8_t checksum = (uint8_t) (~checksum_local);
  uint8_t ax_move[] =
    { 0xff, 0xff, id, 0x07, 0x03, 0x1E, angle_low, angle_high, speed_low,
	speed_high, checksum };

  for (uint8_t i = 0; i < 11; i++)
    {
      uart_send_byte (ax_move[i]);
    }
}

void
solar_out_l ()
{
  ax_move (10, 40, 520);
}

void
solar_in_l ()
{
  ax_move (10, 680, 520);
}

void
solar_out_r ()
{
  ax_move (9, 1000, 520);
}

void
solar_in_r ()
{
  ax_move (9, 360, 520);
}

void
mechanism_up ()
{
  ax_move (7, 784, 250);
}

void
mechanism_half_up ()
{
  ax_move (7, 675, 100);
}

void
mechanism_half_down ()
{
  ax_move (7, 675, 200);
}

void
mechanism_down_pot ()
{
  ax_move (7, 750, 200);
}

void
mechanism_down ()
{
  ax_move (7, 512, 100);
}

void
mechanism_open ()
{
  ax_move (4, 485, 500);
}

void
mechanism_open_almost ()
{
  ax_move (4, 550, 500);
}

void
mechanism_open_slow ()
{
  ax_move (4, 485, 100);
}

void
mechanism_half_open ()
{
  ax_move (4, 730, 500);
}

void
mechanism_close ()
{
  ax_move (4, 900, 400);
}

