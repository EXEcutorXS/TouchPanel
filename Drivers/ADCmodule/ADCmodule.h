/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H
#define __ADC_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"

/* Defines ------------------------------------------------------------------*/
#define NUMB_OF_DMA_CYCLE   16
#define NUMB_OF_CHAN   9                    // количество измеряемых каналов
#define LEN_ARRAY_DMA   NUMB_OF_CHAN*NUMB_OF_DMA_CYCLE      // длина массива для DMA

#define NUMB_COULD_AVG   20
//------------------------------------------------------------------------------------
// таблица сопротивлений B57332V5103F360 Rt (номинал 10 КОм)  от -55 до 115 *С с шагом 5*
//                           -55     -50      -45     -40     -35     -30    -25     -20     -15     -10     -5      0       5       10      15      20     25
const float NTC_table[] = { 446.05, 332.81, 250.44, 190.03, 145.36, 112.06, 87.041, 68.104, 53.665, 42.576, 34.001, 27.326, 22.096, 17.973, 14.703, 12.093, 10,
//                            30       35      40      45      50      55      60     65      70      75      80      85     90      95       100     105      110      115
                            8.3113, 6.9418, 5.8255, 4.9112, 4.1587, 3.5365, 3.0197, 2.5888, 2.2278, 1.9243, 1.6681, 1.451, 1.2663, 1.1088, 0.97381, 0.85788, 0.75795, 0.67155
};
// таблица сопротивлений MF52A103F3950 Rt (номинал 10 КОм)  от -55 до 115 *С с шагом 5*
//                           -55     -50      -45     -40     -35     -30    -25     -20     -15     -10     -5      0       5       10      15      20     25
const float NTC_table2[] = { 500.05, 400.81, 340.44, 280.66, 223.84, 159.78,116.02, 87.261, 67.214, 52.35, 40.929, 32.049, 25.149, 19.856, 15.677, 12.485, 10,
//                            30       35      40      45      50      55      60     65      70      75      80      85     90      95       100     105      110      115    120    125    130
                             8.06,   6.533, 5.325,   4.361,  3.59,   2.968,  2.467,  2.06,  1.728,  1.457,  1.235,  1.052, 0.899,   0.772,   0.666,  0.575,   0.499,   0.433,  0.377, 0.329, 0.286
};
//-----------------------------------------------------------------------------------
enum
{
    chTempMOSFET,               // Датчик температуры транзисторов
    chTC1,                      // Температура термопары 1
//    chCur,                    // измеритель тока (в отдельном канале АЦП)
    chTS3,                      // Температура датчика 3 (внешний датчик/ перегрев) MCU_exttemp/MCU_Temp
    chTS2,                      // Температура датчика 2 (входящий воздух/ жидкость) MCU_OverTemp/MCU_IntTemp  
//    chVolt,                   // напряжение питания
    chTC2,                      // Tcoupl2           (в отдельном канале АЦП)
    chAux1,                     // Вход фотодиода (резерв)
    chTS1,                      // Температура датчика 1  (температура выхода) MCU_AUX_ADC2         
    chTcpu,                     // Температура процессора
    chURef,                     // Опорное напряжение для АЦП
};

/* Classes ------------------------------------------------------------------*/
class ADCmodule
{
   
    public:
        ADCmodule(void);

        void initialize(void);
        void enableADC(void);
        void disable(void);
        void enable(void);
        void handler(void);
        void calibrationTc(void);
        void Averaging(void); // осреднение значений, накопленных в буфере DMA

        uint16_t AdcVals[NUMB_OF_CHAN];          // буфер для каналов
        uint16_t AdcAverVals[NUMB_OF_CHAN];          // буфер для средних значений каналов
        uint16_t bufDMA[LEN_ARRAY_DMA];             // Буфер для DMA 
        float ion;
        float TempBoard;
        float coldT;
        float ts1, ts2, ts3;
    

    private:
        //float calcKTY(float u);              // функция расчета температуры для KTY
        //float calcLM(float u);               // функция расчета температуры для LM
        void  checkFault(void);              // ПРОВЕРКА ИЗМЕРЯЕМЫХ ВЕЛИЧИН НА ОШИБКУ
        float calcKTY(uint16_t adcVal, uint16_t ref);
        float calcLM(uint16_t adcVal, uint16_t ref);
        float calcNTC(uint16_t adcVal, uint16_t ref);
        void TScorrection(float* ts, uint8_t purpose);
        float NTCcalculate(uint16_t adcVal, float ref);
        
        float coldTSum;
        uint16_t coldTCnt;
        uint32_t SumADCch[NUMB_OF_CHAN];     // здесь суммируются значения ацп
        uint16_t cntSum;
        
};

extern ADCmodule adc;

#endif /* __DATASTORAGE_H */
