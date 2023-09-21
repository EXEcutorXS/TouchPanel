/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PGN_H
#define __PGN_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"

/* Classes ------------------------------------------------------------------*/
class CAN_PGN
{
    public:
        CAN_PGN(void);
        uint8_t getT(int16_t AT);
        void n10(uint8_t TransType, uint8_t TransAddr);
        void n11(uint8_t TransType, uint8_t TransAddr);
        void n12(uint8_t TransType, uint8_t TransAddr);
        void n13(uint8_t TransType, uint8_t TransAddr);
        void n14(uint8_t TransType, uint8_t TransAddr);
        void n15(uint8_t TransType, uint8_t TransAddr);
        void n16(uint8_t TransType, uint8_t TransAddr);
        void n17(uint8_t TransType, uint8_t TransAddr);
        void n18(uint8_t TransType, uint8_t TransAddr);
        void n20(uint8_t TransType, uint8_t TransAddr);              //�������������
        void n28(uint8_t TransType, uint8_t TransAddr);              //����� ���������
        void n29(uint8_t TransType, uint8_t TransAddr, uint8_t AMP); //��������� ��������
        void n30(uint8_t TransType, uint8_t TransAddr);              //��������� ������������, ��������� ����������. ����������� ������� �������. ���������� ������ ��������� ����������
        void n31(uint8_t TransType, uint8_t TransAddr);              //����� ������
        void n32(uint8_t TransType, uint8_t TransAddr);              //��������� ������ ����������� �������������.
        void n33(uint8_t TransType, uint8_t TransAddr, uint8_t AMP); //ID
        void n39(uint8_t TransType, uint8_t TransAddr);              //��������� ��������� ���������
        void n40(uint8_t TransType, uint8_t TransAddr);
    
private:
};
extern CAN_PGN canPGN;

#endif /* __PGN_H */
