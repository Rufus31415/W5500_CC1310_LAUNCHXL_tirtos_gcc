/*
 Print.cpp - Base class that provides print() and print_newline()
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
 
 Modified 23 November 2006 by David A. Mellis
 Modified 03 August 2015 by Chuck Todd
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef ARDUINO
#include "Arduino.h"
#endif
#include "Printfg.h"

// Public Methods //////////////////////////////////////////////////////////////

/* default implementation: may be overridden */
//size_t methods->write(object, void* object, PrintMethods* methods, const uint8_t *buffer, size_t size)
//{
//  size_t n = 0;
//  while (size--) {
//    if (write_byte(methods, *buffer++)) n++;
//    else break;
//  }
//  return n;
//}

size_t print(void* object, PrintMethods* methods, const char str[])
{
  return write_string(object, methods, str);
}

size_t print_char(void* object, PrintMethods* methods, char c)
{
  return write_byte(object, methods, c);
}

size_t print_uchar(void* object, PrintMethods* methods, unsigned char b)
{
	return print_uchar_base(object, methods, (unsigned long)b, DEC);
}
size_t print_uchar_base(void* object, PrintMethods* methods, unsigned char b, int base)
{
	return print_ulong_base(object, methods, (unsigned long)b, base);
}

size_t print_int_base(void* object, PrintMethods* methods, int n, int base)
{
	return print_long_base(object, methods, (long)n, base);
}
size_t print_int(void* object, PrintMethods* methods, int n)
{
	return print_int_base(object, methods, (long)n, DEC);
}

size_t print_uint_base(void* object, PrintMethods* methods, unsigned int n, int base)
{
	return print_ulong_base(object, methods, (unsigned long)n, base);
}
size_t print_uint(void* object, PrintMethods* methods, unsigned int n)
{
	return print_uint_base(object, methods, (unsigned long)n, DEC);
}

size_t print_long_base(void* object, PrintMethods* methods, long n, int base)
{
  if (base == 0) {
    return write_byte(object, methods, n);
  } else if (base == 10) {
    if (n < 0) {
      int t = print_char(object, methods, '-');
      n = -n;
      return printNumber(object, methods, n, 10) + t;
    }
    return printNumber(object, methods, n, 10);
  } else {
    return printNumber(object, methods, n, base);
  }
}
size_t print_long(void* object, PrintMethods* methods, long n) {
	return print_long_base(object, methods, n, DEC);
}

size_t print_ulong_base(void* object, PrintMethods* methods, unsigned long n, int base)
{
	if (base == 0) return write_byte(object, methods, n);
	else return printNumber(object, methods, n, base);
}
size_t print_ulong(void* object, PrintMethods* methods, unsigned long n)
{
	return print_ulong_base(object, methods, n, DEC);
}

size_t print_double_resolution(void* object, PrintMethods* methods, double n, int digits)
{
	return printFloat(object, methods, n, digits);
}
size_t print_double(void* object, PrintMethods* methods, double n)
{
	return printFloat(object, methods, n, 2);
}

size_t print_newline(void* object, PrintMethods* methods)
{
  return write_string(object, methods, "\r\n");
}

size_t println(void* object, PrintMethods* methods, const char c[])
{
  size_t n = print(object, methods, c);
  n += print_newline(object, methods);
  return n;
}

size_t println_char(void* object, PrintMethods* methods, char c)
{
  size_t n = print_char(object, methods, c);
  n += print_newline(object, methods);
  return n;
}

size_t println_uchar_base(void* object, PrintMethods* methods, unsigned char b, int base)
{
  size_t n = print_uchar_base(object, methods, b, base);
  n += print_newline(object, methods);
  return n;
}
size_t println_uchar(void* object, PrintMethods* methods, unsigned char b) {
	return println_uchar_base(object, methods, b, DEC);
}

size_t println_int_base(void* object, PrintMethods* methods, int num, int base)
{
  size_t n = print_int_base(object, methods, num, base);
  n += print_newline(object, methods);
  return n;
}
size_t println_int(void* object, PrintMethods* methods, int num) {
	return println_int_base(object, methods,num, DEC);
}

size_t println_uint_base(void* object, PrintMethods* methods, unsigned int num, int base)
{
  size_t n = print_uint_base(object, methods, num, base);
  n += print_newline(object, methods);
  return n;
}
size_t println_uint(void* object, PrintMethods* methods, unsigned int num) {
	return println_uint_base(object, methods, num, DEC);
}

size_t println_long_base(void* object, PrintMethods* methods, long num, int base)
{
  size_t n = print_long_base(object, methods, num, base);
  n += print_newline(object, methods);
  return n;
}
size_t println_long(void* object, PrintMethods* methods, long num) {
	return println_long_base(object, methods, num, DEC);
}

size_t println_ulong_base(void* object, PrintMethods* methods, unsigned long num, int base)
{
  size_t n = print_ulong_base(object, methods, num, base);
  n += print_newline(object, methods);
  return n;
}
size_t println_ulong(void* object, PrintMethods* methods, unsigned long num)
{
	return println_ulong_base(object, methods, num, DEC);
}

size_t println_double_resolution(void* object, PrintMethods* methods, double num, int digits)
{
  size_t n = print_double_resolution(object, methods, num, digits);
  n += print_newline(object, methods);
  return n;
}
size_t println_double(void* object, PrintMethods* methods, double num)
{
	return println_double_resolution(object, methods, num, 2);
}

// Private Methods /////////////////////////////////////////////////////////////

size_t printNumber(void* object, PrintMethods* methods, unsigned long n, uint8_t base)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *str = &buf[sizeof(buf) - 1];

  *str = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    char c = n % base;
    n /= base;

    *--str = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  return write_string(object, methods, str);
}

size_t printFloat(void* object, PrintMethods* methods, double number, uint8_t digits)
{ 
  size_t n = 0;
  
  if (isnan(number)) return print(object, methods, "nan");
  if (isinf(number)) return print(object, methods, "inf");
  if (number > 4294967040.0) return print (object, methods, "+ovf");  // constant determined empirically
  if (number <-4294967040.0) return print (object, methods, "-ovf");  // constant determined empirically
  
  // Handle negative numbers
  if (number < 0.0)
  {
     n += print_char(object, methods, '-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  n += print_ulong(object, methods, int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    n += print(object, methods, ".");
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    unsigned int toPrint = (unsigned int)(remainder);
    n += print_uint(object, methods, toPrint);
    remainder -= toPrint; 
  } 
  
  return n;
}

size_t write_byte(void* object, PrintMethods* methods, uint8_t b)
{
	return methods->write(object, &b, 1);
}

size_t write_string(void* object, PrintMethods* methods, const char *str) {
	if (str == NULL) return 0;
	return methods->write(object, (const uint8_t *)str, strlen(str));
}

size_t write_string_length( void* object, PrintMethods* methods, const char *buffer, size_t size) {
	return methods->write(object, (const uint8_t *)buffer, size);
}