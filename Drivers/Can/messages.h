/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MESSAGES_H
#define __MESSAGES_H

/* Includes ------------------------------------------------------------------*/
#include "gd32f30x.h"

#define MEMORY_0_ERASE              0                                //������� ������ ������ �������� ������
#define MEMORY_1_WRITE_TO_RAM       1                                //������ ������ 
#define MEMORY_2_WRITE_ALL_TO_FLASH 2                                //���������� ������ ������ 
#define MEMORY_3_READ               3                                //������ ������
#define MEMORY_4_ANSWER             4                                //����� �� ������ - �������� ������� �������
#define MEMORY_5_ANSWER             5                                //����� �� ������ - ���������� ��������� �������
#define MEMORY_6_READ               6                                //������ ���������� ��������� (�� paramsname)

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

        bool PumpStatus_f;                                           //������/��������� ����� �� ��������� �����. ���� ����� ���� ������� ������ �� ������ ����� (CAN - PGN 19)
        bool ManualModeStatus_f;
        bool PCBTestStatus_f;

        bool cmdRunHeater_f;                                         //������ �� ������� �������
        bool cmdStopHeater_f;                                        //��������� �� ������� �������
        bool cmdRunPump_f;                                           //������ ������ ����� �� ������� �������

        bool btnDWater_f;                                            //���� ��������/���������
        bool DWaterFlow_f;                                           //�������� ��� ��� ������ ������ ������� ����
        //int16_t Ttank;                                               //����������� ����

        bool FaultLink_f;                                            //��� ����� � �������
        bool SendMessages1s_f;                                       //�������� ������ ��������� ��� � �������
        bool SignalingRun_f;                                         //true - ��� ������ �� ��������. �� ������� �� ������

    private:
        uint8_t FaultLinkCnt;
        bool CheckConnection_f;                                      //true - ������� �� ������ � ������� �����������
        uint32_t GetParameter(uint16_t ASPN);
};

extern Messages messages;

                        
//------------------------------------------------------------------------------------------------
#define VAR_STAGE                        1//uint8_t  ;������                                        ; V                ; %.0f           ; ������       ;
#define VAR_MODE                         2//uint8_t  ;����� ������                                  ; V                ; %.0f           ; �����        ;
#define VAR_RUNNING_TIME                 3//uint32_t ;����� ����� ������, c                         ; V                ; %.0f           ; ��.������    ;
#define VAR_MODE_TIME                    4//uint32_t ;����� ������ �� ������, c                     ; V                ; %.0f           ; ��.���.      ;

#define VAR_U_SUPPLY                     5//uint16_t ;���������� �������, �*100                     ; V/100            ; %.2f           ; U, �         ;
#define VAR_T_FLAME                      6//int16_t  ;����������� ��                                ; V                ; %.0f           ; �����        ;
#define VAR_T_FRAME                      7//int16_t  ;����������� ����                              ; V                ; %.0f           ; �����        ;
#define VAR_T_PANEL                      8//int8_t   ;����������� ������� � ��                      ; V                ; %.0f           ; �������      ;
#define VAR_T_EXT                        9//int16_t  ;����������� ��������� �������                 ; V                ; %.0f           ; ������       ;
#define VAR_T_INLET                     10//int16_t  ;����������� �� �����                          ; V                ; %.0f           ; ������       ;
#define VAR_T_SETPOINT                  11//int8_t   ;����������� �������                           ; V                ; %.0f           ; ��������     ;
#define VAR_PWR_DEFINED                 12//uint8_t  ;�������� ��������                             ; V                ; %.0f           ; � ���        ;
#define VAR_PWR_REALIZED                13//uint8_t  ;������� ��������                              ; V                ; %.0f           ; � ���        ;
#define VAR_WORK_BY                     14//uint8_t  ;�� ������ ������� �������                     ; V                ; %.0f           ; ������ ��    ;
#define VAR_FAN_REV_DEFINED             15//uint8_t  ;�������� ������� ��                           ; V                ; %.0f           ; ��.���       ;
#define VAR_FAN_REV_MEASURED            16//uint8_t  ;���������� ������� ��                         ; V                ; %.0f           ; ��. ���      ;
#define VAR_FP_FREQ_DEFINED             17//uint16_t ;�������� ������� ��, ��*100                   ; V/100            ; %.2f           ; �� ���       ;
#define VAR_FP_FREQ_REALIZED            18//uint16_t ;������������� ������� ��, ��*100              ; V/100            ; %.2f           ; �� ���       ;
#define VAR_GP_PERIOD                   19//uint32_t ;������ �����                                  ; V                ; %.0f           ; ��. ���      ;
#define VAR_GP_PULSE                    20//uint32_t ;������� �����                                 ; V                ; %.0f           ; ��. ���      ;
#define VAR_GP_PERCENT                  21//uint8_t  ;�������� �������� �������� �����              ; V                ; %.0f           ; ��. %        ;
#define VAR_FLAME_CALIBRATION           22//uint16_t ;������������� �������� ��������� ��           ; V                ; %.0f           ; �.�. ��      ;
#define VAR_FRAME_CALIBRATION           23//uint16_t ;������������� �������� ��������� �������      ; V                ; %.0f           ; �.�. ����    ;

#define VAR_FAULT_CODE                  24//uint8_t  ;��� �������������                             ; V                ; %.0f           ; ������       ;
#define VAR_FAULT_BLINK_CODE            25//uint8_t  ;��� �������������. ���������� �������� (�������); V              ; %.0f           ; ������.���   ;
#define VAR_FAULT_BYTE0                 26//uint8_t  ;���� ��������������1                          ; V                ; %.0f           ; �.������0    ;
#define VAR_FAULT_BYTE1                 27//uint8_t  ;���� ��������������2                          ; V                ; %.0f           ; �.������1    ;
#define VAR_FAULT_BYTE2                 28//uint8_t  ;���� ��������������3                          ; V                ; %.0f           ; �.������2    ;
#define VAR_FAULT_BYTE3                 29//uint8_t  ;���� ��������������4                          ; V                ; %.0f           ; �.������3    ;
#define VAR_FAULT_BYTE4                 30//uint8_t  ;���� ��������������5                          ; V                ; %.0f           ; �.������4    ;
#define VAR_FAULT_BYTE5                 31//uint8_t  ;���� ��������������6                          ; V                ; %.0f           ; �.������5    ;
#define VAR_FAULT_BYTE6                 32//uint8_t  ;���� ��������������7                          ; V                ; %.0f           ; �.������6    ;
#define VAR_FAULT_BYTE7                 33//uint8_t  ;���� ��������������8                          ; V                ; %.0f           ; �.������7    ;

#define VAR_IGNITION_ATTEMPT            34//uint8_t  ;������� �������                               ; V                ; %.0f           ; �����������  ;
#define VAR_WAIT_MODE_ALLOWED           35//uint8_t  ;�������� ������ �����                         ; V                ; %.0f           ; ����.���     ;
#define VAR_FLAME_MINIMUM               36//int16_t  ;����������� �������� ��                       ; V                ; %.0f           ; �� min       ;
#define VAR_FLAME_LIMIT                 37//int16_t  ;������� ����� �������                         ; V                ; %.0f           ; �� lim       ;
#define VAR_FLAME_BREAK_NUM             38//uint8_t  ;���������� ������ ������� �� ����� ������     ; V                ; %.0f           ; ���.��.��    ;
#define VAR_OVERHEAT_TBORDER            39//int16_t  ;������� ���������                             ; V                ; %.0f           ; ��.������    ;
#define VAR_T_LIQUID                    40//int16_t  ;����������� ��������                          ; V                ; %.0f           ; ��           ;
#define VAR_T_OVERHEAT                  41//int16_t  ;����������� ���������                         ; V                ; %.0f           ; ����         ;
#define VAR_DT_FLAME                    42//int16_t  ;��������� ����������� �������                 ; V                ; %.0f           ; DT ��        ;
#define VAR_DT_LIQUID                   43//int16_t  ;��������� ����������� ��������                ; V                ; %.0f           ; DT ��        ;
#define VAR_DT_OVERHEAT                 44//int16_t  ;��������� ����������� ���������               ; V                ; %.0f           ; DT ����      ;
#define VAR_RELAY_STATE                 45//uint8_t  ;��������� ���� (���/����)                     ; V                ; %.0f           ; ����         ;
#define VAR_PUMP_STATE                  46//uint8_t  ;��������� ����� (���/����)                    ; V                ; %.0f           ; �����        ;
#define VAR_SIGNALING_STATE             47//uint8_t  ;��������� ������������ (������ ����/���)      ; V                ; %.0f           ; ������������ ;
#define VAR_ENGINE_STATE                48//uint8_t  ;��������� ��������� (�������/����������)      ; V                ; %.0f           ; ���������    ;

#define VAR_ADC_CH0                     49//uint16_t ;��� ����� 0                                   ; V                ; %.0f           ; ��� 0        ;
#define VAR_ADC_CH1                     50//uint16_t ;��� ����� 1                                   ; V                ; %.0f           ; ��� 1        ;
#define VAR_ADC_CH2                     51//uint16_t ;��� ����� 2                                   ; V                ; %.0f           ; ��� 2        ;
#define VAR_ADC_CH3                     52//uint16_t ;��� ����� 3                                   ; V                ; %.0f           ; ��� 3        ;
#define VAR_ADC_CH4                     53//uint16_t ;��� ����� 4                                   ; V                ; %.0f           ; ��� 4        ;
#define VAR_ADC_CH5                     54//uint16_t ;��� ����� 5                                   ; V                ; %.0f           ; ��� 5        ;
#define VAR_ADC_CH6                     55//uint16_t ;��� ����� 6                                   ; V                ; %.0f           ; ��� 6        ;
#define VAR_ADC_CH7                     56//uint16_t ;��� ����� 7                                   ; V                ; %.0f           ; ��� 7        ;
#define VAR_ADC_CH8                     57//uint16_t ;��� ����� 8                                   ; V                ; %.0f           ; ��� 8        ;
#define VAR_ADC_CH9                     58//uint16_t ;��� ����� 9                                   ; V                ; %.0f           ; ��� 9        ;

#define VAR_T_CPU                       59//int16_t  ;����������� ����������                        ; V                ; %.0f           ; �cpu         ;
#define VAR_PRESSURE                    60//uint8_t  ;����������� ��������, ���                     ; V                ; %.0f           ; ����.,���    ;
#define VAR_T_TANK                      61//int16_t  ;����������� ���� � �������� ���������         ; V                ; %.0f           ; �����        ;

#define VAR_GLOWPLUG_STATE              62//uint8_t  ;��������� ����� (���/����)                     ; V                ; %.0f           ; ����         ;

#define VAR_START_BB_ERR_RECORD 0xFFFFFFFA//uint32_t ;������ ������ � ������������� � ��
//----------------------------------------------------------------------------------------------

#endif /* __MESSAGES_H */
