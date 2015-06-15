#include"all.h"


void UartInit(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);						  		 		//使能UART模块
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);						   				//使能RX0/TX0所在GPIO模块
	GPIOPinTypeUART(GPIO_PORTA_BASE, (GPIO_PIN_0|GPIO_PIN_1));			   				//使能RX0/TX0
	UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(),9600,UART_CONFIG_WLEN_8|		    //配置UART，波特率为9600,数据位8位
						UART_CONFIG_PAR_NONE|										    //停止位1位
						UART_CONFIG_STOP_ONE);											//无校验						   						
	UARTEnable(UART0_BASE);																//使能UART
}
//
