/******************************************************************************
* ООО ДД-информ
* Самара
* 
* Программисты: Кубышкин К.А., Батырев Е.В.
* 
* __.09.2020                   08.12.2022
* Описание:
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "core.h"
///#include "version.h"

Can can;

//-------------------------------------------------------------------
Can::Can(void)
{
    idType=27;                                   //тип устройства
    idAddress=0;                                 //адрес устройства
    Interrupt_f=false;
}

//-------------------------------------------------------------------
void Can::initialize(void)
{
    uint16_t bdrt = 250;
#ifdef IS_MINSK
/* baudrate 500Kbps */
    bdrt = 500;
#else
/* baudrate 250Kbps */
    bdrt = 250;
#endif
    idType = core.typeDevice;
    initialize(CAN_NORMAL_MODE, bdrt);
}
//-------------------------------------
void Can::initialize(uint8_t mode, uint16_t baudr)
{ 
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_PMU);
    
    can_deinit(CAN0);
    
    // настройка входа/выхода CAN
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_pin_remap_config(GPIO_CAN_PARTIAL_REMAP,ENABLE); // GPIO_CAN_FULL_REMAP // GPIO_CAN_PARTIAL_REMAP
    
    // настройка выхода STB
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_bit_set(GPIOB, GPIO_PIN_1);
    
    ///*
    can_parameter_struct canInitStruct;
    can_struct_para_init(CAN_INIT_STRUCT, &canInitStruct);
    //Configure CAN
    canInitStruct.working_mode = mode; // CAN_LOOPBACK_MODE                              
    canInitStruct.resync_jump_width = CAN_BT_SJW_1TQ;                          
    canInitStruct.time_segment_1 = CAN_BT_BS1_13TQ; // 13                           
    canInitStruct.time_segment_2 = CAN_BT_BS2_2TQ;   // 2                          
    canInitStruct.time_triggered = DISABLE;                                    
    canInitStruct.auto_bus_off_recovery = ENABLE;                             
    canInitStruct.auto_wake_up = ENABLE; // DISABLE; //                                      
    canInitStruct.no_auto_retrans = DISABLE;                                    
    canInitStruct.rec_fifo_overwrite = DISABLE;                                
    canInitStruct.trans_fifo_order = ENABLE; // DISABLE;                                  
    canInitStruct.prescaler = (SystemCoreClock/1000/(canInitStruct.time_segment_1+canInitStruct.time_segment_2+3)/baudr);                               
    
    can_init(CAN0, &canInitStruct);
    
    can_filter_parameter_struct canfilterSetup;
    can_struct_para_init(CAN_FILTER_STRUCT, &canfilterSetup);
    canfilterSetup.filter_fifo_number = CAN_FIFO1;
    canfilterSetup.filter_bits = CAN_FILTERBITS_32BIT;
    canfilterSetup.filter_mode = CAN_FILTERMODE_MASK;
    canfilterSetup.filter_number = 0;
    canfilterSetup.filter_enable = ENABLE;
    can_filter_init(&canfilterSetup);

    //can_interrupt_enable(CAN0, CAN_INT_RFNE1); // |CAN_INT_WERR|CAN_INT_PERR|CAN_INT_ERRN |CAN_INT_ERR  CAN_INT_RFF1 CAN_INT_RFNE1
    //can_interrupt_enable(CAN0, CAN_INT_WERR | CAN_INT_PERR | CAN_INT_ERRN | CAN_INT_ERR); // |CAN_INT_WERR|CAN_INT_PERR|CAN_INT_ERRN |CAN_INT_ERR  
            // CAN_INT_RFF0 | CAN_INT_RFF1 | CAN_INT_RFO0 | CAN_INT_RFO1 | 
    can_interrupt_enable(CAN0, CAN_INT_RFNE0 | CAN_INT_RFNE1); 

    nvic_irq_enable(CAN0_RX1_IRQn, 1, 1);
    
    gpio_bit_reset(GPIOB, GPIO_PIN_1);
}
//--------------------------------------------------------------------
void Can::shutdown(void)
{ 
    gpio_bit_set(GPIOB, GPIO_PIN_1);
}
//---------------------------------------------------
void Can::handler(void)
{
    
}
//-------------------------------------------------------------------
void Can::SendMessage(uint32_t AID, uint8_t AD0, uint8_t AD1, uint8_t AD2, uint8_t AD3, uint8_t AD4, uint8_t AD5, uint8_t AD6, uint8_t AD7)
{
    uint8_t TransmitMailBox=0;
//    uint8_t status;

    TxMessage.tx_sfid = 0x01ff;
    TxMessage.tx_efid = AID;
    TxMessage.tx_ff = CAN_FF_EXTENDED;
    TxMessage.tx_ft = CAN_FT_DATA;
    TxMessage.tx_dlen = 8;
    TxMessage.tx_data[0] = AD0;
    TxMessage.tx_data[1] = AD1;
    TxMessage.tx_data[2] = AD2;
    TxMessage.tx_data[3] = AD3;
    TxMessage.tx_data[4] = AD4;
    TxMessage.tx_data[5] = AD5;
    TxMessage.tx_data[6] = AD6;
    TxMessage.tx_data[7] = AD7;

    uint32_t timeout = core.getTick();
    do {
        __disable_irq();
        TransmitMailBox=can_message_transmit(CAN0, &TxMessage); // ищем пустой ящик для передачи
        __enable_irq();
        if ((core.getTick() - timeout) > 2) break;              // если не находим за 2 мс, то выходим
    } while (TransmitMailBox == CAN_NOMAILBOX);
    
    timeout = core.getTick();
    if (TransmitMailBox != CAN_NOMAILBOX) {                   // если ящик найден, значит идет передача.
        while (can_transmit_states(CAN0, TransmitMailBox) != CAN_TRANSMIT_OK) { // ждем окончания успешной передачи
            if ((core.getTick() - timeout) > 10) {             // если за 5 мс не передалось, то
                can_transmission_stop(CAN0, TransmitMailBox); // останавливаем передачу и
                break; //                                     // выходим
            }
        }
    } 
    
}
//-------------------------------------------------------------------
//Сохранить сообщение RxMessage в буфере MsgBuf
void Can::SaveMessage(void)
{
    for(uint8_t i=0; i<MSG_BUF_SIZE; i++) {
        if (!MsgBuf[i].Received_f) {
            MsgBuf[i].Received_f=true;
            uint8_t size = sizeof(can_receive_message_struct);
            uint8_t* pSrc = (uint8_t*)&RxMessage; 
            uint8_t* pDst = (uint8_t*)&MsgBuf[i].Msg;
            for (uint8_t x=0; x < size; x++) {
                pDst[x] = pSrc[x];
            }
            return;
        }
    }
}
//-------------------------------------------------------------------
//Прерывание по приему CAN
extern "C" void CAN0_RX1_IRQHandler(void)
{
    if (can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFL0) == SET) {                  // Прерывание получения пакета в буфер FIFO 0
        // Флаг сбрасывается автоматически после прочтения последнего сообщения
        can.Interrupt_f=true;
        can_message_receive(CAN0, CAN_FIFO0, &can.RxMessage);                 // Получим сообщение
        can.SaveMessage();
        can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFL0);
    }
    if (can_interrupt_flag_get(CAN0, CAN_INT_FLAG_RFL1) == SET) {                  // Прерывание получения пакета в буфер FIFO 1
       // Флаг сбрасывается автоматически после прочтения последнего сообщения
        can.Interrupt_f=true;
        can_message_receive(CAN0, CAN_FIFO1, &can.RxMessage);                 // Получим сообщение
        can.SaveMessage();
        can_interrupt_flag_clear(CAN0, CAN_INT_FLAG_RFL1);
    }
}
//-----------------------------------------------------------------
