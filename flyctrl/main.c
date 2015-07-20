#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "UART_LPC17xx.h"
#include "qPIDs.h"
#include <string.h>

unsigned char uart1_rxbuf[16];
unsigned char uart1_rxindex = 0;
unsigned char uart1_rxdone = 0;
char txbuf[50];

static uint8_t wr_buf[16 + 2];

static float ax,ay,az,T; 
static float wx,wy,wz; 
static float anglex,angley,anglez; 
static const float g=9.8;

static float set_ax,set_ay,set_az; 
//static float set_wx,set_wy,set_wz; 
//static float set_anglex,set_angley,set_anglez; 

static float out_ax,out_ay,out_az; 
//static float out_wx,out_wy,out_wz; 
//static float out_anglex,out_angley,out_anglez; 

//static float motor[4];
qPID pid_ax,pid_ay;



void Delay()
{
	int i=0;
	for (i=0;i<60000;i++)
	{
	}
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
		out_ax = qPID_Procees(&pid_ax, set_ax, ax);
		out_ay = qPID_Procees(&pid_ay, set_ay, ay);
//		memcpy(txbuf, &out_ax,4);
//		memcpy(txbuf+4, &out_ay,4);
//		memcpy(txbuf+8, &set_ax,4);
//		memcpy(txbuf+12, &set_ay,4);
//		memcpy(txbuf+16, &ax,4);
//		memcpy(txbuf+20, &ay,4);
//		txbuf[24]=0xD;
//		txbuf[25]=0xA;
//		USART0drv->Send(txbuf, 26);

}

void UART0_Init()
{
	USART0drv->Initialize(myUSART0_callback);
	USART0drv->PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	USART0drv->Control(ARM_USART_MODE_ASYNCHRONOUS |
		ARM_USART_DATA_BITS_8 |
		ARM_USART_PARITY_NONE |
		ARM_USART_STOP_BITS_1 |
		ARM_USART_FLOW_CONTROL_NONE, 19200);
	/* Enable Receiver and Transmitter lines */
	USART0drv->Control (ARM_USART_CONTROL_TX, 1);
	USART0drv->Control (ARM_USART_CONTROL_RX, 1);
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

static unsigned char checksum(unsigned char * data, unsigned char len)
{
	unsigned char i,check;
	for(i=0;i<len-1;i++)
	{
		check += data[i];
	}
	if(check==data[len-1])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static float pid_set[4][4] = {
	{0.1, 1/0.02, 0.0, 4.0},
	{0.2, 1/0.02, 0.0, 4.0},
	{0.5, 1/0.02, 0.0, 4.0},
	{0.5, 1/0.02, 0.0, 4.0}
	};

void PID_Init(qPID * pid, unsigned char index)
{
	pid->AntiWindup = ENABLED;
	pid->Bumpless = ENABLED;
	pid->Mode = AUTOMATIC;
	pid->OutputMax = 1.0;
	pid->OutputMin = -1.0;
	pid->Ts = 0.005;
	pid->b = 1.0;
	pid->c = 1.0;
	
	qPID_Init(pid);
	
	pid->K = pid_set[index][0];
	pid->Ti = pid_set[index][1];
	pid->Td = pid_set[index][2];
	pid->Nd = pid_set[index][3];
	
}

uint32_t rxerror = 0;
int main()
{
	int i,j;
//	unsigned int temp;

	UART0_Init();
	UART1_Init();
		
//	PWM_Init();

	
	PID_Init(&pid_ax, 0);
	PID_Init(&pid_ay, 1);

  SystemCoreClockUpdate ();                 /* Update system core clock       */  
	SysTick_Config(SystemCoreClock/10);

	while(1)
	{	
		while(USART1drv->GetRxCount>0)
		{
			USART1drv->Receive(&uart1_rxbuf[uart1_rxindex],1);
			switch(uart1_rxindex)
			{
				case 0:
					if(uart1_rxbuf[uart1_rxindex] == 0x55)
					{
						uart1_rxindex++;
					}
				break;
				case 1:
					if((uart1_rxbuf[uart1_rxindex] == 0x51)
						||(uart1_rxbuf[uart1_rxindex] == 0x52)
						||(uart1_rxbuf[uart1_rxindex] == 0x53))
					{
						uart1_rxindex++;
					}
					else
					{
						uart1_rxindex = 0;
					}
				break;
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
					uart1_rxindex++;
				break;
				case 10:
					uart1_rxindex=0;
//					if(checksum(uart1_rxbuf,11))
					{
//						uart1_rxdone=1;
						switch(uart1_rxbuf[1])
						{
							case 0x51:
								ax=((int16_t)(uart1_rxbuf[3]<<8|uart1_rxbuf[2]))/32768.0*16;
								ay=((int16_t)(uart1_rxbuf[5]<<8|uart1_rxbuf[4]))/32768.0*16;
								az=((int16_t)(uart1_rxbuf[7]<<8|uart1_rxbuf[6]))/32768.0*16;
								T=((int16_t)(uart1_rxbuf[9]<<8|uart1_rxbuf[8]))/340.0+36.25;
							break;
							case 0x52:
								wx=(uart1_rxbuf[3]<<8|uart1_rxbuf[2])/32768.0*2000;
								wy=(uart1_rxbuf[5]<<8|uart1_rxbuf[4])/32768.0*2000;
								wz=(uart1_rxbuf[7]<<8|uart1_rxbuf[6])/32768.0*2000;
							break;
							case 0x53:
								anglex=(uart1_rxbuf[3]<<8|uart1_rxbuf[2])/32768.0*180;
								angley=(uart1_rxbuf[5]<<8|uart1_rxbuf[4])/32768.0*180;
								anglez=(uart1_rxbuf[7]<<8|uart1_rxbuf[6])/32768.0*180;
							break;
							default:
							break;
						}
					}
//					else
//					{
//						rxerror++;
//					}
				break;
				default:
					uart1_rxindex=0;
				break;
			}
		}	
	}
}
