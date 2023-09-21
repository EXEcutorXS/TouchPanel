
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LEDLINE_H
#define __LEDLINE_H

/* Includes ------------------------------------------------------------------*/
#include "PeripheralLib.h"
//---------------------------------------------------------------

/* Classes ------------------------------------------------------------------*/
class TLedLine
{
    public:
        void initialize(void);
        void handler(void);  
        void On(void);
        void Off(void);
        bool getState(void);

    private:
        bool On_f;
};
extern TLedLine LedLine;

#endif 
