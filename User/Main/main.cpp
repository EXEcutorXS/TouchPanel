/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private function prototypes -----------------------------------------------*/
//==============================================================================

const uint8_t _CRC[11]  __attribute__((at(CRC_ADDRESS))) = 
{
    0x55, 0x55, 0x55, 0x55,                                             // длина ПО
    0x55, 0x55,                                                         // CRC ПО
    _FIRMWARE_VER_1,_FIRMWARE_VER_2,_FIRMWARE_VER_3,_FIRMWARE_VER_4,    // версия ПО
    _FIRMWARE_RES                                                       // резерв
};

//-----------------------------------------------------
int main(void)
{

    __disable_irq();
    core.initialize();
    core.InterfaceInit(INTERFACE_USART);
    sensor.initialize();
    buzzer.initialize();
    display.initialize();
    LedLine.initialize();
    stove.initialize();

    //***********************************
    
    __enable_irq();

    while(1) {
       
        core.handler();
        core.InterfaceHandler();
        sensor.handler();
        LedLine.handler();
        buzzer.handler();
        display.handler();
        stove.handler();

    }
}
//-----------------------------------------------------

