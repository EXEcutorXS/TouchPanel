/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H
#define __CAN_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"
#include "gd32f30x_can.h"

/* Classes ------------------------------------------------------------------*/
class Can
{
    #define MSG_BUF_SIZE 10                                          //размер приемного буфера. Не более 255!!! иначе надо везде менять тип
    typedef struct {
        bool Received_f;
        can_receive_message_struct Msg;
    } TReceivedMsg;

    public:
        Can(void);
        void initialize(void);
        void SendMessage(uint32_t AID, uint8_t AD0, uint8_t AD1, uint8_t AD2, uint8_t AD3, uint8_t AD4, uint8_t AD5, uint8_t AD6, uint8_t AD7);
        void SaveMessage(void);                                      //Сохранить сообщение RxMessage в буфере MsgBuf
        void initialize(uint8_t mode, uint16_t baudr);
        void handler(void);
        void shutdown(void);

        #define CAN_CONTROL_DEVICE_TYPE  126                         //Тип устройства управления (Пульт, блок управления отопительной системой, стенд, диагностическое оборудование и т.п.)
        #define CAN_ADDRESS_MAX            7                         //Максимально возможный адрес устройства
        uint8_t idType;                                              //Тип устройства
        uint8_t idAddress;                                           //Адрес устройства
        int ID;                                                      //Идентификатор сообщения

        can_trasnmit_message_struct TxMessage;    
        can_receive_message_struct RxMessage;
        TReceivedMsg MsgBuf[MSG_BUF_SIZE];                           //Буфер принимаемых сообщений
    
        bool Interrupt_f;                                            //Произошло прерывание по приему CAN
    private:
        
        
    
};
extern Can can;

#endif /* __CAN_H */
