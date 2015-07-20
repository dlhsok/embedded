#include "LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"

const PIN motor[2] = {{2,2},{2,3}};
int main()
{
	int i,j;
	LPC_SC->PCON |=(1<<6);
	LPC_SC->PCLKSEL0 |=(3<<12);
	PIN_Configure(motor[0].Portnum, motor[0].Pinnum, PIN_FUNC_1, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	PIN_Configure(motor[1].Portnum, motor[1].Pinnum, PIN_FUNC_1, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	LPC_PWM1->MCR = 1<<1;
	LPC_PWM1->PR = 125-1;
	LPC_PWM1->MR0 = 500;
	LPC_PWM1->MR3 = 50 + 125 * 0.50;
	LPC_PWM1->PCR = (1<<11);
	LPC_PWM1->TCR = (1<<0)|(1<<3);
	LPC_PWM1->TCR = (1<<0);
	while(1)
	{
		for(i=0; i<1000; i++)
		{
			LPC_PWM1->MR3 = 50 + 200 * i / 1000;
			j=1000000;
			while(j--);
		}
		for(i=1000; i>0; i--)
		{
			LPC_PWM1->MR3 = 50 + 200 * i / 1000;
			j=1000000;
			while(j--);
		}
	}
}
