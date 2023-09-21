
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
        void receiveIntHandler(uint8_t AByte);          // ���������� ���������� ������ ������
        //uint16_t GetReceivedBytes(void);               // ������� ���� �������
        uint16_t GetReceiveBufferPos(void);
        bool startTransmission(uint8_t *buf, uint16_t len);
        void transmitNextByte(void);                    // �������� ���������� ����� ������
        //void processTimeOut(void);                      // ������� ��������. ����� �������� � ���������� ����������
        void send(uint8_t* buf, uint32_t len);
        void usartIrqHandler(void);
        uint32_t GetBaudrate(void);
        
        void SetTxPinAsTx(void);
        bool TxPinAsPort_f;

        bool isTransmission;                            // � �������� ��������
        uint32_t timerTransPause;
        uint32_t timerTrans;
        static const int BUFFER_SIZE = 512;             //������ ��������� �������

    private:
        

        uint8_t packetIn[BUFFER_SIZE];            // �������� �����
        uint8_t packetOut[BUFFER_SIZE];           // ������ ��� ������������ ������ ��������

        uint16_t receiveCounter;                       // ������� � ������ ������
        //uint16_t readCounter;                          // ������� � ������ ������ ��� ������
        //uint16_t packetTimeOut;                          // ��� ������� "��� �����" ������ ������������ ������
        //uint16_t answerTimeOut;                          // ��� ������� "��� �����" ��� ������ ������ �� �������
        uint16_t transmitCounter;                        // ������� ������ ��� �������� ������
        uint16_t lengthPacketOut;
        uint16_t lengthPacketIn;
        uint16_t TimeoutCnt;
};
extern Usart usart;

#endif /* __USART_H */
