/******************************************************************************
* ООО DD-inform
* Самара
*
* Программисты: Батырев Е.В.
*
* 31.10.2022
* Описание:
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "param_tools.h"
#include "ADCmodule.h"
#include "fuel_pump.h"
#include "fan.h"
#include "glow_plug.h"
#include "core.h"
#include "params_name.h"
#include "pmu.h"

//#define VREFINT_CAL_ADDR    ((uint16_t*) ((uint32_t) 0x1FFFF7BA))       // калибровочный коэф. для опорного напряжения
//#define TS_CAL1_ADDR        ((uint16_t*) ((uint32_t) 0x1FFFF7B8))       // калибровочный коэф. 1 для датчика температуры
//#define TS_CAL2_ADDR        ((uint16_t*) ((uint32_t) 0x1FFFF7C2))       // калибровочный коэф. 2 для датчика температуры

ADCmodule adc;
//-----------------------------------------------------
ADCmodule::ADCmodule(void)
{

}
//-----------------------------------------------------
void ADCmodule::initialize(void)                                      // ИНИЦИАЛИЗАЦИЯ ПЕРИФЕРИИ
{
    for (uint8_t i = 0; i < NUMB_OF_CHAN; i++) {
        SumADCch[i] = 0;     // обнуляем массив
        AdcVals[i] = 0;          // буфер для каналов
        AdcAverVals[i] = 0;          // буфер для средних значений каналов
    }
    cntSum = 0;
    coldTSum = 0;
    coldTCnt = 0;
    coldT = 0;
    // настройка портов аналогового ввода
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_DMA0);

    adc_disable(ADC0);
    adc_deinit(ADC0);
    dma_deinit(DMA0, DMA_CH0);

    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_1);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_3);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_4);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_5);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_6); 
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_7);     
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_1);
    
    // Настраиваем АЦП
    adc_resolution_config(ADC0 , ADC_RESOLUTION_12B);
    adc_mode_config(ADC_MODE_FREE);
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);        // !
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    adc_special_function_config(ADC0, ADC_INSERTED_CHANNEL_AUTO, ENABLE);
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, NUMB_OF_CHAN);
    uint8_t i = 0;
    //adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_0, ADC_SAMPLETIME_71POINT5);
    adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_1, ADC_SAMPLETIME_239POINT5); // ADC_SAMPLETIME_71POINT5
    adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_2, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_4, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_5, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_7, ADC_SAMPLETIME_239POINT5); // ADC_SAMPLETIME_71POINT5
    adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_8, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_9, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_16, ADC_SAMPLETIME_239POINT5);
    adc_regular_channel_config(ADC0, i++, ADC_CHANNEL_17, ADC_SAMPLETIME_239POINT5);
    adc_channel_length_config(ADC0, ADC_INSERTED_CHANNEL, 2);
    adc_inserted_channel_config(ADC0,0,ADC_CHANNEL_3,ADC_SAMPLETIME_239POINT5);
    adc_inserted_channel_config(ADC0,1,ADC_CHANNEL_6,ADC_SAMPLETIME_239POINT5);
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);        // !
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);        // !
    adc_flag_clear(ADC0,ADC_FLAG_EOIC);
    
    adc_tempsensor_vrefint_enable(); // Включаем внутренний датчик температуры процессора // Включаем источник опорного напряжения

    adc_interrupt_disable(ADC0 , ADC_INT_EOC);
    adc_interrupt_disable(ADC0 , ADC_INT_EOIC);
    adc_enable(ADC0);                                // Включаем АЦП        // !
    adc_calibration_enable(ADC0);
    adc_dma_mode_enable(ADC0);
    
    adc_watchdog_single_channel_enable(ADC0,ADC_CHANNEL_3);
    adc_watchdog_threshold_config(ADC0,0,1700);
    adc_interrupt_enable(ADC0, ADC_INT_WDE);
    
//    NVIC_SetPriority(ADC0_1_IRQn, 0);
//    NVIC_EnableIRQ(ADC0_1_IRQn);
    nvic_irq_enable(ADC0_1_IRQn, 0, 4);
    
    dma_periph_address_config(DMA0, DMA_CH0, (uint32_t)(&ADC_RDATA(ADC0)));
    dma_memory_address_config(DMA0, DMA_CH0, (uint32_t)this->bufDMA);
    dma_circulation_enable(DMA0, DMA_CH0);
    dma_transfer_number_config(DMA0, DMA_CH0, LEN_ARRAY_DMA);
    dma_priority_config(DMA0, DMA_CH0, DMA_PRIORITY_HIGH);
    dma_memory_width_config(DMA0, DMA_CH0, DMA_MEMORY_WIDTH_16BIT); // DMA_MEMORY_WIDTH_16BIT
    dma_periph_width_config (DMA0, DMA_CH0, DMA_PERIPHERAL_WIDTH_16BIT);    // DMA_PERIPHERAL_WIDTH_16BIT
    dma_memory_increase_enable(DMA0, DMA_CH0);
    dma_periph_increase_disable(DMA0, DMA_CH0);
    dma_transfer_direction_config(DMA0, DMA_CH0, DMA_PERIPHERAL_TO_MEMORY);
    
    dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
    dma_channel_enable(DMA0, DMA_CH0);
    
    adc_interrupt_enable(ADC0, ADC_INT_EOIC);                                   // Для измерения разницы тока исполнительных элементов во включенном и выключенном состояниях
                                                                                // защита от КЗ на длинном жгуте
    
    adc_enable(ADC0);                                                           // Включаем АЦП        // !
    adc_calibration_enable(ADC0);
    adc_dma_mode_enable(ADC0);
    
//    NVIC_SetPriority(DMA0_Channel0_IRQn, 2);// разрешаем прерывание
//    NVIC_EnableIRQ(DMA0_Channel0_IRQn);
    nvic_irq_enable(DMA0_Channel0_IRQn, 1, 3);
}
//-----------------------------------------------------
void ADCmodule::disable(void)
{
    adc_disable(ADC0);
    rcu_periph_clock_disable(RCU_ADC0);
    rcu_periph_clock_disable(RCU_DMA0);
}
//-----------------------------------------------------
void ADCmodule::enable(void)
{
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_1);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_3);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_4);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_5);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_6);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_0);    
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_1);
  
    dma_interrupt_enable(DMA0, DMA_CH0, DMA_INT_FTF);
    dma_channel_enable(DMA0, DMA_CH0);
    adc_enable(ADC0);                                                           // Включаем АЦП        // !

    adc_calibration_enable(ADC0);
    adc_dma_mode_enable(ADC0);

    nvic_irq_enable(DMA0_Channel0_IRQn, 1, 3);
//    NVIC_SetPriority(DMA0_Channel0_IRQn, 2);                                      // разрешаем прерывание
//    NVIC_EnableIRQ(DMA0_Channel0_IRQn);
    
}
//-----------------------------------------------------
void ADCmodule::handler(void)                                                 // ФУНКИЯ ПЕРИОДИЧЕСКОГО ОПРОСА
{
    static uint32_t lastTickCalc=0;                                             // время на расчет измеряемых величин
    float val, tcpu;
    int32_t gainTC;

    if (core.getTick()-lastTickCalc > 250) {                                   // раз в 250 мсек расчитываем аналоговые величины
        lastTickCalc=core.getTick();                                            // сохраняем настоящее время
        __disable_irq();
        for (uint8_t ch = 0; ch < NUMB_OF_CHAN; ch++) {
            AdcAverVals[ch] = SumADCch[ch] / cntSum;
            SumADCch[ch] = 0;
        } 
        cntSum = 0;
        __enable_irq();
        
        ion = 1200.0 * 4095.0/AdcAverVals[chURef];
        static const float cIon = 3300.0;
        measure.setVal((char*)VOLTAGE_CPU, (float)(ion/1000.0));
        measure.setVal((char*)VOLTAGE_REFERENCE, (float)(cIon/1000.0));
        
        val = (float)AdcAverVals[chTcpu] * cIon/4095;
        tcpu = ((float)(1450 - val)/4.1 + 25.0 - 5.0);//  с поправкой на разогрев процессора
        measure.setVal((char*)TEMPERATURE_CPU, (float)tcpu);
                
        uint8_t typeTS = parameter.getVal((char*)PURPOSE_TYPE_TS);

        if (typeTS == 0) {
            // тип датчика KTY
            ts1 = calcKTY(AdcAverVals[chTS1], cIon);
            ts2 = calcKTY(AdcAverVals[chTS2], cIon);
            ts3 = calcKTY(AdcAverVals[chTS3], cIon);
        }
        else if (typeTS == 1) {
            // тип датчика LM
            ts1 = calcLM(AdcAverVals[chTS1], cIon);
            ts2 = calcLM(AdcAverVals[chTS2], cIon);
            ts3 = calcLM(AdcAverVals[chTS3], cIon);
        }
        else if (typeTS == 2) {
            // тип датчика - NTC
            ts1 = calcNTC(AdcAverVals[chTS1], cIon);
            ts2 = calcNTC(AdcAverVals[chTS2], cIon);
            ts3 = calcNTC(AdcAverVals[chTS3], cIon);
        }
        
        if (parameter.getVal((char*)PURPOSE_CORRECTION_TS) == 1) {         
            TScorrection(&ts1, (uint8_t)parameter.getVal((char*)PURPOSE_TS1));
            TScorrection(&ts2, (uint8_t)parameter.getVal((char*)PURPOSE_TS2));
            TScorrection(&ts3, (uint8_t)parameter.getVal((char*)PURPOSE_TS3));         
        }
        
        measure.setVal((char*)TS1, ts1);
        measure.setVal((char*)TS2, ts2);
        measure.setVal((char*)TS3, ts3);
        
        if (parameter.getVal((char*)PURPOSE_OUT_PUMP) == 0) {
            // помпа не используется в воздушном отопителе
            coldTSum += core.getTSvalue(PURP_T_IN_AIR);
            coldTCnt++;
            if (coldTCnt >= NUMB_COULD_AVG) {
                coldT = (float)coldTSum/NUMB_COULD_AVG;
                coldTSum = 0;
                coldTCnt = 0;
            }
        } else {
            coldT = tcpu; 
            TempBoard = NTCcalculate(AdcAverVals[chTempMOSFET], (float)(cIon));      
            if ( (TempBoard > -50.0) && (TempBoard < 70.0) ) {
                if (TempBoard < tcpu) coldT = TempBoard;
            } 
        }
        //measure.setVal((char*)Tcj, (float)(TEMPERATURE_COLD_JUNC));// 
        gainTC = parameter.getVal((char*)PURPOSE_GAIN_TC);
        val = (int16_t)(cIon * (AdcAverVals[chTC1]-parameter.getVal((char*)CALIBRATION_VALUE_TC1))/4095);
        measure.setVal((char*)TC1, (float)(val*26/gainTC+coldT));// 0.663 0.270 0.196 (val*0.196+tcpu)

        val = (int16_t)(cIon * (AdcAverVals[chTC2]-parameter.getVal((char*)CALIBRATION_VALUE_TC2))/4095);
        measure.setVal((char*)TC2, (float)(val*26/gainTC+coldT));// 0.663 0.270 0.196 (val*0.196+tcpu)

        measure.setVal((char*)CURRENT_GLOW_PLUG, (float)glowPlug.current);
        
        
        
    }

}
//-----------------------------------------------------
void ADCmodule::TScorrection(float* ts, uint8_t purpose)
{
    uint8_t comp = 4;
        if (purpose == 4) {  // Т перегрева
           *ts = *ts + 6;
        } else 
        if (purpose == 5) {  // Т жидкости
           if (*ts > 30) comp += 1;
           if (*ts > 60) comp += 1;
           *ts = *ts + comp;   
        }
}
//----------------------------------------------
void ADCmodule::calibrationTc(void)
{
    parameter.setVal((char*)CALIBRATION_VALUE_TC1, AdcAverVals[chTC1]);
    parameter.setVal((char*)CALIBRATION_VALUE_TC2, AdcAverVals[chTC2]);
    //parameter.setVal((char*)CALIBRATION_VALUE_TC3, AdcAverVals[chTC3]);
    
}
//-----------------------------------------------------
/*
float ADCmodule::calcKTY(float u)                                               // РАСЧЕТ ТЕМПЕРАТУРЫ ДТ и ДП
{
    return u*u*u*36.72-u*u*149.27+u*333-310.15;                                 // квадратичная апроксимация данных датчика из таблицы
} 
//-----------------------------------------------------
float ADCmodule::calcLM(float u)                                               // РАСЧЕТ ТЕМПЕРАТУРЫ ДТ и ДП
{
    return u*2*100 - 273;                                                             // линейная апроксимация данных датчика
}
*/
//------------------------------------------------------
float ADCmodule::calcKTY(uint16_t adcVal, uint16_t ref)                                               // РАСЧЕТ ТЕМПЕРАТУРЫ ДТ и ДП
{
    float u = (float)adcVal*ref/4095.0/1000.0;
    
    return (float)(u*u*u*36.72-u*u*149.27+u*333-310.15);                                 // квадратичная апроксимация данных датчика из таблицы
}
//------------------------------------------------------
float ADCmodule::calcLM(uint16_t adcVal, uint16_t ref)                                               // РАСЧЕТ ТЕМПЕРАТУРЫ ДТ и ДП
{
    float u = (float)adcVal*ref/4095.0/1000.0;
    
    return u*2*100 - 273.0;                                                             // линейная апроксимация данных датчика
}
//------------------------------------------------------
// расчет температуры по NTC резистору MF52A103F3950 (датчик температуры бинар5)  
float ADCmodule::calcNTC(uint16_t adcVal, uint16_t refin)                                 // РАСЧЕТ ТЕМПЕРАТУРЫ ДТ NTC
{
    float Rnom = 9.9; // 9900.9 ОМ при парал 1 МОМ
    float ref = (float)refin/1000.0;
    float u = (float)adcVal*ref/4095.0;
    float div = ref-u;
    if (div == 0) div = 0.01;
    float R = u*Rnom/(div); // 10 - 10КОм // R = U*R1/(ION-U)
    float temp = -55.0;
    uint8_t i = 0;
    uint8_t lastIndx = (sizeof(NTC_table2)/sizeof(NTC_table2[0]))-1; 
    float a = NTC_table2[lastIndx-1];
    float b = NTC_table2[lastIndx];
    if (R > 600.0) temp = -100.0; // обрыв датчика
    else if (R > NTC_table2[0]) { // слишком низкая температура 
        temp = -60.0;
    } else {
        for (i = 0; i < (lastIndx-1); i++) {
            a = NTC_table2[i];
            b = NTC_table2[i+1];
            if ( (R <= a) && (R >= b) ) {
                break;
            }
        }
        float dT = (a-R)*5 / (a-b);
        temp = -55.0 + i*5 + dT;
    }
    return temp;                                                           // линейная апроксимация данных датчика
}
//-----------------------------------------------------
// В прерывании высчитывается среднее значение по массиву DMA и 
// полученное значение суммируется в SumADCch[ch] для дальнейшего осреднения
// в основном цикле за больший период
void ADCmodule::Averaging(void)
{
    uint32_t Sum;
    
    for (uint8_t ch = 0; ch < NUMB_OF_CHAN; ch++) {
        Sum = 0;
        for(uint8_t i = 0; i < NUMB_OF_DMA_CYCLE; i++) {
            Sum += bufDMA[ch + i*NUMB_OF_CHAN];          
        } 
        AdcVals[ch] = Sum / NUMB_OF_DMA_CYCLE;
        SumADCch[ch] += AdcVals[ch];
    } 
    cntSum++;
}
//-----------------------------------------------------
// расчет температуры по NTC резистору B57332V5103F360 на плате
    
float ADCmodule::NTCcalculate(uint16_t adcVal, float refin)
{
    float Rnom = 10.0; // 10000.0 ОМ 
    float ref = (float)refin/1000.0;
    float u = (float)adcVal*ref/4095.0;
    float div = ref-u;
    if (div == 0) div = 0.01;
    float R = u*Rnom/(div); // 10 - 10КОм // R = U*R1/(ION-U)
    float temp = -55.0;
    uint8_t i = 0;
    uint8_t lastIndx = (sizeof(NTC_table)/sizeof(NTC_table[0]))-1; 
    float a = NTC_table[lastIndx-1];
    float b = NTC_table[lastIndx];
    
    if (R > NTC_table[0]) { // слишком низкая температура или обрыв датчика
        temp = -60.0;
    } else {
        for (i = 0; i < (lastIndx-1); i++) {
            a = NTC_table[i];
            b = NTC_table[i+1];
            if ( (R <= a) && (R >= b) ) {
                break;
            }
        }
        float dT = (a-R)*5 / (a-b);
        temp = -55.0 + i*5 + dT;
    }
    return temp;
}
//-----------------------------------------------------
extern "C" void DMA0_Channel0_IRQHandler(void)                                             // ОБРАБОТКА ПРЕРЫВАНИЯ от DMA
{
    if (dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_FTF)){
        dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_FTF);
        adc.Averaging();
    }
}
//=======================================================================

