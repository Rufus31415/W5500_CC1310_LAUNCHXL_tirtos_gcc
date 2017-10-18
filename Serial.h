// Serial.h

#ifndef _SERIAL_h
#define _SERIAL_h

#ifdef __cplusplus
extern "C" {
#endif

void Serial_begin();

void Serial_print(char* txt);
void Serial_println(char* txt);
void Serial_write(char c);

#ifdef __cplusplus
}
#endif

#endif

