/*
 * ArduinoCompatibility.c
 *
 *  Created on: 25 sept. 2017
 *      Author: flore
 */



#include <unistd.h>
#include <time.h>
#include <ti/sysbios/hal/Seconds.h>

unsigned long millis(void){
    //return (CLK_getltime() * CLK_getprd()) / CLK_countspms();
    Seconds_Time T;

    Seconds_getTime(&T);

    return T.secs*1000 + T.nsecs/1e6;
}

void delay(unsigned long ms){
    usleep(ms*1000);
}

