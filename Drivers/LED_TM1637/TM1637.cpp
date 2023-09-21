//
//    ћодуль св€зи блока по последовательному каналу USART
//    

/* Includes ------------------------------------------------------------------*/
#include "TM1637.h"
#include "core.h"

T_TM1637 tm1637;
//-----------------------------------------------------

//-----------------------------------------------------
void T_TM1637::initialize(void)
{
    if ((DIO_PORT == GPIOA) || (CLK_PORT == GPIOA)) rcu_periph_clock_enable(RCU_GPIOA);
    if ((DIO_PORT == GPIOB) || (CLK_PORT == GPIOB)) rcu_periph_clock_enable(RCU_GPIOB);
    if ((DIO_PORT == GPIOC) || (CLK_PORT == GPIOC)) rcu_periph_clock_enable(RCU_GPIOC);
    gpio_bit_set(DIO_PORT, DIO_PIN); 
    gpio_bit_set(CLK_PORT, CLK_PIN); 
    gpio_init(DIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DIO_PIN);
    gpio_init(CLK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, CLK_PIN);
    gpio_bit_set(DIO_PORT, DIO_PIN); 
    gpio_bit_set(CLK_PORT, CLK_PIN); 
    brightness = 7;
}
//-----------------------------------------------------
void T_TM1637::handler(void)
{

}
//-----------------------------------------------------
void T_TM1637::ConfigDIOinput(void)
{
    gpio_bit_set(DIO_PORT, DIO_PIN); 
    gpio_init(DIO_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, DIO_PIN); 
}
//-----------------------------------------------------
void T_TM1637::ConfigDIOout(void)
{
    gpio_bit_set(DIO_PORT, DIO_PIN);
    gpio_init(DIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DIO_PIN); 
    gpio_bit_set(DIO_PORT, DIO_PIN); 
}
//----------------------------------------------------
#define     setDIO()        gpio_bit_set(DIO_PORT, DIO_PIN) 

#define     resetDIO()      gpio_bit_reset(DIO_PORT, DIO_PIN)

#define     setCLK()        gpio_bit_set(CLK_PORT, CLK_PIN)

#define     resetCLK()      gpio_bit_reset(CLK_PORT, CLK_PIN)

//---------------------------------------------
void T_TM1637::Delay_tick(uint16_t i) // n tick delay
{
    uint32_t timeout = (SystemCoreClock/1000000/4)*i;
    while(timeout > 0) {
        timeout--;
    }
}
// ======================================
void T_TM1637::I2CStart(void) // 
{
    setCLK();
    setDIO();
    Delay_tick(I2C_TIMING);
    resetDIO();
    Delay_tick(I2C_TIMING);
    resetCLK();
}
// =============================================
inline void T_TM1637::WaitASK(void) // 
{
    uint16_t timeout = (SystemCoreClock/1000000/4)*I2C_TIMING;
    while(gpio_input_bit_get(DIO_PORT, DIO_PIN) == SET)
    {
        if (timeout > 0) timeout--;
        else break;
    }
}
//-----------------------------------------------
inline void T_TM1637::I2Cask(void) // 1637 Answer
{
    resetCLK();  
    ConfigDIOinput();
    Delay_tick(I2C_TIMING/2); //
    WaitASK();
    setCLK();
    Delay_tick(I2C_TIMING);
    resetCLK();
    ConfigDIOout();
}
// ========================================
void T_TM1637::I2CStop(void) // 1637 Stop
{
    resetCLK();
    Delay_tick(I2C_TIMING);
    resetDIO();
    Delay_tick(I2C_TIMING);
    setCLK();
    Delay_tick(I2C_TIMING);
    setDIO();
}
//---------------------------------------------------------
void T_TM1637::I2CWriteByte(uint8_t oneByte) // write a byte
{
    for (uint8_t i = 0; i < 8; i++)
    {
        resetCLK();
        Delay_tick(I2C_TIMING/2);
        if (oneByte & 0x01) setDIO();
        else resetDIO();
        Delay_tick(I2C_TIMING/2);
        oneByte = oneByte >> 1;
        setCLK();
        Delay_tick(I2C_TIMING);
    }
    I2Cask();
}
//---------------------------------------------------------
void T_TM1637::WriteCommand(uint8_t cmd) // 
{
    I2CStart();
    I2CWriteByte(DATA_CMD_SET | cmd); //
    I2CStop();
}
//---------------------------------------------------------
void T_TM1637::DisplayOn(uint8_t bright) // 
{
    brightness = bright;
    if (brightness > 7) brightness = 7;
    I2CStart();
    I2CWriteByte(DISPLAY_CMD_SET | DISPLAY_ON | bright); // 
    I2CStop();
}
//-----------------------------------------------------------
void T_TM1637::DisplayOff(void) // 
{
    I2CStart();
    I2CWriteByte(DISPLAY_CMD_SET | DISPLAY_OFF); // 
    I2CStop();
}
//-------------------------------------------------------------
void T_TM1637::WriteDisplay(uint8_t* data) // 
{
    WriteCommand(DATA_CMD_ADR_INC);
    //DisplayOff();
    I2CStart();
    I2CWriteByte(ADDRESS_CMD_SET | ADDRESS_POS_1); // 
    for(uint8_t i = 0; i < 6; i++) // 
    {
        I2CWriteByte(data[i]); // 
    }
    I2CStop();
    DisplayOn(brightness);

}
//==============================================================


//-----------------------------------------------------
