/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H
#define __ADC_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"

/* Defines ------------------------------------------------------------------*/
#define NUMB_OF_DMA_CYCLE   16
#define NUMB_OF_CHAN   9                    // ���������� ���������� �������
#define LEN_ARRAY_DMA   NUMB_OF_CHAN*NUMB_OF_DMA_CYCLE      // ����� ������� ��� DMA

#define NUMB_COULD_AVG   20
//------------------------------------------------------------------------------------
// ������� ������������� B57332V5103F360 Rt (������� 10 ���)  �� -55 �� 115 *� � ����� 5*
//                           -55     -50      -45     -40     -35     -30    -25     -20     -15     -10     -5      0       5       10      15      20     25
const float NTC_table[] = { 446.05, 332.81, 250.44, 190.03, 145.36, 112.06, 87.041, 68.104, 53.665, 42.576, 34.001, 27.326, 22.096, 17.973, 14.703, 12.093, 10,
//                            30       35      40      45      50      55      60     65      70      75      80      85     90      95       100     105      110      115
                            8.3113, 6.9418, 5.8255, 4.9112, 4.1587, 3.5365, 3.0197, 2.5888, 2.2278, 1.9243, 1.6681, 1.451, 1.2663, 1.1088, 0.97381, 0.85788, 0.75795, 0.67155
};
// ������� ������������� MF52A103F3950 Rt (������� 10 ���)  �� -55 �� 115 *� � ����� 5*
//                           -55     -50      -45     -40     -35     -30    -25     -20     -15     -10     -5      0       5       10      15      20     25
const float NTC_table2[] = { 500.05, 400.81, 340.44, 280.66, 223.84, 159.78,116.02, 87.261, 67.214, 52.35, 40.929, 32.049, 25.149, 19.856, 15.677, 12.485, 10,
//                            30       35      40      45      50      55      60     65      70      75      80      85     90      95       100     105      110      115    120    125    130
                             8.06,   6.533, 5.325,   4.361,  3.59,   2.968,  2.467,  2.06,  1.728,  1.457,  1.235,  1.052, 0.899,   0.772,   0.666,  0.575,   0.499,   0.433,  0.377, 0.329, 0.286
};
//-----------------------------------------------------------------------------------
enum
{
    chTempMOSFET,               // ������ ����������� ������������
    chTC1,                      // ����������� ��������� 1
//    chCur,                    // ���������� ���� (� ��������� ������ ���)
    chTS3,                      // ����������� ������� 3 (������� ������/ ��������) MCU_exttemp/MCU_Temp
    chTS2,                      // ����������� ������� 2 (�������� ������/ ��������) MCU_OverTemp/MCU_IntTemp  
//    chVolt,                   // ���������� �������
    chTC2,                      // Tcoupl2           (� ��������� ������ ���)
    chAux1,                     // ���� ��������� (������)
    chTS1,                      // ����������� ������� 1  (����������� ������) MCU_AUX_ADC2         
    chTcpu,                     // ����������� ����������
    chURef,                     // ������� ���������� ��� ���
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
        void Averaging(void); // ���������� ��������, ����������� � ������ DMA

        uint16_t AdcVals[NUMB_OF_CHAN];          // ����� ��� �������
        uint16_t AdcAverVals[NUMB_OF_CHAN];          // ����� ��� ������� �������� �������
        uint16_t bufDMA[LEN_ARRAY_DMA];             // ����� ��� DMA 
        float ion;
        float TempBoard;
        float coldT;
        float ts1, ts2, ts3;
    

    private:
        //float calcKTY(float u);              // ������� ������� ����������� ��� KTY
        //float calcLM(float u);               // ������� ������� ����������� ��� LM
        void  checkFault(void);              // �������� ���������� ������� �� ������
        float calcKTY(uint16_t adcVal, uint16_t ref);
        float calcLM(uint16_t adcVal, uint16_t ref);
        float calcNTC(uint16_t adcVal, uint16_t ref);
        void TScorrection(float* ts, uint8_t purpose);
        float NTCcalculate(uint16_t adcVal, float ref);
        
        float coldTSum;
        uint16_t coldTCnt;
        uint32_t SumADCch[NUMB_OF_CHAN];     // ����� ����������� �������� ���
        uint16_t cntSum;
        
};

extern ADCmodule adc;

#endif /* __DATASTORAGE_H */
