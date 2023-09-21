//
//    Модуль связи блока по последовательному каналу USART
//    

/* Includes ------------------------------------------------------------------*/
#include "CapSensor.h"
#include "core.h"

CapacitSensor sensor;

//-----------------------------------------------------

#define CLK_LOW()           gpio_bit_reset(SENS_CLK_PORT, SENS_CLK_PIN)
#define CLK_HIGH()          gpio_bit_set(SENS_CLK_PORT, SENS_CLK_PIN) 
#define DATA_LOW()          gpio_bit_reset(SENS_DO_PORT, SENS_DO_PIN) 
#define DATA_HIGH()         gpio_bit_set(SENS_DO_PORT, SENS_DO_PIN)
#define READ_DATA_LINE()    gpio_input_bit_get(SENS_DO_PORT, SENS_DO_PIN)


#define TIMER_MKS   TIMER6
#define TICK_TIME   50

//-----------------------------------------------------
void CapacitSensor::initialize(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(SENS_DO_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, SENS_DO_PIN);
    gpio_init(SENS_CLK_PORT, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, SENS_CLK_PIN);
    gpio_bit_set(SENS_DO_PORT, SENS_DO_PIN); 
    gpio_bit_set(SENS_CLK_PORT, SENS_CLK_PIN); 
    rcu_periph_clock_enable(RCU_TIMER6);
    timer_deinit(TIMER_MKS);
    ClearSensors();

}
//-----------------------------------------------------
void CapacitSensor::handler(void)
{
    ReadSensors();
	
	bool tempPressed = false;
	for(int i=0;i<SENS_NUMB;i++)
	if (state[i])
		tempPressed=true;
	AnyPressed = tempPressed;
      
}
//-----------------------------------------------------
void CapacitSensor::ClearSensors(void)
{
    for(uint8_t N=0; N<SENS_NUMB; N++) {
        sensPresCnt[N] = 0;
        SensState[N] = NO_PRESS;
    }
}
//-----------------------------------------------------
void CapacitSensor::tickDelay(uint32_t mks)
{
    uint16_t tm = SystemCoreClock/1000000/5*mks;
    for (uint16_t i=0; i<tm; i++)
    {
    }
//    uint32_t del = SystemCoreClock/1000000/8*mks;
//    timer_disable(TIMER_MKS);
//    timer_update_event_enable(TIMER_MKS);
//    timer_counter_up_direction(TIMER_MKS);
//    timer_prescaler_config(TIMER_MKS, 8, TIMER_PSC_RELOAD_NOW);
//    timer_autoreload_value_config(TIMER_MKS, del);
//    timer_counter_value_config(TIMER_MKS , 0);
//    timer_single_pulse_mode_config(TIMER_MKS, TIMER_SP_MODE_SINGLE);
//    timer_enable(TIMER_MKS);
//    timer_interrupt_flag_clear(TIMER_MKS, TIMER_INT_FLAG_UP);
//    uint32_t timeout = 1000;
//    while (timer_flag_get(TIMER_MKS, TIMER_FLAG_UP) == RESET) {
//        if (timeout > 0) timeout--;
//        else break;
//    }
//    timer_flag_clear(TIMER_MKS, TIMER_FLAG_UP);
//    timer_disable(TIMER_MKS);
}
//-----------------------------------------------------
void CapacitSensor::ReadSensors(void)
{
    static uint32_t timer = 0;
    if ((core.getTick() - timer) >= READ_PERIOD) {
        timer = core.getTick();
        
//        DATA_LOW();
//        tickDelay(TICK_TIME);
        CLK_LOW();
        tickDelay(TICK_TIME);
        CLK_HIGH();
        DATA_HIGH();
        
        uint8_t i = 0;
        for (i=0; i<SENS_NUMB; i++)
        {
            CLK_HIGH();
            tickDelay(TICK_TIME);
            if (READ_DATA_LINE() == RESET) state[i] = 1; // нажата
            else state[i] = 0;
            CLK_LOW();
            tickDelay(TICK_TIME);
        }
        uint8_t numbPressed = 0;
        for (i=0; i<4; i++)
        {
            CLK_HIGH();
            tickDelay(TICK_TIME);
            if (READ_DATA_LINE() == SET) numbPressed += 1<<i;
            CLK_LOW();
            tickDelay(TICK_TIME);
        }
        uint8_t stop = 0;
        for (i=0; i<3; i++)
        {
            CLK_HIGH();
            tickDelay(TICK_TIME);
            if (READ_DATA_LINE() == SET) stop += 1<<i;
            CLK_LOW();
            tickDelay(TICK_TIME);
        }
        DATA_HIGH();
        CLK_HIGH();
        if ((stop == 2)&&(numbPressed < 5)) {
            for (i=0; i<SENS_NUMB; i++)
            {
                if (state[i] == 1) {
                    sensPresCnt[i]++;
                } else {
                    if ((sensPresCnt[i] > 0)&&(sensPresCnt[i] < LONG_PRESS_TIME)) {
                        SensState[i] = SHORT_PRESS;
                    } else {
                        if (sensPresCnt[i] == 0) SensState[i] = NO_PRESS;
                    }
                    sensPresCnt[i] = 0;
                    
                }
                if (sensPresCnt[i] > LONG_PRESS_TIME) {
                    SensState[i] = LONG_PRESS;
                }         
            }  
        }        
    }
}
//-----------------------------------------------------

//-----------------------------------------------------
