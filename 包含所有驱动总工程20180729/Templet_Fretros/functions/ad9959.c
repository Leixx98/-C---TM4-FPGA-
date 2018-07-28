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
#include "utils/buttons.h"

#include "functions/delay.h"
#include "functions/ad9959.h"
#include "functions/uartprint.h"

//*****************************************************************************

#define AD9959_TASK_PRIO           3        //�������ȼ�
#define AD9959_STK_SIZE           256       //�����ջ��С

//*****************************************************************************

u8 CSR_DATA0[1] = {0x10};     // �� CH0
u8 CSR_DATA1[1] = {0x20};      // �� CH1
u8 CSR_DATA2[1] = {0x40};      // �� CH2
u8 CSR_DATA3[1] = {0x80};      // �� CH3		
u8 CSR_DATA[1]   = {0xc0};      //��CH2,CH3
																	
u8 FR1_DATA[3] = {0xD0,0x00,0x00};
u8 FR2_DATA[2] = {0x08,0x00};//default Value = 0x0000
u8 CFR_DATA[3] = {0x00,0x03,0x00};//default Value = 0x000302	   
																	
u8 CPOW0_DATA[2] = {0x00,0x00};//default Value = 0x0000   @ = POW/2^14*360
																	


u8 LSRR_DATA[2] = {0x00,0x00};//default Value = 0x----
																	
u8 RDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------
																	
u8 FDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------

//AD9959��ʼ��
void Init_AD9959(void)  
{ 

// 	u8 FR1_DATA[3] = {0xD0,0x00,0x00};//20��Ƶ Charge pump control = 75uA FR1<23> -- VCO gain control =0ʱ system clock below 160 MHz;
    
    //ʱ�ӳ�ʼ��
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);

	//��������Ϊ���
	GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5,GPIO_DIR_MODE_OUT);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_4|GPIO_PIN_5,GPIO_DIR_MODE_OUT);
    GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2,GPIO_DIR_MODE_OUT);
    GPIODirModeSet(GPIO_PORTH_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_DIR_MODE_OUT);
	//��������Ϊ2mA������
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_4|GPIO_PIN_5 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	    
    GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	        
		
	Intserve();  //IO�ڳ�ʼ��
    IntReset();  //AD9959��λ  

  WriteData_AD9959(FR1_ADD,3,FR1_DATA,1);//д���ܼĴ���1
//  WriteData_AD9959(FR2_ADD,2,FR2_DATA,1);
//  WriteData_AD9959(CFR_ADD,3,CFR_DATA,1);
//  WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
//  WriteData_AD9959(ACR_ADD,3,ACR_DATA,0);
//  WriteData_AD9959(LSRR_ADD,2,LSRR_DATA,0);
//  WriteData_AD9959(RDW_ADD,2,RDW_DATA,0);
//  WriteData_AD9959(FDW_ADD,4,FDW_DATA,1);
   //д���ʼƵ��
//  Write_Quadrature(10000);
	
////	Write_frequence(3,50);
	Write_frequence(1,100000);
////	Write_frequence(1,50);
////	Write_frequence(2,50);

	
//    Write_Phase(2,83);
} 
//��ʱ
void delay1 (u32 length)
{
	length = length*12;
   while(length--);
}
//IO�ڳ�ʼ��
void Intserve(void)		   
{   
    AD9959_PWR_CLR;
    CS_SET;
    SCLK_CLR;
    UPDATE_CLR;
    PS0_CLR;
    PS1_CLR;
    PS2_CLR;
    PS3_CLR;
    SDIO0_CLR;
    SDIO1_CLR;
    SDIO2_CLR;
    SDIO3_CLR;
}
//AD9959��λ
void IntReset(void)	  
{
  Reset_CLR;
	delay1(1);
	Reset_SET;
	delay1(30);
	Reset_CLR;
}
 //AD9959��������
void IO_Update(void)  
{
	UPDATE_CLR;
	delay1(4);
	UPDATE_SET;
	delay1(4);
	UPDATE_CLR;
}
/*--------------------------------------------
�������ܣ�������ͨ��SPI��AD9959д����
RegisterAddress: �Ĵ�����ַ
NumberofRegisters: �����ֽ���
*RegisterData: ������ʼ��ַ
temp: �Ƿ����IO�Ĵ���
----------------------------------------------*/
void WriteData_AD9959(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp)
{
	u8	ControlValue = 0;
	u8	ValueToWrite = 0;
	u8	RegisterIndex = 0;
	u8	i = 0;

	ControlValue = RegisterAddress;
//д���ַ
	SCLK_CLR;
	CS_CLR;	 
	for(i=0; i<8; i++)
	{
		SCLK_CLR;
		if(0x80 == (ControlValue & 0x80))
		SDIO0_SET;	  
		else
		SDIO0_CLR;	  
		SCLK_SET;
		ControlValue <<= 1;
         delay1(2);
	}
	SCLK_CLR;
//д������
	for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i=0; i<8; i++)
		{
			SCLK_CLR;
			if(0x80 == (ValueToWrite & 0x80))
			SDIO0_SET;	  
			else
			SDIO0_CLR;	  
			SCLK_SET;
			ValueToWrite <<= 1;
             delay1(2);
		}
		SCLK_CLR;		
	}	
	if(temp==1)
        IO_Update();	
    CS_SET;
} 
/*---------------------------------------
�������ܣ�����ͨ�����Ƶ��
Channel:  ���ͨ��
Freq:     ���Ƶ��
---------------------------------------*/
void Write_frequence(u8 Channel,u32 Freq)
{	 
		u8 CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//�м����
	  u32 Temp;            
	  Temp=(u32)Freq*8.589934592;	   //������Ƶ�����ӷ�Ϊ�ĸ��ֽ�  4.294967296=(2^32)/500000000
	  CFTW0_DATA[3]=(u8)Temp;
	  CFTW0_DATA[2]=(u8)(Temp>>8);
	  CFTW0_DATA[1]=(u8)(Temp>>16);
	  CFTW0_DATA[0]=(u8)(Temp>>24);
	  if(Channel==0)	  
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA0,0);//���ƼĴ���д��CH0ͨ��
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.���CH0�趨Ƶ��
		}
	  else if(Channel==1)	
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA1,0);//���ƼĴ���д��CH1ͨ��
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.���CH1�趨Ƶ��	
	  }
	  else if(Channel==2)	
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA2,0);//���ƼĴ���д��CH2ͨ��
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.���CH2�趨Ƶ��	
	  }
	  else if(Channel==3)	
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0);//���ƼĴ���д��CH3ͨ��
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.���CH3�趨Ƶ��	
	  }																																																																										 
	
} 
/*---------------------------------------
�������ܣ�����ͨ���������
Channel:  ���ͨ��
Ampli:    �������
---------------------------------------*/
void Write_Amplitude(u8 Channel, u16 Ampli)
{ 
	u16 A_temp;//=0x23ff;
	u8 ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 
	
  A_temp=Ampli|0x1000;
	ACR_DATA[2] = (u8)A_temp;  //��λ����
  ACR_DATA[1] = (u8)(A_temp>>8); //��λ����
  if(Channel==0)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA0,0); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
	}
  else if(Channel==1)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA1,0); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1);
	}
  else if(Channel==2)
  {
	  WriteData_AD9959(CSR_ADD,1,CSR_DATA2,0); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
	}
  else if(Channel==3)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
	}
}
/*---------------------------------------
�������ܣ�����ͨ�������λ
Channel:  ���ͨ��
Phase:    �����λ,��Χ��0~16383(��Ӧ�Ƕȣ�0��~360��)
---------------------------------------*/
void Write_Phase(u8 Channel,u16 Phase)
{
	u16 P_temp=0;
  P_temp=(u16)((Phase*16384)/360);
	CPOW0_DATA[1]=(u8)P_temp;
	CPOW0_DATA[0]=(u8)(P_temp>>8);
	if(Channel==0)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA0,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
  else if(Channel==1)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA1,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
  else if(Channel==2)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA2,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
  else if(Channel==3)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
}	 


/*---------------------------------------
�������ܣ�����˫ͨ�����Ƶ��
Freq:     ���Ƶ��
Phase:    ���Ƶ�ʣ�0~100M
---------------------------------------*/
void Write_Quadrature(uint32_t Freq)
{		
      u8 ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 
      u8 CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//�м����
	  u32 Temp;      
      u16 A_temp;//=0x23ff;
      u16 P_temp=0;
      P_temp=(u16)((90*16384)/360);
	 CPOW0_DATA[1]=(u8)P_temp;
	 CPOW0_DATA[0]=(u8)(P_temp>>8);
      
      A_temp=867|0x1000;      
	  Temp=(u32)Freq*8.589934592;	   //������Ƶ�����ӷ�Ϊ�ĸ��ֽ�  4.294967296=(2^32)/500000000
	  
      CFTW0_DATA[3]=(u8)Temp;
	  CFTW0_DATA[2]=(u8)(Temp>>8);
	  CFTW0_DATA[1]=(u8)(Temp>>16);
	  CFTW0_DATA[0]=(u8)(Temp>>24);
      
      ACR_DATA[2] = (u8)A_temp;  //��λ����
      ACR_DATA[1] = (u8)(A_temp>>8); //��λ����
      
      WriteData_AD9959(CSR_ADD,1,CSR_DATA,0);   //ѡ��ͨ��2��3
      WriteData_AD9959(ACR_ADD,3,ACR_DATA,1);   //д�����
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.���CH2,3�趨Ƶ��	
      
       WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0);   //ѡ��ͨ��3
       WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1); //д����λֵ
      
}

/*******************************************
Function name              :AD9959_DemoTask
Function use               :Run the AD9959 to set Frequency of channel 2,3
Function parameter         :None
Function return            :None
********************************************/
void AD9959_DemoTask(void *pvParameters){
	uint8_t Sweep_Freq=0,Sweep_Ampl=0;                         //��ӦɨƵ��ɨ��
	uint32_t start=0,final=0,step=0,time=0;                        //��Ӧ��ʼ����ֹ��������ʱ��
	uint32_t frequency=10000,amplitude=500;                //���ó�ʼƵ�ʣ�����
	Init_AD9959();
	Write_frequence(0,frequency);
	Write_frequence(0,amplitude);
	while(1)
	{
		if(Mode==AD9959_Point_Freq)                          //��⵽ģʽΪ��Ƶģʽʱ
		{
			UARTprintf("%d\n",receivevalue);          					//���������
			Sweep_Ampl=0;Sweep_Freq=0;                          //�ر�ɨƵ��ɨ����
			Write_frequence(AD9959_Channel,receivevalue);       //д��õ���Ƶ��ֵ
			receivevalue=0;                                     //��0
			Mode=0xff;                                          //�����ģʽ
		}
		else if(Mode==AD9959_Point_Ampl)                      //��⵽ģʽΪ��Ƶģʽʱ
		{
			UARTprintf("%d\n",receivevalue);          //���������
			Sweep_Ampl=0;Sweep_Freq=0;													//�ر�ɨƵ��ɨ����
			Write_Amplitude(AD9959_Channel,receivevalue);				//д��õ��ķ���ֵ
			receivevalue=0; 																		//��0
			Mode=0xff; 																					//�����ģʽ
		}
		else if(Mode==AD9959_Sweep_Freq)                     //��⵽ģʽΪɨƵģʽʱ
		{
			UARTprintf("%d\n",receivebuffer[3]);
			UARTprintf("%d\n",receivebuffer[2]);
			UARTprintf("%d\n",receivebuffer[1]);
			UARTprintf("%d\n",receivebuffer[0]);
			Sweep_Freq=1;                												//��ɨƵģʽ
			start=receivebuffer[3];receivebuffer[3]=0;          //�õ��ĸ�����������������
			final=receivebuffer[2];receivebuffer[2]=0;
			step=receivebuffer[1];receivebuffer[1]=0;
			time=receivebuffer[0];receivebuffer[0]=0;
			
			frequency=start;                                    //д���ʼƵ��
			
			Mode=0xff;                                          //�����ģʽ����ͬ
		}
		else if(Mode==AD9959_Sweep_Ampl)
		{
			UARTprintf("%d\n",receivebuffer[3]);
			UARTprintf("%d\n",receivebuffer[2]);
			UARTprintf("%d\n",receivebuffer[1]);
			UARTprintf("%d\n",receivebuffer[0]);			
			Sweep_Ampl=1;
			start=receivebuffer[3];receivebuffer[3]=0;
			final=receivebuffer[2];receivebuffer[2]=0;
			step=receivebuffer[1];receivebuffer[1]=0;
			time=receivebuffer[0];receivebuffer[0]=0;
			
			amplitude=start;
			Mode=0xff;
		}		
		
		if(Sweep_Freq)
		{
			UARTprintf("%d\n",frequency);
			Write_frequence(AD9959_Channel,frequency);
			frequency+=step;
			if(frequency==final)
				frequency=start;
			vTaskDelay(time);
		}
		
		if(Sweep_Ampl)
		{
			Write_Amplitude(AD9959_Channel,amplitude);
			amplitude+=step;
			if(amplitude==final)
				amplitude=start;
			vTaskDelay(time);
		}
		
		vTaskDelay(10);
		
	}
}

/*******************************************
Function name              :AD9959TaskInit
Function use               :Get data from ADS1256 in single channel and continuously mode.
Function parameter         :Channel  -- A single channel
Function return            :uint32_t data -- The data from ADS1256
********************************************/
void
AD9959TaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        AD9959_DemoTask,    //������
                       (const portCHAR *)"AD9959", //�������� 
                        AD9959_STK_SIZE,              // �����ջ��С 
                        NULL,       
                        AD9959_TASK_PRIO,
                        NULL
    );
        
}



