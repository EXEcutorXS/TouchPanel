
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STOVE_H
#define __STOVE_H

/* Includes ------------------------------------------------------------------*/
#include "PeripheralLib.h"
//---------------------------------------------------------------
#define     SENS_ON               0
#define     SENS_PLUS             1
#define     SENS_MINUS            2

#define     STANDBY           0
#define     OFF               10
#define     OFF_BLINK         20
#define     ON                30
#define     ON_BLINK          40
#define     POWER             50
#define     SET_POWER         60
#define     TEMP              70
#define     ERROR             80
#define     LOCK              90
#define     LOCK_OFF          100
#define     TIMER             110
#define     EDIT_TIMER        111
#define     TIMER_ON          112
#define     TIMER_OFF         115
#define     TEST              150


#define     POWER_STATE_TIMEOUT     3000
#define     ON_STATE_TIMEOUT        5000
#define     OFF_STATE_TIMEOUT       60000
#define     TEST_STATE_TIMEOUT      3000
#define     TEST_BEEP               1000
#define     START_PRESS_TIMEOUT     10000
#define     TOSLEEP_TIMEOUT         10000
#define     TIMER_PRESS_TIMEOUT     10000
#define     LOCK_STATE_TIMEOUT      2000
#define     CHECK_STATE_TIME        3000

#define     ON_BEEP_TIME            1000
#define     OFF_BEEP_TIME           500
#define     SHORT_BEEP_TIME          50
#define     LOCK_BEEP_TIME          500
#define     TIMERON_BEEP_TIME       200
#define     LOC_BEEP_TIME           300

#define     OFF_BLINK_PERIOD        500
#define     ERROR_BLINK_PERIOD      1000

#define     MAX_TIMER_TIME          180
#define     MIN_TIMER_TIME          5

/* Classes ------------------------------------------------------------------*/
class TStove
{
    public:
        //bool sleep_f;
        void initialize(void);
        void handler(void);  

    private:
        uint8_t State;
        //uint16_t TimerTime; // minute
        uint32_t BeginStateTime;
        bool ManualClearError_f;
        uint8_t seconds;
        uint16_t TimerMinutes;
        bool TimerOn_f;
        bool Locked_f;
    
        void SetState(uint8_t st);
        void UpdateDisplay();
        void TripleBeep(void);
        void DoubleBeep(void);
        void TimerEnable(uint16_t min);
        void TimerDisable(void);
        void TimerHandler(void);
};
extern TStove stove;

#endif 
