#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"

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
#include "functions/uartprint.h"
#include "functions/uartreceive.h"
#include "functions/fpgadrawline.h"


#define FPGADRAWLINE_TASK_PRIO          3        //�������ȼ�
#define FPGADRAWLINE_STK_SIZE           128       //�����ջ��С 
uint8_t UART2_ReceiveBuffer[2047];                //UART2���ڽ�������
int16_t UART2_ValueBuffer[600];                   //����ϲ����ֵ������   
//*****************************************************************************
//
// ��ͼ����
//
//*****************************************************************************
void
DemoFPGADRAWLINETask(void *pvParameters)
{
	uint16_t i=0;
	uint8_t f=0xff;
	uint32_t linepoint=0;
	while(1)
	{
		if(FPGA_DrawLine==FPGA_DrawLineEnable)
		{
			FPGA_DrawLine=FPGA_DrawLineDisable;
			taskENTER_CRITICAL();                //�����ٽ�Σ���ֹ���ձ����
			//���մ�������        
			printf("ref_stop%c%c%c",f,f,f);             //�ر���Ļˢ�£������ݷ��������ˢ����Ļ
			printf("cle 1,0%c%c%c",f,f,f);              //�����Ļ
			for(i=0;i<4;i++)
			{
				printf("add 1,0,%d%c%c%c",0,f,f,f);
				printf("add 1,0,%d%c%c%c",0,f,f,f);	
			}				
			for(i=4;i<254;i++)
			{
					linepoint=sqrt(UART_ValueBuffer[i]);
					printf("add 1,0,%d%c%c%c",linepoint/100,f,f,f);
					printf("add 1,0,%d%c%c%c",linepoint/100,f,f,f);
			}
			printf("ref_star%c%c%c",f,f,f);     //�ָ�ˢ��
			taskEXIT_CRITICAL();                 //�˳��ٽ��
		}
		 vTaskDelay(100);
	}
}
//*****************************************************************************
//
// �����������ݻ�ͼ����
//
//*****************************************************************************
void
FPGADRAWLINETaskInit(void)
{
    //
    // Create the fpgadrawline task.
    //
    xTaskCreate(
                        DemoFPGADRAWLINETask,                       //������
                       (const portCHAR *)"FPGADRAWLINE", //�������� 
                        FPGADRAWLINE_STK_SIZE,              // �����ջ��С 
                        NULL,       
                        FPGADRAWLINE_TASK_PRIO,
                        NULL
    );
    
    
}

