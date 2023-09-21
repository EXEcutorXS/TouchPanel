
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LEDDRIVER_H
#define __TM1637_H

/* Includes ------------------------------------------------------------------*/
#include "PeripheralLib.h"
//---------------------------------------------------------------
#define     CLOCK_TIMING        8
#define     DO_PORT         GPIOB
#define     DO_PIN          GPIO_PIN_15
#define     CLK_PORT        GPIOB
#define     CLK_PIN         GPIO_PIN_13
#define     WRITE_PORT      GPIOB
#define     WRITE_PIN       GPIO_PIN_11
#define     ENABLE_PORT     GPIOB
#define     ENABLE_PIN      GPIO_PIN_12
#define     PWM_PORT        GPIOB
#define     PWM_PIN         GPIO_PIN_0
//===============================================

static const uint8_t BitOrder[] = { 6, 7, 1, 2, 3, 5, 4, 0 };

/* Classes ------------------------------------------------------------------*/
class TLedDriver
{
    public:

        void initialize(void);
        void handler(void);  
        void WriteDisplay(uint8_t* data);
        void DisplayOn(uint8_t bright);
        void DisplayOff(void);

    private:
        void setDO(void);
        void resetDO(void);
        void setCLK(void);
        void resetCLK(void);
        void setWrite(void);
        void resetWrite(void);
    
        void Delay_tick(uint16_t i);
        void WriteByte(uint8_t oneByte);
        void WriteCommand(uint8_t cmd);
        uint8_t ConvertBitOrder(uint8_t bt);

};
extern TLedDriver ledDriver;

#endif 
