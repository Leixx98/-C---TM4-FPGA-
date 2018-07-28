#ifndef __UARTRECEIVE_H__
#define __UARTRECEIVE_H__

#define SORT_ENABLE    0x01
#define SORT_DISABLE   0x00
#define FPGA_DrawLineEnable  0x02
#define FPGA_DrawLineDisable  0x03


extern uint8_t SORT_Status;                             //�����־λ
extern uint8_t UART_ReceiveBuffer[6500];                //UART2���ڽ�������
extern uint32_t UART_ValueBuffer[300];                   //����ϲ����ֵ������ 
extern uint8_t FPGA_DrawLine;                         //��ͼ��־λ


void UARTRECEIVETaskInit(void);

#endif



