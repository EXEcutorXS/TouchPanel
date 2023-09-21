/******************************************************************************
* ООО Теплостар
* Самара
* 
* Программисты: Кубышкин К.А.
* 
* __.01.2021
* Описание:
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "pgn.h"
#include "core.h"
#include "can.h"
#include "ADCmodule.h"
#include "warming_up.h"
//#include "errors.h"
//#include "flame.h"
#include "fan.h"
#include "pump.h"
#include "fuel_pump.h"
#include "glow_plug.h"
//#include "version.h"
//#include "press_spi.h"
//#include "bb_common.h"
#include "signaling.h"
#include "engine.h"
#include "relay.h"
//#include "config.h"
#include "param_tools.h"
#include "params_name.h"
#include "GD_press_spi.h"
#include "pu_4.h"
#include "correct_mode.h"

CAN_PGN canPGN;

//-------------------------------------------------------------------
CAN_PGN::CAN_PGN(void)
{
}
//-------------------------------------------------------------------
uint8_t CAN_PGN::getT(int16_t AT)
{
    if (AT<-75)     return 0;
    else {
        if (AT>179) return 254;
        else        return (uint8_t)(AT+75);
    }
}
//-------------------------------------------------------------------
void CAN_PGN::n10(uint8_t TransType, uint8_t TransAddr)
{
    /*
    uint8_t stage, phase;
    uint8_t st=0, md=0;
    
    stage = measure.getVal((char*)HTR_STAGE);
    phase = measure.getVal((char*)HTR_PHASE);
    if (stage == 'Z'){
        st = 0;
    }
    else if (stage == 'H'){
        st = 2;
    }
    else if (stage == 'W'){
        st = 3;
        ///if (phase == 8){
        ///    md = 30;
        ///}
        if (phase == 13){
            md = 30;
        }
    }
    else if (stage == 'F'){
        st = 4;
    }

    can.SendMessage((10<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                    st,md,measure.getVal((char*)HTR_FAULT_CODE),252+pump.counterFault,measure.getVal((char*)HTR_FAULT_CODE),measure.getVal((char*)HTR_FAULT_CODE),0xFF,0xFF);
    */
    
    can.SendMessage((10<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                    correctMode.stage, correctMode.mode, measure.getVal((char*)HTR_FAULT_CODE),252+pump.counterFault,measure.getVal((char*)HTR_WARNING_CODE), PU4.FaultBlinkCode,0xFF,0xFF);
}
//-------------------------------------------------------------------
void CAN_PGN::n11(uint8_t TransType, uint8_t TransAddr)
{
    uint16_t volt = (uint16_t)(measure.getVal((char*)HTR_VOLTAGE)*10);
    can.SendMessage((11<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        volt>>8, volt&0x00ff, (uint8_t)pressSpi.PressurekPa, 0xff,0xff,0xFF,0xFF,0xFF);
}
//-------------------------------------------------------------------
void CAN_PGN::n12(uint8_t TransType, uint8_t TransAddr)
{
    uint16_t FD,FR;
    FD=fuelPump.frequencyDefine*100; 
    FR=fuelPump.frequencyDefine*100;
    
    can.SendMessage((12<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                         fan.revolutionDefine,fan.revolution,FD>>8,FD,FR>>8,FR,glowPlug.percentDefine,0xF0+(signaling.isOn<<4)+(relay.isOn<<2)+pump.isOn);
}
//-------------------------------------------------------------------
void CAN_PGN::n13(uint8_t TransType, uint8_t TransAddr)
{
    int16_t Tflame, Tliquid, TOverheat;
    int8_t Tcpu = measure.getVal((char*)TEMPERATURE_CPU)+75;
    
    Tflame = core.getTSvalue(PURP_T_FLAME);
    Tliquid = core.getTSvalue(PURP_T_LIQUID)+75;
    TOverheat = core.getTSvalue(PURP_T_OVERHEAT)+75;

    can.SendMessage((13<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        Tflame>>8,Tflame, Tcpu, Tliquid, TOverheat, 0xFF,0xFF,0xFF);
}
//-------------------------------------------------------------------
void CAN_PGN::n14(uint8_t TransType, uint8_t TransAddr)
{
    int16_t Minimum = measure.getVal((char*)FLAME_BREAK_BORDER);
    int16_t spDTflame = parameter.getVal((char*)FLAME_BORDER_DELTA);
    int16_t limitH = parameter.getVal((char*)FLAME_BORDER_H);
    uint8_t stage = measure.getVal((char*)HTR_STAGE);
    uint8_t phase = measure.getVal((char*)HTR_PHASE);
    int16_t limitW = parameter.getArr((char*)FLAME_FAILURE_W, core.getPower(stage, phase))*(-1);
        
    can.SendMessage((14<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        Minimum>>8, Minimum,
                        limitH>>8, limitH,
                        limitW>>8, limitW, 
                        spDTflame>>8, spDTflame);
    
}
//-------------------------------------------------------------------
void CAN_PGN::n15(uint8_t TransType, uint8_t TransAddr)
{
    can.SendMessage((15<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        adc.AdcAverVals[0]>>8,adc.AdcAverVals[0],adc.AdcAverVals[1]>>8,adc.AdcAverVals[1],adc.AdcAverVals[2]>>8,adc.AdcAverVals[2],adc.AdcAverVals[3]>>8,adc.AdcAverVals[3]);
}
//-------------------------------------------------------------------
void CAN_PGN::n16(uint8_t TransType, uint8_t TransAddr)
{
    can.SendMessage((16<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        adc.AdcAverVals[4]>>8,adc.AdcAverVals[4],adc.AdcAverVals[5]>>8,adc.AdcAverVals[5],adc.AdcAverVals[6]>>8,adc.AdcAverVals[6],adc.AdcAverVals[7]>>8,adc.AdcAverVals[7]);
}
//-------------------------------------------------------------------
void CAN_PGN::n17(uint8_t TransType, uint8_t TransAddr)
{
    can.SendMessage((17<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,adc.AdcAverVals[8]>>8,adc.AdcAverVals[8],0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
}
//-------------------------------------------------------------------
void CAN_PGN::n18(uint8_t TransType, uint8_t TransAddr)              //Версия ПО
{ 
    uint32_t version = parameter.getVal((char*)VERSION_DATA_CYCLOGRAMM);
    uint8_t* pv = (uint8_t*)&version;
    can.SendMessage((18<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        pv[0], pv[1], pv[2], pv[3],0xFF, 18, 1, 23); // 0xFF,VER_DAY,VER_MONTH,VER_YEAR
    /*
        can.SendMessage((18<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        VER_PRODUCT_TYPE,VER_VOLTAGE,VER_PRODUCT_SUBTYPE,VER_ASSEMBLAGE_NUMBER,0xFF,VER_DAY,VER_MONTH,VER_YEAR);
    */
}
//-------------------------------------------------------------------
void CAN_PGN::n20(uint8_t TransType, uint8_t TransAddr)              //Неисправности
{
    uint8_t err = measure.getVal((char*)HTR_FAULT_CODE);
    can.SendMessage((18<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        err, PU4.FaultBlinkCode, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
}
//-------------------------------------------------------------------
void CAN_PGN::n28(uint8_t TransType, uint8_t TransAddr)              //Время наработки
{
    uint32_t totaltime = measure.getVal((char*)WORK_TIME_ALL);
    uint32_t totalPowerTime = 0; 
    for(uint8_t i=0; i<10; i++) {
        totalPowerTime += measure.getArr((char*)WORK_TIME_STRENGTH, (uint16_t)i);
    }
    
    can.SendMessage((28<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        totalPowerTime>>24, totalPowerTime>>16, totalPowerTime>>8, totalPowerTime,
                        totaltime>>24, totaltime>>16, totaltime>>8, totaltime);
}
//-------------------------------------------------------------------
void CAN_PGN::n29(uint8_t TransType, uint8_t TransAddr, uint8_t AMP)
{   
    uint32_t ID=(29<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress;
    if (parameter.getVal((char*)SENSOR_PRESSURE) != 0) { // если датчик включен
        uint32_t dP = measure.getVal((char*)AMPLITUDE_PRESSURE);
        float Press = measure.getVal((char*)HTR_PRESSURE);
        uint32_t P=(uint32_t)(Press*1000);
        uint32_t Min = pressSpi.min;
        uint32_t Max = pressSpi.max;
        
        if (AMP==1) { can.SendMessage(ID,1, (uint8_t)Press, Max>>16,Max>>8,Max, Min>>16,Min>>8,Min); return; }
        if (AMP==2) { can.SendMessage(ID,2, dP>>8,dP, 252+pressSpi.isFlame_f, P>>16,P>>8,P, 0xFF); return; }
        if (AMP==0xFF) {
            can.SendMessage(ID,1, (uint8_t)Press, Max>>16,Max>>8,Max, Min>>16,Min>>8,Min);
            can.SendMessage(ID,2, dP>>8,dP, 252+pressSpi.isFlame_f, P>>16,P>>8,P, 0xFF);
            return;
        }
    }
    can.SendMessage(ID,AMP,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);      //нет данных для этого мультипакета
}
//-------------------------------------------------------------------
//Состояние сигнализации, двигателя автомобиля. Температура датчика воздуха. Напряжение канала двигателя автомобиля
void CAN_PGN::n30(uint8_t TransType, uint8_t TransAddr)
{
    uint16_t U=(uint16_t)(measure.getVal((char*)HTR_VOLTAGE)*10);
    int16_t tair = (int16_t)core.getTSvalue(PURP_T_CABIN);
    can.SendMessage((30<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                        0xF0+(engine.isOn<<2)+signaling.isOn,tair>>8,tair,U>>8,U,0xFF,0xFF,0xFF);
}
//-------------------------------------------------------------------
//Время работы
void CAN_PGN::n31(uint8_t TransType, uint8_t TransAddr)
{
    uint32_t RunningTime = measure.getVal((char*)TIME_WORK);
    uint32_t ModeTime = measure.getVal((char*)TIME_PHASE);
    
    can.SendMessage((31<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                    RunningTime>>24,RunningTime>>16,RunningTime>>8,RunningTime,ModeTime>>24,ModeTime>>16,ModeTime>>8,ModeTime);
}
//-------------------------------------------------------------------
void CAN_PGN::n32(uint8_t TransType, uint8_t TransAddr)
{
    uint16_t Val=setting.getVal((char*)SETUP_WORK_TIME);
    uint8_t V8=240+(0<<2)+0;
    can.SendMessage((32<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                    Val>>8,Val,(warmingUp.isOn<<4)+measure.getVal((char*)HTR_PHASE),0xFF,0xFF,V8,0xFF,0xFF);
}
//-------------------------------------------------------------------
void CAN_PGN::n33(uint8_t TransType, uint8_t TransAddr, uint8_t AMP)
{
    uint32_t ID=(33<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress;

    if (AMP==1) { can.SendMessage(ID,1,parameter.getArr((char*)ID_HEATER, 0),
                                       parameter.getArr((char*)ID_HEATER, 1),
                                       parameter.getArr((char*)ID_HEATER, 2),
                                       parameter.getArr((char*)ID_HEATER, 3),
                                       0xFF,0xFF,0xFF); return; }
    if (AMP==2) { can.SendMessage(ID,2,parameter.getArr((char*)ID_HEATER, 4),
                                       parameter.getArr((char*)ID_HEATER, 5),
                                       parameter.getArr((char*)ID_HEATER, 6),
                                       parameter.getArr((char*)ID_HEATER, 7),
                                       0xFF,0xFF,0xFF); return; }
    if (AMP==3) { can.SendMessage(ID,3,parameter.getArr((char*)ID_HEATER, 8),
                                       parameter.getArr((char*)ID_HEATER, 9),
                                       0xFF,
                                       0xFF,
                                       0xFF,0xFF,0xFF); return; }
    if (AMP==0xFF) {
        can.SendMessage(ID,1,parameter.getArr((char*)ID_HEATER, 0),
                             parameter.getArr((char*)ID_HEATER, 1),
                             parameter.getArr((char*)ID_HEATER, 2),
                             parameter.getArr((char*)ID_HEATER, 3),
                             0xFF,0xFF,0xFF);
        can.SendMessage(ID,2,parameter.getArr((char*)ID_HEATER, 4),
                             parameter.getArr((char*)ID_HEATER, 5),
                             parameter.getArr((char*)ID_HEATER, 6),
                             parameter.getArr((char*)ID_HEATER, 7),
                             0xFF,0xFF,0xFF);
        can.SendMessage(ID,3,parameter.getArr((char*)ID_HEATER, 8),
                             parameter.getArr((char*)ID_HEATER, 9),
                             0xFF,
                             0xFF,
                             0xFF,0xFF,0xFF);
        return;
    }
    can.SendMessage(ID,AMP,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);      //нет данных для этого мультипакета
}
//-------------------------------------------------------------------
void CAN_PGN::n39(uint8_t TransType, uint8_t TransAddr)
{
//    uint8_t V8;
//    V8=(Pump.GetDriverStatus()<<6)+(GlowPlug.GetDriverStatus()<<4)+(Relay.GetDriverStatus()<<2)+FuelPump.GetDriverStatus();
//    can.SendMessage((39<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
//                    V8,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
}
//-------------------------------------------------------------------
// Уровень и расход топлива
void CAN_PGN::n40(uint8_t TransType, uint8_t TransAddr)
{
    /*
    uint16_t fuelConsumptionAfterFull, fuelConsumptionAll;
    
    fuelConsumptionAfterFull = adc.fuelConsumptionAfterFull + 0.5;
    fuelConsumptionAll = adc.fuelConsumptionAll + 0.5;
    
    can.SendMessage((31<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,
                    adc.fuelLevel,
                    adc.fuelConsumptionPerHour>>8, adc.fuelConsumptionPerHour,
                    fuelConsumptionAfterFull>>8, fuelConsumptionAfterFull,
                    fuelConsumptionAll>>8, fuelConsumptionAll,
                    0xFF);
    */
}
