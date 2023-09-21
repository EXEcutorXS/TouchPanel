//
//    ћодуль св€зи блока по последовательному каналу USART
//    

/* Includes ------------------------------------------------------------------*/
#include "LedLine.h"
#include "core.h"
#include "CapSensor.h"

TLedLine LedLine;
//-----------------------------------------------------

//-----------------------------------------------------
void TLedLine::initialize(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    gpio_bit_reset(GPIOA, GPIO_PIN_11); 
    On_f = false;   
}
//-----------------------------------------------------
void TLedLine::handler(void)
{
	if (sensor.AnyPressed) 
    {
		On();
	} else {
		Off();
    }
}
//---------------------------------------------
void TLedLine::On(void)
{
    gpio_bit_set(GPIOA, GPIO_PIN_11);  
    On_f = true;
}
//---------------------------------------------
void TLedLine::Off(void)
{
    gpio_bit_reset(GPIOA, GPIO_PIN_11); 
    On_f = false;    
}
//-----------------------------------------------------
bool TLedLine::getState(void)
{
    return On_f;    
}
//-----------------------------------------------------
