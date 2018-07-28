#include <stdint.h>
#include <stdbool.h>
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
#include "driverlib/timer.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "utils/uartstdio.h"


//*****************************************************************************
//
// The interrupt handler for the first timer interrupt.
//
//*****************************************************************************
void
Timer0IntHandler(void)
{
    // Clear the timer interrupt.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //�������
    UARTprintf("TIM0�䳡\n");
}

void
Timer1IntHandler(void)
{
    // Clear the timer interrupt.
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //�������
    UARTprintf("TIM1���\n");
}

void TIM0_Init(uint32_t time)
{
   // Enable processor interrupts.
    IntMasterEnable();
    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);    
    // Configure the two 32-bit periodic timers.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    //���ü�ʱʱ��,��λHZ
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/(time-1));
    // Setup the interrupts for the timer timeouts.
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //д�붨ʱ���жϵ����ȼ�
    IntPrioritySet(INT_TIMER0A_TM4C129, 2);
    //ʹ�ܶ�ʱ��
    TimerEnable(TIMER0_BASE, TIMER_A);
}

void TIM1_Init(uint32_t time)
{
   // Enable processor interrupts.
    IntMasterEnable();
    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);    
    // Configure the two 32-bit periodic timers.
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    //���ü�ʱʱ��,��λHZ
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet()/(time-1));
    // Setup the interrupts for the timer timeouts.
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    //д�붨ʱ���жϵ����ȼ�
    IntPrioritySet(INT_TIMER1A_TM4C129, 3);
    //ʹ�ܶ�ʱ��
    TimerEnable(TIMER1_BASE, TIMER_A);
}





