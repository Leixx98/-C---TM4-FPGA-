//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
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
/*****************************************************************************
�������д��������Ƶ�ģ�飬ע����uartreceive.c������
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "functions/uartprint.h"
//*****************************************************************************

#define UARTPRINT_TASK_PRIO          5        //�������ȼ�
#define UARTPRINT_STK_SIZE          256       //�����ջ��С
TaskHandle_t UARTPRINTTaskHandler;            //������

uint8_t UARTReceive_Buffer[100];         //���ڽ�������
uint8_t Mode;                            //���յ��������Ӧ��ģʽ
uint8_t AD9959_Channel;                  //AD9959ͨ��ѡ��
uint8_t ADS1115_Channel;                 //ADS1115ͨ��ѡ��
uint8_t ADS11152_Channel;                //ADS1115ͨ��ѡ��
uint8_t DAC5618_Channel;                 //DAC5618ͨ��ѡ��
uint8_t DAC56182_Channel;                //DAC56182ͨ��ѡ��
uint8_t ADS1256_Channel;                 //ADS1256ͨ��ѡ��
uint8_t DAC8501_Channel;                 //DAC8501ͨ��ѡ��
float DAC5618Value;                      //DAC5618��ѹֵ
float DAC56182Value;                     //DAC56182��ѹֵ
float DAC8501Value;                      //DAC8501��ѹֵ
float DAC8811Value;                      //DAC8811��ѹֵ
uint32_t receivevalue;                   //���ڽ��յ���ֵ
uint32_t receivebuffer[4];               //���ڽ��յ���ɨ�����

extern uint32_t SysClock;                //ϵͳʱ��

//*****************************************************************************
//
// �ض���printf����
//
//*****************************************************************************
int fputc(int ch, FILE *f)
{
  UARTCharPut(UART5_BASE,(uint8_t)ch);
  return ch;
}

//*****************************************************************************
//
// This task toggles the user selected LED at a user selected frequency. User
// can make the selections by pressing the left and right buttons.
//
//*****************************************************************************
void
DemoUARTPRINTTask(void *pvParameters)
{
		uint8_t i,j;
		uint16_t arraycount;
		for(i=0;i<4;i++)
			receivebuffer[i]=0;
		arraycount=0;i=0;j=0;receivevalue=0,Mode=0xff;	
    while(1)
    {
			      if(UARTCharsAvail(UART5_BASE))
            { 
               while(UARTReceive_Buffer[arraycount]!=0x0d)               //�Ӵ����л������
 							 { 
									arraycount++;
									UARTReceive_Buffer[arraycount] = UARTCharGet(UART5_BASE);     //���뻺������
							 }			
							 while(UARTReceive_Buffer[arraycount]!=0x0d)
								 UARTReceive_Buffer[++arraycount] = UARTCharGet(UART5_BASE);
							 if(UARTReceive_Buffer[1]==0x0a)                      //���0x0a������֤�����յ��˸�ʽ��ȷ������
							 {
									 if(UARTReceive_Buffer[2]==0x00)                  //�ж������Ӧ��ģʽ����Ƶ�������ɨƵ��ɨ��
											 Mode=AD9959_Point_Freq; 
									 else if(UARTReceive_Buffer[2]==0x01)
											 Mode=AD9959_Point_Ampl;
									 else if(UARTReceive_Buffer[2]==0x02)
											 Mode=AD9959_Sweep_Freq;			
									 else if(UARTReceive_Buffer[2]==0x03)
											 Mode=AD9959_Sweep_Ampl;
									 else if(UARTReceive_Buffer[2]==0x04)					 
											 Mode=ADS1115_Channel_Open;					
									 else if(UARTReceive_Buffer[2]==0x05)
											 Mode=ADS1115_Channel_Close;	
									 else if(UARTReceive_Buffer[2]==0x06)
											 Mode=ADS11152_Channel_Open;
									 else if(UARTReceive_Buffer[2]==0x07)
											 Mode=ADS11152_Channel_Close;
									 else if(UARTReceive_Buffer[2]==0x08)
											 Mode=DAC5618_VoltageSet;
									 else if(UARTReceive_Buffer[2]==0x09)
											 Mode=DAC56182_VoltageSet;
									 else if(UARTReceive_Buffer[2]==0x0A)
											 Mode=ADS1256_Channel_Open;									 
									 else if(UARTReceive_Buffer[2]==0x0B)
											 Mode=ADS1256_Channel_Close;	
									 else if(UARTReceive_Buffer[2]==0x0C)
											 Mode=DAC8501_VoltageSet;
									 else if(UARTReceive_Buffer[2]==0x0e)
											 Mode=DAC8811_VoltageSet;
									 else if(UARTReceive_Buffer[2]==0x0f)        //��ģ����ʱû��
											 Mode=ADF4351_FreqSet;   
									 else if(UARTReceive_Buffer[2]==0x10)       
											 Mode=FPGA_DataSend;
									 else if(UARTReceive_Buffer[2]==0x11)        
										   Mode=FPGA_DrawStart;
									 else if(UARTReceive_Buffer[2]==0x12)
										   Mode=FPGA_DrawStop;
									 
									 
									           
									 if(Mode==AD9959_Point_Freq||Mode==AD9959_Point_Ampl)            //��Ϊ��Ƶ���ߵ��ʱ��ֻ�����һ������
									 {
											AD9959_Channel=UARTReceive_Buffer[3];        //��õ�ǰͨ��ֵ
											for(i=4;i<arraycount;i++)                   //������������
											{
												if(UARTReceive_Buffer[i]>47)                //���ݵ�ֵ����Ϊ48������47������ֵ��С��47
															receivevalue+=(UARTReceive_Buffer[i]-48)*(pow(10,(arraycount-i-1)));  //����õ���Ӧ������
											}
//											UARTprintf("%d\n",receivevalue);          //���������
									 }
									 else if(Mode==AD9959_Sweep_Freq||Mode==AD9959_Sweep_Ampl)      //��ģʽΪɨƵ����ɨ��ʱ����Ҫ�����ĸ�����
									 {
										 AD9959_Channel=UARTReceive_Buffer[3];        //��õ�ǰͨ��ֵ
										 j=arraycount-2;arraycount=0;
										 for(i=0;i<4;i++)
										 {
											 while(UARTReceive_Buffer[j]!=0x0b)   //ÿ������ĩβ����0x0b
											 {
												 	if(UARTReceive_Buffer[j]>47)
														receivebuffer[i]+=(UARTReceive_Buffer[j]-48)*(pow(10,arraycount));
													j--;arraycount++;
											 }
											 j--;
											 arraycount=0;
										 }
									 }
									 else if(Mode==ADS1115_Channel_Open)            //��ģʽΪ����ADS1115ͨ��ʱ
										 ADS1115_Channel=UARTReceive_Buffer[3];       //���ͨ��
									 else if(Mode==ADS1115_Channel_Close)           //��ģʽΪ�ر�ADS1115ͨ��ʱ
										 ADS1115_Channel=UARTReceive_Buffer[3];       //�ر�ͨ��
									 else if(Mode==ADS11152_Channel_Open)           //��ģʽΪ����ADS11152ͨ��ʱ 
										 ADS11152_Channel=UARTReceive_Buffer[3];       //���ͨ��
									 else if(Mode==ADS11152_Channel_Close)          //��ģʽΪ����ADS11152ͨ��ʱ 
										 ADS11152_Channel=UARTReceive_Buffer[3];       //���ͨ��			
									 else if(Mode==DAC5618_VoltageSet)              //������DAC5618ͨ����ѹʱ
									 {
										 DAC5618_Channel=UARTReceive_Buffer[3];        //���ͨ��
										 DAC5618Value=(UARTReceive_Buffer[4]-48)+(float)((UARTReceive_Buffer[6]-48)*0.1)+(float)((UARTReceive_Buffer[7]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[8]-48)*0.001);
									 }
									 else if(Mode==DAC56182_VoltageSet)              //������DAC5618ͨ����ѹʱ
									 {
										 DAC56182_Channel=UARTReceive_Buffer[3];        //���ͨ��
										 DAC56182Value=(UARTReceive_Buffer[4]-48)+(float)((UARTReceive_Buffer[6]-48)*0.1)+(float)((UARTReceive_Buffer[7]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[8]-48)*0.001);
									 }
									 else if(Mode==ADS1256_Channel_Open)             //��ģʽΪ��ADS1256ͨ��ʱ
									 {
										 ADS1256_Channel=UARTReceive_Buffer[3];       //���ͨ��
									 }
									 else if(Mode==ADS1256_Channel_Close)             //��ģʽΪ�ر�ADS1256ͨ��ʱ
									 {
										 ADS1256_Channel=UARTReceive_Buffer[3];
									 }
									 else if(Mode==DAC8501_VoltageSet)              //��ģʽΪ����DAC8501ͨ����ѹʱ
									 {
										 DAC8501_Channel=UARTReceive_Buffer[3];       //���ͨ��
										 DAC8501Value=(UARTReceive_Buffer[4]-48)+(float)((UARTReceive_Buffer[6]-48)*0.1)+(float)((UARTReceive_Buffer[7]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[8]-48)*0.001);
									 }
									 else if(Mode==DAC8811_VoltageSet)       //��ģʽΪ����DAC8811��ѹʱ
									 {
										 if(UARTReceive_Buffer[3]==0x2D)      //����⵽�и���ʱ
										 {
											 DAC8811Value=(UARTReceive_Buffer[4]-48)+(float)((UARTReceive_Buffer[6]-48)*0.1)+(float)((UARTReceive_Buffer[7]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[8]-48)*0.001);
											 DAC8811Value=0-DAC8811Value;
											 UARTprintf("%f\n",DAC8811Value);
										 }
										 else                                //û�и���ʱ
										 {
												DAC8811Value=(UARTReceive_Buffer[3]-48)+(float)((UARTReceive_Buffer[5]-48)*0.1)+(float)((UARTReceive_Buffer[6]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[7]-48)*0.001);	
												UARTprintf("%f\n",DAC8811Value);												
										 }
									 }
									 else if(Mode==FPGA_DataSend)
									 {
										 	for(i=3;i<arraycount;i++)                   //������������
											{
												if(UARTReceive_Buffer[i]>47)                //���ݵ�ֵ����Ϊ48������47������ֵ��С��47
															receivevalue+=(UARTReceive_Buffer[i]-48)*(pow(10,(arraycount-i-1)));  //����õ���Ӧ������
											}
									 }
									 
							 }	
							arraycount=0;i=0;j=0;
						}		
					vTaskDelay(30);
    }
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
void
UARTPRINTTaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        DemoUARTPRINTTask,                       //������
                       (const portCHAR *)"UART", //�������� 
                        UARTPRINT_STK_SIZE,              // �����ջ��С 
                        NULL,       
                        UARTPRINT_TASK_PRIO,
                        &UARTPRINTTaskHandler
    );
    
    
}
