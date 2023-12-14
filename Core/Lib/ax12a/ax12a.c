/*
 * ax12a.c
 *
 *  Created on: Nov 11, 2023
 *      Author: lazar
 */

#include "ax12a.h"

#include "../uart/uart.h"

/*
 * ugao: (OD 0 DO 1023 => 0D 0 stepeni DO 300 stepeni)
 * 0 = 0
 * 0x1ff = 511 = 150 stepeni
 * 0x3ff = 1023 = 300 stepeni
 *
 * brzina: (OD 1 DO 1023 => OD minimalne DO maksimalne) (0 je takodje maksimalna za dat napon, NEMA KONTROLE BRZINE!)
 * 0 = max bez kontrole
 * 1 = minimum
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

//pinguje status ax-a, u kruzni bafer ubacuje, u hex gledaj u live expression
//  uint8_t ax_ping[] = {0xFF, 0xFF, 0x01, 0x02, 0x01, 0xFB};
