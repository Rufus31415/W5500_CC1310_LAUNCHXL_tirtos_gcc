
#include "Serial.h"

#include <ti/drivers/UART.h>
#include "string.h"
#include "Board.h"
UART_Handle uart;


void Serial_begin(void){

    UART_Params uartParams;    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;

    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }

}



void Serial_print(char* txt){
    UART_write(uart, txt, strlen(txt));
}
void Serial_println(char* txt){
    Serial_print(txt);
    Serial_print("\n\r");
}
void Serial_write(char c){
    UART_write(uart, &c, 1);
}
