
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUZER_H
#define __BUZER_H

/* Includes ------------------------------------------------------------------*/
#include "PeripheralLib.h"
//---------------------------------------------------------------

/* Classes ------------------------------------------------------------------*/
class TBuzzer
{
    public:

        void initialize(void);
        void handler(void);  
        void StartBeep(uint16_t pulseTime);
        void StartSeriesBeep(uint16_t pulseTime, uint16_t Period, uint8_t numbPulse);

    private:
        void SetOutOn(void);
        void SetOutOff(void);

        uint32_t PulseStartTime;
        uint16_t PulseTime;
        uint16_t RepeatPeriod;
        uint8_t NumbRepeat;
        bool On_f;
};
extern TBuzzer buzzer;

#endif 
