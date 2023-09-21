//
//    ћодуль св€зи блока по последовательному каналу USART
//    

/* Includes ------------------------------------------------------------------*/
#include "Buzzer.h"
#include "core.h"

TBuzzer buzzer;
//-----------------------------------------------------

//-----------------------------------------------------
void TBuzzer::initialize(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    RepeatPeriod = 0;
    NumbRepeat = 0;

}
//-----------------------------------------------------
void TBuzzer::handler(void)
{
    if (On_f) {
        if ((core.getTick() - PulseStartTime) > PulseTime) {        
            SetOutOff();
        }
    }  
    if ((core.getTick() - PulseStartTime) > RepeatPeriod) {  
        PulseStartTime = core.getTick();        
        if (NumbRepeat > 0) {
            NumbRepeat--;         
            SetOutOn();        
        }
    }
}
//---------------------------------------------
void TBuzzer::SetOutOn(void)
{
    gpio_bit_set(GPIOA, GPIO_PIN_12);  
    On_f = true;
}
//---------------------------------------------
void TBuzzer::SetOutOff(void)
{
    gpio_bit_reset(GPIOA, GPIO_PIN_12); 
    On_f = false;    
}
//-----------------------------------------------------
void TBuzzer::StartBeep(uint16_t pulseTime)
{
    if (!On_f) {
        PulseStartTime = core.getTick();
        PulseTime = pulseTime;
        NumbRepeat = 0;
        RepeatPeriod = 5000;
        SetOutOn();
    }
}
//-----------------------------------------------------
void TBuzzer::StartSeriesBeep(uint16_t pulseTime, uint16_t Period, uint8_t numbPulse)
{
    if (!On_f) {
        PulseStartTime = core.getTick();
        PulseTime = pulseTime;
        RepeatPeriod = Period;
        NumbRepeat = numbPulse-1;
        SetOutOn();
    }
}
//-----------------------------------------------------
