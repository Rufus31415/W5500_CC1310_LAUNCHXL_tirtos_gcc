// SPIfg.h

#ifndef _SPIFG_h
#define _SPIFG_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	uint8_t SPI_transf(uint8_t data);

	void SPI_begin();

	void  initSS();
void setSS();
void resetSS();

#ifdef __cplusplus
}
#endif
#endif

