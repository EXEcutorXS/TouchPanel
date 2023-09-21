
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H
#define __USART_H

/* Includes ------------------------------------------------------------------*/
//#include "gd32f30x.h"
#include "PeripheralLib.h"
#include "usartParams.h"
/* Classes ------------------------------------------------------------------*/
class Usart
{
    public:
        
        UsartParams_t usartParams;
        uint32_t usartType;
        uint32_t TxGpio;
        uint32_t TxPin;
        uint32_t RxGpio;
        uint32_t RxPin;
        IRQn_Type usartIrq;
        IRQn_Type dmaTxIrq;
        IRQn_Type dmaRxIrq;
        uint32_t dmaTxType;
        dma_channel_enum dmaTxCh;
        uint32_t dmaRxType;
        dma_channel_enum dmaRxCh;

        Usart(void);
        void initialize(UsartParams_t* uartParams);
        void handler(void);
        void ClearBuffer(void);
        uint8_t GetByte(uint16_t index);
        void changeBaudrate(uint32_t baudrate);
        void receiveIntHandler(uint8_t AByte);          // обработчик прерывания приема байтов
        //uint16_t GetReceivedBytes(void);               // сколько байт принято
        uint16_t GetReceiveBufferPos(void);
        bool startTransmission(uint8_t *buf, uint16_t len);
        void transmitNextByte(void);                    // отправка следующего байта пакета
        //void processTimeOut(void);                      // счетчик таймаута. вызов ставится в обработчик прерывание
        void send(uint8_t* buf, uint32_t len);
        void usartIrqHandler(void);
        uint32_t GetBaudrate(void);
        
        void SetTxPinAsTx(void);
        bool TxPinAsPort_f;

        bool isTransmission;                            // в процессе отправки
        uint32_t timerTransPause;
        uint32_t timerTrans;
        static const int BUFFER_SIZE = 512;             //размер буферного массива

    private:
        

        uint8_t packetIn[BUFFER_SIZE];            // принятый пакет
        uint8_t packetOut[BUFFER_SIZE];           // массив для формирования пакета отправки

        uint16_t receiveCounter;                       // позиция в буфере приема
        //uint16_t readCounter;                          // позиция в буфере приема при чтении
        //uint16_t packetTimeOut;                          // для отсчета "нет связи" внутри принимаемого пакета
        //uint16_t answerTimeOut;                          // для отсчета "нет связи" при приеме ответа на команду
        uint16_t transmitCounter;                        // счетчик байтов для передачи пакета
        uint16_t lengthPacketOut;
        uint16_t lengthPacketIn;
        uint16_t TimeoutCnt;
};
extern Usart usart;

#endif /* __USART_H */
