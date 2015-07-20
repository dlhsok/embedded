//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
//  UG-2864HSWEG01 (2864-46p) Reference Code
//
//    Dot Matrix: 128*64
//    Driver IC : SSD1306 (Solomon Systech)
//    Interface : 8-bit 68XX/80XX Parallel, 3-/4-wire SPI
//    Revision  :
//    Date      : 2009/06/05
//    Author    :
//    Editor    : Humphrey Lin
//
//  Copyright (c) Univision Technology Inc.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "NEW_8051.H"	   
//#include "ssd1306.h"
#include "zimo.h"
#include "newszimo.h"
#include "ds18b20.h"


/////////////////////////////////
sbit UP=P3^2;
sbit DOWN=P3^3;
//////////////////////////////////
unsigned char code Name[]={43,69,45,69,73,36,73,65,78,58,73,0};
						// kemeidianzi
unsigned char code Tel[]={17,21,25,21,19,24,22,21,18,25,19,0};
						// 15953865293
unsigned char t0=0;t1=0;t2=0;t3=0;
unsigned char temp=0,s=0,min=0,hour=0;
unsigned char state=1;
unsigned int Temperature=0;

////////////////////////////////////
bit display_sign=1;

/////////////////////////////////
void welcome(void);
void tm0_ini(void);
void delay(unsigned char i);
void delay_ms(unsigned int time);

//#define	M68				// 8-bit 68XX Parallel
						//   BS1=1; BS2=1
#define		I80				// 8-bit 80XX Parallel
						//   BS1=0; BS2=1
//#define	SPI				// 4-wire SPI
						//   BS1=0; BS2=0
						//   The unused pins should be connected with VSS mostly or floating (D2).
						//   Please refer to the SSD1306 specification for detail.
#define demo  //因为demo板I/O口与OLED模块I/O口反向，定义这个宏来把数据反向的代码编译进去，如硬件一一对应则注释掉即可，


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

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Pin Definition
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define xData	P0				// Parallel Data Input/Output

//sbit SCLK=P0^0;				// Serial Clock Input
//sbit SDIN=P0^1;				// Serial Data Input

sbit RES=P1^2;				// Reset
sbit CS=P1^4;				// Chip Select
sbit DC=P1^3;				// Data/Command Control

//sbit E=P2^3;				// Read/Write Enable
//sbit RW=P2^6;				// Read/Write Select

sbit RD_oled=P1^1;			// Read Signal
sbit WR_oled=P1^0;			// Write Signal
////


unsigned char i;

extern unsigned char code num[10][16];
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

void send_UART(unsigned char i)
{ ES=0;
  TI=0;
  SBUF=i;
  while(TI==0);
  TI=0;
  ES=1;
}

void stc12C4052ad_uart_init(void)
{  SCON=0X50;		//
   TMOD=0X21;		//
   TH1=0xff;	 //19200bps
   TL1=0xfc;	 //19200bps
   TR1=1;
   ES=1;	//打开串口中断
   EA=1; 
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Read/Write Sequence
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifdef M68					// 8-bit 68XX Parallel
void Write_Command(unsigned char Data)
{
	DC=0;
	CS=0;
	RW=0;
	E=1;
	#ifdef demo
		xData=SetData(Data);
	#else
		xData=Data;
	#endif
	E=0;
	RW=1;
	CS=1;
	DC=1;
}


void Write_Data(unsigned char Data)
{
	DC=1;
	CS=0;
	RW=0;
	E=1;
	#ifdef demo
		xData=SetData(Data);
	#else
		xData=Data;
	#endif
	E=0;
	RW=1;
	CS=1;
	DC=1;
}
#endif


#ifdef I80					// 8-bit 80XX Parallel
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
#endif


#ifdef SPI					// 4-wire SPI
void Write_Command(unsigned char Data)
{
unsigned char i;

	CS=0;
	DC=0;
	for (i=0; i<8; i++)
	{
		SCLK=0;
		SDIN=(Data&0x80)>>7;
		Data = Data << 1;
//		uDelay(1);
		SCLK=1;
//		uDelay(1);
	}
//	SCLK=0;
	DC=1;
	CS=1;
}


void Write_Data(unsigned char Data)
{
unsigned char i;

	CS=0;
	DC=1;
	for (i=0; i<8; i++)
	{
		SCLK=0;
		SDIN=(Data&0x80)>>7;
		Data = Data << 1;
//		uDelay(1);
		SCLK=1;
//		uDelay(1);
	}
//	SCLK=0;
	DC=1;
	CS=1;
}
#endif


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Start_Column(unsigned char d)
{
	Write_Command(0x00+d%16);		// Set Lower Column Start Address for Page Addressing Mode
						//   Default => 0x00
	Write_Command(0x10+d/16);		// Set Higher Column Start Address for Page Addressing Mode
						//   Default => 0x10
}


void Set_Addressing_Mode(unsigned char d)
{
	Write_Command(0x20);			// Set Memory Addressing Mode
	Write_Command(d);			//   Default => 0x02
						//     0x00 => Horizontal Addressing Mode
						//     0x01 => Vertical Addressing Mode
						//     0x02 => Page Addressing Mode
}


void Set_Column_Address(unsigned char a, unsigned char b)
{
	Write_Command(0x21);			// Set Column Address
	Write_Command(a);			//   Default => 0x00 (Column Start Address)
	Write_Command(b);			//   Default => 0x7F (Column End Address)
}


void Set_Page_Address(unsigned char a, unsigned char b)
{
	Write_Command(0x22);			// Set Page Address
	Write_Command(a);			//   Default => 0x00 (Page Start Address)
	Write_Command(b);			//   Default => 0x07 (Page End Address)
}


void Set_Start_Line(unsigned char d)
{
	Write_Command(0x40|d);			// Set Display Start Line
						//   Default => 0x40 (0x00)
}


void Set_Contrast_Control(unsigned char d)
{
	Write_Command(0x81);			// Set Contrast Control
	Write_Command(d);			//   Default => 0x7F
}


void Set_Charge_Pump(unsigned char d)
{
	Write_Command(0x8D);			// Set Charge Pump
	Write_Command(0x10|d);			//   Default => 0x10
						//     0x10 (0x00) => Disable Charge Pump
						//     0x14 (0x04) => Enable Charge Pump
}


void Set_Segment_Remap(unsigned char d)
{
	Write_Command(0xA0|d);			// Set Segment Re-Map
						//   Default => 0xA0
						//     0xA0 (0x00) => Column Address 0 Mapped to SEG0
						//     0xA1 (0x01) => Column Address 0 Mapped to SEG127
}


void Set_Entire_Display(unsigned char d)
{
	Write_Command(0xA4|d);			// Set Entire Display On / Off
						//   Default => 0xA4
						//     0xA4 (0x00) => Normal Display
						//     0xA5 (0x01) => Entire Display On
}


void Set_Inverse_Display(unsigned char d)
{
	Write_Command(0xA6|d);			// Set Inverse Display On/Off
						//   Default => 0xA6
						//     0xA6 (0x00) => Normal Display
						//     0xA7 (0x01) => Inverse Display On
}


void Set_Multiplex_Ratio(unsigned char d)
{
	Write_Command(0xA8);			// Set Multiplex Ratio
	Write_Command(d);			//   Default => 0x3F (1/64 Duty)
}


void Set_Display_On_Off(unsigned char d)	
{
	Write_Command(0xAE|d);			// Set Display On/Off
						//   Default => 0xAE
						//     0xAE (0x00) => Display Off
						//     0xAF (0x01) => Display On
}


void Set_Start_Page(unsigned char d)
{
	Write_Command(0xB0|d);			// Set Page Start Address for Page Addressing Mode
						//   Default => 0xB0 (0x00)
}


void Set_Common_Remap(unsigned char d)
{
	Write_Command(0xC0|d);			// Set COM Output Scan Direction
						//   Default => 0xC0
						//     0xC0 (0x00) => Scan from COM0 to 63
						//     0xC8 (0x08) => Scan from COM63 to 0
}


void Set_Display_Offset(unsigned char d)
{
	Write_Command(0xD3);			// Set Display Offset
	Write_Command(d);			//   Default => 0x00
}


void Set_Display_Clock(unsigned char d)
{
	Write_Command(0xD5);			// Set Display Clock Divide Ratio / Oscillator Frequency
	Write_Command(d);			//   Default => 0x80
						//     D[3:0] => Display Clock Divider
						//     D[7:4] => Oscillator Frequency
}


void Set_Precharge_Period(unsigned char d)
{
	Write_Command(0xD9);			// Set Pre-Charge Period
	Write_Command(d);			//   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
						//     D[3:0] => Phase 1 Period in 1~15 Display Clocks
						//     D[7:4] => Phase 2 Period in 1~15 Display Clocks
}


void Set_Common_Config(unsigned char d)
{
	Write_Command(0xDA);			// Set COM Pins Hardware Configuration
	Write_Command(0x02|d);			//   Default => 0x12 (0x10)
						//     Alternative COM Pin Configuration
						//     Disable COM Left/Right Re-Map
}


void Set_VCOMH(unsigned char d)
{
	Write_Command(0xDB);			// Set VCOMH Deselect Level
	Write_Command(d);			//   Default => 0x20 (0.77*VCC)
}


void Set_NOP()
{
	Write_Command(0xE3);			// Command for No Operation
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Global Variables
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xff//0xCF


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Patterns
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 
unsigned char code kemei[][128] = { 

0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,0XF0,0XF0,0XF8,0XFC,0XFC,0XFC,0XFE,
0XFE,0XFE,0XFE,0XFE,0XFE,0XFE,0XFE,0XFE,0XFC,0XFC,0XFC,0XF8,0XF0,0X60,0X60,0X80,
0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X80,
0X00,0X00,0X80,0X80,0X80,0X00,0X80,0X00,0X00,0X80,0X80,0X00,0X00,0X00,0X00,0X80,
0X80,0X80,0X80,0X00,0X00,0X00,0X00,0X00,0X80,0X80,0X80,0X80,0X00,0X00,0X80,0X80,
0X80,0X80,0X80,0X80,0XC0,0XC0,0X80,0X80,0X80,0X80,0X80,0X80,0X00,0X00,0X00,0X80,
0X80,0X80,0X80,0X80,0X80,0X80,0X80,0X80,0X80,0X80,0X80,0X80,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0XF0,0XFC,0XFF,0XFF,0XE0,0X01,0X07,0X1F,0X1F,0XDF,0XDF,0XDF,0X8F,
0X8F,0X8F,0X8F,0X87,0X87,0X87,0X83,0X83,0X03,0X01,0X01,0X00,0X00,0X0C,0X0E,0X1F,
0X1F,0X3E,0X3C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X04,0XF5,
0X1D,0XFF,0XFF,0X1D,0XF5,0X00,0X77,0XE6,0XCC,0XFF,0XFF,0X00,0X00,0X00,0X00,0XB5,
0XB5,0XB5,0XB5,0XB5,0XFF,0XFF,0XFF,0XB5,0XB5,0XB5,0XB5,0XB5,0X00,0X00,0XFF,0XFF,
0XFF,0X99,0X99,0X99,0XFF,0XFF,0X99,0X99,0X99,0X99,0XFF,0XFF,0X00,0X00,0XC0,0XC1,
0XC1,0XC9,0XD9,0XD9,0XD9,0XD9,0XD9,0XFD,0XFF,0XC7,0XC7,0XC1,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0XEF,0XEF,0XCF,0XCF,0X8F,0X8E,0X00,0X00,0X00,0X00,0X03,0X0F,0X1F,
0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X7F,0X1F,
0X0F,0X06,0X02,0X80,0X00,0X00,0X00,0X80,0X00,0X00,0X00,0X00,0X80,0X80,0X8C,0X0F,
0X00,0X1F,0X1F,0X00,0X0F,0X08,0X06,0X06,0X06,0X0F,0X1F,0X03,0X00,0X00,0X80,0X99,
0X19,0X19,0X0F,0X0F,0X03,0X03,0X03,0X0F,0X0F,0X09,0X19,0X19,0X00,0X00,0X81,0X81,
0X01,0X01,0X81,0X01,0X0F,0X1F,0X19,0X19,0X19,0X99,0X99,0X99,0X00,0X00,0X00,0X88,
0X98,0X98,0X18,0X18,0X98,0X98,0X1C,0X8F,0X07,0X00,0X00,0X80,0X80,0X80,0X80,0X00,
0X00,0X00,0X00,0X80,0X80,0X00,0X00,0X00,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X01,0X07,0X0F,0X1F,0X3F,0X7F,0XFF,0XFE,0XFE,0XFC,0XF8,0XF0,
0XF0,0XE0,0XC3,0X8F,0X1F,0X3F,0XFF,0XFF,0XFF,0XFF,0XEF,0X03,0X01,0X00,0X00,0X00,
0X60,0XF2,0XFE,0X1F,0XDF,0XFC,0X1F,0XE1,0XFC,0XFF,0XDF,0XC9,0X03,0X03,0XE1,0XFC,
0X3F,0X07,0XF3,0XFE,0X7F,0XFF,0X33,0XF8,0X1F,0XE3,0XF8,0XFF,0XDF,0XDB,0X01,0X03,
0XF1,0XFE,0X3F,0X0F,0X01,0X00,0X00,0X00,0XC0,0XF0,0XFE,0XCF,0XCB,0X41,0X03,0XE1,
0XFC,0XFF,0XC7,0X01,0XE0,0XE0,0XFC,0XDF,0XDF,0XC1,0X03,0XE1,0XF8,0XFF,0XC7,0XC3,
0X03,0X03,0X61,0XF7,0X7F,0X1F,0X03,0XE1,0XFC,0X7E,0X1F,0XDF,0X79,0X0F,0X03,0XE0,
0XF0,0XDF,0XC7,0X63,0X1F,0X07,0XF8,0X7E,0X1F,0XFF,0XF9,0X1C,0X03,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X01,0X03,0X03,
0X07,0X07,0X07,0X07,0X07,0X02,0X00,0X03,0X03,0X01,0X01,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X08,0XF8,0X08,0X00,0XF8,0XA8,0XA8,0X00,0XF8,0X80,0X80,0X00,0XA0,0X00,0X00,0X00,
0X00,0X00,0XF8,0X00,0XA8,0XA8,0XF8,0X00,0XE8,0XA8,0XB8,0X00,0XE8,0XA8,0XB8,0X00,
0XE8,0XA8,0XB8,0X00,0XB8,0XA8,0XF8,0X00,0X08,0X08,0XF8,0X00,0XE8,0XA8,0XB8,0X00,
0XB8,0XA8,0XE8,0X00,0XB8,0XA8,0XE8,0X00,0XA8,0XA8,0XF8,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

unsigned char code shiyan[][128]  = { 
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X1F,0X01,0X00,0X1F,
0X15,0X15,0X00,0X1F,0X10,0X10,0X00,0X14,0X00,0X00,0X00,0X00,0X00,0X1F,0X00,0X15,
0X15,0X1F,0X00,0X1D,0X15,0X17,0X00,0X1D,0X15,0X17,0X00,0X1D,0X15,0X17,0X00,0X17,
0X15,0X1F,0X00,0X01,0X01,0X1F,0X00,0X1D,0X15,0X17,0X00,0X17,0X15,0X1D,0X00,0X17,
0X15,0X1D,0X00,0X15,0X15,0X1F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};



unsigned char code Ascii_1[240][5]={		// Refer to "Times New Roman" Font Database...
						//   Basic Characters
	{0x00,0x00,0x4F,0x00,0x00},		//   (  1)  ! - 0x0021 Exclamation Mark
	{0x00,0x07,0x00,0x07,0x00},		//   (  2)  " - 0x0022 Quotation Mark
	{0x14,0x7F,0x14,0x7F,0x14},		//   (  3)  # - 0x0023 Number Sign
	{0x24,0x2A,0x7F,0x2A,0x12},		//   (  4)  $ - 0x0024 Dollar Sign
	{0x23,0x13,0x08,0x64,0x62},		//   (  5)  % - 0x0025 Percent Sign
	{0x36,0x49,0x55,0x22,0x50},		//   (  6)  & - 0x0026 Ampersand
	{0x00,0x05,0x03,0x00,0x00},		//   (  7)  ' - 0x0027 Apostrophe
	{0x00,0x1C,0x22,0x41,0x00},		//   (  8)  ( - 0x0028 Left Parenthesis
	{0x00,0x41,0x22,0x1C,0x00},		//   (  9)  ) - 0x0029 Right Parenthesis
	{0x14,0x08,0x3E,0x08,0x14},		//   ( 10)  * - 0x002A Asterisk
	{0x08,0x08,0x3E,0x08,0x08},		//   ( 11)  + - 0x002B Plus Sign
	{0x00,0x50,0x30,0x00,0x00},		//   ( 12)  , - 0x002C Comma
	{0x08,0x08,0x08,0x08,0x08},		//   ( 13)  - - 0x002D Hyphen-Minus
	{0x00,0x60,0x60,0x00,0x00},		//   ( 14)  . - 0x002E Full Stop
	{0x20,0x10,0x08,0x04,0x02},		//   ( 15)  / - 0x002F Solidus
	{0x3E,0x51,0x49,0x45,0x3E},		//   ( 16)  0 - 0x0030 Digit Zero
	{0x00,0x42,0x7F,0x40,0x00},		//   ( 17)  1 - 0x0031 Digit One
	{0x42,0x61,0x51,0x49,0x46},		//   ( 18)  2 - 0x0032 Digit Two
	{0x21,0x41,0x45,0x4B,0x31},		//   ( 19)  3 - 0x0033 Digit Three
	{0x18,0x14,0x12,0x7F,0x10},		//   ( 20)  4 - 0x0034 Digit Four
	{0x27,0x45,0x45,0x45,0x39},		//   ( 21)  5 - 0x0035 Digit Five
	{0x3C,0x4A,0x49,0x49,0x30},		//   ( 22)  6 - 0x0036 Digit Six
	{0x01,0x71,0x09,0x05,0x03},		//   ( 23)  7 - 0x0037 Digit Seven
	{0x36,0x49,0x49,0x49,0x36},		//   ( 24)  8 - 0x0038 Digit Eight
	{0x06,0x49,0x49,0x29,0x1E},		//   ( 25)  9 - 0x0039 Dight Nine
	{0x00,0x36,0x36,0x00,0x00},		//   ( 26)  : - 0x003A Colon
	{0x00,0x56,0x36,0x00,0x00},		//   ( 27)  ; - 0x003B Semicolon
	{0x08,0x14,0x22,0x41,0x00},		//   ( 28)  < - 0x003C Less-Than Sign
	{0x14,0x14,0x14,0x14,0x14},		//   ( 29)  = - 0x003D Equals Sign
	{0x00,0x41,0x22,0x14,0x08},		//   ( 30)  > - 0x003E Greater-Than Sign
	{0x02,0x01,0x51,0x09,0x06},		//   ( 31)  ? - 0x003F Question Mark
	{0x32,0x49,0x79,0x41,0x3E},		//   ( 32)  @ - 0x0040 Commercial At
	{0x7E,0x11,0x11,0x11,0x7E},		//   ( 33)  A - 0x0041 Latin Capital Letter A
	{0x7F,0x49,0x49,0x49,0x36},		//   ( 34)  B - 0x0042 Latin Capital Letter B
	{0x3E,0x41,0x41,0x41,0x22},		//   ( 35)  C - 0x0043 Latin Capital Letter C
	{0x7F,0x41,0x41,0x22,0x1C},		//   ( 36)  D - 0x0044 Latin Capital Letter D
	{0x7F,0x49,0x49,0x49,0x41},		//   ( 37)  E - 0x0045 Latin Capital Letter E
	{0x7F,0x09,0x09,0x09,0x01},		//   ( 38)  F - 0x0046 Latin Capital Letter F
	{0x3E,0x41,0x49,0x49,0x7A},		//   ( 39)  G - 0x0047 Latin Capital Letter G
	{0x7F,0x08,0x08,0x08,0x7F},		//   ( 40)  H - 0x0048 Latin Capital Letter H
	{0x00,0x41,0x7F,0x41,0x00},		//   ( 41)  I - 0x0049 Latin Capital Letter I
	{0x20,0x40,0x41,0x3F,0x01},		//   ( 42)  J - 0x004A Latin Capital Letter J
	{0x7F,0x08,0x14,0x22,0x41},		//   ( 43)  K - 0x004B Latin Capital Letter K
	{0x7F,0x40,0x40,0x40,0x40},		//   ( 44)  L - 0x004C Latin Capital Letter L
	{0x7F,0x02,0x0C,0x02,0x7F},		//   ( 45)  M - 0x004D Latin Capital Letter M
	{0x7F,0x04,0x08,0x10,0x7F},		//   ( 46)  N - 0x004E Latin Capital Letter N
	{0x3E,0x41,0x41,0x41,0x3E},		//   ( 47)  O - 0x004F Latin Capital Letter O
	{0x7F,0x09,0x09,0x09,0x06},		//   ( 48)  P - 0x0050 Latin Capital Letter P
	{0x3E,0x41,0x51,0x21,0x5E},		//   ( 49)  Q - 0x0051 Latin Capital Letter Q
	{0x7F,0x09,0x19,0x29,0x46},		//   ( 50)  R - 0x0052 Latin Capital Letter R
	{0x46,0x49,0x49,0x49,0x31},		//   ( 51)  S - 0x0053 Latin Capital Letter S
	{0x01,0x01,0x7F,0x01,0x01},		//   ( 52)  T - 0x0054 Latin Capital Letter T
	{0x3F,0x40,0x40,0x40,0x3F},		//   ( 53)  U - 0x0055 Latin Capital Letter U
	{0x1F,0x20,0x40,0x20,0x1F},		//   ( 54)  V - 0x0056 Latin Capital Letter V
	{0x3F,0x40,0x38,0x40,0x3F},		//   ( 55)  W - 0x0057 Latin Capital Letter W
	{0x63,0x14,0x08,0x14,0x63},		//   ( 56)  X - 0x0058 Latin Capital Letter X
	{0x07,0x08,0x70,0x08,0x07},		//   ( 57)  Y - 0x0059 Latin Capital Letter Y
	{0x61,0x51,0x49,0x45,0x43},		//   ( 58)  Z - 0x005A Latin Capital Letter Z
	{0x00,0x7F,0x41,0x41,0x00},		//   ( 59)  [ - 0x005B Left Square Bracket
	{0x02,0x04,0x08,0x10,0x20},		//   ( 60)  \ - 0x005C Reverse Solidus
	{0x00,0x41,0x41,0x7F,0x00},		//   ( 61)  ] - 0x005D Right Square Bracket
	{0x04,0x02,0x01,0x02,0x04},		//   ( 62)  ^ - 0x005E Circumflex Accent
	{0x40,0x40,0x40,0x40,0x40},		//   ( 63)  _ - 0x005F Low Line
	{0x01,0x02,0x04,0x00,0x00},		//   ( 64)  ` - 0x0060 Grave Accent
	{0x20,0x54,0x54,0x54,0x78},		//   ( 65)  a - 0x0061 Latin Small Letter A
	{0x7F,0x48,0x44,0x44,0x38},		//   ( 66)  b - 0x0062 Latin Small Letter B
	{0x38,0x44,0x44,0x44,0x20},		//   ( 67)  c - 0x0063 Latin Small Letter C
	{0x38,0x44,0x44,0x48,0x7F},		//   ( 68)  d - 0x0064 Latin Small Letter D
	{0x38,0x54,0x54,0x54,0x18},		//   ( 69)  e - 0x0065 Latin Small Letter E
	{0x08,0x7E,0x09,0x01,0x02},		//   ( 70)  f - 0x0066 Latin Small Letter F
	{0x06,0x49,0x49,0x49,0x3F},		//   ( 71)  g - 0x0067 Latin Small Letter G
	{0x7F,0x08,0x04,0x04,0x78},		//   ( 72)  h - 0x0068 Latin Small Letter H
	{0x00,0x44,0x7D,0x40,0x00},		//   ( 73)  i - 0x0069 Latin Small Letter I
	{0x20,0x40,0x44,0x3D,0x00},		//   ( 74)  j - 0x006A Latin Small Letter J
	{0x7F,0x10,0x28,0x44,0x00},		//   ( 75)  k - 0x006B Latin Small Letter K
	{0x00,0x41,0x7F,0x40,0x00},		//   ( 76)  l - 0x006C Latin Small Letter L
	{0x7C,0x04,0x18,0x04,0x7C},		//   ( 77)  m - 0x006D Latin Small Letter M
	{0x7C,0x08,0x04,0x04,0x78},		//   ( 78)  n - 0x006E Latin Small Letter N
	{0x38,0x44,0x44,0x44,0x38},		//   ( 79)  o - 0x006F Latin Small Letter O
	{0x7C,0x14,0x14,0x14,0x08},		//   ( 80)  p - 0x0070 Latin Small Letter P
	{0x08,0x14,0x14,0x18,0x7C},		//   ( 81)  q - 0x0071 Latin Small Letter Q
	{0x7C,0x08,0x04,0x04,0x08},		//   ( 82)  r - 0x0072 Latin Small Letter R
	{0x48,0x54,0x54,0x54,0x20},		//   ( 83)  s - 0x0073 Latin Small Letter S
	{0x04,0x3F,0x44,0x40,0x20},		//   ( 84)  t - 0x0074 Latin Small Letter T
	{0x3C,0x40,0x40,0x20,0x7C},		//   ( 85)  u - 0x0075 Latin Small Letter U
	{0x1C,0x20,0x40,0x20,0x1C},		//   ( 86)  v - 0x0076 Latin Small Letter V
	{0x3C,0x40,0x30,0x40,0x3C},		//   ( 87)  w - 0x0077 Latin Small Letter W
	{0x44,0x28,0x10,0x28,0x44},		//   ( 88)  x - 0x0078 Latin Small Letter X
	{0x0C,0x50,0x50,0x50,0x3C},		//   ( 89)  y - 0x0079 Latin Small Letter Y
	{0x44,0x64,0x54,0x4C,0x44},		//   ( 90)  z - 0x007A Latin Small Letter Z
	{0x00,0x08,0x36,0x41,0x00},		//   ( 91)  { - 0x007B Left Curly Bracket
	{0x00,0x00,0x7F,0x00,0x00},		//   ( 92)  | - 0x007C Vertical Line
	{0x00,0x41,0x36,0x08,0x00},		//   ( 93)  } - 0x007D Right Curly Bracket
	{0x02,0x01,0x02,0x04,0x02},		//   ( 94)  ~ - 0x007E Tilde
	{0x3E,0x55,0x55,0x41,0x22},		//   ( 95)  C - 0x0080 <Control>
	{0x00,0x00,0x00,0x00,0x00},		//   ( 96)    - 0x00A0 No-Break Space
	{0x00,0x00,0x79,0x00,0x00},		//   ( 97)  ! - 0x00A1 Inverted Exclamation Mark
	{0x18,0x24,0x74,0x2E,0x24},		//   ( 98)  c - 0x00A2 Cent Sign
	{0x48,0x7E,0x49,0x42,0x40},		//   ( 99)  L - 0x00A3 Pound Sign
	{0x5D,0x22,0x22,0x22,0x5D},		//   (100)  o - 0x00A4 Currency Sign
	{0x15,0x16,0x7C,0x16,0x15},		//   (101)  Y - 0x00A5 Yen Sign
	{0x00,0x00,0x77,0x00,0x00},		//   (102)  | - 0x00A6 Broken Bar
	{0x0A,0x55,0x55,0x55,0x28},		//   (103)    - 0x00A7 Section Sign
	{0x00,0x01,0x00,0x01,0x00},		//   (104)  " - 0x00A8 Diaeresis
	{0x00,0x0A,0x0D,0x0A,0x04},		//   (105)    - 0x00AA Feminine Ordinal Indicator
	{0x08,0x14,0x2A,0x14,0x22},		//   (106) << - 0x00AB Left-Pointing Double Angle Quotation Mark
	{0x04,0x04,0x04,0x04,0x1C},		//   (107)    - 0x00AC Not Sign
	{0x00,0x08,0x08,0x08,0x00},		//   (108)  - - 0x00AD Soft Hyphen
	{0x01,0x01,0x01,0x01,0x01},		//   (109)    - 0x00AF Macron
	{0x00,0x02,0x05,0x02,0x00},		//   (110)    - 0x00B0 Degree Sign
	{0x44,0x44,0x5F,0x44,0x44},		//   (111) +- - 0x00B1 Plus-Minus Sign
	{0x00,0x00,0x04,0x02,0x01},		//   (112)  ` - 0x00B4 Acute Accent
	{0x7E,0x20,0x20,0x10,0x3E},		//   (113)  u - 0x00B5 Micro Sign
	{0x06,0x0F,0x7F,0x00,0x7F},		//   (114)    - 0x00B6 Pilcrow Sign
	{0x00,0x18,0x18,0x00,0x00},		//   (115)  . - 0x00B7 Middle Dot
	{0x00,0x40,0x50,0x20,0x00},		//   (116)    - 0x00B8 Cedilla
	{0x00,0x0A,0x0D,0x0A,0x00},		//   (117)    - 0x00BA Masculine Ordinal Indicator
	{0x22,0x14,0x2A,0x14,0x08},		//   (118) >> - 0x00BB Right-Pointing Double Angle Quotation Mark
	{0x17,0x08,0x34,0x2A,0x7D},		//   (119) /4 - 0x00BC Vulgar Fraction One Quarter
	{0x17,0x08,0x04,0x6A,0x59},		//   (120) /2 - 0x00BD Vulgar Fraction One Half
	{0x30,0x48,0x45,0x40,0x20},		//   (121)  ? - 0x00BF Inverted Question Mark
	{0x70,0x29,0x26,0x28,0x70},		//   (122) `A - 0x00C0 Latin Capital Letter A with Grave
	{0x70,0x28,0x26,0x29,0x70},		//   (123) 'A - 0x00C1 Latin Capital Letter A with Acute
	{0x70,0x2A,0x25,0x2A,0x70},		//   (124) ^A - 0x00C2 Latin Capital Letter A with Circumflex
	{0x72,0x29,0x26,0x29,0x70},		//   (125) ~A - 0x00C3 Latin Capital Letter A with Tilde
	{0x70,0x29,0x24,0x29,0x70},		//   (126) "A - 0x00C4 Latin Capital Letter A with Diaeresis
	{0x70,0x2A,0x2D,0x2A,0x70},		//   (127)  A - 0x00C5 Latin Capital Letter A with Ring Above
	{0x7E,0x11,0x7F,0x49,0x49},		//   (128) AE - 0x00C6 Latin Capital Letter Ae
	{0x0E,0x51,0x51,0x71,0x11},		//   (129)  C - 0x00C7 Latin Capital Letter C with Cedilla
	{0x7C,0x55,0x56,0x54,0x44},		//   (130) `E - 0x00C8 Latin Capital Letter E with Grave
	{0x7C,0x55,0x56,0x54,0x44},		//   (131) 'E - 0x00C9 Latin Capital Letter E with Acute
	{0x7C,0x56,0x55,0x56,0x44},		//   (132) ^E - 0x00CA Latin Capital Letter E with Circumflex
	{0x7C,0x55,0x54,0x55,0x44},		//   (133) "E - 0x00CB Latin Capital Letter E with Diaeresis
	{0x00,0x45,0x7E,0x44,0x00},		//   (134) `I - 0x00CC Latin Capital Letter I with Grave
	{0x00,0x44,0x7E,0x45,0x00},		//   (135) 'I - 0x00CD Latin Capital Letter I with Acute
	{0x00,0x46,0x7D,0x46,0x00},		//   (136) ^I - 0x00CE Latin Capital Letter I with Circumflex
	{0x00,0x45,0x7C,0x45,0x00},		//   (137) "I - 0x00CF Latin Capital Letter I with Diaeresis
	{0x7F,0x49,0x49,0x41,0x3E},		//   (138)  D - 0x00D0 Latin Capital Letter Eth
	{0x7C,0x0A,0x11,0x22,0x7D},		//   (139) ~N - 0x00D1 Latin Capital Letter N with Tilde
	{0x38,0x45,0x46,0x44,0x38},		//   (140) `O - 0x00D2 Latin Capital Letter O with Grave
	{0x38,0x44,0x46,0x45,0x38},		//   (141) 'O - 0x00D3 Latin Capital Letter O with Acute
	{0x38,0x46,0x45,0x46,0x38},		//   (142) ^O - 0x00D4 Latin Capital Letter O with Circumflex
	{0x38,0x46,0x45,0x46,0x39},		//   (143) ~O - 0x00D5 Latin Capital Letter O with Tilde
	{0x38,0x45,0x44,0x45,0x38},		//   (144) "O - 0x00D6 Latin Capital Letter O with Diaeresis
	{0x22,0x14,0x08,0x14,0x22},		//   (145)  x - 0x00D7 Multiplcation Sign
	{0x2E,0x51,0x49,0x45,0x3A},		//   (146)  O - 0x00D8 Latin Capital Letter O with Stroke
	{0x3C,0x41,0x42,0x40,0x3C},		//   (147) `U - 0x00D9 Latin Capital Letter U with Grave
	{0x3C,0x40,0x42,0x41,0x3C},		//   (148) 'U - 0x00DA Latin Capital Letter U with Acute
	{0x3C,0x42,0x41,0x42,0x3C},		//   (149) ^U - 0x00DB Latin Capital Letter U with Circumflex
	{0x3C,0x41,0x40,0x41,0x3C},		//   (150) "U - 0x00DC Latin Capital Letter U with Diaeresis
	{0x0C,0x10,0x62,0x11,0x0C},		//   (151) `Y - 0x00DD Latin Capital Letter Y with Acute
	{0x7F,0x12,0x12,0x12,0x0C},		//   (152)  P - 0x00DE Latin Capital Letter Thom
	{0x40,0x3E,0x01,0x49,0x36},		//   (153)  B - 0x00DF Latin Capital Letter Sharp S
	{0x20,0x55,0x56,0x54,0x78},		//   (154) `a - 0x00E0 Latin Small Letter A with Grave
	{0x20,0x54,0x56,0x55,0x78},		//   (155) 'a - 0x00E1 Latin Small Letter A with Acute
	{0x20,0x56,0x55,0x56,0x78},		//   (156) ^a - 0x00E2 Latin Small Letter A with Circumflex
	{0x20,0x55,0x56,0x55,0x78},		//   (157) ~a - 0x00E3 Latin Small Letter A with Tilde
	{0x20,0x55,0x54,0x55,0x78},		//   (158) "a - 0x00E4 Latin Small Letter A with Diaeresis
	{0x20,0x56,0x57,0x56,0x78},		//   (159)  a - 0x00E5 Latin Small Letter A with Ring Above
	{0x24,0x54,0x78,0x54,0x58},		//   (160) ae - 0x00E6 Latin Small Letter Ae
	{0x0C,0x52,0x52,0x72,0x13},		//   (161)  c - 0x00E7 Latin Small Letter c with Cedilla
	{0x38,0x55,0x56,0x54,0x18},		//   (162) `e - 0x00E8 Latin Small Letter E with Grave
	{0x38,0x54,0x56,0x55,0x18},		//   (163) 'e - 0x00E9 Latin Small Letter E with Acute
	{0x38,0x56,0x55,0x56,0x18},		//   (164) ^e - 0x00EA Latin Small Letter E with Circumflex
	{0x38,0x55,0x54,0x55,0x18},		//   (165) "e - 0x00EB Latin Small Letter E with Diaeresis
	{0x00,0x49,0x7A,0x40,0x00},		//   (166) `i - 0x00EC Latin Small Letter I with Grave
	{0x00,0x48,0x7A,0x41,0x00},		//   (167) 'i - 0x00ED Latin Small Letter I with Acute
	{0x00,0x4A,0x79,0x42,0x00},		//   (168) ^i - 0x00EE Latin Small Letter I with Circumflex
	{0x00,0x4A,0x78,0x42,0x00},		//   (169) "i - 0x00EF Latin Small Letter I with Diaeresis
	{0x31,0x4A,0x4E,0x4A,0x30},		//   (170)    - 0x00F0 Latin Small Letter Eth
	{0x7A,0x11,0x0A,0x09,0x70},		//   (171) ~n - 0x00F1 Latin Small Letter N with Tilde
	{0x30,0x49,0x4A,0x48,0x30},		//   (172) `o - 0x00F2 Latin Small Letter O with Grave
	{0x30,0x48,0x4A,0x49,0x30},		//   (173) 'o - 0x00F3 Latin Small Letter O with Acute
	{0x30,0x4A,0x49,0x4A,0x30},		//   (174) ^o - 0x00F4 Latin Small Letter O with Circumflex
	{0x30,0x4A,0x49,0x4A,0x31},		//   (175) ~o - 0x00F5 Latin Small Letter O with Tilde
	{0x30,0x4A,0x48,0x4A,0x30},		//   (176) "o - 0x00F6 Latin Small Letter O with Diaeresis
	{0x08,0x08,0x2A,0x08,0x08},		//   (177)  + - 0x00F7 Division Sign
	{0x38,0x64,0x54,0x4C,0x38},		//   (178)  o - 0x00F8 Latin Small Letter O with Stroke
	{0x38,0x41,0x42,0x20,0x78},		//   (179) `u - 0x00F9 Latin Small Letter U with Grave
	{0x38,0x40,0x42,0x21,0x78},		//   (180) 'u - 0x00FA Latin Small Letter U with Acute
	{0x38,0x42,0x41,0x22,0x78},		//   (181) ^u - 0x00FB Latin Small Letter U with Circumflex
	{0x38,0x42,0x40,0x22,0x78},		//   (182) "u - 0x00FC Latin Small Letter U with Diaeresis
	{0x0C,0x50,0x52,0x51,0x3C},		//   (183) 'y - 0x00FD Latin Small Letter Y with Acute
	{0x7E,0x14,0x14,0x14,0x08},		//   (184)  p - 0x00FE Latin Small Letter Thom
	{0x0C,0x51,0x50,0x51,0x3C},		//   (185) "y - 0x00FF Latin Small Letter Y with Diaeresis
	{0x1E,0x09,0x09,0x29,0x5E},		//   (186)  A - 0x0104 Latin Capital Letter A with Ogonek
	{0x08,0x15,0x15,0x35,0x4E},		//   (187)  a - 0x0105 Latin Small Letter A with Ogonek
	{0x38,0x44,0x46,0x45,0x20},		//   (188) 'C - 0x0106 Latin Capital Letter C with Acute
	{0x30,0x48,0x4A,0x49,0x20},		//   (189) 'c - 0x0107 Latin Small Letter C with Acute
	{0x38,0x45,0x46,0x45,0x20},		//   (190)  C - 0x010C Latin Capital Letter C with Caron
	{0x30,0x49,0x4A,0x49,0x20},		//   (191)  c - 0x010D Latin Small Letter C with Caron
	{0x7C,0x45,0x46,0x45,0x38},		//   (192)  D - 0x010E Latin Capital Letter D with Caron
	{0x20,0x50,0x50,0x7C,0x03},		//   (193) d' - 0x010F Latin Small Letter D with Caron
	{0x1F,0x15,0x15,0x35,0x51},		//   (194)  E - 0x0118 Latin Capital Letter E with Ogonek
	{0x0E,0x15,0x15,0x35,0x46},		//   (195)  e - 0x0119 Latin Small Letter E with Ogonek
	{0x7C,0x55,0x56,0x55,0x44},		//   (196)  E - 0x011A Latin Capital Letter E with Caron
	{0x38,0x55,0x56,0x55,0x18},		//   (197)  e - 0x011B Latin Small Letter E with Caron
	{0x00,0x44,0x7C,0x40,0x00},		//   (198)  i - 0x0131 Latin Small Letter Dotless I
	{0x7F,0x48,0x44,0x40,0x40},		//   (199)  L - 0x0141 Latin Capital Letter L with Stroke
	{0x00,0x49,0x7F,0x44,0x00},		//   (200)  l - 0x0142 Latin Small Letter L with Stroke
	{0x7C,0x08,0x12,0x21,0x7C},		//   (201) 'N - 0x0143 Latin Capital Letter N with Acute
	{0x78,0x10,0x0A,0x09,0x70},		//   (202) 'n - 0x0144 Latin Small Letter N with Acute
	{0x7C,0x09,0x12,0x21,0x7C},		//   (203)  N - 0x0147 Latin Capital Letter N with Caron
	{0x78,0x11,0x0A,0x09,0x70},		//   (204)  n - 0x0148 Latin Small Letter N with Caron
	{0x38,0x47,0x44,0x47,0x38},		//   (205) "O - 0x0150 Latin Capital Letter O with Double Acute
	{0x30,0x4B,0x48,0x4B,0x30},		//   (206) "o - 0x0151 Latin Small Letter O with Double Acute
	{0x3E,0x41,0x7F,0x49,0x49},		//   (207) OE - 0x0152 Latin Capital Ligature Oe
	{0x38,0x44,0x38,0x54,0x58},		//   (208) oe - 0x0153 Latin Small Ligature Oe
	{0x7C,0x15,0x16,0x35,0x48},		//   (209)  R - 0x0158 Latin Capital Letter R with Caron
	{0x78,0x11,0x0A,0x09,0x10},		//   (210)  r - 0x0159 Latin Small Letter R with Caron
	{0x48,0x54,0x56,0x55,0x20},		//   (211) 'S - 0x015A Latin Capital Letter S with Acute
	{0x20,0x48,0x56,0x55,0x20},		//   (212) 's - 0x015B Latin Small Letter S with Acute
	{0x48,0x55,0x56,0x55,0x20},		//   (213)  S - 0x0160 Latin Capital Letter S with Caron
	{0x20,0x49,0x56,0x55,0x20},		//   (214)  s - 0x0161 Latin Small Letter S with Caron
	{0x04,0x05,0x7E,0x05,0x04},		//   (215)  T - 0x0164 Latin Capital Letter T with Caron
	{0x08,0x3C,0x48,0x22,0x01},		//   (216) t' - 0x0165 Latin Small Letter T with Caron
	{0x3C,0x42,0x45,0x42,0x3C},		//   (217)  U - 0x016E Latin Capital Letter U with Ring Above
	{0x38,0x42,0x45,0x22,0x78},		//   (218)  u - 0x016F Latin Small Letter U with Ring Above
	{0x3C,0x43,0x40,0x43,0x3C},		//   (219) "U - 0x0170 Latin Capital Letter U with Double Acute
	{0x38,0x43,0x40,0x23,0x78},		//   (220) "u - 0x0171 Latin Small Letter U with Double Acute
	{0x0C,0x11,0x60,0x11,0x0C},		//   (221) "Y - 0x0178 Latin Capital Letter Y with Diaeresis
	{0x44,0x66,0x55,0x4C,0x44},		//   (222) 'Z - 0x0179 Latin Capital Letter Z with Acute
	{0x48,0x6A,0x59,0x48,0x00},		//   (223) 'z - 0x017A Latin Small Letter Z with Acute
	{0x44,0x64,0x55,0x4C,0x44},		//   (224)  Z - 0x017B Latin Capital Letter Z with Dot Above
	{0x48,0x68,0x5A,0x48,0x00},		//   (225)  z - 0x017C Latin Small Letter Z with Dot Above
	{0x44,0x65,0x56,0x4D,0x44},		//   (226)  Z - 0x017D Latin Capital Letter Z with Caron
	{0x48,0x69,0x5A,0x49,0x00},		//   (227)  z - 0x017E Latin Small Letter Z with Caron
	{0x00,0x02,0x01,0x02,0x00},		//   (228)  ^ - 0x02C6 Modifier Letter Circumflex Accent
	{0x00,0x01,0x02,0x01,0x00},		//   (229)    - 0x02C7 Caron
	{0x00,0x01,0x01,0x01,0x00},		//   (230)    - 0x02C9 Modifier Letter Macron
	{0x01,0x02,0x02,0x01,0x00},		//   (231)    - 0x02D8 Breve
	{0x00,0x00,0x01,0x00,0x00},		//   (232)    - 0x02D9 Dot Above
	{0x00,0x02,0x05,0x02,0x00},		//   (233)    - 0x02DA Ring Above
	{0x02,0x01,0x02,0x01,0x00},		//   (234)  ~ - 0x02DC Small Tilde
	{0x7F,0x05,0x15,0x3A,0x50},		//   (235) Pt - 0x20A7 Peseta Sign
	{0x3E,0x55,0x55,0x41,0x22},		//   (236)  C - 0x20AC Euro Sign
	{0x18,0x14,0x08,0x14,0x0C},		//   (237)    - 0x221E Infinity
	{0x44,0x4A,0x4A,0x51,0x51},		//   (238)  < - 0x2264 Less-Than or Equal to
	{0x51,0x51,0x4A,0x4A,0x44},		//   (239)  > - 0x2265 Greater-Than or Equal to
	{0x74,0x42,0x41,0x42,0x74},		//   (240)    - 0x2302 House
};


unsigned char code Ascii_2[107][5]={		// Refer to "Times New Roman" Font Database...
						//   Greek & Japanese Letters
	{0x7E,0x11,0x11,0x11,0x7E},		//   (  1)  A - 0x0391 Greek Capital Letter Alpha
	{0x7F,0x49,0x49,0x49,0x36},		//   (  2)  B - 0x0392 Greek Capital Letter Beta
	{0x7F,0x02,0x01,0x01,0x03},		//   (  3)    - 0x0393 Greek Capital Letter Gamma
	{0x70,0x4E,0x41,0x4E,0x70},		//   (  4)    - 0x0394 Greek Capital Letter Delta
	{0x7F,0x49,0x49,0x49,0x41},		//   (  5)  E - 0x0395 Greek Capital Letter Epsilon
	{0x61,0x51,0x49,0x45,0x43},		//   (  6)  Z - 0x0396 Greek Capital Letter Zeta
	{0x7F,0x08,0x08,0x08,0x7F},		//   (  7)  H - 0x0397 Greek Capital Letter Eta
	{0x3E,0x49,0x49,0x49,0x3E},		//   (  8)    - 0x0398 Greek Capital Letter Theta
	{0x00,0x41,0x7F,0x41,0x00},		//   (  9)  I - 0x0399 Greek Capital Letter Iota
	{0x7F,0x08,0x14,0x22,0x41},		//   ( 10)  K - 0x039A Greek Capital Letter Kappa
	{0x70,0x0E,0x01,0x0E,0x70},		//   ( 11)    - 0x039B Greek Capital Letter Lamda
	{0x7F,0x02,0x0C,0x02,0x7F},		//   ( 12)  M - 0x039C Greek Capital Letter Mu
	{0x7F,0x04,0x08,0x10,0x7F},		//   ( 13)  N - 0x039D Greek Capital Letter Nu
	{0x63,0x5D,0x49,0x5D,0x63},		//   ( 14)    - 0x039E Greek Capital Letter Xi
	{0x3E,0x41,0x41,0x41,0x3E},		//   ( 15)  O - 0x039F Greek Capital Letter Omicron
	{0x41,0x3F,0x01,0x3F,0x41},		//   ( 16)    - 0x03A0 Greek Capital Letter Pi
	{0x7F,0x09,0x09,0x09,0x06},		//   ( 17)  P - 0x03A1 Greek Capital Letter Rho
	{0x63,0x55,0x49,0x41,0x41},		//   ( 18)    - 0x03A3 Greek Capital Letter Sigma
	{0x01,0x01,0x7F,0x01,0x01},		//   ( 19)  T - 0x03A4 Greek Capital Letter Tau
	{0x03,0x01,0x7E,0x01,0x03},		//   ( 20)    - 0x03A5 Greek Capital Letter Upsilon
	{0x08,0x55,0x7F,0x55,0x08},		//   ( 21)    - 0x03A6 Greek Capital Letter Phi
	{0x63,0x14,0x08,0x14,0x63},		//   ( 22)  X - 0x03A7 Greek Capital Letter Chi
	{0x07,0x48,0x7F,0x48,0x07},		//   ( 23)    - 0x03A8 Greek Capital Letter Psi
	{0x5E,0x61,0x01,0x61,0x5E},		//   ( 24)    - 0x03A9 Greek Capital Letter Omega
	{0x38,0x44,0x48,0x30,0x4C},		//   ( 25)  a - 0x03B1 Greek Small Letter Alpha
	{0x7C,0x2A,0x2A,0x2A,0x14},		//   ( 26)  B - 0x03B2 Greek Small Letter Beta
	{0x44,0x38,0x04,0x04,0x08},		//   ( 27)  r - 0x03B3 Greek Small Letter Gamma
	{0x30,0x4B,0x4D,0x59,0x30},		//   ( 28)    - 0x03B4 Greek Small Letter Delta
	{0x28,0x54,0x54,0x44,0x20},		//   ( 29)    - 0x03B5 Greek Small Letter Epsilon
	{0x00,0x18,0x55,0x52,0x22},		//   ( 30)    - 0x03B6 Greek Small Letter Zeta
	{0x3E,0x04,0x02,0x02,0x7C},		//   ( 31)  n - 0x03B7 Greek Small Letter Eta
	{0x3C,0x4A,0x4A,0x4A,0x3C},		//   ( 32)    - 0x03B8 Greek Small Letter Theta
	{0x00,0x3C,0x40,0x20,0x00},		//   ( 33)  i - 0x03B9 Greek Small Letter Iota
	{0x7C,0x10,0x28,0x44,0x40},		//   ( 34)  k - 0x03BA Greek Small Letter Kappa
	{0x41,0x32,0x0C,0x30,0x40},		//   ( 35)    - 0x03BB Greek Small Letter Lamda
	{0x7E,0x20,0x20,0x10,0x3E},		//   ( 36)  u - 0x03BC Greek Small Letter Mu
	{0x1C,0x20,0x40,0x20,0x1C},		//   ( 37)  v - 0x03BD Greek Small Letter Nu
	{0x14,0x2B,0x2A,0x2A,0x60},		//   ( 38)    - 0x03BE Greek Small Letter Xi
	{0x38,0x44,0x44,0x44,0x38},		//   ( 39)  o - 0x03BF Greek Small Letter Omicron
	{0x44,0x3C,0x04,0x7C,0x44},		//   ( 40)    - 0x03C0 Greek Small Letter Pi
	{0x70,0x28,0x24,0x24,0x18},		//   ( 41)  p - 0x03C1 Greek Small Letter Rho
	{0x0C,0x12,0x12,0x52,0x60},		//   ( 42)    - 0x03C2 Greek Small Letter Final Sigma
	{0x38,0x44,0x4C,0x54,0x24},		//   ( 43)    - 0x03C3 Greek Small Letter Sigma
	{0x04,0x3C,0x44,0x20,0x00},		//   ( 44)  t - 0x03C4 Greek Small Letter Tau
	{0x3C,0x40,0x40,0x20,0x1C},		//   ( 45)  v - 0x03C5 Greek Small Letter Upsilon
	{0x18,0x24,0x7E,0x24,0x18},		//   ( 46)    - 0x03C6 Greek Small Letter Phi
	{0x44,0x28,0x10,0x28,0x44},		//   ( 47)  x - 0x03C7 Greek Small Letter Chi
	{0x0C,0x10,0x7E,0x10,0x0C},		//   ( 48)    - 0x03C8 Greek Small Letter Psi
	{0x38,0x44,0x30,0x44,0x38},		//   ( 49)  w - 0x03C9 Greek Small Letter Omega
	{0x0A,0x0A,0x4A,0x2A,0x1E},		//   ( 50)    - 0xFF66 Katakana Letter Wo
	{0x04,0x44,0x34,0x14,0x0C},		//   ( 51)    - 0xFF67 Katakana Letter Small A
	{0x20,0x10,0x78,0x04,0x00},		//   ( 52)    - 0xFF68 Katakana Letter Small I
	{0x18,0x08,0x4C,0x48,0x38},		//   ( 53)    - 0xFF69 Katakana Letter Small U
	{0x48,0x48,0x78,0x48,0x48},		//   ( 54)    - 0xFF6A Katakana Letter Small E
	{0x48,0x28,0x18,0x7C,0x08},		//   ( 55)    - 0xFF6B Katakana Letter Small O
	{0x08,0x7C,0x08,0x28,0x18},		//   ( 56)    - 0xFF6C Katakana Letter Small Ya
	{0x40,0x48,0x48,0x78,0x40},		//   ( 57)    - 0xFF6D Katakana Letter Small Yu
	{0x54,0x54,0x54,0x7C,0x00},		//   ( 58)    - 0xFF6E Katakana Letter Small Yo
	{0x18,0x00,0x58,0x40,0x38},		//   ( 59)    - 0xFF6F Katakana Letter Small Tu
	{0x08,0x08,0x08,0x08,0x08},		//   ( 60)    - 0xFF70 Katakana-Hiragana Prolonged Sound Mark
	{0x01,0x41,0x3D,0x09,0x07},		//   ( 61)    - 0xFF71 Katakana Letter A
	{0x10,0x08,0x7C,0x02,0x01},		//   ( 62)    - 0xFF72 Katakana Letter I
	{0x0E,0x02,0x43,0x22,0x1E},		//   ( 63)    - 0xFF73 Katakana Letter U
	{0x42,0x42,0x7E,0x42,0x42},		//   ( 64)    - 0xFF74 Katakana Letter E
	{0x22,0x12,0x0A,0x7F,0x02},		//   ( 65)    - 0xFF75 Katakana Letter O
	{0x42,0x3F,0x02,0x42,0x3E},		//   ( 66)    - 0xFF76 Katakana Letter Ka
	{0x0A,0x0A,0x7F,0x0A,0x0A},		//   ( 67)    - 0xFF77 Katakana Letter Ki
	{0x08,0x46,0x42,0x22,0x1E},		//   ( 68)    - 0xFF78 Katakana Letter Ku
	{0x04,0x03,0x42,0x3E,0x02},		//   ( 69)    - 0xFF79 Katakana Letter Ke
	{0x42,0x42,0x42,0x42,0x7E},		//   ( 70)    - 0xFF7A Katakana Letter Ko
	{0x02,0x4F,0x22,0x1F,0x02},		//   ( 71)    - 0xFF7B Katakana Letter Sa
	{0x4A,0x4A,0x40,0x20,0x1C},		//   ( 72)    - 0xFF7C Katakana Letter Shi
	{0x42,0x22,0x12,0x2A,0x46},		//   ( 73)    - 0xFF7D Katakana Letter Su
	{0x02,0x3F,0x42,0x4A,0x46},		//   ( 74)    - 0xFF7E Katakana Letter Se
	{0x06,0x48,0x40,0x20,0x1E},		//   ( 75)    - 0xFF7F Katakana Letter So
	{0x08,0x46,0x4A,0x32,0x1E},		//   ( 76)    - 0xFF80 Katakana Letter Ta
	{0x0A,0x4A,0x3E,0x09,0x08},		//   ( 77)    - 0xFF81 Katakana Letter Chi
	{0x0E,0x00,0x4E,0x20,0x1E},		//   ( 78)    - 0xFF82 Katakana Letter Tsu
	{0x04,0x45,0x3D,0x05,0x04},		//   ( 79)    - 0xFF83 Katakana Letter Te
	{0x00,0x7F,0x08,0x10,0x00},		//   ( 80)    - 0xFF84 Katakana Letter To
	{0x44,0x24,0x1F,0x04,0x04},		//   ( 81)    - 0xFF85 Katakana Letter Na
	{0x40,0x42,0x42,0x42,0x40},		//   ( 82)    - 0xFF86 Katakana Letter Ni
	{0x42,0x2A,0x12,0x2A,0x06},		//   ( 83)    - 0xFF87 Katakana Letter Nu
	{0x22,0x12,0x7B,0x16,0x22},		//   ( 84)    - 0xFF88 Katakana Letter Ne
	{0x00,0x40,0x20,0x1F,0x00},		//   ( 85)    - 0xFF89 Katakana Letter No
	{0x78,0x00,0x02,0x04,0x78},		//   ( 86)    - 0xFF8A Katakana Letter Ha
	{0x3F,0x44,0x44,0x44,0x44},		//   ( 87)    - 0xFF8B Katakana Letter Hi
	{0x02,0x42,0x42,0x22,0x1E},		//   ( 88)    - 0xFF8C Katakana Letter Fu
	{0x04,0x02,0x04,0x08,0x30},		//   ( 89)    - 0xFF8D Katakana Letter He
	{0x32,0x02,0x7F,0x02,0x32},		//   ( 90)    - 0xFF8E Katakana Letter Ho
	{0x02,0x12,0x22,0x52,0x0E},		//   ( 91)    - 0xFF8F Katakana Letter Ma
	{0x00,0x2A,0x2A,0x2A,0x40},		//   ( 92)    - 0xFF90 Katakana Letter Mi
	{0x38,0x24,0x22,0x20,0x70},		//   ( 93)    - 0xFF91 Katakana Letter Mu
	{0x40,0x28,0x10,0x28,0x06},		//   ( 94)    - 0xFF92 Katakana Letter Me
	{0x0A,0x3E,0x4A,0x4A,0x4A},		//   ( 95)    - 0xFF93 Katakana Letter Mo
	{0x04,0x7F,0x04,0x14,0x0C},		//   ( 96)    - 0xFF94 Katakana Letter Ya
	{0x40,0x42,0x42,0x7E,0x40},		//   ( 97)    - 0xFF95 Katakana Letter Yu
	{0x4A,0x4A,0x4A,0x4A,0x7E},		//   ( 98)    - 0xFF96 Katakana Letter Yo
	{0x04,0x05,0x45,0x25,0x1C},		//   ( 99)    - 0xFF97 Katakana Letter Ra
	{0x0F,0x40,0x20,0x1F,0x00},		//   (100)    - 0xFF98 Katakana Letter Ri
	{0x7C,0x00,0x7E,0x40,0x30},		//   (101)    - 0xFF99 Katakana Letter Ru
	{0x7E,0x40,0x20,0x10,0x08},		//   (102)    - 0xFF9A Katakana Letter Re
	{0x7E,0x42,0x42,0x42,0x7E},		//   (103)    - 0xFF9B Katakana Letter Ro
	{0x0E,0x02,0x42,0x22,0x1E},		//   (104)    - 0xFF9C Katakana Letter Wa
	{0x42,0x42,0x40,0x20,0x18},		//   (105)    - 0xFF9D Katakana Letter N
	{0x02,0x04,0x01,0x02,0x00},		//   (106)    - 0xFF9E Katakana Voiced Sound Mark
	{0x07,0x05,0x07,0x00,0x00},		//   (107)    - 0xFF9F Katakana Semi-Voiced Sound Mark
};


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM(unsigned char Data)
{
unsigned char i,j;

	for(i=0;i<8;i++)
	{
		Set_Start_Page(i);
		Set_Start_Column(0x00);

		for(j=0;j<128;j++)
		{
			Write_Data(Data);
		}
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)
//
//    a: Start Page
//    b: End Page
//    c: Start Column
//    d: Total Columns
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block(unsigned char Data, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
unsigned char i,j;
	
	for(i=a;i<(b+1);i++)
	{
		Set_Start_Page(i);
		Set_Start_Column(c);

		for(j=0;j<d;j++)
		{
			Write_Data(Data);
		}
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Checkboard (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Checkerboard()
{
unsigned char i,j;
	
	for(i=0;i<8;i++)
	{
		Set_Start_Page(i);
		Set_Start_Column(0x00);

		for(j=0;j<64;j++)
		{
			Write_Data(0x55);
			Write_Data(0xaa);
		}
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Frame (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Frame()
{
unsigned char i,j;
	
	Set_Start_Page(0x00);
	Set_Start_Column(XLevel);

	for(i=0;i<Max_Column;i++)
	{
		Write_Data(0x01);
	}

	Set_Start_Page(0x07);
	Set_Start_Column(XLevel);

	for(i=0;i<Max_Column;i++)
	{
		Write_Data(0x80);
	}

	for(i=0;i<8;i++)
	{
		Set_Start_Page(i);

		for(j=0;j<Max_Column;j+=(Max_Column-1))
		{
			Set_Start_Column(XLevel+j);

			Write_Data(0xFF);
		}
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Character (5x7)
//
//    a: Database
//    b: Ascii
//    c: Start Page
//    d: Start Column
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Font57(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
unsigned char *Src_Pointer;
unsigned char i;

	switch(a)
	{
		case 1:
			Src_Pointer=&Ascii_1[(b-1)][0];
			break;
		case 2:
			Src_Pointer=&Ascii_2[(b-1)][0];
			break;
	}
	Set_Start_Page(c);
	Set_Start_Column(d);

	for(i=0;i<5;i++)
	{
		Write_Data(*Src_Pointer);
		Src_Pointer++;
	}
	Write_Data(0x00);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show String
//
//    a: Database
//    b: Start Page
//    c: Start Column
//    * Must write "0" in the end...
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_String(unsigned char a, unsigned char *Data_Pointer, unsigned char b, unsigned char c)
{
unsigned char *Src_Pointer;

	Src_Pointer=Data_Pointer;
	Show_Font57(1,96,b,c);			// No-Break Space
						//   Must be written first before the string start...

	while(1)
	{
		Show_Font57(a,*Src_Pointer,b,c);
		Src_Pointer++;
		c+=6;
		if(*Src_Pointer == 0) break;
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Start Page
//    b: End Page
//    c: Start Column
//    d: Total Columns
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Pattern(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
unsigned char *Src_Pointer;
unsigned char i,j;

	Src_Pointer=Data_Pointer;
	for(i=a;i<(b+1);i++)
	{
		Set_Start_Page(i);
		Set_Start_Column(c);

		for(j=0;j<d;j++)
		{
			Write_Data(*Src_Pointer);
			Src_Pointer++;
		}
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Vertical / Fade Scrolling (Partial or Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Upward)
//       "0x01" (Downward)
//    b: Set Top Fixed Area
//    c: Set Vertical Scroll Area
//    d: Set Numbers of Row Scroll per Step
//    e: Set Time Interval between Each Scroll Step
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Vertical_Scroll(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e)
{
unsigned int i,j;	

	Write_Command(0xA3);			// Set Vertical Scroll Area
	Write_Command(b);			//   Default => 0x00 (Top Fixed Area)
	Write_Command(c);			//   Default => 0x40 (Vertical Scroll Area)

	switch(a)
	{
		case 0:
			for(i=0;i<c;i+=d)
			{
				Set_Start_Line(i);
				for(j=0;j<e;j++)
				{
					uDelay(200);
				}
			}
			break;
		case 1:
			for(i=0;i<c;i+=d)
			{
				Set_Start_Line(c-i);
				for(j=0;j<e;j++)
				{
					uDelay(200);
				}
			}
			break;
	}
	Set_Start_Line(0x00);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Continuous Horizontal Scrolling (Partial or Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Rightward)
//       "0x01" (Leftward)
//    b: Define Start Page Address
//    c: Define End Page Address
//    d: Set Time Interval between Each Scroll Step in Terms of Frame Frequency
//    e: Delay Time
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Horizontal_Scroll(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e)
{
	Write_Command(0x26|a);			// Horizontal Scroll Setup
	Write_Command(0x00);			//           => (Dummy Write for First Parameter)
	Write_Command(b);
	Write_Command(d);
	Write_Command(c);
	Write_Command(0x2F);			// Activate Scrolling
	Delay(e);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Continuous Vertical / Horizontal / Diagonal Scrolling (Partial or Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Vertical & Rightward)
//       "0x01" (Vertical & Leftward)
//    b: Define Start Row Address (Horizontal / Diagonal Scrolling)
//    c: Define End Page Address (Horizontal / Diagonal Scrolling)
//    d: Set Top Fixed Area (Vertical Scrolling)
//    e: Set Vertical Scroll Area (Vertical Scrolling)
//    f: Set Numbers of Row Scroll per Step (Vertical / Diagonal Scrolling)
//    g: Set Time Interval between Each Scroll Step in Terms of Frame Frequency
//    h: Delay Time
//    * d+e must be less than or equal to the Multiplex Ratio...
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Continuous_Scroll(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f, unsigned char g, unsigned char h)
{
	Write_Command(0xA3);			// Set Vertical Scroll Area
	Write_Command(d);			//   Default => 0x00 (Top Fixed Area)
	Write_Command(e);			//   Default => 0x40 (Vertical Scroll Area)

	Write_Command(0x29+a);			// Continuous Vertical & Horizontal Scroll Setup
	Write_Command(0x00);			//           => (Dummy Write for First Parameter)
	Write_Command(b);
	Write_Command(g);
	Write_Command(c);
	Write_Command(f);
	Write_Command(0x2F);			// Activate Scrolling
	Delay(h);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Deactivate Scrolling (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Deactivate_Scroll()
{
	Write_Command(0x2E);			// Deactivate Scrolling
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Fade In (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_In()
{
unsigned int i;	

	Set_Display_On_Off(0x01);
	for(i=0;i<(Brightness+1);i++)
	{
		Set_Contrast_Control(i);
		uDelay(200);
		uDelay(200);
		uDelay(200);
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Fade Out (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_Out()
{
unsigned int i;	

	for(i=(Brightness+1);i>0;i--)
	{
		Set_Contrast_Control(i-1);
		uDelay(200);
		uDelay(200);
		uDelay(200);
	}
	Set_Display_On_Off(0x00);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Sleep Mode
//
//    "0x00" Enter Sleep Mode
//    "0x01" Exit Sleep Mode
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Sleep(unsigned char a)
{
	switch(a)
	{
		case 0:
			Set_Display_On_Off(0x00);
			Set_Entire_Display(0x01);
			break;
		case 1:
			Set_Entire_Display(0x00);
			Set_Display_On_Off(0x01);
			break;
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Connection Test
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Test()
{
unsigned char i;

	RES=0;
	for(i=0;i<200;i++)
	{
		uDelay(200);
	}
	RES=1;

	Set_Entire_Display(0x01);		// Enable Entire Display On (0x00/0x01)

	while(1)
	{
		Set_Display_On_Off(0x01);	// Display On (0x00/0x01)
		Delay(2);
		Set_Display_On_Off(0x00);	// Display Off (0x00/0x01)
		Delay(2);
	}
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Initialization
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_Init_E()				// VCC Supplied Externally
{
unsigned char i;

	RES=0;
	for(i=0;i<200;i++)
	{
		uDelay(200);
	}
	RES=1;

	Set_Display_On_Off(0x00);		// Display Off (0x00/0x01)
	Set_Display_Clock(0x80);		// Set Clock as 100 Frames/Sec
	Set_Multiplex_Ratio(0x3F);		// 1/64 Duty (0x0F~0x3F)
	Set_Display_Offset(0x00);		// Shift Mapping RAM Counter (0x00~0x3F)
	Set_Start_Line(0x00);			// Set Mapping RAM Display Start Line (0x00~0x3F)
	Set_Charge_Pump(0x00);			// Disable Embedded DC/DC Converter (0x00/0x04)
	Set_Addressing_Mode(0x02);		// Set Page Addressing Mode (0x00/0x01/0x02)
	Set_Segment_Remap(0x01);		// Set SEG/Column Mapping (0x00/0x01)
	Set_Common_Remap(0x08);			// Set COM/Row Scan Direction (0x00/0x08)
	Set_Common_Config(0x10);		// Set Sequential Configuration (0x00/0x10)
	Set_Contrast_Control(Brightness);	// Set SEG Output Current
	Set_Precharge_Period(0x22);		// Set Pre-Charge as 2 Clocks & Discharge as 2 Clock
	Set_VCOMH(0x40);			// Set VCOM Deselect Level
	Set_Entire_Display(0x00);		// Disable Entire Display On (0x00/0x01)
	Set_Inverse_Display(0x00);		// Disable Inverse Display On (0x00/0x01)

	Fill_RAM(0x00);				// Clear Screen

	Set_Display_On_Off(0x01);		// Display On (0x00/0x01)
}


void OLED_Init_I()				// VCC Generated by Internal DC/DC Circuit
{
unsigned char i;

	RES=0;
	for(i=0;i<200;i++)
	{
		uDelay(200);
		//uDelay(200);
	}
	RES=1;

	Set_Display_On_Off(0x00);		// Display Off (0x00/0x01)
	Set_Display_Clock(0x80);		// Set Clock as 100 Frames/Sec
	Set_Multiplex_Ratio(0x3F);		// 1/64 Duty (0x0F~0x3F)
	Set_Display_Offset(0x00);		// Shift Mapping RAM Counter (0x00~0x3F)
	Set_Start_Line(0x00);			// Set Mapping RAM Display Start Line (0x00~0x3F)
	Set_Charge_Pump(0x04);			// Enable Embedded DC/DC Converter (0x00/0x04)
	Set_Addressing_Mode(0x02);		// Set Page Addressing Mode (0x00/0x01/0x02)
	Set_Segment_Remap(0x01);		// Set SEG/Column Mapping (0x00/0x01)
	Set_Common_Remap(0x00);			// Set COM/Row Scan Direction (0x00/0x08)  ////////////////
	Set_Common_Config(0x10);		// Set Sequential Configuration (0x00/0x10)
	Set_Contrast_Control(Brightness);	// Set SEG Output Current
	Set_Precharge_Period(0xF1);		// Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	Set_VCOMH(0x40);			// Set VCOM Deselect Level
	Set_Entire_Display(0x00);		// Disable Entire Display On (0x00/0x01)
	Set_Inverse_Display(0x00);		// Disable Inverse Display On (0x00/0x01)

	Fill_RAM(0x00);				// Clear Screen

	Set_Display_On_Off(0x01);		// Display On (0x00/0x01)
}
/*******************************function************************************************************/
//*******************显示X，Y坐标处的一个字符。一行可以显示16个字符。X为0到15，Y为0到3**************
void disc(unsigned char X,unsigned char Y,unsigned char * c)
{
	unsigned char n;
	Set_Common_Remap(0x00);			// Set COM/Row Scan Direction (0x00/0x08) 
	Write_Command (0xb7-(Y<<1));	   //b7 Page Address
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
	Set_Common_Remap(0x00);			// Set COM/Row Scan Direction (0x00/0x08) 
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
	Set_Common_Remap(0x00);			// Set COM/Row Scan Direction (0x00/0x08) 
	Write_Command (0xb7-(Y<<1));
	if(X%2)
		Write_Command (0x08);
	else
		Write_Command (0x00);
	Write_Command (0x10+(X>>1));
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


/////////////////////////////////////////

void welcome(void)
{
  //一屏显示32个汉字
 // dish(0,0,dian);dish(2,0,zi);dish(4,0,wen);dish(6,0,du);dish(8,0,shidu);dish(10,0,zhou);dish(12,0,yi);dish(14,0,er);
 // dish(0,1,dian);dish(2,1,zi);dish(4,1,wen);dish(6,1,du);dish(8,1,shidu);dish(10,1,zhou);dish(12,1,yi);dish(14,1,er);
 // dish(0,2,dian);dish(2,2,zi);dish(4,2,wen);dish(6,2,du);dish(8,2,shidu);dish(10,2,zhou);dish(12,2,yi);dish(14,2,er);
 // dish(0,3,dian);dish(2,3,zi);dish(4,3,wen);dish(6,3,du);dish(8,3,shidu);dish(10,3,zhou);dish(12,3,yi);dish(14,3,er);
  
//温度湿度
  disn(1,0,2);disn(2,0,0);disn(3,0,1);disn(4,0,0);dish(5,0,year);
  disn(7,0,1);disn(8,0,0);dish(9,0,month);disn(11,0,0);disn(12,0,1);dish(13,0,day);

  disn(1,1,1);disn(2,1,2);disc(3,1,mao);disn(4,1,0);
  disn(5,1,0);disc(6,1,mao);disn(7,1,0);disn(8,1,0);

  dish(11,1,zhou);dish(13,1,si);

  dish(1,2,wen);dish(3,2,du);disc(5,2,mao);    //dish 全字
  //disn(6,2,1);

  //disn(7,2,1);disn(8,2,9);disc(9,2,dianhao);disn(10,2,6);//disn 数字
  dish(13,2,dufuhao);//dish

  dish(1,3,shidu);dish(3,3,du);disc(5,3,mao);
  //disn(6,3,1);
  disn(7,3,6);disn(8,3,2);disc(9,3,dianhao);disn(10,3,4);//disn 数字
  disc(12,3,baifhao);disc(13,3,R);disc(14,3,H);//disc半字
	   
}

void tm0_ini(void)
{
   TMOD=0x01;
   ET0=1;
   EA=1;
   TR0=1;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Main Program
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void main()
{ 	
	P1=0xFF;
	P3=0xFF;
	Init_DS18B20();
	OLED_Init_I();
	//OLED_Init_E();
	stc12C4052ad_uart_init();

 //Fill_RAM(0x00);			// Clear Screen
 //Set_Common_Remap(0x08);			// Set COM/Row Scan Direction (0x00/0x08)  两种显示字需要的开始地址不同
// Show Pattern - UniV OLED
// Show_Pattern(&UniV[0][0],0x02,0x05,XLevel+0x28,0x30);
// Show_Pattern(&kemei[0][0],0x02,0x06,XLevel+0x00,0x80); //show keimeidianzi
 //Show_Pattern(&shiyan[0][0],0x00,0x07,XLevel+0x00,0x80);
	Delay(1);
//	while(1);

	while(1)
	{	send_UART(0x55);
    	send_UART(0xaa);

	welcome();
	Temperature = ReadTemperature();
   //Temperature = 0x00fa;
  
 t0 = Temperature/1000;
 t1 = Temperature%1000/100;
 t2 = Temperature%100/10;
 t3 = Temperature%10;
 disn(7,2,t0);disn(8,2,t1);disn(9,2,t2);disc(10,2,dianhao);disn(11,2,t3);//disn 数字

 send_UART(0x55);
 send_UART(wh);

 send_UART(0xaa);
 send_UART(wl);	
 Delay(5);
 Fill_RAM(0x00);			// Clear Screen


	   Set_Common_Remap(0x08);			// Set COM/Row Scan Direction (0x00/0x08)  两种显示字需要的开始地址不同

  Fill_RAM(0x00);			// Clear Screen
  Show_Pattern(&kemei[0][0],0x02,0x06,XLevel+0x00,0x80); //show keimeidianzi
 		Delay(10);
 // Fill_RAM(0x00);			// Clear Screen
// Show_Pattern(&shiyan[0][0],0x00,0x07,XLevel+0x00,0x80);
	// Fade In/Out (Full Screen)
		Fade_Out();
		Fade_In();
		Fade_Out();
		Fade_In();
		Delay(1);

	// Scrolling (Partial or Full Screen)
		Vertical_Scroll(0x00,0x00,Max_Row,0x01,0x20);
						// Upward
		Delay(1);
		Vertical_Scroll(0x01,0x00,Max_Row,0x01,0x20);
						// Downward
		Delay(1);
		Deactivate_Scroll();
		Continuous_Scroll(0x00,0x00,0x00,0x00,0x20,0x01,0x00,0x01);
						// Upward - Top Area
		Continuous_Scroll(0x00,0x00,0x00,0x00,0x20,0x1F,0x00,0x01);
						// Downward - Top Area
		Continuous_Scroll(0x00,0x00,0x03,0x00,0x20,0x01,0x00,0x02);
						// Up & Rightward - Top Area
		Continuous_Scroll(0x01,0x00,0x03,0x00,0x20,0x1F,0x00,0x02);
						// Down & Leftward - Top Area
		Continuous_Scroll(0x01,0x04,0x07,0x00,0x20,0x01,0x00,0x02);
						// Upward - Top Area
						// Leftward - Bottom Area
		Continuous_Scroll(0x00,0x04,0x07,0x00,0x20,0x1F,0x00,0x02);
						// Downward - Top Area
						// Rightward - Bottom Area
		Deactivate_Scroll();

	// All Pixels On (Test Pattern)
		Fill_RAM(0xFF);
		Delay(1);

	// Checkerboard (Test Pattern)
		Checkerboard();
		Delay(1);
		Fill_RAM(0x00);			// Clear Screen

	// Frame (Test Pattern)
		Frame();
		Delay(1);

	// Show String - keimei 15953865293
		Show_String(1,&Name,0x03,XLevel+0x25);
		Show_String(1,&Tel,0x04,XLevel+0x16);
		Delay(1);
		Fill_RAM(0x00);			// Clear Screen		  
		send_UART(0xaa);
		send_UART(0x55);	  
	}			
}