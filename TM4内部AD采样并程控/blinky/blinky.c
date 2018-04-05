#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/buttons.h"
#include "driverlib/adc.h"
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

int32_t t;

//*****************************************************************************
//
// Delay for the specified number of seconds.  Depending upon the current
// SysTick value, the delay will be between N-1 and N seconds (i.e. N-1 full
// seconds are guaranteed, aint32_t with the remainder of the current second).
//
//*****************************************************************************

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
UART0Configure(void)
{
	//ʹ��UART0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	//ʹ��GPIOA
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	UARTClockSourceSet(UART0_BASE,UART_CLOCK_PIOSC);
	//����UART1����
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);	
	//����UART1��ز���
//	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(),115200,
//											(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
//												UART_CONFIG_PAR_NONE));
//	//ʹ��UART1�ж�
//	IntEnable(INT_UART1);
//  UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);	
//	//UART1ע���ж�
//	UARTIntRegister(UART0_BASE, UART1IntHandler);
	UARTStdioConfig(0,115200,16000000);
}

void AD8253PinConfig()
{
    //使能时钟
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //引脚设置为输出
    GPIODirModeSet(GPIO_PORTC_BASE,GPIO_PIN_6|GPIO_PIN_7,GPIO_DIR_MODE_OUT);
    //设置上拉
    GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
}




//������
int main(void)
{	
  uint32_t ADC_Value=1,ADC_LasValue=0;
  //for循环用和改变增益标志位  
  uint8_t i,Gain=1;           
//	char low,high;
//	char b=0xff;
	//����ʱ��
	SysCtlClockSet(SYSCTL_SYSDIV_10| SYSCTL_USE_PLL| SYSCTL_OSC_MAIN |
								SYSCTL_XTAL_16MHZ);
	//���ô���
	UART0Configure();
	//ʹ�ܶ˿�F
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//ʹ�ܶ˿�E
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	//ʹ��ADC0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    
	//��������
    AD8253PinConfig();
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1);
	
	ADCHardwareOversampleConfigure(ADC0_BASE,8); 
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0); 
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0|ADC_CTL_IE | ADC_CTL_END );
	ADCSequenceEnable(ADC0_BASE, 0);
	ADCProcessorTrigger(ADC0_BASE, 0); 

//	ADCIntClear(ADC0_BASE,0);
	while(1)
	{
        while(ADC_Value!=ADC_LasValue)
        {
                ADC_LasValue = ADC_Value;
				ADCProcessorTrigger(ADC0_BASE, 0); 
				while(!ADCIntStatus(ADC0_BASE, 0, false)) 
				{
				}  
				ADCIntClear(ADC0_BASE,0);
				ADCSequenceDataGet(ADC0_BASE, 0, &ADC_Value );
				ADC_Value =ADC_Value*825>>10;
        }
		UARTprintf(" %dmV ",ADC_Value);     
        if(Gain==1&&ADC_Value<130)
         {
             Gain = 10;
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_PIN_6);
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
         }
         
         if(Gain==10&&ADC_Value>2400)
         {
            Gain = 1;
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
         }
        ADC_Value = true;ADC_LasValue = false;
				SysCtlDelay(SysCtlClockGet()/5);
        
	}
}
