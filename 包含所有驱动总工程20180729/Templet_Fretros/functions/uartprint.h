//*****************************************************************************
//
// led_task.h - Prototypes for the LED task.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#ifndef __UARTPRINT_H__
#define __UARTPRINT_H__

//��ͬģʽ��Ӧֵ
#define AD9959_Point_Freq       0
#define AD9959_Point_Ampl       1
#define AD9959_Sweep_Freq       2
#define AD9959_Sweep_Ampl       3
#define ADS1115_Channel_Open    4
#define ADS1115_Channel_Close   5
#define ADS11152_Channel_Open   6
#define ADS11152_Channel_Close  7
#define DAC5618_VoltageSet      8
#define DAC56182_VoltageSet     9
#define ADS1256_Channel_Open    10
#define ADS1256_Channel_Close   11
#define DAC8501_VoltageSet      12
#define DAC8811_VoltageSet      14         //13��0d,�պ��ǽ�����,�������
#define ADF4351_FreqSet         15
#define FPGA_DataSend           16
#define FPGA_DrawStart          17
#define FPGA_DrawStop           18

extern uint8_t Mode;                            //���յ��������Ӧ��ģʽ
extern uint8_t AD9959_Channel;                  //AD9959ͨ��ѡ��
extern uint32_t receivevalue;                   //���ڽ��յ���ֵ
extern uint32_t receivebuffer[4];               //���ڽ��յ���ɨ�����
extern uint8_t ADS1115_Channel;                 //ADS1115ͨ��ѡ��
extern uint8_t ADS11152_Channel;                //ADS11152ͨ��ѡ��
extern uint8_t DAC5618_Channel;                 //DAC5618ͨ��ѡ��
extern uint8_t DAC56182_Channel;                //DAC56182ͨ��ѡ��
extern uint8_t DAC8501_Channel;                 //DAC8501ͨ��ѡ��
extern float DAC8501Value;                      //DAC8501��ѹֵ
extern float DAC5618Value;                      //DAC5618��ѹֵ
extern float DAC56182Value;                     //DAC56182��ѹֵ
extern float DAC8811Value;                      //DAC8811��ѹֵ
extern uint8_t ADS1256_Channel;                 //ADS1256ͨ��ѡ��
//*****************************************************************************
//
// Prototypes for the LED task.
//
//*****************************************************************************
extern void UARTPRINTTaskInit(void);

#endif // __LED_TASK_H__
