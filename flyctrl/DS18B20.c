#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
/***********ds18b20子程序*************************/

/***********ds18b20延迟子函数（晶振11.0592MHz ）*******/

void delay_18B20(unsigned int i)
{
	unsigned int j=i*16;
	while(j--);
}

/**********ds18b20初始化函数**********************/

void Init_DS18B20(void)
{
  unsigned char x=0;
	PIN_Configure(2, 0, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
	GPIO_SetDir(2, 0, GPIO_DIR_OUTPUT);
	GPIO_PinWrite(2, 0, 1);

//DQ = 1;          //DQ复位
	GPIO_PinWrite(2, 0, 1);
  delay_18B20(80); //稍做延时
//  DQ = 0;          //单片机将DQ拉低
	GPIO_PinWrite(2, 0, 0);
  delay_18B20(800); //精确延时 大于 480us
//  DQ = 1;          //拉高总线
	GPIO_PinWrite(2, 0, 1);
  delay_18B20(140);
//  x=DQ;
	x = GPIO_PinRead(2, 0);
	delay_18B20(200);
}

/***********ds18b20读一个字节**************/

unsigned char ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{
//			DQ = 0; // 给脉冲信号
		GPIO_PinWrite(2, 0, 0);
		delay_18B20(5);
		dat>>=1;
//			DQ = 1; // 给脉冲信号
		GPIO_PinWrite(2, 0, 1);
		delay_18B20(5);
//			if(DQ)
		if(GPIO_PinRead(2, 0))
			dat|=0x80;
		delay_18B20(40);     //40
	}
  return(dat);
}

/*************ds18b20写一个字节****************/

void WriteOneChar(unsigned char dat)
{
   unsigned char i=0;
   for (i=8; i>0; i--)
   {
//     DQ = 0;
		 delay_18B20(5);
		 GPIO_PinWrite(2, 0, 0);
		 delay_18B20(5);
//     DQ = dat&0x01;
 		 GPIO_PinWrite(2, 0, dat&0x01);
    delay_18B20(50); //50
//     DQ = 1;
		 GPIO_PinWrite(2, 0, 1);
     dat>>=1;
	}
}

/**************读取ds18b20当前温度************/

unsigned int ReadTemp(void)
{
	unsigned char a=0;
	unsigned char b=0;
	unsigned int temp_value=0;

	Init_DS18B20();
	WriteOneChar(0xCC);     

	WriteOneChar(0x44);  

	delay_18B20(1000);      

	Init_DS18B20();
	WriteOneChar(0xCC);  
	WriteOneChar(0xBE);  

	delay_18B20(1000);

	a=ReadOneChar();    //读取温度值低位
	b=ReadOneChar();      //读取温度值高位

	temp_value = b<<8;
	temp_value |= a;
	return temp_value;
       
}
