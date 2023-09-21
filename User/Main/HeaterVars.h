
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HEATER_H
#define __HEATER_H

/* Includes ------------------------------------------------------------------*/
#include "PeripheralLib.h"
//---------------------------------------------------------------

#define     CMD_START   1
#define     CMD_STOP    3
#define     STAGE_Z     0
#define     STAGE_P     1
#define     STAGE_H     2
#define     STAGE_W     3
#define     STAGE_F     4

/* Classes ------------------------------------------------------------------*/
class HeaterVars
{
    public:
        
        HeaterVars(void);
        void initialize(void);
        void handler(void);  
//        void Start(void);
//        void Stop(void);

        uint8_t Stage;
        uint8_t Mode;
        uint8_t FaultCode;
        int8_t Tinlet;
        int8_t Text;
        uint16_t Usupply; // *10
        int16_t Tflame;
        int16_t Tframe;
        uint8_t ModePanel;
        uint8_t FanRevDefined;                     
        uint8_t FanRevMeasured;                    
        uint16_t FuelPumpFrequencyRealized; // * 100
        int16_t FlameCalibrationValue;
        int16_t FrameCalibrationValue;          
        uint8_t WorkModeBy;                   
        int16_t TSetpoint;                    
        uint8_t WaitModeIsAllowed;            
        uint8_t PowerDefined;                
        uint8_t PowerRealized; 

        uint8_t VersionProductType;
        uint8_t VersionVoltage;
        uint8_t VersionProductSubtype;
        uint8_t VersionAsm;
        uint8_t VersionDay;
        uint8_t VersionMonth;
        uint8_t VersionYear;
        uint8_t SetWorkModeBy;
        int8_t SetTSetpoint;
        uint8_t SetWaitModeIsAllowed;
        uint8_t SetPowerDefined;
        //bool isLinkError;
        
        uint8_t PanelCommand; // CMD_START
        bool CmdStart_f;
        bool CmdStop_f;

    private:
        

};
extern HeaterVars heater;

#endif 
