
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TM1637_H
#define __TM1637_H

/* Includes ------------------------------------------------------------------*/
#include "PeripheralLib.h"
//---------------------------------------------------------------
#define     I2C_TIMING   4
#define     DIO_PORT   GPIOB
#define     DIO_PIN    GPIO_PIN_14
#define     CLK_PORT   GPIOB
#define     CLK_PIN    GPIO_PIN_15

#define     DATA_CMD_SET        0x40
#define     DISPLAY_CMD_SET     0x80
#define     ADDRESS_CMD_SET     0xC0

#define     DATA_CMD_ADR_INC    0x00
#define     DATA_CMD_ADR_FIX    0x04

#define     ADDRESS_POS_1       0x00
#define     ADDRESS_POS_2       0x01
#define     ADDRESS_POS_3       0x02
#define     ADDRESS_POS_4       0x03
#define     ADDRESS_POS_5       0x04
#define     ADDRESS_POS_6       0x05

#define     DISPLAY_ON          0x08
#define     DISPLAY_OFF         0x00
#define     DISPLAY_PULSE_1_16         0x00
#define     DISPLAY_PULSE_2_16         0x01
#define     DISPLAY_PULSE_4_16         0x02
#define     DISPLAY_PULSE_10_16        0x03
#define     DISPLAY_PULSE_11_16        0x04
#define     DISPLAY_PULSE_12_16        0x05
#define     DISPLAY_PULSE_13_16        0x06
#define     DISPLAY_PULSE_14_16        0x07
    
/* Classes ------------------------------------------------------------------*/
class T_TM1637
{
    public:

        void initialize(void);
        void handler(void);  
        void WriteDisplay(uint8_t* data);
        void DisplayOn(uint8_t bright);
        void DisplayOff(void);

    private:
//        void setDIO(void);
//        void resetDIO(void);
//        void setCLK(void);
//        void resetCLK(void);
        void Delay_tick(uint16_t i);
        void ConfigDIOinput(void);
        void ConfigDIOout(void);
        void WaitASK(void);
        void I2CStart(void);
        void I2CStop(void);
        void I2Cask(void);
        void I2CWriteByte(uint8_t oneByte);
        void WriteCommand(uint8_t cmd);
        uint8_t brightness;


};
extern T_TM1637 tm1637;

#endif 
