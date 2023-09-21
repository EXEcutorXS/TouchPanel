
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
        void processReceivedData(void);                 // ����� ������ �� ����������������� ������ �� ������, ����� �������� � main.
        void receiveIntHandler(uint8_t AByte);          // ���������� ���������� ������ ������
        void transmitNextByte(void);                    // �������� ���������� ����� ������
        void processTimeOut(void);                      // ������� ��������. ����� �������� � ���������� ����������
        void send(uint8_t* buf, uint32_t len);
        void usartIrqHandler(void);
        void calculateCRC(uint8_t inByte);
        uint16_t calculateCRC(uint8_t* buf, uint16_t size);
        void sendPacket(uint8_t code, uint8_t dataLen, uint16_t cmd, uint8_t* data);
        void requests(void);

        uint32_t baudrate;
        bool isTransmission;                            // � �������� ��������
        //uint8_t verProtocol;                            // ����� ������ ��������� �����
        uint32_t timerTransPause;
        uint32_t timerTrans;
        bool RequestsEnable_f;

    private:
        
        void CmdStart(void);
        void CmdStop(void);
        void CmdSendParam(void);
        void CmdReadParam(void);
        static const int PACKET_HEADER = 170;           //��������� ������
        static const int BUFFER_SIZE = 512;             //������ ��������� �������

        uint16_t linkCnt;
        uint8_t packetIn[BUFFER_SIZE];            // �������� �����
        uint8_t packetOut[BUFFER_SIZE];           // ������ ��� ������������ ������ ��������
        //uint16_t bufferCursorW;                          // ������ ��� ������ ������ � �����
        uint16_t bufferCursorR;                          // ������ ��� ������ ������ �� �����
        uint16_t packetCounter;                          // ������� �������� ���� � ������
        uint16_t packetTimeOut;                          // ��� ������� "��� �����" ������ ������������ ������
        uint16_t transmitCounter;                        // ������� ������ ��� �������� ������
        uint16_t SendedCmd;
        uint16_t crc;                                   // ��� �������� ����������� �����

        bool isWaitHeaderByte;                          // �������� ��������� ������

        uint16_t lengthPacketOut;
        uint8_t lengthPacketIn;
        int16_t TempCorrect;
};
extern Protocol3 protocol3;

#endif /* __PROTOCOL3_H */
