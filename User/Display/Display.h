
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DISPLAY_H
#define __DISPLAY_H

/* Includes ------------------------------------------------------------------*/
#include "PeripheralLib.h"
#define     SEG_NUMB    4
#define     DEFAULT_BLINK_PERIOD    1000
#define     UPDATE_PERIOD           250

//---------------------------------------------------------------
    static const uint8_t digitToSegment[] = {
         // XGFEDCBA
          0x3F,//0b00111111,    // 0 //  0
          0x06,//0b00000110,    // 1 //  1
          0x5B,//0b01011011,    // 2 //  2
          0x4F,//0b01001111,    // 3 //  3
          0x66,//0b01100110,    // 4 //  4
          0x6D,//0b01101101,    // 5 //  5
          0x7D,//0b01111101,    // 6 //  6
          0x07,//0b00000111,    // 7 //  7
          0x7F,//0b01111111,    // 8 //  8
          0x6F,//0b01101111,    // 9 //  9
          0x77,//0b01110111,    // A // 10
          0x7C,//0b01111100,    // b // 11
          0x58,//0b01011000,    // c // 12
          0x5E,//0b01011110,    // d // 13
          0x79,//0b01111001,    // E // 14
          0x71,//0b01110001,    // F // 15
          0x40,//0b01000000,    // - // 16
          0x54,//0b01010100,    // n // 17
          0x78,//0b01111000,    // t // 18
          0x50,//0b01010000,    // r // 19
          0x73,//0b01110011,    // P // 20
          0x38,//0b00111000,    // L // 21
          0x39,//0b00111001,    // C // 22
          0x5C,//0b01011100,    // o // 23
        };

/* Classes ------------------------------------------------------------------*/
class TDisplay
{
    public:
        uint8_t ViewData[SEG_NUMB];
        uint16_t BlinkPeriod;
        uint8_t BlinkSegSet[SEG_NUMB];
        uint8_t SegState[SEG_NUMB];
    
        void initialize(void);
        void handler(void); 
        void Off(void); 
        void viewON(void);
        void viewBlack(void);
        void viewError(uint8_t err);
        void viewPower(uint8_t pwr, bool timerState);
        void viewTemp(int16_t temp);
        void viewOff(void);
        void viewLock(void);
        void viewLockOff();
        void viewTimer(uint16_t time);
        void viewTimerOn(void);
        void viewTimerOff(void);
        void Test(void);
        void SetBlink(uint8_t blnk, uint16_t period);
        void BlinkOff(void);
        void SetBlink(uint8_t blnkStartSeg, uint8_t blnkSegNumb, uint16_t period);
        void SetBlinkAll(uint16_t period);

    private:


};
extern TDisplay display;

#endif 
