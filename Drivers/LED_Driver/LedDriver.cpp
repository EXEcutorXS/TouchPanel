//
//    Модуль связи блока по последовательному каналу USART
//    

/* Includes ------------------------------------------------------------------*/
#include "LedDriver.h"
#include "core.h"

TLedDriver ledDriver;
//-----------------------------------------------------

//-----------------------------------------------------
void TLedDriver::initialize(void)
{
    if ((DO_PORT == GPIOA) || (CLK_PORT == GPIOA) || (WRITE_PORT == GPIOA)) rcu_periph_clock_enable(RCU_GPIOA); 
    if ((DO_PORT == GPIOB) || (CLK_PORT == GPIOB) || (WRITE_PORT == GPIOB)) rcu_periph_clock_enable(RCU_GPIOB);
    if ((DO_PORT == GPIOC) || (CLK_PORT == GPIOC) || (WRITE_PORT == GPIOC)) rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(DO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DO_PIN);
    gpio_init(CLK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, CLK_PIN);
    gpio_init(WRITE_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, WRITE_PIN);
	gpio_init(ENABLE_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, ENABLE_PIN);
	gpio_init(PWM_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PWM_PIN);
    gpio_bit_set(DO_PORT, DO_PIN); 
    gpio_bit_reset(CLK_PORT, CLK_PIN); 
    gpio_bit_reset(WRITE_PORT, WRITE_PIN); 
	gpio_bit_reset(PWM_PORT, PWM_PIN); 

}
//-----------------------------------------------------
void TLedDriver::handler(void)
{

}
//-----------------------------------------------------
inline void TLedDriver::setDO(void)        
{
    gpio_bit_set(DO_PORT, DO_PIN);
}
//-----------------------------------------------------
inline void TLedDriver::resetDO(void)        
{
    gpio_bit_reset(DO_PORT, DO_PIN);
}
//-----------------------------------------------------
inline void TLedDriver::setCLK(void)        
{
    gpio_bit_set(CLK_PORT, CLK_PIN);
}
//-----------------------------------------------------
inline void TLedDriver::resetCLK(void)        
{
    gpio_bit_reset(CLK_PORT, CLK_PIN);
}
//-----------------------------------------------------
inline void TLedDriver::setWrite(void)        
{
    gpio_bit_set(WRITE_PORT, WRITE_PIN);
}
//-----------------------------------------------------
inline void TLedDriver::resetWrite(void)        
{
    gpio_bit_reset(WRITE_PORT, WRITE_PIN);
}
//================================================

//---------------------------------------------
inline void TLedDriver::Delay_tick(uint16_t i) // n tick delay
{
    uint32_t timeout = (SystemCoreClock/1000000/4)*i;
    while(timeout > 0) {
        timeout--;
    }
}
//---------------------------------------------------------
//-- convert bitmap ----- меняем порядок бит
uint8_t TLedDriver::ConvertBitOrder(uint8_t bt) // 
{
   // static const uint8_t BitOrder[] = { 6, 7, 1, 2, 3, 5, 4, 0 };
    uint8_t outByte = 0;
    for(uint8_t bitNumb = 0; bitNumb < 7; bitNumb++)
    {
        if ((bt & (1 << bitNumb)) != 0) outByte |= (1 << BitOrder[bitNumb]);       
    }
    return outByte;
}
//-----------------------------------------------------------
void TLedDriver::WriteByte(uint8_t oneByte) // write a byte. First bit 7
{
    resetCLK();
    //Delay_tick(CLOCK_TIMING/2);
    uint8_t bt = ConvertBitOrder(oneByte); // меняем порядок бит
    for (uint8_t i = 0; i < 8; i++)
    {    // биты инвертируются // первым передается старший бит         
        if ((bt & 0x80) == 0) setDO(); // 
        else resetDO();
        Delay_tick(CLOCK_TIMING);
        bt = bt << 1;
        setCLK();
        Delay_tick(CLOCK_TIMING);
        resetCLK();
    }    
}
//---------------------------------------------------------
void TLedDriver::DisplayOff(void) // 
{
    uint8_t data[4];
    data[0] = 0xff;
    data[1] = 0xff;
    data[2] = 0xff;
    data[3] = 0xff;
    WriteDisplay(data); // 
}
//-------------------------------------------------------------
void TLedDriver::WriteDisplay(uint8_t* data) // 
{
    resetWrite();
    WriteByte(data[3]);
    WriteByte(data[2]);
    WriteByte(data[1]);
    WriteByte(data[0]);
    Delay_tick(CLOCK_TIMING);
    setWrite();
    Delay_tick(CLOCK_TIMING);
    resetWrite();
}
//==============================================================


//-----------------------------------------------------
