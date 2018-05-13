/*******************************************
项目：移相器
功能：将输入的方波按步进3.6度前移或滞后
标准电压：3.3V
最低电压：1.645V
*******************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"
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
#include "utils/ad9959.h"


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

int32_t t;
uint8_t UART_Buffer[20];
uint8_t Array_Count;
//*****************************************************************************
//
// Delay for the specified number of seconds.  Depending upon the current
// SysTick value, the delay will be between N-1 and N seconds (i.e. N-1 full
// seconds are guaranteed, aint32_t with the remainder of the current second).
//
//*****************************************************************************
int fputc(int ch, FILE *f)
{
  UARTCharPut(UART1_BASE,(uint8_t)ch);
  return ch;
}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_GPIOPinConfigure(GPIO_PC4_U1RX);
    ROM_GPIOPinConfigure(GPIO_PC5_U1TX);   
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    ROM_GPIOPinConfigure(GPIO_PC6_U3RX);
    ROM_GPIOPinConfigure(GPIO_PC7_U3TX);   
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);    
    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
    UARTConfigSetExpClk(UART1_BASE,SysCtlClockGet(),9600,
                                              (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
                                              UART_CONFIG_PAR_NONE));
    UARTConfigSetExpClk(UART3_BASE,SysCtlClockGet(),115200,
                                              (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
                                              UART_CONFIG_PAR_NONE));                                          
}



//主函数
int main(void)
{
    uint8_t i;
    uint16_t AmpValue=801;
    uint16_t PhaValue=83;
    uint32_t FreValue=1000;
	//设置时钟
	SysCtlClockSet(SYSCTL_SYSDIV_10| SYSCTL_USE_PLL| SYSCTL_OSC_MAIN |
								SYSCTL_XTAL_16MHZ);
	//配置串口
	ConfigureUART();
    //AD9959初始化
    Init_AD9959(); 
	//使能GPIOF
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//设置PF1为输出
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	//设置PF1为2mA弱上拉
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	//点灯
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
	while(1)
	{
               //捕捉到了数据头
               if(UARTCharsAvail(UART1_BASE))
               {       
                    while(UART_Buffer[Array_Count]!=0x0d)
                    {
                        Array_Count++;
                        UART_Buffer[Array_Count] = UARTCharGet(UART1_BASE);
                    }
                       if(UART_Buffer[1]==0x0a)                
                       {
                           if(UART_Buffer[2]==0x01)
                               AmpValue+=1;
                           else if(UART_Buffer[2]==0x02)
                               AmpValue-=1;
                       }
                       
                        else if(UART_Buffer[1]==0x0b)
                        {
                            if(UART_Buffer[2]==0x01)
                            {
                                FreValue=0;
                                for(i=0;i<Array_Count;i++)
                                {
                                  if(UART_Buffer[i]>47)
                                        FreValue+=(UART_Buffer[i]-48)*(pow(10,(Array_Count-i-1)));
                                }
                            }
                            
                            else if(UART_Buffer[2]==0x02)
                            {
                                PhaValue = 0;
                                 for(i=0;i<Array_Count;i++)
                                {
                                  if(UART_Buffer[i]>47)
                                       PhaValue +=(UART_Buffer[i]-48)*(pow(10,(Array_Count-i-1)));
                                }
                            }
                        }
                        UARTprintf(" %d ",AmpValue);
                        UARTprintf(" %d ",FreValue);  
                        Write_frequence(2,FreValue);                            
                        Write_frequence(3,FreValue);   
                        Write_Phase(3,PhaValue);
                        if(UARTCharsAvail(UART1_BASE))
                            Array_Count=UARTCharGet(UART1_BASE);
                        Array_Count = 0;
                }                
	}
	
}
