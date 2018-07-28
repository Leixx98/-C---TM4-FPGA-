#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"

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
#include "utils/buttons.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "functions/led_task.h"
#include "functions/delay.h"
#include "functions/uartprint.h"
#include "functions/uartreceive.h"



#define UARTRECEIVE_TASK_PRIO          4        //�������ȼ�
#define UARTRECEIVE_STK_SIZE           256       //�����ջ��С 

uint8_t SORT_Status;                             //�����־λ
uint8_t UART_ReceiveBuffer[6500];                //UART2���ڽ�������
uint32_t UART_ValueBuffer[300];                 //�ϲ����ֵ������
uint8_t FPGA_DrawLine=0;                         //��ͼ��־λ

//*****************************************************************************
//
// ��������
//
//*****************************************************************************
void
DemoUARTRECEIVETask(void *pvParameters)
{
	uint16_t i=0,j=0;
	while(1)
	{

		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4,GPIO_PIN_4);
		if(UARTCharsAvail(UART2_BASE))
		{			
			taskENTER_CRITICAL();                //�����ٽ�Σ���ֹ���ձ����
			//���մ�������
			for(i=0;i<1600;i++)
			{
				UART_ReceiveBuffer[i]=UARTCharGet(UART2_BASE);
			}
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4,0);
			while(UARTCharsAvail(UART2_BASE))
				UART_ReceiveBuffer[i++]=UARTCharGet(UART2_BASE);      //������ڶ��໺��
			for(i=0;i<1600;i++)                    //�ϲ�����
			{
				if(UART_ReceiveBuffer[i]==0xab&&UART_ReceiveBuffer[i+1]==0xcd)
				{
						UART_ValueBuffer[++j]=(uint32_t)(UART_ReceiveBuffer[i-4]<<24)|(uint32_t)(UART_ReceiveBuffer[i-3]<<16)
																	|(uint32_t)(UART_ReceiveBuffer[i-2]<<8)|(uint32_t)(UART_ReceiveBuffer[i-1]);
//					  UARTprintf("%d  %d\n",j,UART_ValueBuffer[j]);
				}	
			}
			j=0;
			taskEXIT_CRITICAL();                 //�˳��ٽ��    
			SORT_Status=SORT_ENABLE;
			FPGA_DrawLine=FPGA_DrawLineEnable;
		}
		 vTaskDelay(500);
	}
}

//*****************************************************************************
//
// ����������������
//
//*****************************************************************************
void
UARTRECEIVETaskInit(void)
{
    //
    // Create the fpgadrawline task.
    //
    xTaskCreate(
                        DemoUARTRECEIVETask,                       //������
                       (const portCHAR *)"UARTREVEIVE", //�������� 
                        UARTRECEIVE_STK_SIZE,              // �����ջ��С 
                        NULL,       
                        UARTRECEIVE_TASK_PRIO,
                        NULL
    );
    
    
}

