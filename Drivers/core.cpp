/******************************************************************************
* ООО Теплостар
* Самара
* 
* Программисты: Клюев А.А.
* 
* 08.08.2018
* Описание:
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "core.h"
#include "usart.h"
#include "protocol_3.h"

Core core;

//-----------------------------------------------------
Core::Core(void)
{
    isLinkError = false;
    msTick = 0;
}
//-----------------------------------------------------
void Core::SystemCoreClockInit(void)
{  
    rcu_periph_clock_enable(RCU_PMU);
//    rcu_osci_on(RCU_IRC48M);
    rcu_osci_on(RCU_IRC8M);
    rcu_lxtal_drive_capability_config(RCU_LXTAL_HIGHDRI); // RCU_LXTAL_LOWDRI // RCU_LXTAL_HIGHDRI

    pmu_ldo_output_select(PMU_LDOVS_HIGH);
    
    rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);
    rcu_apb2_clock_config(RCU_APB2_CKAHB_DIV1);
    rcu_apb1_clock_config(RCU_APB1_CKAHB_DIV1);
      
//    if (rcu_osci_stab_wait(RCU_IRC48M) == SUCCESS) { // 
//        rcu_pllpresel_config(RCU_PLLPRESRC_IRC48M); // RCU_PLLPRESRC_HXTAL / RCU_PLLPRESRC_IRC48M 
//        rcu_predv0_config(RCU_PREDV0_DIV2); // HXTAL_VALUE/2 * 6 = 48 000 000
//        rcu_pll_config(RCU_PLLSRC_HXTAL_IRC48M, RCU_PLL_MUL2); // RCU_PLLSRC_IRC8M_DIV2 / RCU_PLLSRC_HXTAL_IRC48M             
//    } else {
//        rcu_osci_off(RCU_IRC48M);
//        rcu_osci_on(RCU_IRC8M);
//        if (rcu_osci_stab_wait(RCU_IRC8M) == SUCCESS) { // IRC8M_VALUE/2 * 12 = 48 000 000
//            rcu_pll_config(RCU_PLLSRC_IRC8M_DIV2, RCU_PLL_MUL12); // RCU_PLLSRC_IRC8M_DIV2 / RCU_PLLSRC_HXTAL_IRC48M
//        }
//    }
    if (rcu_osci_stab_wait(RCU_IRC8M) == SUCCESS) { // IRC8M_VALUE/2 * 12 = 48 000 000
        rcu_pll_config(RCU_PLLSRC_IRC8M_DIV2, RCU_PLL_MUL12); // RCU_PLLSRC_IRC8M_DIV2 / RCU_PLLSRC_HXTAL_IRC48M
    }
    
    pmu_highdriver_mode_enable();
    pmu_highdriver_switch_select(PMU_HIGHDR_SWITCH_EN);
    rcu_osci_on(RCU_PLL_CK); 
    if (rcu_osci_stab_wait(RCU_PLL_CK) == SUCCESS) {
        rcu_system_clock_source_config(RCU_CKSYSSRC_PLL);// RCU_CKSYSSRC_IRC8M / RCU_CKSYSSRC_HXTAL / RCU_CKSYSSRC_PLL
    } else {
        rcu_osci_off(RCU_PLL_CK); 
        rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);// RCU_CKSYSSRC_IRC8M / RCU_CKSYSSRC_HXTAL / RCU_CKSYSSRC_PLL
    }
     
    SystemCoreClockUpdate();
}

//---------------------------------------------------
void Core::InterfaceInit(uint8_t Interf)
{

    if (Interf == 2) { // 1-USART, 2-CAN, 3-LIN
        //can.initialize();
    } else
    if (Interf == 3) { // 1-USART, 2-CAN, 3-LIN
        //lin.initialize();
    } else {
        Interf = 1;
    }
    if (Interf == 1) { // 1-USART, 2-CAN, 3-LIN
        UsartParams_t usartParam;
        usartParam.usartNumb = 0;       // 0; 1; 2; 3; 4
        usartParam.baudrate = 9600;         // ..., 2400, ...
        usartParam.word_length = 8;   // 7; 8; 9
        usartParam.stop_bit = USART_STOPBIT_1;         // 1s; 1.5s; 2s - 
        usartParam.usart_parity = USART_PARITY_NONE; // NONE / EVEN / ODD
        usartParam.data_first = USART_DATAFIRST_LSB;     // MSB / LSB
        usartParam.TransferType = USART_DMA_TRANSFER; // DMA / Interrupt
        usartParam.RxPort.port = PortA;   // PortA..
        usartParam.RxPort.pin = 10;     // 0-15
        usartParam.TxPort.port = PortA;
        usartParam.TxPort.pin = 9;
        usart.initialize(&usartParam);
        protocol3.initialize();
        core.verProtocol = 3;
    } 
    InterfaceType = Interf;
}
//-------------------------------------------------
void Core::InterfaceHandler(void)
{
    if (InterfaceType == 1) { // 1-USART, 2-CAN, 3-LIN
        usart.handler();
        protocol3.handler();
    } else
    if (InterfaceType == 2) { // 1-USART, 2-CAN, 3-LIN
        //can.handler();
        //messages.handler();     
    } else
    if (InterfaceType == 3) { // 1-USART, 2-CAN, 3-LIN
        //lin.initialize();
    } else {

    }
}
//--------------------------------------------------
void Core::initialize(void)
{
    
//    __disable_irq(); // in main
    rcu_periph_clock_enable(RCU_PMU);
    rcu_periph_clock_enable(RCU_AF);
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
    //remapTable(); //Bootloader is off
    
    SystemCoreClockInit();// настройка частоты
    
    SysTick_Config(SystemCoreClock/1000);
    NVIC_SetPriority(SysTick_IRQn, 1);
    initUnusedPins();
    //InitWDT();
   /*
    rcu_periph_clock_enable(RCU_TIMER5);    
    timer_prescaler_config(TIMER5, SystemCoreClock/1000000-1, TIMER_PSC_RELOAD_NOW);
    timer_autoreload_value_config(TIMER5, (uint16_t)0xFFFF);
    timer_enable(TIMER5);
    */
 //   __enable_irq(); // in main
    
}
//--------------------------------------------------
void Core::initUnusedPins(void)
{
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7);
    gpio_bit_reset(GPIOA, GPIO_PIN_7); 
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_8);
    gpio_bit_reset(GPIOA, GPIO_PIN_8); 
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_15);
    gpio_bit_reset(GPIOA, GPIO_PIN_15); 
    
    
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_1);
    gpio_bit_reset(GPIOB, GPIO_PIN_1); 
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_2);
    gpio_bit_reset(GPIOB, GPIO_PIN_2); 
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_3);
    gpio_bit_reset(GPIOB, GPIO_PIN_3); 
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_10);
    gpio_bit_reset(GPIOB, GPIO_PIN_10); 
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_14);
    gpio_bit_reset(GPIOB, GPIO_PIN_14); 
    
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_13);
    gpio_bit_reset(GPIOC, GPIO_PIN_13); 
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_14);
    gpio_bit_reset(GPIOC, GPIO_PIN_14); 
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_15);
    gpio_bit_reset(GPIOC, GPIO_PIN_15); 
    
    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_0);
    gpio_bit_reset(GPIOD, GPIO_PIN_0); 
    gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_1);
    gpio_bit_reset(GPIOD, GPIO_PIN_1); 
    
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
    gpio_bit_reset(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6); 
}
//--------------------------------------------------
void Core::handler(void)
{   
    ResetWDT();
}
//-----------------------------------------------------
void Core::delay_mks(uint32_t delay)
{
    
}
//-----------------------------------------------------
void Core::delayMs(uint32_t delay)
{
    uint32_t timer = core.getTick();
    while((core.getTick() - timer) < delay);
}
//-----------------------------------------------------
void Core::protectedFlash(void)
{
    /*
    if (FLASH_GetReadOutProtectionStatus() == RESET){
        FLASH_Unlock();
        FLASH_ReadOutProtection(ENABLE);
        FLASH_Lock();
    }
    */
}
//-----------------------------------------------------
void Core::incTick(void)
{
    msTick++;
}
//-----------------------------------------------------
uint32_t Core::getTick(void)
{
    return msTick;
}
//-----------------------------------------------------
void Core::setTimer(uint32_t value)
{
    tickTimer = msTick + value;
}
//-----------------------------------------------------
uint32_t Core::getTimer(void)
{
    int32_t i;
    
    i = tickTimer - msTick;
    if (i < 0) i = 0;
    return i;
}
//-----------------------------------------------------
void Core::resetTimer(void)
{
    tickTimer = msTick;
}
//-----------------------------------------------------
void Core::enterSleepMode(void)
{
    pmu_deinit();
    //dbg_low_power_enable(DBG_LOW_POWER_STANDBY); 
    pmu_wakeup_pin_enable();
    pmu_to_standbymode(WFI_CMD);
    
    while(true){

    }
}
//-----------------------------------------------------
void Core::disablePeriph(void)
{

    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
    RCU_CTL &= ~RCU_CTL_PLLEN;
    
    gpio_init(GPIOA,GPIO_MODE_AIN,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);
    gpio_init(GPIOB,GPIO_MODE_AIN,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);
    gpio_init(GPIOC,GPIO_MODE_AIN,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);
    gpio_init(GPIOD,GPIO_MODE_AIN,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);
    gpio_init(GPIOE,GPIO_MODE_AIN,GPIO_OSPEED_2MHZ,GPIO_PIN_ALL);
    gpio_init(GPIOF,GPIO_MODE_AIN,GPIO_OSPEED_2MHZ,GPIO_PIN_0);
    gpio_init(GPIOF,GPIO_MODE_AIN,GPIO_OSPEED_2MHZ,GPIO_PIN_1);
    gpio_bit_reset(GPIOF, GPIO_PIN_0);
    gpio_bit_reset(GPIOF, GPIO_PIN_1);

    rcu_periph_clock_disable(RCU_GPIOF);
    rcu_periph_clock_disable(RCU_USART0);
    rcu_periph_clock_disable(RCU_USART1);
    rcu_periph_clock_disable(RCU_USART2);
    rcu_periph_clock_disable(RCU_UART3);
    rcu_periph_clock_disable(RCU_UART4);
    rcu_periph_clock_disable(RCU_TIMER0);
    rcu_periph_clock_disable(RCU_TIMER1);
    rcu_periph_clock_disable(RCU_TIMER2);
    rcu_periph_clock_disable(RCU_TIMER3);
    rcu_periph_clock_disable(RCU_TIMER4);
    rcu_periph_clock_disable(RCU_TIMER5);
    rcu_periph_clock_disable(RCU_TIMER6);
    rcu_periph_clock_disable(RCU_TIMER7);
    rcu_periph_clock_disable(RCU_SPI0);
    rcu_periph_clock_disable(RCU_I2C0);
    rcu_periph_clock_disable(RCU_I2C1);
    rcu_periph_clock_disable(RCU_WWDGT);
    rcu_periph_clock_disable(RCU_DMA0);
    rcu_periph_clock_disable(RCU_ADC0);
    rcu_periph_clock_disable(RCU_ADC1);
    rcu_periph_clock_disable(RCU_ADC2);
    rcu_periph_clock_disable(RCU_DAC);
    rcu_periph_clock_disable(RCU_USBD);
    rcu_periph_clock_disable(RCU_CAN0);
    rcu_periph_clock_disable(RCU_DMA0);
    rcu_periph_clock_disable(RCU_DMA1);
    rcu_periph_clock_disable(RCU_CRC);
}
//-----------------------------------------------------
void Core::enablePeriph(void)
{
    SystemInit();
}
//-----------------------------------------------------
void Core::remapTable(void)
{
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, MAIN_PROGRAM_START_ADDRESS - FLASH_BASE);
}
//-----------------------------------------------------
void Core::InitWDT(void)
{
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    
    /* wait till IRC40K is ready */
    while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K)){
    }

    /* confiure FWDGT counter clock: 40KHz(IRC40K) / 64 = 0.625 KHz */
    fwdgt_config(0x0FFF,FWDGT_PSC_DIV64);
    
    /* After 1.6 seconds to generate a reset */
    fwdgt_enable();
}
//----------------------------------------------
void Core::ResetWDT(void)
{
    fwdgt_counter_reload();
}
//-----------------------------------------------------
extern "C" void SysTick_Handler(void)
{
    core.incTick();         // системный таймер
}
//-----------------------------------------------------
extern "C" void NMI_Handler(void)
{
    
}
//-----------------------------------------------------
extern "C" void HardFault_Handler(void)
{  
  NVIC_SystemReset();
}
//-----------------------------------------------------
void Hard_fault_handler_c( uint32_t *hardFaultArgs)
{
    NVIC_SystemReset();
}
//-----------------------------------------------------

