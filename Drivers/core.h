/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CORE_H
#define __CORE_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"

/* Defines ------------------------------------------------------------------*/
#define _FIRMWARE_VER_1     18
#define _FIRMWARE_VER_2     0
#define _FIRMWARE_VER_3     2
#define _FIRMWARE_VER_4     2
#define _FIRMWARE_RES       1

#define MAIN_PROGRAM_START_ADDRESS  0x08008000
#define MAIN_PROGRAM_END_ADDRESS    0x0803F000
#define CRC_ADDRESS                 0x0801C000
#define PAGE_SIZE                   2048
#define NUMB_PAGES                  ((MAIN_PROGRAM_END_ADDRESS-MAIN_PROGRAM_START_ADDRESS)/PAGE_SIZE)
#define NVIC_VectTab_RAM            0x20000000
#define DATA_OFFSET                 0xBFFC

#define ALIGN       volatile __attribute__((aligned(0x04)))
    
#define     INTERFACE_USART       1
#define     INTERFACE_CAN         2
#define     INTERFACE_LIN         3
    
/* Classes ------------------------------------------------------------------*/
class Core
{
    public:
        Core(void);
        void initialize(void);
        void handler(void);
        void delayMs(uint32_t delay);
        void delay_mks(uint32_t delay);
        void protectedFlash(void);
        void incTick(void);
        uint32_t getTick(void);
        void setTimer(uint32_t value);
        uint32_t getTimer(void);
        void resetTimer(void);
        void enterSleepMode(void);
        void disablePeriph(void);
        void enablePeriph(void);
        void InterfaceInit(uint8_t Interf);
        void InterfaceHandler(void);
    
        //uint8_t typeDevice;
        uint8_t verProtocol;
        bool isLinkError;
        uint8_t InterfaceType;

    private:
        void SystemCoreClockInit(void);
        void initUnusedPins(void);
        void remapTable(void);
        void InitWDT(void);
        void ResetWDT(void);
    
        uint32_t msTick;
        uint32_t tickTimer;
        
};
extern Core core;

/* Info ------------------------------------------------------------------*/


#endif /* __CORE_H */
