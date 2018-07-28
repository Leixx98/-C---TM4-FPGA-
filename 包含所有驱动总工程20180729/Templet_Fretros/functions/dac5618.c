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
#include "functions/dac5618.h"
#include "functions/uartprint.h"

//*****************************************************************************

#define DAC5618_TASK_PRIO          2        //�������ȼ�
#define DAC5618_STK_SIZE           128       //�����ջ��С

//*****************************************************************************

/*******************************************
Function name              :TLV5618_SDAPort_In
Function use               :Set SDA port as input.
Function parameter         :None
Function return            :None
********************************************/
void TLV5618_SDAPort_Out(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_0,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
}

/*******************************************
�������ƣ�TLV5618_Init
��    �ܣ���ʼ��SPI����
��    ������
����ֵ  ����
********************************************/
void TLV5618_Init(void)        //DAC5618  SPI��ʼ��
{

    TLV5618_CS_HIGH;            //����Ƭѡ
    delay_us(3);
    TLV5618_SCL_SET;         //����ʱ����
    delay_us(3);      //����ʱ
   TLV5618_SDA_CLR;           //�õ�������
    delay_us(3);
}
/*******************************************
�������ƣ�DAC_Conver
��    �ܣ���һ��16λ����������������Ӧ
��    ����temp-16λ����������
����ֵ  ����
********************************************/

void DAC_Conver(uint temp)
{
        unsigned char i;
        TLV5618_SDAPort_Out(); //��������Ϊ���
        TLV5618_CS_LOW;       //����Ƭѡ���������ݴ���
        TLV5618_SCL_SET;
        for(i=0;i<16;i++)
        {
        	if((temp & 0x8000) ==0) 
                {TLV5618_SDA_CLR;} //�������λΪ0���õ������ߣ���������
            else
                {TLV5618_SDA_SET;}
            TLV5618_SCL_CLR;
            delay_us(1);
            temp<<=1;
            TLV5618_SCL_SET;        //�����ض�ȡ����
        }
        TLV5618_CS_HIGH;   //����Ƭѡ���ر����ݴ���
}

/*******************************************
�������ƣ� VoltToData
��    �ܣ���ģ���ѹֵת��Ϊ16λ��������
��    ����out_volt-ģ���ѹֵ
����ֵ  ��16λ��������
���䣺//(uint)(out_volt*4095)/(2*2.048) �Ƚ���ѹֵת��Ϊ0~4095������
        //0x0fff&  ת����16λ������������12λΪ����λ
********************************************/
uint VoltToData(float out_volt) //˵��refΪ�ο�Դ��out_voltΪ���ģ���ѹֵ
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
void Write_A(float out_volt)
{
   int Data_A;
   Data_A=0xc000|VoltToData(out_volt);//Aͨ��
   DAC_Conver(Data_A);
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
void Write_B(float out_volt)
{
   int Data_A;
   Data_A=0x4000|VoltToData(out_volt);//Bͨ��
   DAC_Conver(Data_A);
}

/*******************************************
Function name              :DAC5618_DemoTask
Function use               :Write the voltage to the dac5618
Function parameter         :None
Function return            :None
********************************************/
void DAC5618_DemoTask(void *pvParameters)
{
  TLV5618_Init();	
	while(1)
	{
			if(Mode==DAC5618_VoltageSet)              //������DAC5618ͨ����ѹʱ
			{
				if(DAC5618_Channel==1)                  //���Ϊͨ��1����д������
					Write_A(DAC5618Value);
				if(DAC5618_Channel==2)                  //���Ϊͨ��2����д������
					Write_B(DAC5618Value);
				Mode=0xff;                              //���ģʽ
			}
			vTaskDelay(100);
	}
}
/*******************************************
Function name              :DAC5618TaskInit
Function use               :Inie the ADS1256 task
Function parameter         :None
Function return            :None
********************************************/
void
DAC5618TaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        DAC5618_DemoTask,    //������
                       (const portCHAR *)"DAC5618", //�������� 
                        DAC5618_STK_SIZE,              // �����ջ��С 
                        NULL,       
                        DAC5618_TASK_PRIO,
                        NULL
    );
        
}

