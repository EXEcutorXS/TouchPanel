//
//    Версия протокола обмена 3.1

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "core.h"
#include "protocol_3.h" 
#include "HeaterVars.h"

Protocol3 protocol3;
//-----------------------------------------------------
Protocol3::Protocol3(void)
{
    linkCnt = 0;
    isWaitHeaderByte=1;
    bufferCursorR=0;
}
//-----------------------------------------------------
void Protocol3::initialize(void)
{
    linkCnt = 0;
    isWaitHeaderByte=1;
    bufferCursorR=0;
    RequestsEnable_f = true;
    heater.SetWorkModeBy = 4;
    heater.SetTSetpoint = 30;
    heater.SetWaitModeIsAllowed = 0;
    heater.SetPowerDefined = 4;
    TempCorrect = 273;
}
//-----------------------------------------------------

void Protocol3::handler(void)
{
    static uint32_t timer = 0;
    
    if (RequestsEnable_f) {
        if (core.verProtocol == 3) {
            requests();
            if ((core.getTick() - timer) >= 10){
                timer = core.getTick();
                processTimeOut();
            }   
        }
    }
    processReceivedData();
}
//-----------------------------------------------------
void Protocol3::parsing(void)     // 
{
uint16_t Command;
    
    if (packetIn[1] == 'C') {
        if ((packetIn[2] == 'B') && (packetIn[3] == 'H')  && (packetIn[4] == 't')) { // переход в загрузчик по 4 протоколу
            *(__IO uint32_t*) (0x20000000+0xBFFC) = 0x0016AA55;
            NVIC_SystemReset();
        }
    }
    Command = ((uint16_t)packetIn[3]<<8) + packetIn[4];
    if (packetIn[1]==4) {                                  //принят ответ на команду. обработка.
        if (true) {    // SendedCmd == Command  //ответ именно на ту команду, которую посылали
            SendedCmd=0;
            switch(Command) {
                case 1: // Пуск произошел
                    heater.PanelCommand = 0; // if (heater.PanelCommand == CMD_START) 
                    if (packetIn[2]>=6) { 
                        if ((packetIn[6] > 0)&&(packetIn[6] <= 4)) heater.WorkModeBy = packetIn[7];
                        heater.TSetpoint = packetIn[8];
                        if (packetIn[8] != 0) heater.WaitModeIsAllowed = packetIn[9];
                        if (packetIn[9] < 10) heater.PowerDefined = packetIn[10];
                    }
                    break;
                case 2:// Параметры заданы
                    if (packetIn[2]>=6) { 
                        if ((packetIn[6] > 0)&&(packetIn[6] <= 4)) heater.WorkModeBy = packetIn[7];
                        heater.TSetpoint = packetIn[8];
                        if (packetIn[8] != 0) heater.WaitModeIsAllowed = packetIn[9];
                        if (packetIn[9] < 10) heater.PowerDefined = packetIn[10];
                    }
                    break;
                case 3: // Стоп
                    heater.PanelCommand = 0; // if (heater.PanelCommand == CMD_STOP) 
                    break;
                case 6: // Версия ПО
                    if (packetIn[2]>=4) { 
                        heater.VersionProductType = packetIn[5];
                        heater.VersionVoltage = packetIn[6];
                        heater.VersionProductSubtype = packetIn[7];
                        heater.VersionAsm = packetIn[8];
                    }
                    if (packetIn[2]>=8) { 
                        heater.VersionDay = packetIn[10];
                        heater.VersionMonth = packetIn[11];
                        heater.VersionYear = packetIn[12];
                    }
                    break;
                case 15:
                    if (packetIn[2]>=26) { //длина пакета 26 или более
                        heater.Stage = packetIn[5];
                        heater.Mode = packetIn[6];
                        heater.FaultCode = packetIn[7];
                        heater.Tinlet = packetIn[8];
                        heater.Text = packetIn[9];
                        heater.Usupply = ((int16_t)packetIn[10]<<8)+packetIn[11]; // *10
                        heater.Tflame = (((int16_t)packetIn[12]<<8)+packetIn[13])-TempCorrect;
                        heater.Tframe = (((int16_t)packetIn[14]<<8)+packetIn[15])-TempCorrect;
                        if ((heater.Tflame < -100)&&(heater.Tframe < -100)) {
                            TempCorrect = 0;
                            heater.Tflame = heater.Tflame + 273;
                            heater.Tframe = heater.Tframe + 273;
                        }                      
                        heater.ModePanel = packetIn[16];
                        heater.FanRevDefined = packetIn[17];
                        heater.FanRevMeasured = packetIn[18];
                        heater.FuelPumpFrequencyRealized = ((uint16_t)packetIn[19]<<8)+packetIn[20]; // *100
                        heater.FlameCalibrationValue = ((int16_t)packetIn[22]<<8)+packetIn[23];
                        heater.FrameCalibrationValue = ((int16_t)packetIn[24]<<8)+packetIn[25];
                        heater.WorkModeBy = packetIn[26];
                        heater.TSetpoint = packetIn[27];
                        heater.WaitModeIsAllowed = packetIn[28];
                        heater.PowerDefined = packetIn[29];
                        heater.PowerRealized = packetIn[30];
                    }
                    else {
                        heater.Stage = packetIn[5];
                        heater.Mode = packetIn[6];
                        heater.FaultCode = packetIn[7];
                        heater.Tinlet = packetIn[8];
                        heater.Text = packetIn[9];
                        heater.Usupply = ((int16_t)packetIn[10]<<8)+packetIn[11]; // *10
                        heater.Tflame = ((int16_t)packetIn[12]<<8)+packetIn[13];
                        heater.ModePanel = packetIn[16];
                    }
                    break;
            }

        }
    } else {
        if (Command == 22) { // переход в загрузчик
            if ((packetIn[2] == 2) && (packetIn[5] == 0xAA) && (packetIn[6] == 0x55)) {
                *(__IO uint32_t*) (0x20000000+0xBFFC) = 0x0016AA55;
                NVIC_SystemReset();
            }
        } else
        if (Command == 6) { // ответ на Запрос версии ПО
            uint8_t data[4];
            data[0] = _FIRMWARE_VER_1;
            data[1] = _FIRMWARE_VER_2;
            data[2] = _FIRMWARE_VER_3;
            data[3] = _FIRMWARE_VER_4;           
            sendPacket(4, 4, 6, data);
        }
    }
}
//-----------------------------------------------------
void Protocol3::CmdStart(void)     // 
{
    uint8_t data[8];
    data[0] = 0xff;
    data[1] = 0xff;
    data[2] = heater.SetWorkModeBy;
    data[3] = heater.SetTSetpoint;
    data[4] = heater.SetWaitModeIsAllowed;
    data[5] = heater.SetPowerDefined;
    core.verProtocol = 3;
    sendPacket(3, 6, 1, data);
}
//-----------------------------------------------------
void Protocol3::CmdSendParam(void)     // 
{
    uint8_t data[8];
    data[0] = 0xff;
    data[1] = 0xff;
    data[2] = heater.SetWorkModeBy;
    data[3] = heater.SetTSetpoint;
    data[4] = heater.SetWaitModeIsAllowed;
    data[5] = heater.SetPowerDefined;
    sendPacket(3, 6, 2, data);
}
//-----------------------------------------------------
void Protocol3::CmdStop(void)     // 
{
    uint8_t data[1];
    sendPacket(3, 0, 3, data);
}
//-----------------------------------------------------
void Protocol3::CmdReadParam(void)     // 
{
    uint8_t data[2];
    data[0] = 1;
    data[1] = 1;
    sendPacket(3, 1, 15, data);
}
//-----------------------------------------------------
void Protocol3::requests(void)     // 
{
    static uint32_t Timer1 = 2000;
    static bool sendParam_f = false;
    //static uint32_t Timer2 = 3000;
    
    if ((core.getTick() - Timer1) >= 1000) {
        Timer1 = core.getTick();
        if (sendParam_f) {
            sendParam_f = false;
        } else {
            if (heater.Stage != STAGE_Z) {
                if (heater.SetPowerDefined != heater.PowerDefined) {
                    sendParam_f = true;
                }
            }
        }
        if (heater.PanelCommand == 0) {
            if (sendParam_f) CmdSendParam();
            else CmdReadParam();  // 
            return;
        }
    
    
//    if ((core.getTick() - Timer2) >= 2000) {
//        Timer2 = core.getTick();

        if (heater.PanelCommand == CMD_STOP) {
            if ((heater.Stage != STAGE_Z)&&(heater.Stage != STAGE_F)) {
                CmdStop();
            } else {
                heater.PanelCommand = 0;
            }
        } else 
        if (heater.PanelCommand == CMD_START) {
            if (heater.Stage == STAGE_Z) {
                CmdStart();
            } else {
                heater.PanelCommand = 0;
                CmdSendParam(); // повторно передаем параметры
            }
        }


    }
}
//-----------------------------------------------------
void Protocol3::sendPacket(uint8_t code, uint8_t dataLen, uint16_t cmd, uint8_t* data)
{
    SendedCmd = cmd;
    packetOut[0] = PACKET_HEADER;
    packetOut[1] = code; // 
    packetOut[2] = dataLen; 
    packetOut[3] = (uint8_t)(cmd >> 8)&0xff;
    packetOut[4] = (uint8_t)cmd&0xff;
    for(uint16_t i=0; i<dataLen; i++) {
        packetOut[i+5] = data[i]; 
    }
    uint16_t crcTemp = calculateCRC(packetOut, dataLen+5);
    packetOut[dataLen+5] = (crcTemp >> 8) & 0xff;
    packetOut[dataLen+6] = crcTemp & 0xff;
    usart.startTransmission(packetOut, dataLen+7);
    
}
//----------------------------------------------------
//прием данных по последовательному каналу от пульта
//вызов ставится в main.
void Protocol3::processReceivedData(void)
{
    uint8_t AByte;
    uint16_t CRCinPacket;
    
    uint16_t nb = usart.GetReceiveBufferPos(); // число принятых байт
    while(bufferCursorR != nb) {
        packetTimeOut=0;
        AByte=usart.GetByte(bufferCursorR++);
        if (bufferCursorR >= usart.BUFFER_SIZE) {
            bufferCursorR=0;
        }
        if (!isWaitHeaderByte) {             //заголовочный байт принят. принимаем пакет
            packetIn[packetCounter]=AByte;
            if (packetCounter==2) {                   //получили размер поля данных             
                if (packetIn[1] > 7) lengthPacketIn = 15;       // protocol4
                else lengthPacketIn=6+AByte;              //посчитали размер принимаемого пакета.
            }
            if (packetCounter <= (lengthPacketIn-2)) { //считаем контрольную сумму на лету
                calculateCRC(AByte);
            }
            if (packetCounter >= lengthPacketIn) {
                if (packetIn[1] > 7) {
                    CRCinPacket = (packetIn[packetCounter]<<8)+packetIn[packetCounter-1]; // CRC for protocol4
                } else {
                    CRCinPacket = (packetIn[packetCounter-1]<<8)+packetIn[packetCounter];
                }
                    
                if (crc == CRCinPacket) {   //контрольная сумма совпадает.
                    parsing();              //обрабатываем пакет
                    linkCnt=0;
                    core.isLinkError=false; 
                    core.verProtocol = 3; // протокол версии 3
                }
                isWaitHeaderByte = true;
                break;
            }
            
            packetCounter++;
        }
        else {                                        //заголовка не было
            if (AByte==PACKET_HEADER) {             //принят заголовок пакета
                isWaitHeaderByte=0;
                lengthPacketIn = 6;
                packetCounter=0;
                packetIn[packetCounter++]=AByte;
                crc=0xFFFF;
                calculateCRC(AByte);
            }
        }
    }
}
//---------------------------------------------------------
uint16_t Protocol3::calculateCRC(uint8_t* buf, uint16_t size)
{
    uint8_t inByte = 0;
    uint16_t clCRC = 0xffff;
    for(uint16_t i=0;i<size;i++) {
        inByte = buf[i];
        uint8_t bt = 0;
        for(uint8_t j=0;j<8;j++) {
            bt=clCRC & 1;
            clCRC=clCRC >> 1;
            if ((inByte & 1) != bt) clCRC=clCRC^0xA001;
            inByte = inByte >> 1;
        }
    }
    return clCRC; 
}
//--------------------------------------------
void Protocol3::calculateCRC(uint8_t inByte)
{
    uint8_t bt = 0;
    for(uint8_t j=1;j<=8;j++) {
        bt=crc & 1;
        crc=crc >> 1;
        if ((inByte & 1) != bt) crc=crc^0xA001;
        inByte = inByte >> 1;
    }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//счетчик таймаута. вызов ставится в обработчик прерывания Таймера0
void Protocol3::processTimeOut(void)
{
    if (isWaitHeaderByte==false) {
        if (packetTimeOut<50) packetTimeOut++;
        if (packetTimeOut==50) {
            isWaitHeaderByte=true;
        }
    } else {
        if (heater.Stage != STAGE_Z) {
            linkCnt++;
            if (linkCnt > 3000) { // 30 s
                core.isLinkError=true;
                linkCnt = 0;
            }
        } else {
            linkCnt = 0;
        }
    }

}
//-----------------------------------------------------
