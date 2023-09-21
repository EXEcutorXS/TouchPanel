//
//    Модуль связи блока по последовательному каналу USART
//    

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "core.h"

Usart usart;
//-----------------------------------------------------
Usart::Usart(void)
{

}
//-----------------------------------------------------
void Usart::initialize(UsartParams_t* uartParams)
{
//    usartType = USART0;
//    TxGpio = GPIOA;
//    RxGpio = GPIOA;
//    TxPin = GPIO_PIN_9; 
//    RxPin = GPIO_PIN_10; 
    
    receiveCounter = 0;
    transmitCounter = 0;
    lengthPacketIn = 0;
    lengthPacketOut = 0;
    
    usartParams.usartNumb = uartParams->usartNumb;       // 0; 1; 2; 3; 4
    usartParams.baudrate = uartParams->baudrate;         // ..., 2400, ...
    usartParams.word_length = uartParams->word_length;   // 7; 8; 9
    usartParams.stop_bit = uartParams->stop_bit;         // 1s; 1.5s; 2s - 
    usartParams.usart_parity = uartParams->usart_parity; // NONE / EVEN / ODD
    usartParams.data_first = uartParams->data_first;     // MSB / LSB
    usartParams.TransferType = uartParams->TransferType; // DMA / Interrupt
    usartParams.RxPort.port = uartParams->RxPort.port;   // PortA..
    usartParams.RxPort.pin = uartParams->RxPort.pin;     // 0-15
    usartParams.TxPort.port = uartParams->TxPort.port;
    usartParams.TxPort.pin = uartParams->TxPort.pin;
    //-------
    TxPin = 1<<usartParams.TxPort.pin;
    RxPin = 1<<usartParams.RxPort.pin;
    
    if (usartParams.usartNumb == 0) usartType = USART0;
    if (usartParams.usartNumb == 1) usartType = USART1;
    if (usartParams.usartNumb == 2) usartType = USART2;
    if (usartParams.usartNumb == 3) usartType = UART3;
    if (usartParams.usartNumb == 4) usartType = UART4;
    
    if (usartType == USART0) {
        usart_deinit(USART0); 
        rcu_periph_clock_enable(RCU_USART0);
        usartIrq = USART0_IRQn;
        if ((usartParams.TxPort.port == PortB) && (usartParams.RxPort.port == PortB)) {
            gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
            rcu_periph_clock_enable(RCU_GPIOB);
            TxGpio = GPIOB; TxPin = GPIO_PIN_6; 
            RxGpio = GPIOB; RxPin = GPIO_PIN_7;
        } else {
            rcu_periph_clock_enable(RCU_GPIOA);
            TxGpio = GPIOA; TxPin = GPIO_PIN_9; 
            RxGpio = GPIOA; RxPin = GPIO_PIN_10;
        }
        dmaTxType = DMA0;
        dmaTxCh = DMA_CH3;
        dmaTxIrq = DMA0_Channel3_IRQn;
        dmaRxType = DMA0;
        dmaRxCh = DMA_CH4;      
        dmaRxIrq = DMA0_Channel4_IRQn;
    }
    if (usartType == USART1) {
        usart_deinit(USART1); 
        rcu_periph_clock_enable(RCU_USART1);
        usartIrq = USART1_IRQn;
        if ((usartParams.TxPort.port == PortD) && (usartParams.RxPort.port == PortD)) {
            gpio_pin_remap_config(GPIO_USART1_REMAP, ENABLE);
            rcu_periph_clock_enable(RCU_GPIOD);
            TxGpio = GPIOD; TxPin = GPIO_PIN_5; 
            RxGpio = GPIOD; RxPin = GPIO_PIN_6;
        } else {
            rcu_periph_clock_enable(RCU_GPIOA);
            TxGpio = GPIOA; TxPin = GPIO_PIN_2; 
            RxGpio = GPIOA; RxPin = GPIO_PIN_3;
        }
        dmaTxType = DMA0;
        dmaTxCh = DMA_CH6;
        dmaTxIrq = DMA0_Channel6_IRQn;
        dmaRxType = DMA0;
        dmaRxCh = DMA_CH5;      
        dmaRxIrq = DMA0_Channel5_IRQn;
    }
    if (usartType == USART2) {
        usart_deinit(USART2); 
        rcu_periph_clock_enable(RCU_USART2);
        usartIrq = USART2_IRQn;
        if ((usartParams.TxPort.port == PortC) && (usartParams.RxPort.port == PortC)) { 
            gpio_pin_remap_config(GPIO_USART2_PARTIAL_REMAP, ENABLE);
            rcu_periph_clock_enable(RCU_GPIOC);
            TxGpio = GPIOC; TxPin = GPIO_PIN_10; 
            RxGpio = GPIOC; RxPin = GPIO_PIN_11;
        } else 
        if ((usartParams.TxPort.port == PortD) && (usartParams.RxPort.port == PortD)) {
            gpio_pin_remap_config(GPIO_USART2_FULL_REMAP, ENABLE);
            rcu_periph_clock_enable(RCU_GPIOD);
            TxGpio = GPIOD; TxPin = GPIO_PIN_8; 
            RxGpio = GPIOD; RxPin = GPIO_PIN_9;
        } else {
            rcu_periph_clock_enable(RCU_GPIOB);
            TxGpio = GPIOB; TxPin = GPIO_PIN_10; 
            RxGpio = GPIOB; RxPin = GPIO_PIN_11;
        }
        dmaTxType = DMA0;
        dmaTxCh = DMA_CH1;
        dmaTxIrq = DMA0_Channel1_IRQn;
        dmaRxType = DMA0;
        dmaRxCh = DMA_CH2;      
        dmaRxIrq = DMA0_Channel2_IRQn;
    }
    if (usartType == UART3) {
        usart_deinit(UART3); 
        rcu_periph_clock_enable(RCU_UART3);
        usartIrq = UART3_IRQn;
        rcu_periph_clock_enable(RCU_GPIOC);
        TxGpio = GPIOC; TxPin = GPIO_PIN_10; 
        RxGpio = GPIOC; RxPin = GPIO_PIN_11;
        dmaTxType = DMA1;
        dmaTxCh = DMA_CH4;
        dmaTxIrq = DMA1_Channel3_Channel4_IRQn;
        dmaRxType = DMA1;
        dmaRxCh = DMA_CH2;      
        dmaRxIrq = DMA1_Channel2_IRQn;
    }
    if (usartType == UART4) {
        usart_deinit(UART4); 
        rcu_periph_clock_enable(RCU_UART4);
        usartIrq = UART4_IRQn;
        rcu_periph_clock_enable(RCU_GPIOC);
        rcu_periph_clock_enable(RCU_GPIOD);
        TxGpio = GPIOC; TxPin = GPIO_PIN_12; 
        RxGpio = GPIOD; RxPin = GPIO_PIN_2;
        usartParams.TransferType = USART_INTERRUPT_TRANSFER;// нет DMA
    }
     
    rcu_periph_clock_enable(RCU_AF);
    // Configure the USART pin
    gpio_init(RxGpio, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, RxPin);      
    gpio_init(TxGpio, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, TxPin);

    if (usartParams.baudrate == 0) usartParams.baudrate = 9600;
    // Configure the USART
    usart_deinit(usartType);
    usart_disable(usartType);
    usart_baudrate_set(usartType, usartParams.baudrate);
    usart_word_length_set(usartType, USART_WL_8BIT);
    usart_stop_bit_set(usartType, USART_STB_1BIT);
    usart_parity_config(usartType, USART_PM_NONE);
    usart_data_first_config(usartType, USART_MSBF_LSB);
    usart_transmit_config(usartType, USART_TRANSMIT_ENABLE);
    usart_receive_config(usartType, USART_RECEIVE_ENABLE);
    
    if (usartParams.TransferType == USART_DMA_TRANSFER) {
        dma_parameter_struct  dma_init_struct;
        rcu_periph_clock_enable(RCU_DMA0);
        rcu_periph_clock_enable(RCU_DMA1);
        
        usart_dma_transmit_config(usartType, USART_DENT_ENABLE);
        usart_dma_receive_config(usartType, USART_DENR_ENABLE);
        
        dma_deinit(dmaRxType, dmaRxCh);
        dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
        dma_init_struct.memory_addr = (uint32_t)packetIn;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number = BUFFER_SIZE;
        dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(usartType));
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
        dma_init(dmaRxType, dmaRxCh, dma_init_struct);
        /* configure DMA mode */
        dma_circulation_enable(dmaRxType, dmaRxCh);
        dma_memory_to_memory_disable(dmaRxType, dmaRxCh);       
        dma_channel_enable(dmaRxType, dmaRxCh);  

        /* initialize DMA channel3 */
        dma_deinit(dmaTxType, dmaTxCh);
        dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
        dma_init_struct.memory_addr = (uint32_t)packetOut;
        dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.number = 15;
        dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(usartType));
        dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority = DMA_PRIORITY_HIGH;
        dma_init(dmaTxType, dmaTxCh, dma_init_struct);    
        /* configure DMA mode */
        dma_circulation_disable(dmaTxType, dmaTxCh);
        dma_memory_to_memory_disable(dmaTxType, dmaTxCh);    
        /* enable DMA transfer complete interrupt */
        dma_interrupt_enable(dmaTxType, dmaTxCh, DMA_INT_FTF);   
        /*configure DMA interrupt*/
        nvic_irq_enable(dmaTxIrq, 0, 0); 
        
    } else {
        usart_interrupt_enable(usartType, USART_INT_RBNE); // USART_INT_TBE // USART_INT_RBNE // USART_INT_TC
        nvic_irq_enable(usartIrq, 1, 1);
    }
    usart_enable(usartType);

}
//-----------------------------------------------------
uint32_t Usart::GetBaudrate(void) 
{
    return usartParams.baudrate;
}
//----------------------------------------------------
void Usart::SetTxPinAsTx(void) 
{
    if (TxPinAsPort_f == true) {
        gpio_init(usart.TxGpio, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, usart.TxPin);
        TxPinAsPort_f = false;
    }
}
//--------------------------------------------------------

void Usart::handler(void)
{
    static uint32_t timer = 0;
    if ((core.getTick() - timer) >= 10) {
        timer = core.getTick();
        TimeoutCnt++;
        if (TimeoutCnt > 200) {
            TimeoutCnt = 0;
        }
    }
    
}
//-----------------------------------------------------

//-----------------------------------------------------
void Usart::changeBaudrate(uint32_t bdrt)
{
//    receiveCounter = 0;
//    transmitCounter = 0;
//    lengthPacketIn = 0;
//    lengthPacketOut = 0;
    
    usart_disable(usartType);
    usart_baudrate_set(usartType, bdrt);
    usart_enable(usartType);
    usartParams.baudrate = bdrt;
}
//-------------------------------------------------
uint16_t Usart::GetReceiveBufferPos(void)
{
    uint16_t nb = 0;
    if (usartParams.TransferType == USART_DMA_TRANSFER) {
        nb = BUFFER_SIZE - dma_transfer_number_get(dmaRxType, dmaRxCh);
    } else {
        nb = receiveCounter;
    }
    return nb;
}
//--------------------------------------------------
uint8_t Usart::GetByte(uint16_t index)
{
    return packetIn[index];
}
//---------------------------------------------------
void Usart::ClearBuffer(void)
{
    if (usartParams.TransferType == USART_DMA_TRANSFER) {
        dma_channel_disable(usart.dmaRxType, usart.dmaRxCh);
        DMA_CHCNT(usart.dmaRxType, usart.dmaRxCh) = (BUFFER_SIZE & DMA_CHANNEL_CNT_MASK);
        dma_channel_enable(usart.dmaRxType, usart.dmaRxCh);
    } 
    receiveCounter = 0;
    lengthPacketIn = 0;
    
}
//-----------------------------------------------------------
//----------------------------------------------------------
//обработчик прерывания приема байтов
void Usart::receiveIntHandler(uint8_t AByte)
{
    TimeoutCnt = 0;
    packetIn[receiveCounter++]=AByte;
    if (receiveCounter >= BUFFER_SIZE) {
        receiveCounter = 0;    
    }
}
//-----------------------------------------------------
//инициализация отправки пакета
bool Usart::startTransmission(uint8_t *buf, uint16_t len)
{
    uint16_t i;

    if (isTransmission == true) return false; // во время передачи не начинаем новую передачу
    if ((core.getTick()-timerTransPause) < 25) return false; // не начинаем новую передачу, пока не пройдет время
    isTransmission=true;
    
    for(i=0; i<len; i++) {
        packetOut[i] = buf[i];
    }

    transmitCounter = 0;
    lengthPacketOut = len;

    if (usartParams.TransferType == USART_DMA_TRANSFER) {
        dma_channel_disable(dmaTxType, dmaTxCh);
        dma_memory_address_config(dmaTxType, dmaTxCh, (uint32_t)packetOut);    
        dma_transfer_number_config(dmaTxType, dmaTxCh, lengthPacketOut);
        dma_channel_enable(dmaTxType, dmaTxCh);
    } else {
        usart_data_transmit(usartType,packetOut[transmitCounter++]);
        usart_interrupt_enable(usartType, USART_INT_TC);   
    }

    return true;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//отправка следующего байта пакета
void Usart::transmitNextByte(void)
{
    if (transmitCounter<lengthPacketOut) {
        usart_data_transmit(usartType,packetOut[transmitCounter++]);
    }
    else {
        usart_interrupt_disable(usartType, USART_INT_TC);//запрещаем прерывание после отправки пакета
        isTransmission=0;                          //отправка пакета завершена
        lengthPacketOut = 0;
        timerTrans = core.getTick();
        timerTransPause = core.getTick();
    }
}

//-----------------------------------------------------
void Usart::usartIrqHandler(void)
{
    uint8_t byte;
    bool isError = true;

    if (usart_interrupt_flag_get(usartType, USART_INT_FLAG_RBNE)){
        byte = usart_data_receive(usartType);
        usart.receiveIntHandler(byte);
        isError = false;
    }
    if (usart_interrupt_flag_get(usartType, USART_INT_FLAG_TC)){
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_TC);
        usart.transmitNextByte();
        isError = false;
    }
    if (usart_interrupt_flag_get(usartType, USART_INT_FLAG_TBE)){
        isError = false;
    }    
    if (usart_interrupt_flag_get(usartType, USART_INT_FLAG_IDLE)){
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_IDLE);
    }    
    if (usart_interrupt_flag_get(usartType, USART_INT_FLAG_ERR_ORERR)){
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_ERR_ORERR);
    }
    else if (usart_interrupt_flag_get(usartType, USART_INT_FLAG_ERR_NERR)){
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_ERR_NERR);
    }
    else if (usart_interrupt_flag_get(usartType, USART_INT_FLAG_ERR_FERR)){
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_ERR_FERR);
    }
    else if (usart_interrupt_flag_get(usartType, USART_INT_FLAG_PERR)){
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_PERR);
    }
    else if (isError){
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_ERR_ORERR);
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_ERR_NERR);
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_ERR_FERR);
        usart_interrupt_flag_clear(usartType, USART_INT_FLAG_PERR);
    }
}
//--------------------------------------------------------------------
extern "C" void USART0_IRQHandler(void)
{
    if (usart.usartIrq == USART0_IRQn) usart.usartIrqHandler();
}
extern "C" void USART1_IRQHandler(void)
{
    if (usart.usartIrq == USART1_IRQn) usart.usartIrqHandler();
}
extern "C" void USART2_IRQHandler(void)
{
    if (usart.usartIrq == USART2_IRQn) usart.usartIrqHandler();
}
extern "C" void UART3_IRQHandler(void)
{
    if (usart.usartIrq == UART3_IRQn) usart.usartIrqHandler();
}
extern "C" void UART4_IRQHandler(void)
{
    if (usart.usartIrq == UART4_IRQn) usart.usartIrqHandler();
}
//-----------------------------------------------------
extern "C" void DMA0_Channel3_IRQHandler(void)                                             // ОБРАБОТКА ПРЕРЫВАНИЯ от DMA
{
    if (dma_interrupt_flag_get(usart.dmaTxType, usart.dmaTxCh, DMA_INT_FLAG_FTF)) {  
        dma_interrupt_flag_clear(usart.dmaTxType, usart.dmaTxCh, DMA_INT_FLAG_FTF);
        usart.isTransmission=0;                               //отправка пакета завершена
        usart.timerTrans = core.getTick();
        usart.timerTransPause = core.getTick();
        dma_channel_disable(usart.dmaTxType, usart.dmaTxCh);
    }
}
//-----------------------------------------------------
extern "C" void DMA0_Channel4_IRQHandler(void)                                             // ОБРАБОТКА ПРЕРЫВАНИЯ от DMA
{
    if (dma_interrupt_flag_get(usart.dmaRxType, usart.dmaRxCh, DMA_INT_FLAG_FTF)) {  
        dma_interrupt_flag_clear(usart.dmaRxType, usart.dmaRxCh, DMA_INT_FLAG_FTF);
        
    }
}
//-----------------------------------------------------
//                DCD     DMA0_Channel0_IRQHandler          ; 27:DMA0 Channel0
//                DCD     DMA0_Channel1_IRQHandler          ; 28:DMA0 Channel1
//                DCD     DMA0_Channel2_IRQHandler          ; 29:DMA0 Channel2
//                DCD     DMA0_Channel3_IRQHandler          ; 30:DMA0 Channel3
//                DCD     DMA0_Channel4_IRQHandler          ; 31:DMA0 Channel4
//                DCD     DMA0_Channel5_IRQHandler          ; 32:DMA0 Channel5
//                DCD     DMA0_Channel6_IRQHandler          ; 33:DMA0 Channel6

//                DCD     DMA1_Channel0_IRQHandler          ; 72:DMA1 Channel0
//                DCD     DMA1_Channel1_IRQHandler          ; 73:DMA1 Channel1
//                DCD     DMA1_Channel2_IRQHandler          ; 74:DMA1 Channel2
//                DCD     DMA1_Channel3_4_IRQHandler        ; 75:DMA1 Channel3 and Channel4
