/********************************************************************
* ООО Адверс
* Самара
*
* Программисты: .
*
* 17.07.2020
* Описание:
********************************************************************/
/* Includes -------------------------------------------------------*/
    #include "messages.h"
    //#include "paramsname.h"
    #include "param_tools.h"
    #include "params_name.h"
    //#include "stage.h"
    //#include "stage_m.h"
    #include "core.h"
    #include "usart.h"
    #include "can.h"
    #include "ADCmodule.h"
    #include "fan.h"
    #include "fuel_pump.h"
    #include "glow_plug.h"
    #include "pump.h"
    //#include "cyclogram.h"
    //#include "config.h"
    //#include "flash_tools.h"
    //#include "overheat.h"
    //#include "flame.h"
    //#include "errors.h"
    //#include "bb_common.h"
    //#include "bb_errors.h"
    //#include "press_spi.h"
    #include "pgn.h"
    //#include "heaterstate.h"
    #include "TankControl.h"
    #include "relay.h"
    #include "correct_mode.h"
    #include "recorder.h"

Messages messages;
//-------------------------------------------------------------------
Messages::Messages(void)
{
    PumpStatus_f=false;
    DWaterFlow_f=false;

    ManualModeStatus_f=false;
    PCBTestStatus_f=false;

    FaultLink_f=false;
    FaultLinkCnt=0;
    CheckConnection_f=true;
    SendMessages1s_f=false;
}
//-------------------------------------------------------------------
void Messages::handler(void)
{
    static uint32_t TimerHandler = core.getTick();

    cmdRunHeater_f=false;
    cmdStopHeater_f=false;
    cmdRunPump_f=false;

    TransmitMessage();

    for(uint8_t i=0; i<MSG_BUF_SIZE; i++) {
        if (can.MsgBuf[i].Received_f) {
            this->ProcessMessage(i);
            can.MsgBuf[i].Received_f=false;
        }
    }

    if ((core.getTick()-TimerHandler)>=1000) {
        TimerHandler = core.getTick();

        if (this->CheckConnection_f && !this->SignalingRun_f) {
            if (measure.getVal((char*)HTR_STAGE) == 'Z') ;//при ожидании команды не определяем потерю связи
            else {
                this->FaultLinkCnt++;
                if (this->FaultLinkCnt>30) this->FaultLink_f=true;
            }
        }
    }
}
//-------------------------------------------------------------------
void Messages::TransmitMessage(void)
{
    static uint32_t TimerTx = core.getTick();
	static uint32_t Timer1s = core.getTick();
    static uint8_t MsgNum=0;

	  if ((core.getTick()-Timer1s)>=1000) {
			  Timer1s = core.getTick();
			  SendMessages1s_f = true;
		}
	
    if ((core.getTick()-TimerTx)>=50) {                              //Периодическая отправка параметров
        TimerTx = core.getTick();

        if (SendMessages1s_f) {
            MsgNum++;
            
            switch(MsgNum) {
//                case 1: 
//                    canPGN.n18(CAN_CONTROL_DEVICE_TYPE,7);  
//                break;
                case 2: 
                    canPGN.n11(CAN_CONTROL_DEVICE_TYPE,7);  
                break;
                case 3: 
                    canPGN.n12(CAN_CONTROL_DEVICE_TYPE,7);  
                break;
                case 4: 
                    canPGN.n13(CAN_CONTROL_DEVICE_TYPE,7);   
                break;
                case 5: 
                    canPGN.n14(CAN_CONTROL_DEVICE_TYPE,7);   
                break;
                case 6: 
                    canPGN.n29(CAN_CONTROL_DEVICE_TYPE,7,1); 
                break;
                case 7: 
                    canPGN.n29(CAN_CONTROL_DEVICE_TYPE,7,2); 
                break;
                case 8: 
                    canPGN.n30(CAN_CONTROL_DEVICE_TYPE,7);   
                break;
                case 9: 
                    canPGN.n31(CAN_CONTROL_DEVICE_TYPE,7);   
                break;
                case 10: 
                    canPGN.n32(CAN_CONTROL_DEVICE_TYPE,7);   
                break;
                
                default:
                    canPGN.n10(CAN_CONTROL_DEVICE_TYPE,7);
                        MsgNum=1;
                        this->SendMessages1s_f=false;
                break;

            } 
            //if (MsgNum < 10) canPGN.n12(CAN_CONTROL_DEVICE_TYPE,7);
        }
    }
}
//-------------------------------------------------------------------
//обработка пакета команды
void Messages::ProcessMessage(uint8_t MsgNum)
{
    uint8_t ReceiverType   =(can.MsgBuf[MsgNum].Msg.rx_efid>>13)&127;
    uint8_t ReceiverAddress=(can.MsgBuf[MsgNum].Msg.rx_efid>>10)&7;
    uint16_t StartPair;
    uint16_t NumPair;
    uint32_t value;
    uint16_t min, max;
    uint16_t numb;
    uint16_t time;

    if (ReceiverType!=can.idType && ReceiverType!=127) return;       //тип устройства не совпадает
    if (ReceiverAddress!=can.idAddress && ReceiverAddress!=7) return;//адрес устройства не совпадает

    uint32_t address, version;
    uint16_t PGNRequest;
    uint16_t V16;
    uint16_t PGN=can.MsgBuf[MsgNum].Msg.rx_efid>>20;
    uint8_t TransType=(can.MsgBuf[MsgNum].Msg.rx_efid>> 3)&127;
    uint8_t TransAddr=can.MsgBuf[MsgNum].Msg.rx_efid&7;
    uint32_t ID, V32;
    uint8_t D[8], V8;
    uint8_t* pv;
    for(uint8_t i=0; i<8; i++) D[i]=can.MsgBuf[MsgNum].Msg.rx_data[i];

    if (TransType==126) this->FaultLinkCnt=0;
    
    usart.timerTrans = core.getTick();

    ID=(PGN<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress;
    switch(PGN) {
        case 1: //Команды
                ID=(2<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress;
                switch((D[0]<<8)+D[1]) {
                    case  0://Кто ты? Запрос для определения устройств в сети
                            version = parameter.getVal((char*)VERSION_DATA_CYCLOGRAMM);
                            pv = (uint8_t*)&version;
                            can.SendMessage(ID,D[0],D[1], pv[0], pv[1], pv[2], pv[3], 0xFF,0xFF);
                            break;
                    case  1://ПУСК
                            this->cmdRunHeater_f=true;
                            if (D[2]!=255 || D[3]!=255) setting.setVal((char*)SETUP_WORK_TIME, (uint32_t)(D[2]<<8)+D[3]);
                            V16=setting.getVal((char*)SETUP_WORK_TIME);// Stage.TimeToWork/60;
                            can.SendMessage(ID,D[0],D[1],V16>>8,V16,0xFF,0xFF,0xFF,0xFF);
                            core.startCommand();
                            break;
                    case  3://СТОП
                            this->cmdStopHeater_f=true;
                            can.SendMessage(ID,D[0],D[1],0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
                            core.stopCommand();
                            measure.isLockWrite = true;
                            measure.resetLockWrite((char*)REV_DEF);
                            measure.isLockWrite = false;
                            break;
                    case  4://Работа только помпы
                            this->cmdRunPump_f=true;
                            if (D[2]!=255 || D[3]!=255) setting.setVal((char*)SETUP_WORK_TIME, (uint32_t)(D[2]<<8)+D[3]);
                            V16=setting.getVal((char*)SETUP_WORK_TIME);// Stage.TimeToWork/60;
                            can.SendMessage(ID,D[0],D[1],V16>>8,V16,0xFF,0xFF,0xFF,0xFF);
                            if (measure.getVal((char*)HTR_STAGE) == 'Z'){
                                core.startWaterPump();
                            }
                            break;
                    case  5: // Clear Error
                            measure.setVal((char*)HTR_FAULT_CODE, (uint8_t)0);//Error.Clear();
                            can.SendMessage(ID,D[0],D[1],measure.getVal((char*)HTR_FAULT_CODE),0xFF,0xFF,0xFF,0xFF,0xFF);
                            break;
                    case  6://Задать параметры работы жидкостного подогревателя
                            time = (D[2]<<8)+D[3];
                            if (time == 0) time = 65000; // большое время работы (почти бесконечность)
                            if (time != 0xffff) setting.setVal((char*)SETUP_WORK_TIME, (uint32_t)time);
                            //if ((D[4]&0x0F)!=0x0F) Stage.SetHeaterMode(D[4]&0xF);
                            if ((D[4]&0xF0)!=0xF0) setting.setVal((char*)WARM_UP_MODE, (int8_t)(D[4]&0xF0)>>4);//Stage.SetWarmUpMode((D[4]&0xF0)>>4);
                            //Stage.SetTemperatures();
                            //V16=(D[5]<<8)+D[6]; if (V16!=0xFFFF) Stage.TsetpointAir=(int16_t)V16;
                            V8=(D[7]&3);
                            if (V8!=3) { if (V8==1) setting.setVal((char*)PUMP_IN_STANDBY, (int8_t)true); if (V8==0) setting.setVal((char*)PUMP_IN_STANDBY, (int8_t)false); }
                            V8=(D[7]&12);
                            if (V8!=12) { if (V8==4) setting.setVal((char*)PUMP_ON_ENGINE, (int8_t)true); if (V8==0) setting.setVal((char*)PUMP_ON_ENGINE, (int8_t)false); }
                            //подготовка и отправка ответа
                            V16=setting.getVal((char*)SETUP_WORK_TIME);// Stage.TimeToWork/60;
                            V8=((uint8_t)setting.getVal((char*)PUMP_ON_ENGINE)<<2)+setting.getVal((char*)PUMP_IN_STANDBY);
                            can.SendMessage(ID,D[0],D[1],V16>>8,V16,((uint8_t)setting.getVal((char*)WARM_UP_MODE)<<4)+0,0xFF,0xFF,V8);
                            break;
                    case  7://Запрос температурных переходов по режимам жидкостного подогревателя
                            /*if (D[2]<POWER_LEVELS_NUM) can.SendMessage(ID,0,8,D[2],Stage.dTWrUp[D[2]]>>8,Stage.dTWrUp[D[2]],Stage.dTWrDown[D[2]]>>8,Stage.dTWrDown[D[2]],0xFF);
                            else */can.SendMessage(ID,D[0],D[1],0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
                            break;
                    case 11://Cброс циклограммы и/или температурных переходов к заводским настройкам
                            //if ((D[2]& 3) == 1) Stage.DefaultCyclogram();  //Cброс циклограммы
                            //if ((D[2]&12) == 4) Stage.DefaultTransitions();//Cброс температурных переходов
                            can.SendMessage(ID,D[0],D[1],D[2],0xFF,0xFF,0xFF,0xFF,0xFF);
                            break;
                    case 20://Калибровка термопар
                            adc.calibrationTc();
                            can.SendMessage(ID,D[0],D[1],adc.AdcAverVals[chTC1]>>8,adc.AdcAverVals[chTC1],adc.AdcAverVals[chTC2]>>8,adc.AdcAverVals[chTC2],0xFF,0xFF);
                            break;
                    case 22://Reset CPU
                            //ToDo сделать проверку принятых байтов AA 55
                            *(__IO uint32_t*)(0x20000000+DATA_OFFSET) = 0x0016AA55;
                            NVIC_SystemReset();
                            break;
                    case 45://Биты реакции на неисправности
                            /*V8=(D[2]&  3); if (V8== 1) Error.NoReactionAll_f=true;        if (V8==0) Error.NoReactionAll_f=false;
                            V8=(D[2]& 12); if (V8== 4) Error.NoReactionFP_f=true;         if (V8==0) Error.NoReactionFP_f=false;
                            V8=(D[2]& 48); if (V8==16) Error.NoReactionFlameBreak_f=true; if (V8==0) Error.NoReactionFlameBreak_f=false;
                            V8=(D[2]&192); if (V8==64) Error.NoReactionGP_f=true;         if (V8==0) Error.NoReactionGP_f=false;
                            V8=(D[3]&  3); if (V8== 1) Error.NoReactionFan_f=true;        if (V8==0) Error.NoReactionFan_f=false;
                            V8=(D[3]& 12); if (V8== 4) Error.NoReactionSensors_f=true;    if (V8==0) Error.NoReactionSensors_f=false;
                            V8=(D[3]& 48); if (V8==16) Error.NoReactionPump_f=true;       if (V8==0) Error.NoReactionPump_f=false;
                            V8=(D[3]&192); if (V8==64) Error.NoReactionOverheat_f=true;   if (V8==0) Error.NoReactionOverheat_f=false;
                            
                            can.SendMessage(ID,D[0],D[1],Error.GetNoReactionByte1(),Error.GetNoReactionByte2(),0xFF,0xFF,0xFF,0xFF);*/
                            can.SendMessage(ID,D[0],D[1],0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
                            break;
                    case 65://Установить значение входа

                            switch(D[2]) {
                                case 7://Т жидкости
                                    measure.isLockWrite = true;
                                    core.setTSvalue(PURP_T_LIQUID, (float)(D[3]*256+D[4]));
                                    measure.isLockWrite = false;
                                   break;
                                case 12://ИП
                                    measure.isLockWrite = true;
                                    core.setTSvalue(PURP_T_FLAME, (float)(D[3]*256+D[4]));
                                    measure.isLockWrite = false;
                                    break;
                                case 10://Т перегрева
                                    measure.isLockWrite = true;
                                    core.setTSvalue(PURP_T_OVERHEAT, (float)(D[3]*256+D[4]));
                                    measure.isLockWrite = false;
                                    break;
                                case 27://Т воздуха
                                    measure.isLockWrite = true;
                                    core.setTSvalue(PURP_T_CABIN, (float)(D[3]*256+D[4]));
                                    measure.isLockWrite = false;
                                    break;
                                default:
                                    measure.isLockWrite = true;
                                    measure.resetLockWrite((char*)TC1);
                                    measure.resetLockWrite((char*)TC2);
                                    measure.resetLockWrite((char*)TS1);
                                    measure.resetLockWrite((char*)TS2);
                                    measure.resetLockWrite((char*)TS3);
                                    measure.isLockWrite = false;
                                    break;
                            }
                            
                            break;
                    case 66://сброс неисправностей
                            if (measure.getVal((char*)HTR_STAGE) == 'Z')  //Ожидание команды
                                measure.setVal((char*)HTR_FAULT_CODE, (uint8_t)0);
                            break;
                    case 67://Вход/выход в стадию M (ручное управление) или T (тестирование блока управления)
                        
                            V8=(D[2]&3);
                            if (V8!=3) {
                                if (V8==1) this->ManualModeStatus_f=true;
                                if (V8==0) this->ManualModeStatus_f=false;
                                this->PCBTestStatus_f=false;
                                if (!ManualModeStatus_f) {
                                    measure.isLockWrite = true;
                                    measure.resetLockWrite((char*)REV_DEF);
                                    measure.resetLockWrite((char*)FREQUENCE_FUEL_PUMP);
                                    measure.resetLockWrite((char*)PERCENT_GLOW_PLUG);
                                    measure.isLockWrite = false;
                                } else {
 
                                }
                            }
                            /*
                            V8=(D[2]&12);//12=00001100b
                            if (V8!=12) {
                                if (V8==4) this->PCBTestStatus_f=true;
                                if (V8==0) this->PCBTestStatus_f=false;
                            } */
                            if (this->ManualModeStatus_f && this->PCBTestStatus_f) this->PCBTestStatus_f=false;
                            can.SendMessage(ID,D[0],D[1], ((uint8_t)(this->PCBTestStatus_f)<<2)+(uint8_t)(this->ManualModeStatus_f),0xFF,0xFF,0xFF,0xFF,0xFF);
                            break;
                    case 68://Задание параметров устройств в стадии M (ручное управление)
                                if (ManualModeStatus_f) {
                                    measure.isLockWrite = true;
                                    if (D[3] != 0xFF) {
                                        measure.setVal((char*)REV_DEF, (uint8_t)D[3]);
                                        fan.revolutionDefine = measure.getVal((char*)REV_DEF);
                                    }
                                    if (D[4] != 0xFF) {
                                        measure.setVal((char*)PERCENT_GLOW_PLUG, (uint8_t)D[4]);
                                        glowPlug.percentDefine = D[4];
                                        glowPlug.setting(D[4]);
                                    }
                                    V16 = (D[5]<<8)+D[6];
                                    if (V16 != 0xFFFF) {
                                        measure.setVal((char*)FREQUENCE_FUEL_PUMP, (float)V16/100);
                                        fuelPump.frequencyDefine=measure.getVal((char*)FREQUENCE_FUEL_PUMP);
                                        fuelPump.setFrequency(fuelPump.frequencyDefine);
                                    }
                                    V8 = (D[2]&3);
                                    if (V8 != 3) {
                                        if (V8==1) pump.on();
                                        if (V8==0) pump.off();
                                    }
                                    measure.isLockWrite = false;
                                }
                            /*if (this->ManualModeStatus_f) {
                                V8=(D[2]&3);
                                if (V8!=3) {
                                    if (V8==1) StageM.PumpState=1;
                                    if (V8==0) StageM.PumpState=0;
                                }
                                if (D[3]!=0xFF) StageM.FanRev=D[3];
                                if (D[4]!=0xFF) StageM.GPPercent=D[4];
                                V16=(D[5]<<8)+D[6];
                                if (V16!=0xFFFF) StageM.FPFrequency100=V16;
                            }*/
                            break;
                    case 69://управление устройствами в %
                            measure.isLockWrite = true;
                            V16=(D[3]<<8)+D[4];
                            switch (D[2]) {
                                case 0://ТН, Гц
                                    if (V16!=0xFFFF) {
                                        if (V16 > 1000) V16 = 1000;
                                        measure.setVal((char*)FREQUENCE_FUEL_PUMP, (float)V16/100);
                                        fuelPump.frequencyDefine=measure.getVal((char*)FREQUENCE_FUEL_PUMP);
                                        fuelPump.setFrequency(fuelPump.frequencyDefine);
                                    }
                                    break;
                                case 1://Реле
                                    if (V16!=0xFFFF) {
                                        if (V16==0) relay.off();
                                        else relay.on();
                                    }
                                    break;
                                case 2://Свеча
                                    if (V16!=0xFFFF) {
                                        if (V16>100) V16=100;
                                        measure.setVal((char*)PERCENT_GLOW_PLUG, (uint8_t)V16);
                                        glowPlug.percentDefine = V16;
                                        glowPlug.setting(V16);
                                    }
                                    break;
                                case 3://Помпа
                                    if (V16!=0xFFFF) {
                                        if (V16==0) pump.off();
                                        else pump.on();
                                    }
                                    break;
                                case 4://ШИМ НВ. Заполнение в %
                                    if (V16!=0xFFFF) {
                                        measure.setVal((char*)REV_DEF, (uint8_t)V16);
                                        fan.revolutionDefine = measure.getVal((char*)REV_DEF);
                                    }
                                    break;
                                case 23://Заданные обороты НВ
                                    if (V16!=0xFFFF) {
                                        measure.setVal((char*)REV_DEF, (uint8_t)V16);
                                        fan.revolutionDefine = measure.getVal((char*)REV_DEF);
                                    }
                                    break;
                                default:
                                    V16=0xFFFF;
                            }
                            measure.isLockWrite = false;
                            can.SendMessage(ID,D[0],D[1],D[2],V16>>8,V16,0xFF,0xFF,0xFF);
                            //can.SendMessage(ID,D[0],D[1],D[2],0xFF,0xFF,0xFF,0xFF,0xFF);
                            break;
                    case 70://
                            /*if ((D[2]&  3)== 1) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xFE)+1;
                            if ((D[2]&  3)== 0) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xFE);
                            if ((D[2]& 12)== 4) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xFD)+2;
                            if ((D[2]& 12)== 0) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xFD);
                            if ((D[2]& 48)==16) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xFB)+4;
                            if ((D[2]& 48)== 0) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xFB);
                            if ((D[2]&192)==64) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xF7)+8;
                            if ((D[2]&192)== 0) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xF7);
                            if ((D[3]&  3)== 1) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xEF)+16;
                            if ((D[3]&  3)== 0) Stage.PCB.DeviceStatusLow=(Stage.PCB.DeviceStatusLow&0xEF);
                    
                            D[3]=D[3]|252;//252 = 11111100b
                            can.SendMessage(ID,D[0],D[1],D[2],D[3],0xFF,0xFF,0xFF,0xFF);*/
                            can.SendMessage(ID,D[0],D[1],0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
                            break;
                    default:can.SendMessage(ID,D[0],D[1],0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
                }
                break;
        case 3: //запрос параметров устройства по SPN
                V16=this->GetParameter((D[0]<<8)+D[1]);
                can.SendMessage((4<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,D[0],D[1],V16>>24,V16>>16,V16>>8,V16,0xFF,0xFF);
                break;
        case 4: //ответ на запрос параметров устройства по SPN
                break;
        case 5: //запись параметров устройства по SPN
                break;
        case 6: //запрос параметров устройства по PGN
                PGNRequest=(D[0]<<8)+D[1];
                switch(PGNRequest) {
                    case 10: canPGN.n10(TransType,TransAddr); break;
                    case 11: canPGN.n11(TransType,TransAddr); break;
                    case 12: canPGN.n12(TransType,TransAddr); break;
                    case 13: canPGN.n13(TransType,TransAddr); break;
//                    case 14: canPGN.n14(TransType,TransAddr); break;
                    case 15: canPGN.n15(TransType,TransAddr); break;
                    case 16: canPGN.n16(TransType,TransAddr); break;
                    case 17: canPGN.n17(TransType,TransAddr); break;
                    case 18: canPGN.n18(TransType,TransAddr); break; //Версия ПО
                    case 20: canPGN.n20(TransType,TransAddr); break; //Неисправности
                    case 28: canPGN.n28(TransType,TransAddr); break; //Время наработки
//                    case 29: canPGN.n29(TransType,TransAddr,D[2]); break;//Параметры давления
                    case 30: canPGN.n30(TransType,TransAddr); break;
                    case 31: canPGN.n31(TransType,TransAddr); break;
                    case 32: canPGN.n32(TransType,TransAddr); break;
                    case 33: canPGN.n33(TransType,TransAddr,D[2]); break;//ID
//                    case 39: canPGN.n39(TransType,TransAddr); break; //состояние драйверов устройств
                    case 40: canPGN.n40(TransType,TransAddr); break;
                    default: //PGN неизвестен
                             can.SendMessage((PGNRequest<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
                }
                break;
        case 7: //работа с параметрами во flash
                switch(D[0]) {
                    case MEMORY_1_WRITE_TO_RAM://---------------  1 - запись параметра
                            numb = (D[2]<<8)+D[3];
                            switch(numb){
                                case 15:
                                    // preheater setpoint
                                    value = (D[4]<<24)+(D[5]<<16)+(D[6]<<8)+D[7];
                                    min = setting.getVal((char*)PREHEATER_SETPOINT_LOW);
                                    max = setting.getVal((char*)PREHEATER_SETPOINT_HIGH);
                                    if ((value <= max) && (value >= min)){
                                        setting.setVal((char*)PREHEATER_SETPOINT_T, (int8_t)value);
                                    }
                                    can.SendMessage(ID,MEMORY_4_ANSWER,D[0],D[2],D[3],D[4],D[5],D[6],D[7]);
                                    break;
                                case 16:
                                    // warm up setpoint
                                    value = (D[4]<<24)+(D[5]<<16)+(D[6]<<8)+D[7];
                                    min = setting.getVal((char*)WARM_UP_SETPOINT_LOW);
                                    max = setting.getVal((char*)WARM_UP_SETPOINT_HIGH);
                                    if ((value <= max) && (value >= min)){
                                        setting.setVal((char*)WARM_UP_SETPOINT_T, (int8_t)value);
                                    }
                                    can.SendMessage(ID,MEMORY_4_ANSWER,D[0],D[2],D[3],D[4],D[5],D[6],D[7]);
                                    break;
                                case 20:
                                    value = (D[4]<<24)+(D[5]<<16)+(D[6]<<8)+D[7];
                                    min = 0;
                                    max = 1;
                                    if ((value <= max) && (value >= min)){
                                        setting.setVal((char*)SIGNALING_IN_ON, (int8_t)(value));
                                    }
                                    can.SendMessage(ID,MEMORY_4_ANSWER,D[0],D[2],D[3],D[4],D[5],D[6],D[7]);
                                    break;
                                case 76:
                                    // furnace setpoint
                                    value = (D[4]<<24)+(D[5]<<16)+(D[6]<<8)+D[7];
                                    min = 0;
                                    max = 100;
                                    if ((value <= max) && (value >= min)){
                                        setting.setVal((char*)FURNACE_SETPONT_T, (int8_t)value);
                                    }
                                    can.SendMessage(ID,MEMORY_4_ANSWER,D[0],D[2],D[3],D[4],D[5],D[6],D[7]);
                                    break;
                                default:
                                    can.SendMessage(ID,MEMORY_5_ANSWER,D[0],D[2],D[3],0xFF,0xFF,0xFF,0xFF);
                            }
                            break;
                    case MEMORY_3_READ://-----------------------  3 - чтение параметра по номеру
                            //чтение параметра по принятому имени (номеру)
                            numb = (D[2]<<8)+D[3];
                            switch(numb){
                                case 15:
                                    // preheater setpoint
                                    V32 = setting.getVal((char*)PREHEATER_SETPOINT_T);
                                    can.SendMessage(ID,MEMORY_4_ANSWER,D[0],D[2],D[3],V32>>24,V32>>16,V32>>8,V32);
                                    break;
                                case 16:
                                    // warm up setpoint
                                    V32 = setting.getVal((char*)WARM_UP_SETPOINT_T);
                                    can.SendMessage(ID,MEMORY_4_ANSWER,D[0],D[2],D[3],V32>>24,V32>>16,V32>>8,V32);
                                    break;
                                case 20:
                                    V32 = setting.getVal((char*)SIGNALING_IN_ON);
                                    can.SendMessage(ID,MEMORY_4_ANSWER,D[0],D[2],D[3],V32>>24,V32>>16,V32>>8,V32);
                                    break;
                                case 76:
                                    // furnace setpoint
                                    V32 = setting.getVal((char*)FURNACE_SETPONT_T);
                                    can.SendMessage(ID,MEMORY_4_ANSWER,D[0],D[2],D[3],V32>>24,V32>>16,V32>>8,V32);
                                    break;
                                default:
                                    can.SendMessage(ID,MEMORY_5_ANSWER,D[0],D[2],D[3],0xFF,0xFF,0xFF,0xFF);
                            }
                            break;
                    default:can.SendMessage(ID,MEMORY_5_ANSWER,D[0],D[2],D[3],0xFF,0xFF,0xFF,0xFF);
                }
                break;
        case 8://работа с ЧЯ
                if (measure.getVal((char*)HTR_STAGE) == 'Z') {                 
                    if ((D[0]&48)==0) {                                     //общие данные ЧЯ
                        switch(D[0]&0x0F) {
                            case MEMORY_0_ERASE://стереть данные
                                recorder.clearCounters();
                                break;
                            case MEMORY_3_READ://чтение данных
                                StartPair=(D[4]<<8)+D[5];                   //первое 2х байтовое значение - номер пары [имя:значение]
                                NumPair  =(D[6]<<8)+D[7];                   //второе 2х байтовое значение - количество пар [имя:значение] для чтения
                                for (uint16_t i=0; i<NumPair; i++) {
                                    uint32_t ReadAddr=CNT_START_ADDRESS+StartPair*8+i*8;
                                    if (ReadAddr > CNT_END_ADDRESS-8) break;
                                    uint32_t VN = recorder.ReadFlash(ReadAddr);
                                    if (VN==0xFFFFFFFF) break;
                                    uint32_t VV = recorder.ReadFlash(ReadAddr+4);
                                    if (VV==0xFFFFFFFF) break;
                                    can.SendMessage(ID,MEMORY_4_ANSWER,VN>>16,VN>>8,VN,VV>>24,VV>>16,VV>>8,VV);
                                }
                                break;
                            case MEMORY_6_READ://чтение единичного параметра (из paramsname)
                                //V32=BBCommon.ReadFromRAM((D[4]<<8)+D[5]);//считали параметр
                                //can.SendMessage(ID,MEMORY_4_ANSWER,0xFF,D[4],D[5],V32>>24,V32>>16,V32>>8,V32);
                                break;
                                
                        }
                    }
                    if ((D[0]&48)==16) {                                             //неисправности ЧЯ
                        switch(D[0]&0x0F) {
                            case MEMORY_0_ERASE://стереть данные
                                for (uint8_t i=0; i<REC_NUMB_PAGES; i++) recorder.eraseFaultPage(i);
                                break;
                            case MEMORY_3_READ: //чтение данных
                                uint16_t StartPair=(D[4]<<8)+D[5];                   //первое 2х байтовое значение - номер пары [имя:значение]
                                uint16_t NumPair  =(D[6]<<8)+D[7];                   //второе 2х байтовое значение - количество пар [имя:значение] для чтения
                                for (uint16_t i=0; i<NumPair; i++) {
                                    uint32_t ReadAddr = REC_START_ADDRESS+StartPair*8+i*8;
                                    if (ReadAddr > (REC_END_ADDRESS-8)) break;
                                    uint32_t VN=recorder.ReadFlash(ReadAddr);
                                    uint32_t VV=recorder.ReadFlash(ReadAddr+4);
                                    can.SendMessage(ID,MEMORY_4_ANSWER,VN>>16,VN>>8,VN,VV>>24,VV>>16,VV>>8,VV);
                                }
                                break;
                        }
                    }
                }
                break;
        case 19: //система отопления: состояние кнопок от центрального блока и температура бака
                if (D[0]==1) {
                    if ((D[1]& 3) == 0) TankCtrl.HeaterState=false;
                    if ((D[1]& 3) == 1) TankCtrl.HeaterState=true;
                    if ((D[1]&12) == 0) TankCtrl.PumpState=false;
                    if ((D[1]&12) == 4) TankCtrl.PumpState=true;
                    if ((D[1]&48) == 0) TankCtrl.btnDWater_f=false;
                    if ((D[1]&48) ==16) TankCtrl.btnDWater_f=true;
                                        TankCtrl.Ttank=((int8_t)D[2])-75;
                    if ((D[4]& 3) == 0) TankCtrl.DWaterFlow_f=false;
                    if ((D[4]& 3) == 1) TankCtrl.DWaterFlow_f=true;
                }
                if (D[0]==2) {
                    TankCtrl.Tsetpoint=        ((int16_t)D[1])-75;      //уставка температуры жидкости подогревателя для перехода в ждущий
                    TankCtrl.Tignition=        ((int16_t)D[2])-75;      //уставка температуры жидкости подогревателя для выхода из ждущего
                    TankCtrl.TignitionDW=      ((int16_t)D[3])-75;      //уставка температуры жидкости подогревателя для выхода из ждущего при разборе воды
                    TankCtrl.TtankSetpoint=    ((int16_t)D[4])-75;      //уставка температуры бака для перехода в ждущий
                    TankCtrl.TtankIgnition=    ((int16_t)D[5])-75;      //уставка температуры бака для выхода из ждущего
                    TankCtrl.TtankIgnitionDW=  ((int16_t)D[6])-75;      //уставка температуры бака для выхода из ждущего при разборе воды
                }
                break;
        case 34: //Считать данные из flash по адресу
                if (measure.getVal((char*)HTR_STAGE) == 'Z') { // Ожидание команды
                    ID=(35<<20)+(TransType<<13)+(TransAddr<<10)+(can.idType<<3)+can.idAddress;
                    uint32_t Addr=(D[0]<<24)+(D[1]<<16)+(D[2]<<8)+D[3]; // адрес начала чтения
                    uint16_t size = (D[4]<<8)+D[5]; // размер данных 
                    for (uint16_t i=0; i<size; i++) {
                        V32=*(__IO uint32_t*)Addr;
                        can.SendMessage(ID,Addr>>24,Addr>>16,Addr>>8,Addr,V32>>24,V32>>16,V32>>8,V32);
                        core.delayMs(5);
                        Addr=Addr+4;
                    }
                }
                break;
    }//switch(PGN)
}
//-------------------------------------------------------------------

//-------------------------------------------------------------------
void Messages::ClearFaults(void)
{
    FaultLinkCnt=0;
    FaultLink_f=false;
}
//-------------------------------------------------------------------
void Messages::CheckConnectionEnabled(void)
{
    CheckConnection_f=true;
}
//-------------------------------------------------------------------
void Messages::CheckConnectionDisabled(void)
{
    CheckConnection_f=false;
}
//================================================================
uint32_t Messages::GetParameter(uint16_t ASPN)
{
    
    
             if (ASPN==VAR_FAULT_CODE      ) { return measure.getVal((char*)HTR_FAULT_CODE);}
        else if (ASPN==VAR_STAGE           ) { return correctMode.stage;}
        else if (ASPN==VAR_MODE            ) { return correctMode.mode;}
        else if (ASPN==VAR_RUNNING_TIME    ) { return measure.getVal((char*)TIME_WORK);}
        else if (ASPN==VAR_MODE_TIME       ) { return measure.getVal((char*)TIME_PHASE);}
        else if (ASPN==VAR_U_SUPPLY        ) { return (uint32_t)(measure.getVal((char*)HTR_VOLTAGE)*100); }
        else if (ASPN==VAR_T_FLAME         ) { return core.getTSvalue(PURP_T_FLAME);}
        else if (ASPN==VAR_T_LIQUID        ) { return core.getTSvalue(PURP_T_LIQUID);}
        else if (ASPN==VAR_T_OVERHEAT      ) { return core.getTSvalue(PURP_T_OVERHEAT);}
        else if (ASPN==VAR_FP_FREQ_DEFINED ) { return (uint16_t)(fuelPump.frequencyDefine*100);}
        else if (ASPN==VAR_FP_FREQ_REALIZED) { return (uint16_t)(fuelPump.frequencyDefine*100);}
        else if (ASPN==VAR_FAN_REV_DEFINED ) { return fan.revolutionDefine;}
        else if (ASPN==VAR_FAN_REV_MEASURED) { return fan.revolution;}
        else if (ASPN==VAR_GP_PERIOD       ) { return 4096;}
        else if (ASPN==VAR_GP_PULSE        ) { return 4096*glowPlug.percentDefine/100;}
        else if (ASPN==VAR_GP_PERCENT      ) { return glowPlug.percentDefine;}
        else if (ASPN==VAR_FLAME_CALIBRATION){ return parameter.getVal((char*)CALIBRATION_VALUE_TC1);} 

        else if (ASPN==VAR_FLAME_LIMIT     ) { return measure.getVal((char*)FLAME_BREAK_BORDER);}
        else if (ASPN==VAR_FLAME_BREAK_NUM ) { return measure.getVal((char*)COUNTER_RETRY_STARTUP);}
        else if (ASPN==VAR_PUMP_STATE      ) { return pump.isOn;}
        else if (ASPN==VAR_ADC_CH0         ) { return adc.AdcAverVals[0];}
        else if (ASPN==VAR_ADC_CH1         ) { return adc.AdcAverVals[1];}
        else if (ASPN==VAR_ADC_CH2         ) { return adc.AdcAverVals[2];}
        else if (ASPN==VAR_ADC_CH3         ) { return adc.AdcAverVals[3];}
        else if (ASPN==VAR_ADC_CH4         ) { return adc.AdcAverVals[4];}
        else if (ASPN==VAR_ADC_CH5         ) { return adc.AdcAverVals[5];}
        else if (ASPN==VAR_ADC_CH6         ) { return adc.AdcAverVals[6];}

        return 0xFFFFFFFF;
}
//============================================================================
