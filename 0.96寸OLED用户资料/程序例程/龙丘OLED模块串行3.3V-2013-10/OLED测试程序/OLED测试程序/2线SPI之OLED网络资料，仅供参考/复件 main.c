#include"systemInit.h"



int main(void)
{
	jtagWait();												//	防锁死
	clockInit();											//	系统时钟设置
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);          //	使能GPIOB模块
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_2); //D0/SCLK/PB2
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_3);//D1/MOSI/PB3
	SetSclk(1);
	SetMosi(0);
	TestMain();
	while(1){};
}
//