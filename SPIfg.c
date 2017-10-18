#include <inttypes.h>
#include "SPIfg.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include "Board.h"

 SPI_Handle  spi;

    uint8_t SPI_transf(uint8_t data) {
        uint8_t tx = data;
        uint8_t rx;

        SPI_Transaction trans;
        trans.txBuf = &tx;
        trans.count = sizeof(tx);
        trans.rxBuf = &rx;
        SPI_transfer(spi, &trans);

        if(trans.status == SPI_STATUS_SUCCESS){
            if(trans.rxBuf == 0){
                return 0;
            }
            else {
               // uint8_t ret = *(uint8_t*)trans.rxBuf;
                if(rx==0){
                return rx;
                }
                else{
                    return rx;

                }
            }
        }
        else
            return 0;
    }

    void SPI_begin() {
          SPI_Params  spiParams;

          SPI_init();

          SPI_Params_init(&spiParams);
          spiParams.transferMode = SPI_MODE_BLOCKING;
          spiParams.transferTimeout = 500000; // timeout 5s
          spiParams.bitRate = 4000000;
          spi = SPI_open(Board_SPI0, &spiParams);

          if (spi == NULL) {
             while(1);
          }
    }

    void initSS() {


    }

    void setSS() {
        GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
    }


    void resetSS() {
        GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_ON);
    }
