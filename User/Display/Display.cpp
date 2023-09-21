//
//    Модуль связи блока по последовательному каналу USART
//    

/* Includes ------------------------------------------------------------------*/
#include "Display.h"
#include "core.h"
#include "LedDriver.h"

TDisplay display;
//-----------------------------------------------------

//-----------------------------------------------------
void TDisplay::initialize(void)
{
    ledDriver.initialize();
    BlinkPeriod = DEFAULT_BLINK_PERIOD;
    for(uint8_t i=0; i<SEG_NUMB; i++) {
        BlinkSegSet[i] = 0; // моргание отключено
        SegState[i] = 1; // элемент включен
    }

}
//-----------------------------------------------------
void TDisplay::handler(void)
{
    static uint32_t timeBlink = 0;
    static uint32_t timeUpdate = 0;
    uint8_t data[SEG_NUMB];
    
    ledDriver.handler();
    
    if ((core.getTick() - timeBlink) >= (BlinkPeriod/2)) {
        timeBlink = core.getTick();
        for(uint8_t i=0; i<SEG_NUMB; i++) {
            if (BlinkSegSet[i] == 1) { // если нужно моргать этим элементом
                if (SegState[i] == 1) SegState[i] = 0; // меняем состояние на противоположное
                else SegState[i] = 1;
            } else {
                SegState[i] = 1; // элемент включен
            }
        }
    }
    if ((core.getTick() - timeUpdate) >= UPDATE_PERIOD) {
        timeUpdate = core.getTick();
        for(uint8_t i=0; i<SEG_NUMB; i++) {
            if (SegState[i] == 1) { // элемент включен
                data[i] = ViewData[i]; // отображаем его символ
            } else { // элемент выключен
                data[i] = 0; // ничего не показываем
            }
        }
        ledDriver.WriteDisplay(data);
    }

}
//-----------------------------------------------------
void TDisplay::Off(void)
{
    ledDriver.DisplayOff();
}
//-----------------------------------------------------
void TDisplay::viewON(void)
{
    for(uint8_t i=0; i<SEG_NUMB; i++) ViewData[i] = 0;
    ViewData[0] = digitToSegment[0];
    ViewData[1] = digitToSegment[17];
}
//-----------------------------------------------------
void TDisplay::viewBlack(void)
{
    for(uint8_t i=0; i<SEG_NUMB; i++) ViewData[i] = 0;
}
//-----------------------------------------------------
void TDisplay::viewError(uint8_t err)
{
    for(uint8_t i=0; i<SEG_NUMB; i++) ViewData[i] = 0;
    ViewData[0] = digitToSegment[14];
    ViewData[1] = digitToSegment[19];
    ViewData[2] = digitToSegment[err/10];
    if (ViewData[2] == digitToSegment[0]) ViewData[2] = 0;
    ViewData[3] = digitToSegment[err%10];
}
//-----------------------------------------------------
void TDisplay::viewLock(void)
{
    for(uint8_t i=0; i<SEG_NUMB; i++) ViewData[i] = 0;
    ViewData[0] = digitToSegment[21]; // L
    ViewData[1] = digitToSegment[23]; // o
    ViewData[2] = digitToSegment[12]; // c
    ViewData[3] = 0;
}
//-----------------------------------------------------
void TDisplay::viewLockOff(void)
{
    ViewData[0] = digitToSegment[21]; // L
    ViewData[1] = digitToSegment[0]; //  O
    ViewData[2] = digitToSegment[15]; // f
    ViewData[3] = digitToSegment[15]; // f
}
//-----------------------------------------------------
void TDisplay::viewTimerOn(void)
{
    ViewData[0] = digitToSegment[18]; // t
    ViewData[1] = digitToSegment[0]; //  O
    ViewData[2] = digitToSegment[17]; // n
    ViewData[3] = 0;
}
//-----------------------------------------------------
void TDisplay::viewTimerOff(void)
{
    ViewData[0] = digitToSegment[18]; // t
    ViewData[1] = digitToSegment[0]; //  O
    ViewData[2] = digitToSegment[15]; // f
    ViewData[3] = digitToSegment[15]; // f
}
//-----------------------------------------------------
void TDisplay::viewPower(uint8_t pwr, bool timerState)
{
    for(uint8_t i=0; i<SEG_NUMB; i++) ViewData[i] = 0;
    pwr++; // показываем мощность 1-10
    if (timerState) ViewData[0] = digitToSegment[18];
    else ViewData[0] = 0;
    ViewData[1] = digitToSegment[20];
    ViewData[2] = digitToSegment[pwr/10];
    if (ViewData[2] == digitToSegment[0]) ViewData[2] = 0;
    ViewData[3] = digitToSegment[pwr%10];
}
//-----------------------------------------------------
void TDisplay::viewTimer(uint16_t time)
{
    for(uint8_t i=0; i<SEG_NUMB; i++) ViewData[i] = 0;
    ViewData[0] = digitToSegment[18];
    ViewData[1] = digitToSegment[time/100];
    time = time%100;
    ViewData[2] = digitToSegment[time/10];
    ViewData[3] = digitToSegment[time%10];
    for(uint8_t i=1; i<3; i++) { // delete leading zero
        if (ViewData[i] == digitToSegment[0]) ViewData[i] = 0;
        else break;
    }
}
//-----------------------------------------------------
void TDisplay::viewTemp(int16_t temp)
{
    bool viewMinus = false;
    uint8_t data[SEG_NUMB];
    for(uint8_t i=0; i<SEG_NUMB; i++) data[i] = 0;
    if (temp < 0) {  
        viewMinus = true;
        temp = -temp;
    }
    data[1] = digitToSegment[temp/100];
    temp = temp%100;
    data[2] = digitToSegment[temp/10];
    data[3] = digitToSegment[temp%10];
    
    for(uint8_t i=1; i<3; i++) { // delete leading zero
        if (data[i] == digitToSegment[0]) data[i] = 0;
        else break;
    }
    if (viewMinus) {
        for(int8_t i=SEG_NUMB; i>0; i--) { //
            if (data[i-1] == 0) {
                data[i-1] = digitToSegment[16]; // вставляем минус
                break;
            }
        }
    }
    for(uint8_t i=0; i<SEG_NUMB; i++) ViewData[i] = data[i];

}
//-----------------------------------------------------
void TDisplay::viewOff(void)
{
    for(uint8_t i=0; i<SEG_NUMB; i++) ViewData[i] = 0;
    ViewData[0] = digitToSegment[0];
    ViewData[1] = digitToSegment[15];
    ViewData[2] = digitToSegment[15];
}
//-----------------------------------------------------
void TDisplay::Test(void)
{
    for(uint8_t i=0; i<SEG_NUMB; i++) ViewData[i] = 0xff;
}
//---------------------------------------------
void TDisplay::SetBlink(uint8_t blnkSegNumb, uint16_t period)
{
    BlinkSegSet[blnkSegNumb] = 1; // будет моргать элемент с номером позиции blnkSegNumb
    BlinkPeriod = period; // и с периодом
}
//---------------------------------------------
void TDisplay::SetBlink(uint8_t blnkStartSeg, uint8_t blnkSegNumb, uint16_t period)
{
    if (blnkStartSeg > 0) blnkStartSeg--;
    uint8_t blnkEndSeg = blnkStartSeg + blnkSegNumb;
    for(uint8_t i=blnkStartSeg; i<blnkEndSeg; i++) {
        BlinkSegSet[i] = 1; // будет моргать элемент с номером позиции blnkSegNumb
    }
    BlinkPeriod = period; // и с периодом
}
//---------------------------------------------
void TDisplay::SetBlinkAll(uint16_t period)
{
    SetBlink(1, SEG_NUMB, period);
}
//---------------------------------------------
void TDisplay::BlinkOff(void)
{
    BlinkPeriod = DEFAULT_BLINK_PERIOD;
    for(uint8_t i=0; i<SEG_NUMB; i++) {
        BlinkSegSet[i] = 0; // моргание отключено
        SegState[i] = 1; // элемент включен
    }
}

//---------------------------------------------


//-----------------------------------------------------
