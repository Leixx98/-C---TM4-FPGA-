#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

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

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "utils/uartstdio.h"

#include "functions/delay.h"
#include "functions/DAC56182.h"
#include "functions/uartprint.h"

//*****************************************************************************

#define DAC56182_TASK_PRIO          2        //�������ȼ�
#define DAC56182_STK_SIZE           128       //�����ջ��С

//*****************************************************************************

/*******************************************
Function name              :TLV56182_SDAPort_In
Function use               :Set SDA port as input.
Function parameter         :None
Function return            :None
********************************************/
void TLV56182_SDAPort_Out(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_0,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
}

/*******************************************
�������ƣ�TLV56182_Init
��    �ܣ���ʼ��SPI����
��    ������
����ֵ  ����
********************************************/
void DAC56182_Init(void)        //DAC56182  SPI��ʼ��
{

    TLV56182_CS_HIGH;            //����Ƭѡ
    delay_us(3);
    TLV56182_SCL_SET;         //����ʱ����
    delay_us(3);      //����ʱ
   TLV56182_SDA_CLR;           //�õ�������
    delay_us(3);
}
/*******************************************
�������ƣ�DAC_Conver
��    �ܣ���һ��16λ����������������Ӧ
��    ����temp-16λ����������
����ֵ  ����
********************************************/

void DAC_Conver2(uint temp)
{
        unsigned char i;
        TLV56182_SDAPort_Out(); //��������Ϊ���
        TLV56182_CS_LOW;       //����Ƭѡ���������ݴ���
        TLV56182_SCL_SET;
        for(i=0;i<16;i++)
        {
        	if((temp & 0x8000) ==0) 
                {TLV56182_SDA_CLR;} //�������λΪ0���õ������ߣ���������
            else
                {TLV56182_SDA_SET;}
            TLV56182_SCL_CLR;
            delay_us(1);
            temp<<=1;
            TLV56182_SCL_SET;        //�����ض�ȡ����
        }
        TLV56182_CS_HIGH;   //����Ƭѡ���ر����ݴ���
}

/*******************************************
�������ƣ� VoltToData
��    �ܣ���ģ���ѹֵת��Ϊ16λ��������
��    ����out_volt-ģ���ѹֵ
����ֵ  ��16λ��������
���䣺//(uint)(out_volt*4095)/(2*2.048) �Ƚ���ѹֵת��Ϊ0~4095������
        //0x0fff&  ת����16λ������������12λΪ����λ
********************************************/
uint VoltToData2(float out_volt) //˵��refΪ�ο�Դ��out_voltΪ���ģ���ѹֵ
{
   int temp2;
   temp2=(unsigned int)((out_volt*4096)/(4*2.048));
   return(temp2&0x0fff);
}//����ѹֵת��Ϊ��Ӧ��12λ����������

/*******************************************
�������ƣ� Write_A
��    �ܣ���Aͨ��д����
��    ����out_volt-ģ���ѹֵ
����ֵ  ����
���䣺//(uint)(out_volt*4095)/(2*2.048) �Ƚ���ѹֵת��Ϊ0~4095������
        //0x0fff&  ת����16λ������������12λΪ����λ
        //0xc000|  ǰ�����4λΪ1100������λ��������ģʽ���������ģ�д���ݵ�Aͨ��
********************************************/
void Write_A2(float out_volt)
{
   int Data_A;
   Data_A=0xc000|VoltToData2(out_volt);//Aͨ��
   DAC_Conver2(Data_A);
}

/*******************************************
�������ƣ� Write_B
��    �ܣ���Aͨ��д����
��    ����out_volt-ģ���ѹֵ
����ֵ  ����
���䣺//(uint)(out_volt*4095)/(2*2.048) �Ƚ���ѹֵת��Ϊ0~4095������
        //0x0fff&  ת����16λ������������12λΪ����λ
        //0xc000|  ǰ�����4λΪ1100������λ��������ģʽ���������ģ�д���ݵ�Aͨ��
********************************************/
void Write_B2(float out_volt)
{
   int Data_A;
   Data_A=0x4000|VoltToData2(out_volt);//Bͨ��
   DAC_Conver2(Data_A);
}

/*******************************************
Function name              :DAC56182_DemoTask
Function use               :Write the voltage to the DAC56182
Function parameter         :None
Function return            :None
********************************************/
void DAC56182_DemoTask(void *pvParameters)
{
  DAC56182_Init();	

	while(1)
	{
			if(Mode==DAC56182_VoltageSet)              //������DAC5618ͨ����ѹʱ
			{
				if(DAC56182_Channel==1)                  //���Ϊͨ��1����д������
					Write_A2(DAC56182Value);
				if(DAC56182_Channel==2)                  //���Ϊͨ��2����д������
					Write_B2(DAC56182Value);
				Mode=0xff;                              //���ģʽ
			}
			vTaskDelay(100);
	}
}
/*******************************************
Function name              :DAC56182TaskInit
Function use               :Inie the ADS1256 task
Function parameter         :None
Function return            :None
********************************************/
void
DAC56182TaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        DAC56182_DemoTask,    //������
                       (const portCHAR *)"DAC56182", //�������� 
                        DAC56182_STK_SIZE,              // �����ջ��С 
                        NULL,       
                        DAC56182_TASK_PRIO,
                        NULL
    );
        
}

