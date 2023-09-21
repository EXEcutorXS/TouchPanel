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
        void n20(uint8_t TransType, uint8_t TransAddr);              //Неисправности
        void n28(uint8_t TransType, uint8_t TransAddr);              //Время наработки
        void n29(uint8_t TransType, uint8_t TransAddr, uint8_t AMP); //Параметры давления
        void n30(uint8_t TransType, uint8_t TransAddr);              //Состояние сигнализации, двигателя автомобиля. Температура датчика воздуха. Напряжение канала двигателя автомобиля
        void n31(uint8_t TransType, uint8_t TransAddr);              //Время работы
        void n32(uint8_t TransType, uint8_t TransAddr);              //Параметры работы жидкостного подогревателя.
        void n33(uint8_t TransType, uint8_t TransAddr, uint8_t AMP); //ID
        void n39(uint8_t TransType, uint8_t TransAddr);              //состояние драйверов устройств
        void n40(uint8_t TransType, uint8_t TransAddr);
    
private:
};
extern CAN_PGN canPGN;

#endif /* __PGN_H */
