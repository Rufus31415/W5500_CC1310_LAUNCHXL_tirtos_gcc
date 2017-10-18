/*
  IPAddress.cpp - Base class that provides IPAddress
  Copyright (c) 2011 Adrian McEwen.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write_byte to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "IPAddress.h"
#include "stdio.h"

void IPAddress_Init_str(IPAddress* ip, const uint8_t *address)
{
//    memcpy(ip->bytes, address, sizeof(ip->bytes));
    ip->bytes[0] = address[0];
    ip->bytes[1] = address[1];
    ip->bytes[2] = address[2];
    ip->bytes[3] = address[3];


}

int IPAddress_fromString(IPAddress* ip, const char *address)
{
    // TODO: add support for "a", "a.b", "a.b.c" formats

    uint16_t acc = 0; // Accumulator
    uint8_t dots = 0;

    while (*address)
    {
        char c = *address++;
        if (c >= '0' && c <= '9')
        {
            acc = acc * 10 + (c - '0');
            if (acc > 255) {
                // Value out of [0..255] range
                return 0;
            }
        }
        else if (c == '.')
        {
            if (dots == 3) {
                // Too much dots (there must be 3 dots)
                return 0;
            }
			ip->bytes[dots++] = acc;
            acc = 0;
        }
        else
        {
            // Invalid char
            return 0;
        }
    }

    if (dots != 3) {
        // Too few dots (there must be 3 dots)
        return 0;
    }
	ip->bytes[3] = acc;
    return 1;
}


void IPAddress_toString(IPAddress ip, char* strIP) {
	sprintf(strIP, "%u.%u.%u.%u", ip.bytes[0], ip.bytes[1], ip.bytes[2], ip.bytes[3]);
}
