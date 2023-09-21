/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MESSAGES_H
#define __MESSAGES_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"

#define MEMORY_0_ERASE              0                                //стереть сектор памяти хранения данных
#define MEMORY_1_WRITE_TO_RAM       1                                //запись данных 
#define MEMORY_2_WRITE_ALL_TO_FLASH 2                                //завершение записи данных 
#define MEMORY_3_READ               3                                //чтение данных
#define MEMORY_4_ANSWER             4                                //ответ на запрос - параметр успешно получен
#define MEMORY_5_ANSWER             5                                //ответ на запрос - невозможно выполнить команду
#define MEMORY_6_READ               6                                //чтение единичного параметра (из paramsname)

/* Classes ------------------------------------------------------------------*/
class Messages
{
    public:
        Messages(void); 
        void handler(void); 
        void TransmitMessage(void);
        void ProcessMessage(uint8_t MsgNum);
        void TransmittingData(void);
        void SendData(void);
        
        void ClearFaults(void);
        void CheckConnectionEnabled(void);
        void CheckConnectionDisabled(void);

        bool PumpStatus_f;                                           //Запуск/остановка помпы по состоянию флага. Флаг может быть получен только по каналу связи (CAN - PGN 19)
        bool ManualModeStatus_f;
        bool PCBTestStatus_f;

        bool cmdRunHeater_f;                                         //Запуск по разовой команде
        bool cmdStopHeater_f;                                        //Остановка по разовой команде
        bool cmdRunPump_f;                                           //Запуск только помпы по разовой команде

        bool btnDWater_f;                                            //Вода включена/выключена
        bool DWaterFlow_f;                                           //Сработал или нет датчик потока бытовой воды
        //int16_t Ttank;                                               //температура бака

        bool FaultLink_f;                                            //нет связи с пультом
        bool SendMessages1s_f;                                       //Отправка пакета сообщений раз в секунду
        bool SignalingRun_f;                                         //true - был запуск по сигналке. не следить на связью

    private:
        uint8_t FaultLinkCnt;
        bool CheckConnection_f;                                      //true - следить за связью с внешним устройством
        uint32_t GetParameter(uint16_t ASPN);
};

extern Messages messages;

                        
//------------------------------------------------------------------------------------------------
#define VAR_STAGE                        1//uint8_t  ;Стадия                                        ; V                ; %.0f           ; Стадия       ;
#define VAR_MODE                         2//uint8_t  ;Режим работы                                  ; V                ; %.0f           ; Режим        ;
#define VAR_RUNNING_TIME                 3//uint32_t ;Общее время работы, c                         ; V                ; %.0f           ; Вр.Работы    ;
#define VAR_MODE_TIME                    4//uint32_t ;Время работы на режиме, c                     ; V                ; %.0f           ; Вр.Реж.      ;

#define VAR_U_SUPPLY                     5//uint16_t ;Напряжение питания, В*100                     ; V/100            ; %.2f           ; U, В         ;
#define VAR_T_FLAME                      6//int16_t  ;Температура ИП                                ; V                ; %.0f           ; Тплам        ;
#define VAR_T_FRAME                      7//int16_t  ;Температура корп                              ; V                ; %.0f           ; Ткорп        ;
#define VAR_T_PANEL                      8//int8_t   ;Температура датчика в ПУ                      ; V                ; %.0f           ; Тпульта      ;
#define VAR_T_EXT                        9//int16_t  ;Температура кабинного датчика                 ; V                ; %.0f           ; Твнешн       ;
#define VAR_T_INLET                     10//int16_t  ;Температура на входе                          ; V                ; %.0f           ; Твхода       ;
#define VAR_T_SETPOINT                  11//int8_t   ;Температура уставки                           ; V                ; %.0f           ; Туставки     ;
#define VAR_PWR_DEFINED                 12//uint8_t  ;Заданная мощность                             ; V                ; %.0f           ; Р зад        ;
#define VAR_PWR_REALIZED                13//uint8_t  ;Текущая мощность                              ; V                ; %.0f           ; Р тек        ;
#define VAR_WORK_BY                     14//uint8_t  ;По какому датчику работал                     ; V                ; %.0f           ; Работа по    ;
#define VAR_FAN_REV_DEFINED             15//uint8_t  ;Заданные обороты НВ                           ; V                ; %.0f           ; Об.зад       ;
#define VAR_FAN_REV_MEASURED            16//uint8_t  ;Измеренные обороты НВ                         ; V                ; %.0f           ; Об. изм      ;
#define VAR_FP_FREQ_DEFINED             17//uint16_t ;Заданная частота ТН, Гц*100                   ; V/100            ; %.2f           ; ТН зад       ;
#define VAR_FP_FREQ_REALIZED            18//uint16_t ;Реализованная частота ТН, Гц*100              ; V/100            ; %.2f           ; ТН тек       ;
#define VAR_GP_PERIOD                   19//uint32_t ;Период свечи                                  ; V                ; %.0f           ; Св. пер      ;
#define VAR_GP_PULSE                    20//uint32_t ;Импульс свечи                                 ; V                ; %.0f           ; Св. имп      ;
#define VAR_GP_PERCENT                  21//uint8_t  ;Заданные проценты мощности свечи              ; V                ; %.0f           ; Св. %        ;
#define VAR_FLAME_CALIBRATION           22//uint16_t ;Калибровочное значение термопары ИП           ; V                ; %.0f           ; К.З. ИП      ;
#define VAR_FRAME_CALIBRATION           23//uint16_t ;Калибровочное значение термопары корпуса      ; V                ; %.0f           ; К.З. Корп    ;

#define VAR_FAULT_CODE                  24//uint8_t  ;Код неисправности                             ; V                ; %.0f           ; Неиспр       ;
#define VAR_FAULT_BLINK_CODE            25//uint8_t  ;Код неисправности. Количество морганий (миганий); V              ; %.0f           ; Неиспр.Миг   ;
#define VAR_FAULT_BYTE0                 26//uint8_t  ;Байт неисправностей1                          ; V                ; %.0f           ; Б.неиспр0    ;
#define VAR_FAULT_BYTE1                 27//uint8_t  ;Байт неисправностей2                          ; V                ; %.0f           ; Б.неиспр1    ;
#define VAR_FAULT_BYTE2                 28//uint8_t  ;Байт неисправностей3                          ; V                ; %.0f           ; Б.неиспр2    ;
#define VAR_FAULT_BYTE3                 29//uint8_t  ;Байт неисправностей4                          ; V                ; %.0f           ; Б.неиспр3    ;
#define VAR_FAULT_BYTE4                 30//uint8_t  ;Байт неисправностей5                          ; V                ; %.0f           ; Б.неиспр4    ;
#define VAR_FAULT_BYTE5                 31//uint8_t  ;Байт неисправностей6                          ; V                ; %.0f           ; Б.неиспр5    ;
#define VAR_FAULT_BYTE6                 32//uint8_t  ;Байт неисправностей7                          ; V                ; %.0f           ; Б.неиспр6    ;
#define VAR_FAULT_BYTE7                 33//uint8_t  ;Байт неисправностей8                          ; V                ; %.0f           ; Б.неиспр7    ;

#define VAR_IGNITION_ATTEMPT            34//uint8_t  ;Попытка розжига                               ; V                ; %.0f           ; ПопыткаРозж  ;
#define VAR_WAIT_MODE_ALLOWED           35//uint8_t  ;Разрешен ждущий режим                         ; V                ; %.0f           ; Ждущ.Реж     ;
#define VAR_FLAME_MINIMUM               36//int16_t  ;Минимальное значение ИП                       ; V                ; %.0f           ; ИП min       ;
#define VAR_FLAME_LIMIT                 37//int16_t  ;Граница срыва пламени                         ; V                ; %.0f           ; ИП lim       ;
#define VAR_FLAME_BREAK_NUM             38//uint8_t  ;Количество срывов пламени во время работы     ; V                ; %.0f           ; Кол.Ср.Пл    ;
#define VAR_OVERHEAT_TBORDER            39//int16_t  ;Граница перегрева                             ; V                ; %.0f           ; Гр.Перегр    ;
#define VAR_T_LIQUID                    40//int16_t  ;Температура жидкости                          ; V                ; %.0f           ; Тж           ;
#define VAR_T_OVERHEAT                  41//int16_t  ;Температура перегрева                         ; V                ; %.0f           ; Тпер         ;
#define VAR_DT_FLAME                    42//int16_t  ;Изменение температуры пламени                 ; V                ; %.0f           ; DT ИП        ;
#define VAR_DT_LIQUID                   43//int16_t  ;Изменение температуры жидкости                ; V                ; %.0f           ; DT Тж        ;
#define VAR_DT_OVERHEAT                 44//int16_t  ;Изменение температуры перегрева               ; V                ; %.0f           ; DT Тпер      ;
#define VAR_RELAY_STATE                 45//uint8_t  ;Состояние реле (ВКЛ/ВЫКЛ)                     ; V                ; %.0f           ; Реле         ;
#define VAR_PUMP_STATE                  46//uint8_t  ;Состояние помпы (ВКЛ/ВЫКЛ)                    ; V                ; %.0f           ; Помпа        ;
#define VAR_SIGNALING_STATE             47//uint8_t  ;Состояние сигнализации (сигнал есть/нет)      ; V                ; %.0f           ; Сигнализация ;
#define VAR_ENGINE_STATE                48//uint8_t  ;Состояние двигателя (заведен/остановлен)      ; V                ; %.0f           ; Двигатель    ;

#define VAR_ADC_CH0                     49//uint16_t ;АЦП канал 0                                   ; V                ; %.0f           ; АЦП 0        ;
#define VAR_ADC_CH1                     50//uint16_t ;АЦП канал 1                                   ; V                ; %.0f           ; АЦП 1        ;
#define VAR_ADC_CH2                     51//uint16_t ;АЦП канал 2                                   ; V                ; %.0f           ; АЦП 2        ;
#define VAR_ADC_CH3                     52//uint16_t ;АЦП канал 3                                   ; V                ; %.0f           ; АЦП 3        ;
#define VAR_ADC_CH4                     53//uint16_t ;АЦП канал 4                                   ; V                ; %.0f           ; АЦП 4        ;
#define VAR_ADC_CH5                     54//uint16_t ;АЦП канал 5                                   ; V                ; %.0f           ; АЦП 5        ;
#define VAR_ADC_CH6                     55//uint16_t ;АЦП канал 6                                   ; V                ; %.0f           ; АЦП 6        ;
#define VAR_ADC_CH7                     56//uint16_t ;АЦП канал 7                                   ; V                ; %.0f           ; АЦП 7        ;
#define VAR_ADC_CH8                     57//uint16_t ;АЦП канал 8                                   ; V                ; %.0f           ; АЦП 8        ;
#define VAR_ADC_CH9                     58//uint16_t ;АЦП канал 9                                   ; V                ; %.0f           ; АЦП 9        ;

#define VAR_T_CPU                       59//int16_t  ;Температура процессора                        ; V                ; %.0f           ; Тcpu         ;
#define VAR_PRESSURE                    60//uint8_t  ;Атмосферное давление, кПа                     ; V                ; %.0f           ; Давл.,кПа    ;
#define VAR_T_TANK                      61//int16_t  ;Температура бака в системах отопления         ; V                ; %.0f           ; Тбака        ;

#define VAR_GLOWPLUG_STATE              62//uint8_t  ;Состояние свечи (ВКЛ/ВЫКЛ)                     ; V                ; %.0f           ; Реле         ;

#define VAR_START_BB_ERR_RECORD 0xFFFFFFFA//uint32_t ;Начало записи о неисправности в ЧЯ
//----------------------------------------------------------------------------------------------

#endif /* __MESSAGES_H */
