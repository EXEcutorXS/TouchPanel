
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORS_H
#define __SENSORS_H

/* Includes ------------------------------------------------------------------*/
#include "PeripheralLib.h"
//---------------------------------------------------------------

#define     SENS_NUMB           8
#define     SENSOR1             0
#define     SENSOR2             1
#define     SENSOR3             2

#define     NO_PRESS            0
#define     SHORT_PRESS         1
#define     LONG_PRESS          2
#define     VERY_LONG_PRESS     3
#define     PUSHED              5

#define     READ_PERIOD               (uint16_t)20
#define     SHORT_PRESS_TIME          ((uint16_t)5/READ_PERIOD)
#define     LONG_PRESS_TIME           ((uint16_t)1500/READ_PERIOD)
#define     VERY_LONG_PRESS_TIME      ((uint16_t)3000/READ_PERIOD)
#define     LIMIT_PRESS_TIME          ((uint16_t)5000/READ_PERIOD)


#define     SENS_DO_PORT       GPIOB
#define     SENS_DO_PIN        GPIO_PIN_8

#define     SENS_CLK_PORT       GPIOB
#define     SENS_CLK_PIN        GPIO_PIN_9


/* Classes ------------------------------------------------------------------*/
class CapacitSensor
{
    public:
        
        void initialize(void);
        void handler(void);  
        void ReadSensors(void);
        void ClearSensors(void);
        bool AnyPressed;
        uint8_t SensState[SENS_NUMB];

    private:
        void tickDelay(uint32_t mks);
        uint16_t sensPresCnt[SENS_NUMB];
        uint8_t state[SENS_NUMB];
        
};
extern CapacitSensor sensor;

#endif 
