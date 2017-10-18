/*
IPAddress.h - Base class that provides IPAddress
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

#ifndef IPAddress_h
#define IPAddress_h

#include <stdint.h>

// A class to make it easier to handle and pass around IP addresses

#ifdef __cplusplus
extern "C" {
#endif


typedef union IPAddress
{
	uint8_t bytes[4];  // IPv4 address
	uint32_t dword;
} IPAddress;

	//void IPAddress_Init(IPAddress* ip);
	//void IPAddress_Init_bytes(IPAddress* ip, uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
	void IPAddress_Init_str(IPAddress* ip, const uint8_t *address);

	int IPAddress_fromString(IPAddress* ip, const char *address);

	void IPAddress_toString(IPAddress ip, char* strIP);

#ifdef __cplusplus
}
#endif

#endif
