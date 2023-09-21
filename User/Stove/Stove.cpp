//
//    ћодуль св€зи блока по последовательному каналу USART
//    

/* Includes ------------------------------------------------------------------*/
#include "Stove.h"
#include "core.h"
#include "CapSensor.h"
#include "protocol_3.h" 
#include "HeaterVars.h"
#include "Buzzer.h"
#include "Display.h"
#include "LedLine.h"

TStove stove;
//-----------------------------------------------------

//-----------------------------------------------------
void TStove::initialize(void)
{
    SetState(STANDBY);
    TripleBeep();
    ManualClearError_f = false;
    seconds = 0;
    TimerMinutes = 60;
    TimerOn_f = 0;
    //sleep_f = false;
}
//-----------------------------------------------------
void TStove::handler(void)
{
    static uint32_t time = 1000;
    
    if (((core.getTick()-time) >= 500)||(sensor.AnyPressed)) {
        time = core.getTick();
        
        if (State == STANDBY) {
            if (sensor.SensState[SENS_ON] == LONG_PRESS) {               
                heater.PanelCommand = CMD_START;
                TimerDisable();
                protocol3.RequestsEnable_f = true;
                Locked_f = false;
                TripleBeep();
                SetState(ON);
                ManualClearError_f = false;
            }
            if ((core.getTick() - BeginStateTime) > ON_STATE_TIMEOUT) {
                if (heater.Stage != STAGE_Z) SetState(ON);
            }        
            if ((core.getTick() - BeginStateTime) > TOSLEEP_TIMEOUT) {
                protocol3.RequestsEnable_f = false;
            }
        } else
        if (State == TEST) {
            if ((core.getTick() - BeginStateTime) > TEST_STATE_TIMEOUT) {
                SetState(STANDBY);
            }
        } else
        if (State == OFF) {
//            if ((core.getTick() - BeginStateTime) > OFF_STATE_TIMEOUT) { 
//                SetState(STANDBY);
//            }
            if ((core.getTick() - BeginStateTime) > CHECK_STATE_TIME) { 
                if (heater.Stage == STAGE_Z) {
                    SetState(STANDBY);
                } else
                if (heater.Stage != STAGE_F) {
                    heater.PanelCommand = CMD_STOP;
                    BeginStateTime = core.getTick();
                } 
            }
            if ((core.getTick() - BeginStateTime) > OFF_STATE_TIMEOUT) { 
                if (heater.Stage == STAGE_F) {
                    if (sensor.SensState[SENS_ON] == LONG_PRESS) {   
                        if ((sensor.SensState[SENS_PLUS] == NO_PRESS)&&(sensor.SensState[SENS_MINUS] == NO_PRESS)) {                        
                            heater.PanelCommand = CMD_START;
                            TimerDisable();
                            protocol3.RequestsEnable_f = true;
                            Locked_f = false;
                            TripleBeep();
                            SetState(ON);
                            ManualClearError_f = false;
                        }
                    }
                }
            }

        } else
        if (State == ERROR) {
            if (heater.FaultCode == 0) SetState(POWER);
            if (sensor.SensState[SENS_ON] == LONG_PRESS) {
                heater.FaultCode = 0;
                ManualClearError_f = true;
                //core.isLinkError = false;
                SetState(STANDBY);
                TripleBeep();
            }
            
        } else 
        if (State == ON) {
            if ((core.getTick() - BeginStateTime) > ON_STATE_TIMEOUT) {
                SetState(POWER);
            }
        } else 
        if (State == POWER) {
            if ((sensor.SensState[SENS_ON] == LONG_PRESS)&&(sensor.SensState[SENS_MINUS] == LONG_PRESS)&&(sensor.SensState[SENS_PLUS] == NO_PRESS)) {
                Locked_f = !Locked_f;
                sensor.ClearSensors();
                //DoubleBeep();
                buzzer.StartBeep(LOC_BEEP_TIME);
                if (Locked_f) SetState(LOCK);
                else SetState(LOCK_OFF);
            }  
            if (!Locked_f) {
                if (sensor.SensState[SENS_PLUS] == SHORT_PRESS) {
                    if (heater.SetPowerDefined < 9) {
                        heater.SetPowerDefined++;
                        buzzer.StartBeep(SHORT_BEEP_TIME);
                    }
                    SetState(POWER);
                }
                if (sensor.SensState[SENS_MINUS] == SHORT_PRESS) {
                    if (heater.SetPowerDefined > 0) {
                        heater.SetPowerDefined--;
                        buzzer.StartBeep(SHORT_BEEP_TIME);
                    }
                    SetState(POWER);
                }              
                if (sensor.SensState[SENS_ON] == SHORT_PRESS) {
                    if (!TimerOn_f) {
                        if ((TimerMinutes <= 10)||((TimerMinutes%5) != 0)) TimerMinutes = 60;
                    }
                    SetState(TIMER);
                    buzzer.StartBeep(SHORT_BEEP_TIME);
                }
                if (sensor.SensState[SENS_ON] == LONG_PRESS) {
                    if ((sensor.SensState[SENS_PLUS] == NO_PRESS)&&(sensor.SensState[SENS_MINUS] == NO_PRESS)) {
                        TripleBeep();
                        sensor.ClearSensors();
                        heater.PanelCommand = CMD_STOP;
                        SetState(OFF);
                    }
                }
            } else {
                if ((sensor.SensState[SENS_ON] == SHORT_PRESS)||(sensor.SensState[SENS_PLUS] == SHORT_PRESS)
                    ||(sensor.SensState[SENS_MINUS] == SHORT_PRESS)||((sensor.SensState[SENS_ON] == LONG_PRESS)&&(sensor.SensState[SENS_PLUS] == NO_PRESS))) {
                    DoubleBeep();
                    SetState(LOCK);
                }
            }

        } else 
        if (State == TIMER) {
            if (sensor.SensState[SENS_PLUS] == SHORT_PRESS) {
                if (TimerMinutes < MAX_TIMER_TIME) {
                    TimerMinutes += 5;
                    buzzer.StartBeep(SHORT_BEEP_TIME);
                }
                if (TimerMinutes > MAX_TIMER_TIME) TimerMinutes = MAX_TIMER_TIME;
                SetState(TIMER);
            }
            if (sensor.SensState[SENS_MINUS] == SHORT_PRESS) {
                if (TimerMinutes > MIN_TIMER_TIME) {
                    TimerMinutes -= 5;
                    buzzer.StartBeep(SHORT_BEEP_TIME);
                }
                if (TimerMinutes < MIN_TIMER_TIME) TimerMinutes = MIN_TIMER_TIME;  
                SetState(TIMER);
            }  
            if (sensor.SensState[SENS_ON] == SHORT_PRESS) {  
                if (TimerOn_f) {
                    SetState(TIMER_OFF);
                    TimerDisable();
                } else {
                    SetState(TIMER_ON);
                    TimerEnable(TimerMinutes);
                }
                buzzer.StartBeep(TIMERON_BEEP_TIME);
            }
            if ((core.getTick() - BeginStateTime) > TIMER_PRESS_TIMEOUT) {
                SetState(POWER);
            }
        } else
        if (State == TIMER_ON) {
            if ((core.getTick() - BeginStateTime) > POWER_STATE_TIMEOUT) {
                SetState(POWER);
                //display.SetBlink(0, 1000);
            }
        } else
        if (State == TIMER_OFF) {
            if ((core.getTick() - BeginStateTime) > POWER_STATE_TIMEOUT) {
                SetState(POWER);
                //display.BlinkOff();
            }
        } else
        if (State == LOCK) {
            if ((core.getTick() - BeginStateTime) > LOCK_STATE_TIMEOUT) {
                SetState(POWER);
            }
        } else 
        if (State == LOCK_OFF) {
            if ((core.getTick() - BeginStateTime) > LOCK_STATE_TIMEOUT) {
                SetState(POWER);
            }
        }
        if (State != STANDBY) {
            if ((core.getTick() - BeginStateTime) > ON_STATE_TIMEOUT) {
                if (heater.Stage == STAGE_Z) {
                    SetState(STANDBY); // 
                } else
                if (heater.Stage == STAGE_F) {
                    if (State != OFF) SetState(OFF);
                } 
                if (core.isLinkError) heater.FaultCode = 30;
                if ((heater.FaultCode != 0) && (!ManualClearError_f)) { // ошибка при работе  и очистка ошибки пользователем
                    if (State != ERROR) {
                        SetState(ERROR);
                    }          
                }
            }        
        }
        
        UpdateDisplay();
        sensor.SensState[SENS_ON] = NO_PRESS;
        sensor.SensState[SENS_PLUS] = NO_PRESS;
        sensor.SensState[SENS_MINUS] = NO_PRESS;
        sensor.AnyPressed = false;
    }

    TimerHandler();
}
//---------------------------------------------
void TStove::TimerHandler(void)
{
    static uint32_t tickTimer = 5000;
    
    if ((core.getTick() - tickTimer) > 1000) {
        tickTimer = core.getTick();
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            if (TimerOn_f) {
                if (TimerMinutes > 0) TimerMinutes--;
                if (TimerMinutes == 0) {
                    // завершение работы
                    TimerOn_f = false;
                    SetState(OFF);
                    heater.PanelCommand = CMD_STOP;
                }
            }
            
        }
    }
}
//---------------------------------------------
void TStove::TimerEnable(uint16_t min)
{
    TimerMinutes = min;
    TimerOn_f = true;
    seconds = 0;
}
//---------------------------------------------
void TStove::TimerDisable(void)
{
    TimerOn_f = false;
    TimerMinutes = 60;
}
//---------------------------------------------
void TStove::TripleBeep(void)
{
    buzzer.StartSeriesBeep(SHORT_BEEP_TIME, 200, 3);
}
//---------------------------------------------
void TStove::DoubleBeep(void)
{
    buzzer.StartSeriesBeep(SHORT_BEEP_TIME, 200, 2);
}
//---------------------------------------------
void TStove::UpdateDisplay(void)
{
    switch(State) {
        case ON:
            display.viewON();           
            break;
        case OFF:
            display.viewOff();
            break;
        case ON_BLINK:
            display.viewON();
            display.SetBlinkAll(OFF_BLINK_PERIOD);
            break;
        case OFF_BLINK:
            display.viewOff();
            display.SetBlinkAll(OFF_BLINK_PERIOD);
            break;
        case POWER:
            display.viewPower(heater.SetPowerDefined, TimerOn_f);
            break;
        case TIMER:
            display.viewTimer(TimerMinutes);
            break;
        case TIMER_ON:
            display.viewTimerOn();
            break;
        case TIMER_OFF:
            display.viewTimerOff();
            break;
        case ERROR:
            display.viewError(heater.FaultCode);
            display.SetBlinkAll(ERROR_BLINK_PERIOD);
            break;
        case TEST:
            display.Test();
            break;
        case LOCK:
            display.viewLock();
            break;
        case LOCK_OFF:
            display.viewLockOff();
            break;
        case STANDBY:
            display.BlinkOff();
            display.viewBlack();
            display.Off();
            break;
        default:
            break;
    }
}
//---------------------------------------------
void TStove::SetState(uint8_t st)
{
    State = st;
    BeginStateTime = core.getTick();
    sensor.ClearSensors();
    
        switch(State) {
        case ON:
            display.BlinkOff(); 
            LedLine.On();      
            break;
        case OFF:
            display.BlinkOff();
            break;
        case ON_BLINK:

            break;
        case OFF_BLINK:
            break;
        case POWER:
            if (TimerOn_f) display.SetBlink(0, 1000);
            else display.BlinkOff();
            break;
        case TIMER:
            display.BlinkOff();
            break;
        case TIMER_ON:
            display.BlinkOff();
            break;
        case TIMER_OFF:
            display.BlinkOff();

            break;
        case ERROR:
            display.viewError(heater.FaultCode);
            display.SetBlinkAll(ERROR_BLINK_PERIOD);
            break;
        case TEST:
            
            break;
        case LOCK:
            display.BlinkOff();
            break;
        case LOCK_OFF:
            display.BlinkOff();
            display.viewLockOff();
            break;
        case STANDBY:
            display.BlinkOff();
            display.viewBlack();
            display.Off();
            LedLine.Off();  
            break;
        default:
            break;
    }
    
}
//---------------------------------------------


//-----------------------------------------------------
