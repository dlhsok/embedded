#include"systemInit.h"


void TimerInit(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);						//	使能定时器T0
	TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR|				//	将T0配置成16位定时器
								TIMER_CFG_A_PERIODIC);				//	将T0的A定时器配置为16位周期定时器
	TimerPrescaleSet(TIMER0_BASE, TIMER_A,99);						//	T0的A定时器预分频100次
	TimerLoadSet(TIMER0_BASE, TIMER_A,1000);						//	设置T0 A定时器的装载值，即定时时间为5ms
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);           		//	使能T0 A定时器溢出中断
	IntMasterEnable();												//	使能处理器中断
	IntEnable(INT_TIMER0A);											//	使能TO A定时器的中断
	TimerEnable(TIMER0_BASE, TIMER_A);								//	使能T0 A定时器计数
	Flag=0;
}


void Timer0A_ISR(void)
{
	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);                 //	清除中断标志位
	if(++a==200)
	{	
		a=0;
		Flag=~Flag;
		}
	}
//