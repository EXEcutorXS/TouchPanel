
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PROTOCOL3_H
#define __PROTOCOL3_H

/* Includes ------------------------------------------------------------------*/
//#include "gd32f30x.h"
#include "PeripheralLib.h"

/* Classes ------------------------------------------------------------------*/
class Protocol3
{
    public:

        Protocol3(void);
        void initialize(void);
        void handler(void);
        void parsing(void);
        void changeBaudrate(uint32_t baudrate);
        void processReceivedData(void);                 // прием данных по последовательному каналу от пульта, вызов ставится в main.
        void receiveIntHandler(uint8_t AByte);          // обработчик прерывания приема байтов
        void transmitNextByte(void);                    // отправка следующего байта пакета
        void processTimeOut(void);                      // счетчик таймаута. вызов ставится в обработчик прерывание
        void send(uint8_t* buf, uint32_t len);
        void usartIrqHandler(void);
        void calculateCRC(uint8_t inByte);
        uint16_t calculateCRC(uint8_t* buf, uint16_t size);
        void sendPacket(uint8_t code, uint8_t dataLen, uint16_t cmd, uint8_t* data);
        void requests(void);

        uint32_t baudrate;
        bool isTransmission;                            // в процессе отправки
        //uint8_t verProtocol;                            // номер версии протокола связи
        uint32_t timerTransPause;
        uint32_t timerTrans;
        bool RequestsEnable_f;

    private:
        
        void CmdStart(void);
        void CmdStop(void);
        void CmdSendParam(void);
        void CmdReadParam(void);
        static const int PACKET_HEADER = 170;           //заголовок пакета
        static const int BUFFER_SIZE = 512;             //размер буферного массива

        uint16_t linkCnt;
        uint8_t packetIn[BUFFER_SIZE];            // принятый пакет
        uint8_t packetOut[BUFFER_SIZE];           // массив для формирования пакета отправки
        //uint16_t bufferCursorW;                          // курсор для записи байтов в буфер
        uint16_t bufferCursorR;                          // курсор для чтения байтов из буфер
        uint16_t packetCounter;                          // счетчик принятых байт в пакете
        uint16_t packetTimeOut;                          // для отсчета "нет связи" внутри принимаемого пакета
        uint16_t transmitCounter;                        // счетчик байтов для передачи пакета
        uint16_t SendedCmd;
        uint16_t crc;                                   // для подсчета контрольной суммы

        bool isWaitHeaderByte;                          // ожидание заголовка пакета

        uint16_t lengthPacketOut;
        uint8_t lengthPacketIn;
        int16_t TempCorrect;
};
extern Protocol3 protocol3;

#endif /* __PROTOCOL3_H */
