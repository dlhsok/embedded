#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "UART_LPC17xx.h"
#include "I2C_LPC17xx.h"
#include "Driver_I2C.h"
#include "Driver_USART.h"
#include <string.h>

const unsigned char STEPMOTOR_Step[8][4] = {{1,1,1,0},{1,1,0,0},{1,1,0,1},{1,0,0,1},{1,0,1,1},{0,0,1,1},{0,1,1,1},{0,1,1,0}};
unsigned char uart0_rxbuf[16];
unsigned char uart0_rxindex = 0;
unsigned char uart0_rxdone = 0;
	
unsigned char uart1_rxbuf[16];
unsigned char uart1_rxindex = 0;
unsigned char uart1_rxdone = 0;
char txbuf[50];


void Delay()
{
	int i=0;
	for (i=0;i<60000;i++)
	{
	}
}

void RTC_IRQHandler()
{
	int i,j;
	LPC_RTC->ILR = 1;
	for(j=0;j<64*64;j++)
	{
		GPIO_PinWrite(2, 7, STEPMOTOR_Step[i%8][0]);
		GPIO_PinWrite(2, 3, STEPMOTOR_Step[i%8][1]);
		GPIO_PinWrite(2, 2, STEPMOTOR_Step[i%8][2]);
		GPIO_PinWrite(2, 1, STEPMOTOR_Step[i%8][3]);
		i++;
		Delay();
	}
	GPIO_PinWrite(2, 7, 0);
	GPIO_PinWrite(2, 3, 0);
	GPIO_PinWrite(2, 2, 0);
	GPIO_PinWrite(2, 1, 0);

}

extern ARM_DRIVER_USART Driver_USART0;
static ARM_DRIVER_USART * USART0drv = &Driver_USART0;

extern ARM_DRIVER_USART Driver_USART1;
static ARM_DRIVER_USART * USART1drv = &Driver_USART1;

void myUSART0_callback(uint32_t event)
{
	switch (event)
	{
		case ARM_USART_EVENT_RECEIVE_COMPLETE:
			uart0_rxdone = 1;
		case ARM_USART_EVENT_TRANSFER_COMPLETE:
		case ARM_USART_EVENT_SEND_COMPLETE:
		case ARM_USART_EVENT_TX_COMPLETE:
		/* Success: Wakeup Thread */
		break;
		case ARM_USART_EVENT_RX_TIMEOUT:
		break;
		case ARM_USART_EVENT_RX_OVERFLOW:
		case ARM_USART_EVENT_TX_UNDERFLOW:
		break;
	}
}

void myUSART1_callback(uint32_t event)
{
	switch (event)
	{
		case ARM_USART_EVENT_RECEIVE_COMPLETE:
		case ARM_USART_EVENT_TRANSFER_COMPLETE:
		case ARM_USART_EVENT_SEND_COMPLETE:
		case ARM_USART_EVENT_TX_COMPLETE:
		/* Success: Wakeup Thread */
		break;
		case ARM_USART_EVENT_RX_TIMEOUT:
		break;
		case ARM_USART_EVENT_RX_OVERFLOW:
		case ARM_USART_EVENT_TX_UNDERFLOW:
		break;
	}
}

void SysTick_Handler (void) {


}



extern unsigned int ReadTemp(void);



void UART0_Init()
{
	USART0drv->Initialize(myUSART0_callback);
	USART0drv->PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	USART0drv->Control(ARM_USART_MODE_ASYNCHRONOUS |
		ARM_USART_DATA_BITS_8 |
		ARM_USART_PARITY_NONE |
		ARM_USART_STOP_BITS_1 |
		ARM_USART_FLOW_CONTROL_NONE, 4800);
	/* Enable Receiver and Transmitter lines */
	USART0drv->Control (ARM_USART_CONTROL_TX, 1);
	USART0drv->Control (ARM_USART_CONTROL_RX, 1);
	NVIC_EnableIRQ(UART0_IRQn);
}

void UART1_Init()
{
	USART1drv->Initialize(myUSART1_callback);
	USART1drv->PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	USART1drv->Control(ARM_USART_MODE_ASYNCHRONOUS |
		ARM_USART_DATA_BITS_8 |
		ARM_USART_PARITY_NONE |
		ARM_USART_STOP_BITS_1 |
		ARM_USART_FLOW_CONTROL_NONE, 9600);
	/* Enable Receiver and Transmitter lines */
	USART1drv->Control (ARM_USART_CONTROL_TX, 1);
	USART1drv->Control (ARM_USART_CONTROL_RX, 1);
//	NVIC_EnableIRQ(UART1_IRQn);
}

//void I2C_Init()
//{
//	DeviceAddr = 0x04;
//	I2Cdrv->Initialize (NULL);
//	I2Cdrv->PowerControl (ARM_POWER_FULL);
//	I2Cdrv->Control (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
//	I2Cdrv->Control (ARM_I2C_BUS_CLEAR, 0);
//}	

void RTC_Init()
{	
	LPC_RTC->ILR = 1;
	LPC_RTC->CCR = 0x12;
	LPC_RTC->CIIR = 0x8;
	LPC_RTC->SEC = 0;
	LPC_RTC->MIN = 59;
	LPC_RTC->HOUR = 23;
	LPC_RTC->DOM = 1;
	LPC_RTC->MONTH = 2;
	LPC_RTC->YEAR = 2015;
	
	LPC_RTC->CCR = 0x11;
	
	NVIC_EnableIRQ(RTC_IRQn);

}

//void StepMotor_Init()
//{
//	PIN_Configure(2, 7, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
//	PIN_Configure(2, 3, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
//	PIN_Configure(2, 2, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
//	PIN_Configure(2, 1, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
//	
//	GPIO_SetDir(2, 7, GPIO_DIR_OUTPUT);
//	GPIO_SetDir(2, 3, GPIO_DIR_OUTPUT);
//	GPIO_SetDir(2, 2, GPIO_DIR_OUTPUT);
//	GPIO_SetDir(2, 1, GPIO_DIR_OUTPUT);
//	
//	GPIO_PinWrite(2, 7, 0);
//	GPIO_PinWrite(2, 3, 0);
//	GPIO_PinWrite(2, 2, 0);
//	GPIO_PinWrite(2, 1, 0);
//}


int main()
{
	int i,j;
	unsigned int temp;
	PIN_Configure(0, 9, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
	PIN_Configure(0, 8, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
	PIN_Configure(0, 7, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
	PIN_Configure(0, 6, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
	GPIO_SetDir(0, 9, GPIO_DIR_OUTPUT);
	GPIO_SetDir(0, 8, GPIO_DIR_OUTPUT);
	GPIO_SetDir(0, 7, GPIO_DIR_OUTPUT);
	GPIO_SetDir(0, 6, GPIO_DIR_OUTPUT);
	GPIO_PinWrite(0, 9, 1);
	GPIO_PinWrite(0, 8, 1);
	GPIO_PinWrite(0, 7, 1);
	GPIO_PinWrite(0, 6, 1);
	UART0_Init();
//	UART1_Init();
	
//	I2C_Init();
	
	RTC_Init();
	
//	StepMotor_Init();
	
//	OLED_Init();
	
//	PWM_Init();


  SystemCoreClockUpdate ();                 /* Update system core clock       */  
	SysTick_Config(SystemCoreClock/10);

	while(1)
	{	
	//		if(uart0_rxdone)
			{
				uart0_rxdone = 0;
				while(USART0drv->GetRxCount>0)
				{
					USART0drv->Receive(uart0_rxbuf,1);
					switch(uart0_rxbuf[0])
					{
						case '1':
							GPIO_PinWrite(0, 9, 0);
							break;
						case '2':
							GPIO_PinWrite(0, 9, 1);
							break;
						case '3':
							GPIO_PinWrite(0, 7, ~GPIO_PinRead(0, 7));
							break;
						case '4':
							GPIO_PinWrite(0, 6, ~GPIO_PinRead(0, 6));
							break;
						default:
							break;
					}
				}
			}
//		}
			
		for(j=0;j<640;j++)
		{
			Delay();
		}
		i=0;
		txbuf[i++] = (LPC_RTC->YEAR/1000%10+0x30);
		txbuf[i++] = (LPC_RTC->YEAR/100%10+0x30);
		txbuf[i++] = (LPC_RTC->YEAR/10%10+0x30);
		txbuf[i++] = (LPC_RTC->YEAR%10+0x30);
		txbuf[i++] = '/';
		txbuf[i++] = (LPC_RTC->MONTH/10+0x30);
		txbuf[i++] = (LPC_RTC->MONTH%10+0x30);
		txbuf[i++] = '/';
		txbuf[i++] = (LPC_RTC->DOM/10+0x30);
		txbuf[i++] = (LPC_RTC->DOM%10+0x30);
		txbuf[i++] = ' ';
		txbuf[i++] = (LPC_RTC->HOUR/10+0x30);
		txbuf[i++] = (LPC_RTC->HOUR%10+0x30);
		txbuf[i++] = ':';
		txbuf[i++] = (LPC_RTC->MIN/10+0x30);
		txbuf[i++] = (LPC_RTC->MIN%10+0x30);
		txbuf[i++] = ':';
		txbuf[i++] = (LPC_RTC->SEC/10+0x30);
		txbuf[i++] = (LPC_RTC->SEC%10+0x30);
		txbuf[i++] = ' ';
		temp = (unsigned int)(((float)ReadTemp() * 0.0625) * 10 + 0.5);
		txbuf[i++] = (char)(temp / 100 % 10 + 0x30);
		txbuf[i++] = (char)(temp / 10 % 10 + 0x30);
		txbuf[i++] = '.';
		txbuf[i++] = (char)(temp % 10 + 0x30);
		txbuf[i++] = 'C';

		
		txbuf[i++] = '\n';
		
	//	USART0drv->Send("\nTime:", 6);
		USART0drv->Send(txbuf, i);
//		LCD_P6x8Str(0, 0, (unsigned char *)txbuf);
	
	}
}
