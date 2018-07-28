#include <stdint.h>
#include <stdbool.h>

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
#include "functions/key.h"

#define KEY_TASK_PRIO          2        //�������ȼ�
#define KEY_STK_SIZE           128       //�����ջ��С 
extern TaskHandle_t UARTPRINTTaskHandler;            //������
//*****************************************************************************
//
// ��������
//
//*****************************************************************************
void
DemoKEYTask(void *pvParameters)
{
    uint8_t ui8Buttons;
    uint8_t ui8ButtonsChanged;
		static uint8_t keychanged; //������־λ������������ʱ��Ϊ1 
    static uint8_t keyvalue;
    while(1)
    {
            //��ⰴ��״̬
            ui8Buttons = ButtonsPoll(&ui8ButtonsChanged, 0);
            if(BUTTON_PRESSED(USR_SW1, ui8Buttons, ui8ButtonsChanged))
						{
                keyvalue=keyvalue>=6?1:keyvalue+1;
								keychanged = 1;
						}
            else if(BUTTON_PRESSED(USR_SW2, ui8Buttons, ui8ButtonsChanged)) 
						{
                keyvalue=keyvalue<=1?6:keyvalue-1;
								keychanged = 1;
						}
            if(keychanged)
						{
							UARTprintf("��ǰ����ֵ��%d\n",keyvalue);
							keychanged = 0;
							switch(keyvalue)
							{
									case 1:
											taskENTER_CRITICAL();   /* �����ٽ��� */                   
											UARTprintf("�ر��ж�\n");
											taskEXIT_CRITICAL();  	/* �˳��ٽ��� */
											portDISABLE_INTERRUPTS();      /* �ر��ж�*/
											break;      
									case 2:
											taskENTER_CRITICAL();   /* �����ٽ��� */	
											UARTprintf("�ָ��ж�\n");
											taskEXIT_CRITICAL();  	/* �˳��ٽ��� */ 
											portENABLE_INTERRUPTS();     /*�ָ��ж�*/
											break;
									case 3:
											taskENTER_CRITICAL();   /* �����ٽ��� */	
											UARTprintf("�ر�TIM\n");
											IntDisable(INT_TIMER0A);	
											IntDisable(INT_TIMER1A);
											taskEXIT_CRITICAL();  	/* �˳��ٽ��� */ 
											break;
									case 4:
											taskENTER_CRITICAL();   /* �����ٽ��� */	
											UARTprintf("�ָ�TIM\n");
											IntEnable(INT_TIMER0A);	
											IntEnable(INT_TIMER1A);
											taskEXIT_CRITICAL();  	/* �˳��ٽ��� */ 
											break;
									case 5:
											taskENTER_CRITICAL();   /* �����ٽ��� */	
											UARTprintf("����UART\n");
											vTaskSuspend(UARTPRINTTaskHandler);
											taskEXIT_CRITICAL();  	/* �˳��ٽ��� */ 
											break;		
									case 6:
											taskENTER_CRITICAL();   /* �����ٽ��� */	
											UARTprintf("�ָ�UART\n");
											vTaskResume(UARTPRINTTaskHandler);
											taskEXIT_CRITICAL();  	/* �˳��ٽ��� */ 
											break;												
							}
					}
    }
}
//*****************************************************************************
//
// ������������
//
//*****************************************************************************
void
KEYTaskInit(void)
{
		//������ʼ��
		ButtonsInit();
    //
    // Create the LED task.
    //
    xTaskCreate(
                        DemoKEYTask,                       //������
                       (const portCHAR *)"KEY", //�������� 
                        KEY_STK_SIZE,              // �����ջ��С 
                        NULL,       
                        KEY_TASK_PRIO,
                        NULL
    );
    
    
}


	



