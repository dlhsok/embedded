#ifndef  _SH1101A_H_
#define  _SH1101A_H_
#include<intrins.h>

#define PAR	   //定义为并行模式，如不是，注释掉即可
#ifdef PAR
	#define demo   //因为demo板数据要反向，定义这个宏来把数据反向的代码编译进去，如硬件一一对应则注释掉即可，
#endif
//以下为接口定义，用户根据自己的板子自行定义

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Delay Time
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void uDelay(unsigned char l)
{
	while(l--);
}


void Delay(unsigned char n)
{
unsigned char i,j,k;

	for(k=0;k<n;k++)
	{
		for(i=0;i<131;i++)
		{
			for(j=0;j<15;j++)
			{
				uDelay(203);	
			}
		}
	}
}


extern unsigned char code num[10][16];

#ifdef PAR
	#define xData P0
	sbit RD_oled=P1^1;
	sbit WR_oled=P1^0;
	sbit DC=P1^3;
	sbit RES=P1^2;
	sbit CS=P1^4;
#else
	sbit CLK=P1^7;
	sbit DATA=P1^6;
	sbit DC=P1^5;
	sbit RES=P1^4;
	sbit CS=P1^2;
#endif
//****************************主要操作函数************************************
void WriteCommand(unsigned com);		//写命令程序
void WriteData(unsigned dat);			//写数据程序
void esbusini(void);					//初始化总线程序
void ini_oled(void);            		//OLED初始化
void ini_dis(void);						//显示初始化(清除缓冲区)

#ifdef demo
sbit A_0=ACC^0;//方便位操作
sbit A_1=ACC^1;
sbit A_2=ACC^2;
sbit A_3=ACC^3;
sbit A_4=ACC^4;
sbit A_5=ACC^5;
sbit A_6=ACC^6;
sbit A_7=ACC^7;

sbit B_0=B^0;//方便位操作
sbit B_1=B^1;
sbit B_2=B^2;
sbit B_3=B^3;
sbit B_4=B^4;
sbit B_5=B^5;
sbit B_6=B^6;
sbit B_7=B^7;

unsigned char SetData(unsigned char DataByte)//转换并设置P0口的数据,如接口可不需此函数
{
	ACC=DataByte;
	B_1=A_0;
	B_0=A_1;

	B_3=A_2;
	B_2=A_3;

	B_5=A_4;
	B_4=A_5;

    B_7=A_6;
	B_6=A_7;
	
	return B;
}
#endif
void Write_Command(unsigned char Data)
{
	DC=0;
	CS=0;
	WR_oled=0;
	#ifdef demo
		xData=SetData(Data);
		//send_UART(Data);
		uDelay(1);
	#else
		xData=Data;
	#endif
	WR_oled=1;
	CS=1;
	DC=1;
}


void Write_Data(unsigned char Data)
{
	DC=1;
	CS=0;
	WR_oled=0;
	#ifdef demo
		xData=SetData(Data);
		//send_UART(Data);
		uDelay(1);
	#else
		xData=Data;
		#endif
	WR_oled=1;
	CS=1;
	DC=1;
}
/*******************************function************************************************************/
//*******************显示X，Y坐标处的一个字符。一行可以显示16个字符。X为0到15，Y为0到3**************
void disc(unsigned char X,unsigned char Y,unsigned char * c)
{
	unsigned char n;
	Write_Command (0xb7-(Y<<1));
	if(X%2)
		Write_Command (0x08);
	else
		Write_Command (0x00);
	Write_Command (0x10+(X>>1));
	for(n=0;n<=15;n+=2)
		Write_Data(*(c+n));

	Write_Command (0xb7-(Y<<1)-1);
	if(X%2)
		Write_Command (0x08);
	else
		Write_Command (0x00);
	Write_Command (0x10+(X>>1));
	for(n=1;n<=15;n+=2)
		Write_Data(*(c+n));
}
// X  is 0 to 7
void dish(unsigned char X,unsigned char Y,unsigned char * h)
{
	unsigned char n;
	Write_Command (0xb7-(Y<<1));
	if(X%2)
		Write_Command (0x08);
	else
		Write_Command (0x00);
	Write_Command (0x10+(X>>1));
	for(n=0;n<=31;n+=2)
		Write_Data(*(h+n));

	Write_Command (0xb7-(Y<<1)-1);
	if(X%2)
		Write_Command (0x08);
	else
		Write_Command (0x00);
	Write_Command (0x10+(X>>1));
	for(n=1;n<=31;n+=2)
		Write_Data(*(h+n));
}
void disn(unsigned char X,unsigned char Y,unsigned char n)
{
	unsigned char m;
	Write_Command (0xb7-(Y<<1));
	if(X%2)
		Write_Command (0x08);
	else
		Write_Command (0x00);
	WriteCommand (0x10+(X>>1));
	for(m=0;m<=15;m+=2)
		Write_Data(*(num[n]+m));                     

	Write_Command (0xb7-(Y<<1)-1);
	if(X%2)
		Write_Command (0x08);
	else
		Write_Command (0x00);
	Write_Command (0x10+(X>>1));
	for(m=1;m<=15;m+=2)
		Write_Data(*(num[n]+m));
}
#endif