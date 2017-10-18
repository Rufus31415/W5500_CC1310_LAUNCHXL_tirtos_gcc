/*
  Print.h - Base class that provides print() and print_newline()
  Copyright (c) 2008 David A. Mellis.  All right reserved.

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

#ifndef Printfg_h
#define Printfg_h

#include <inttypes.h>
#include <stdio.h> // for size_t

//
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

#ifdef __cplusplus
extern "C" {
#endif


// pointer to write function

typedef size_t(*PrintWrite)(void* object, const uint8_t *buf, size_t size);

typedef struct PrintMethods_ {
	PrintWrite write;
} PrintMethods;

    size_t printNumber(void* object, PrintMethods* methods, unsigned long, uint8_t);
    size_t printFloat(void* object, PrintMethods* methods, double, uint8_t);


	size_t write_byte(void* object, PrintMethods* methods, uint8_t b);

	size_t write_string(void* object, PrintMethods* methods, const char *str);

	size_t write_string_length(void* object, PrintMethods* methods, const char *buffer, size_t size);

    size_t print(void* object, PrintMethods* methods, const char[]);
    size_t print_char(void* object, PrintMethods* methods, char);
    size_t print_uchar(void* object, PrintMethods* methods, unsigned char);
    size_t print_int(void* object, PrintMethods* methods, int);
    size_t print_uint(void* object, PrintMethods* methods, unsigned int);
    size_t print_long(void* object, PrintMethods* methods, long);
    size_t print_ulong(void* object, PrintMethods* methods, unsigned long);
    size_t print_double(void* object, PrintMethods* methods, double);
	size_t print_uchar_base(void* object, PrintMethods* methods, unsigned char, int);
	size_t print_int_base(void* object, PrintMethods* methods, int, int);
	size_t print_uint_base(void* object, PrintMethods* methods, unsigned int, int);
	size_t print_long_base(void* object, PrintMethods* methods, long, int);
	size_t print_ulong_base(void* object, PrintMethods* methods, unsigned long, int);
	size_t print_double_resolution(void* object, PrintMethods* methods, double, int);

    size_t println(void* object, PrintMethods* methods, const char[]);
    size_t println_char(void* object, PrintMethods* methods, char);
    size_t println_uchar(void* object, PrintMethods* methods, unsigned char);
    size_t println_int(void* object, PrintMethods* methods, int);
    size_t println_uint(void* object, PrintMethods* methods, unsigned int);
    size_t println_long(void* object, PrintMethods* methods, long);
    size_t println_ulong(void* object, PrintMethods* methods, unsigned long);
    size_t println_double(void* object, PrintMethods* methods, double);
	size_t println_uchar_base(void* object, PrintMethods* methods, unsigned char, int);
	size_t println_int_base(void* object, PrintMethods* methods, int, int);
	size_t println_uint_base(void* object, PrintMethods* methods, unsigned int, int);
	size_t println_long_base(void* object, PrintMethods* methods, long, int);
	size_t println_ulong_base(void* object, PrintMethods* methods, unsigned long, int);
	size_t println_double_resolution(void* object, PrintMethods* methods, double, int);
	size_t print_newline(void* object, PrintMethods* methods);


#define GENERATE_PRINT_HEADER(ClassName)  extern PrintMethods  ClassName##PrintMethods;\
	size_t ClassName##_write_byte(ClassName * object, uint8_t b);\
	size_t ClassName##_write_string(ClassName * object, const char *str);\
	size_t ClassName##_write(void* eth, const uint8_t *buffer, size_t size);\
	size_t ClassName##_print(ClassName * object, const char[]);\
	size_t ClassName##_print_char(ClassName * object, char c);\
	size_t ClassName##_print_uchar(ClassName * object, unsigned char c);\
	size_t ClassName##_print_int(ClassName * object, int);\
	size_t ClassName##_print_uint(ClassName * object, unsigned int);\
	size_t ClassName##_print_long(ClassName * object, long);\
	size_t ClassName##_print_ulong(ClassName * object, unsigned long);\
	size_t ClassName##_print_double(ClassName * object, double);\
	size_t ClassName##_println(ClassName * object, const char[]);\
	size_t ClassName##_println_char(ClassName * object, char);\
	size_t ClassName##_println_uchar(ClassName * object, unsigned char);\
	size_t ClassName##_println_int(ClassName * object, int);\
	size_t ClassName##_println_uint(ClassName * object, unsigned int);\
	size_t ClassName##_println_long(ClassName * object, long);\
	size_t ClassName##_println_ulong(ClassName * object, unsigned long);\
	size_t ClassName##_println_double(ClassName * object, double);\
	size_t ClassName##_print_newline(ClassName * object);


#define GENERATE_PRINT_INSTANCE(ClassName) PrintMethods  ClassName##PrintMethods = { & ClassName##_write };\
	size_t ClassName##_write_byte(ClassName * eth, uint8_t b) {\
		return write_byte(eth, & ClassName##PrintMethods, b);\
	}\
	size_t ClassName##_write_string(ClassName * eth, const char *str) {\
		return write_string(eth, & ClassName##PrintMethods, str);\
	}\
	size_t ClassName##_write_string_length(ClassName * eth, const char *buffer, size_t size) {\
		return write_string_length(eth, & ClassName##PrintMethods, buffer, size);\
	}\
	size_t ClassName##_print(ClassName * eth, const char* buffer) {\
		return print(eth, & ClassName##PrintMethods, buffer);\
	}\
	size_t ClassName##_print_char(ClassName * eth, char c) {\
		return print_char(eth, & ClassName##PrintMethods, c);\
	}\
	size_t ClassName##_print_uchar(ClassName * eth, unsigned char c) {\
		return print_uchar(eth, & ClassName##PrintMethods, c);\
	}\
	size_t ClassName##_print_int(ClassName * eth, int v) {\
		return print_int(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_print_uint(ClassName * eth, unsigned int v) {\
		return print_uint(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_print_long(ClassName * eth, long v) {\
		return print_long(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_print_ulong(ClassName * eth, unsigned long v) {\
		return print_ulong(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_print_double(ClassName * eth, double v) {\
		return print_double(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_println(ClassName * eth, const char* str) {\
		return println(eth, & ClassName##PrintMethods, str);\
	}\
	size_t ClassName##_println_char(ClassName * eth, char c) {\
		return println_char(eth, & ClassName##PrintMethods, c);\
	}\
	size_t ClassName##_println_uchar(ClassName * eth, unsigned char c) {\
		return println_uchar(eth, & ClassName##PrintMethods, c);\
	}\
	size_t ClassName##_println_int(ClassName * eth, int v) {\
		return println_int(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_println_uint(ClassName * eth, unsigned int v) {\
		return println_uint(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_println_long(ClassName * eth, long v) {\
		return println_long(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_println_ulong(ClassName * eth, unsigned long v) {\
		return println_ulong(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_println_double(ClassName * eth, double v) {\
		return println_double(eth, & ClassName##PrintMethods, v);\
	}\
	size_t ClassName##_print_newline(ClassName * eth) {\
		return print_newline(eth, & ClassName##PrintMethods);\
	}

#ifdef __cplusplus
}
#endif


#endif
