#include"systemInit.h"
//#include <hidef.h>
#include <stdio.h> 
#include <math.h>
//#include <MC9S12XS128.h>        /* common defines and macros */
//#include "derivative.h"      /* derivative-specific definitions */
#include "nAsciidot.h"

#define XLevelL		0x00
#define XLevelH		0x10
#define XLevel		((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xCF


/*
两线SPI使用说明：
VBT 供内部DC-DC电压，3.3~4.3V，如果使用5V电压，为保险起见串一个100~500欧的电阻
VCC 供内部逻辑电压 1.8~6V
PEN 逻辑电压使能开关，高电平为打开，可由MCU控制或直接接到VCC
GND 地

BS0 高电平
BS1 低电平
BS2 低电平
CS  低电平 ，如果要复用MCU的SCLK和MOSI管脚，CS由MCU控制

DC  低电平 ，也可悬空，但最好设为低电平
RES 悬空由模块上电复位，可由MCU控制（应与MCU同时上电）
RD  低电平 ，也可悬空，但最好设为低电平
RW  低电平 ，也可悬空，但最好设为低电平
RD  低电平 ，也可悬空，但最好设为低电平

D0（SCLK） ，时钟脚，由MCU控制
D1（MOSI） ，主输出从输入数据脚，由MCU控制
D2 悬空

D3-D7 ， 低电平 ， 也可悬空，但最好设为低电平

WriteCommand() WriteData()内部调用软延时函数uDelay()，用于调整时序和速度,
可根据MCU的速度调整uDelay()的参数来实现调整操作的速度

*/

/*----------------------------------------------------------------------------
以下函数与硬件有关
----------------------------------------------------------------------------*/

void PortInit(void)
{
   //在这里加入代码，设置让SCLK（D0），MOSI（D1）这两个管脚具有对外驱动能力

}
void SetSclk(unsigned char val)
{
    if (val==0)
    {
        // 在这里加入代码，控制SCLK（D0）管脚为低电平
		GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,0);
    }
    else
    {
        // 在这里加入代码，控制SCLK（D0）管脚为高电平
		GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,~0);
    }
}


void SetMosi(unsigned char val)
{
    if (val==0)
    {
        // 在这里加入代码，控制MOSI（D1）管脚为低电平
		GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,0);	
    }
    else
    {
        // 在这里加入代码，控制MOSI（D1）管脚为高电平
		GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,~0);	
    }
}


/*----------------------------------------------------------------------------
以下函数与硬件无关，均为对SSD0306的控制函数
----------------------------------------------------------------------------*/
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

void WriteCommand(unsigned char Data)
{
    unsigned char i;

    SetSclk(0);
    SetMosi(0);
    uDelay(15);
    SetSclk(1);
    uDelay(15);

    for (i=0; i<8; i++)
    {
        SetSclk(0);
        if ((Data&0x80)>>7)
        {
            SetMosi(1);
        }
        else
        {
            SetMosi(0);
        }
        Data = Data << 1;
        uDelay(15);
        SetSclk(1);
        uDelay(15);
    }
}


void WriteData(unsigned char Data)
{
    unsigned char i;

    SetSclk(0);
    SetMosi(1);
    uDelay(15);
    SetSclk(1);
    uDelay(15);

    for (i=0; i<8; i++)
    {
        SetSclk(0);
        if ((Data&0x80)>>7)
        {
            SetMosi(1);
        }
        else
        {
            SetMosi(0);
        }
        Data = Data << 1;
        uDelay(15);
        SetSclk(1);
        uDelay(15);
    }

}

void SetStartColumn(unsigned char d)
{
	WriteCommand(0x00+d%16);		// Set Lower Column Start Address for Page Addressing Mode
						//   Default => 0x00
	WriteCommand(0x10+d/16);		// Set Higher Column Start Address for Page Addressing Mode
						//   Default => 0x10
}

void SetAddressingMode(unsigned char d)
{
	WriteCommand(0x20);			// Set Memory Addressing Mode
	WriteCommand(d);			//   Default => 0x02
						//     0x00 => Horizontal Addressing Mode
						//     0x01 => Vertical Addressing Mode
						//     0x02 => Page Addressing Mode
}

void SetColumnAddress(unsigned char a, unsigned char b)
{
	WriteCommand(0x21);			// Set Column Address
	WriteCommand(a);			//   Default => 0x00 (Column Start Address)
	WriteCommand(b);			//   Default => 0x7F (Column End Address)
}

void SetPageAddress(unsigned char a, unsigned char b)
{
	WriteCommand(0x22);			// Set Page Address
	WriteCommand(a);			//   Default => 0x00 (Page Start Address)
	WriteCommand(b);			//   Default => 0x07 (Page End Address)
}

void SetStartLine(unsigned char d)
{
	WriteCommand(0x40|d);			// Set Display Start Line
						//   Default => 0x40 (0x00)
}

void SetContrastControl(unsigned char d)
{
	WriteCommand(0x81);			// Set Contrast Control
	WriteCommand(d);			//   Default => 0x7F
}

void Set_Charge_Pump(unsigned char d)
{
	WriteCommand(0x8D);			// Set Charge Pump
	WriteCommand(0x10|d);			//   Default => 0x10
						//     0x10 (0x00) => Disable Charge Pump
						//     0x14 (0x04) => Enable Charge Pump
}

void Set_Segment_Remap(unsigned char d)
{
	WriteCommand(0xA0|d);			// Set Segment Re-Map
						//   Default => 0xA0
						//     0xA0 (0x00) => Column Address 0 Mapped to SEG0
						//     0xA1 (0x01) => Column Address 0 Mapped to SEG127
}

void Set_Entire_Display(unsigned char d)
{
	WriteCommand(0xA4|d);			// Set Entire Display On / Off
						//   Default => 0xA4
						//     0xA4 (0x00) => Normal Display
						//     0xA5 (0x01) => Entire Display On
}

void Set_Inverse_Display(unsigned char d)
{
	WriteCommand(0xA6|d);			// Set Inverse Display On/Off
						//   Default => 0xA6
						//     0xA6 (0x00) => Normal Display
						//     0xA7 (0x01) => Inverse Display On
}

void Set_Multiplex_Ratio(unsigned char d)
{
	WriteCommand(0xA8);			// Set Multiplex Ratio
	WriteCommand(d);			//   Default => 0x3F (1/64 Duty)
}

void Set_Display_On_Off(unsigned char d)
{
	WriteCommand(0xAE|d);			// Set Display On/Off
						//   Default => 0xAE
						//     0xAE (0x00) => Display Off
						//     0xAF (0x01) => Display On
}

void SetStartPage(unsigned char d)
{
	WriteCommand(0xB0|d);			// Set Page Start Address for Page Addressing Mode
						//   Default => 0xB0 (0x00)
}

void Set_Common_Remap(unsigned char d)
{
	WriteCommand(0xC0|d);			// Set COM Output Scan Direction
						//   Default => 0xC0
						//     0xC0 (0x00) => Scan from COM0 to 63
						//     0xC8 (0x08) => Scan from COM63 to 0
}

void Set_Display_Offset(unsigned char d)
{
	WriteCommand(0xD3);			// Set Display Offset
	WriteCommand(d);			//   Default => 0x00
}

void Set_Display_Clock(unsigned char d)
{
	WriteCommand(0xD5);			// Set Display Clock Divide Ratio / Oscillator Frequency
	WriteCommand(d);			//   Default => 0x80
						//     D[3:0] => Display Clock Divider
						//     D[7:4] => Oscillator Frequency
}

void Set_Precharge_Period(unsigned char d)
{
	WriteCommand(0xD9);			// Set Pre-Charge Period
	WriteCommand(d);			//   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
						//     D[3:0] => Phase 1 Period in 1~15 Display Clocks
						//     D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

void Set_Common_Config(unsigned char d)
{
	WriteCommand(0xDA);			// Set COM Pins Hardware Configuration
	WriteCommand(0x02|d);			//   Default => 0x12 (0x10)
						//     Alternative COM Pin Configuration
						//     Disable COM Left/Right Re-Map
}

void Set_VCOMH(unsigned char d)
{
	WriteCommand(0xDB);			// Set VCOMH Deselect Level
	WriteCommand(d);			//   Default => 0x20 (0.77*VCC)
}

void Set_NOP(void)
{
	WriteCommand(0xE3);			// Command for No Operation
}

/******************************************************************************
  * @function name : DisplayArea
  * @brief  Display Area
  * @param  :
  * @retval :
  */
void FillArea(unsigned char spage, unsigned char epage,
                 unsigned char scolumn, unsigned char ecolumn,
                 unsigned char pt)
{
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char h = 0;
    unsigned char w = 0;

    h = epage - spage + 1;
    w = ecolumn - scolumn;

    while ( j<h )
    {
        SetStartPage(spage + j);
        SetStartColumn(scolumn);
        for (i=0; i<w; i++)
        {
              WriteData(pt);
        }

        j++;
    }
}
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Frame (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void CheckOutline(void)
{		
    unsigned char i,j;
    FillArea(0,7,0,128,0);

    SetStartPage(0x00);
    SetStartColumn(XLevel);

    for(i=0;i<Max_Column;i++)
    {
        WriteData(0x01);
    }

    SetStartPage(0x01);
    SetStartColumn(XLevel);

    for(i=0;i<Max_Column;i++)
    {
        WriteData(0x80);
    }

    SetStartPage(0x02);
    SetStartColumn(XLevel);

    for(i=0;i<Max_Column;i++)
    {
        WriteData(0x01);
    }

    SetStartPage(0x07);
    SetStartColumn(XLevel);

    for(i=0;i<Max_Column;i++)
    {
        WriteData(0x80);
    }

    for(i=0;i<8;i++)
    {
        SetStartPage(i);

        for(j=0;j<Max_Column;j+=(Max_Column-1))
        {
            SetStartColumn(XLevel+j);
            WriteData(0xFF);
        }
    }
}

void OledInit(void)
{	
	Delay(5);
    //从上电到下面开始初始化要有足够的时间，即等待RC复位完毕
    Set_Display_On_Off(0x00);		// Display Off (0x00/0x01)
    Set_Display_Clock(0x80);		// Set Clock as 100 Frames/Sec
    Set_Multiplex_Ratio(0x3F);		// 1/64 Duty (0x0F~0x3F)
    Set_Display_Offset(0x00);		// Shift Mapping RAM Counter (0x00~0x3F)
    SetStartLine(0x00);			    // Set Mapping RAM Display Start Line (0x00~0x3F)
    Set_Charge_Pump(0x04);		    // Enable Embedded DC/DC Converter (0x00/0x04)
    SetAddressingMode(0x02);		// Set Page Addressing Mode (0x00/0x01/0x02)
    Set_Segment_Remap(0x01);		// Set SEG/Column Mapping (0x00/0x01)
    Set_Common_Remap(0x08);			// Set COM/Row Scan Direction (0x00/0x08)
    Set_Common_Config(0x10);		// Set Sequential Configuration (0x00/0x10)
    SetContrastControl(Brightness);	// Set SEG Output Current
    Set_Precharge_Period(0xF1);		// Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    Set_VCOMH(0x40);			   // Set VCOM Deselect Level
    Set_Entire_Display(0x00);		// Disable Entire Display On (0x00/0x01)
    Set_Inverse_Display(0x00);		// Disable Inverse Display On (0x00/0x01)
    FillArea(0,7,0,128,0);			// Clear Screen
	 
    Set_Display_On_Off(0x01);		// Display On (0x00/0x01)
}

/*----------------------------------------------------------------------------
以下函数为应用函数
----------------------------------------------------------------------------*/
/******************************************************************************
  * @function name : Display8x16Str
  * @brief  Display8x16Str
  * @param  :
  * @retval :
  */
void Display8x16Str(unsigned char page, unsigned char column, unsigned char *str) //8x16Ascii字符
{
    unsigned char i = 0;
    //unsigned char temp = 0;
    unsigned int padd= 0;
    while(*str != '\0')
    {
        SetStartPage(page);
        SetStartColumn(column);
        padd = (*str-32)*16;
        for (i=0; i<8; i++)
        {
             WriteData(nAsciiDot8x16[padd+i]);
        }

        SetStartPage(page+1);
        SetStartColumn(column);
        for (i=8; i<16; i++)
        {
           WriteData(nAsciiDot8x16[padd+i]);
        }
        str++;
        column += 8;
   }
}

void DisplayChinese(unsigned char page, unsigned char column, unsigned int location)//16x16中文字符
{ 	
		unsigned char i = 0;
		location=location*32;
        SetStartPage(page);
        SetStartColumn(column);
        for (i=0; i<16; i++)
        {
             WriteData(GB16X16[location+i]);
        }
        SetStartPage(page+1);
        SetStartColumn(column);
        for (i=16; i<32; i++)
        {
           WriteData(GB16X16[location+i]);
        }
}
  
void Displaypicture1(unsigned char page ,unsigned char column)  //爱心（图片）
{ 	
	unsigned char i = 0;
    unsigned char const picture1[]=
	{
	0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x80,0xC0,//
	0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x0F,0x1F,0x3F,0x7F,//
	0xFF,0xFF,0xFF,0xFE,0xFE,0xFF,0xFF,0xFF,0x7F,0x3F,0x1F,0x0F,0x07,0x00,0x00,0x00,//
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x03,0x01,0x00,//
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//未命名文件0
	};
        SetStartPage(page);
        SetStartColumn(column);
        for (i=0; i<24; i++){WriteData(picture1[i]);}
		SetStartPage(page+1);
        SetStartColumn(column);
		for (i=24; i<48; i++){WriteData(picture1[i]);}
		SetStartPage(page+2);
        SetStartColumn(column);
		for (i=48; i<72; i++){WriteData(picture1[i]);}
}
void Displaypictureclear1(unsigned char page ,unsigned char column)  //爱心(图片清除)
{ 	
	unsigned char i = 0;
        SetStartPage(page);
        SetStartColumn(column);
        for (i=0; i<24; i++){WriteData(0x00);}
		SetStartPage(page+1);
        SetStartColumn(column);
		for (i=24; i<48; i++){WriteData(0x00);}
		SetStartPage(page+2);
        SetStartColumn(column);
		for (i=48; i<72; i++){WriteData(0x00);}
}	

void TestMain(void)
{	

	unsigned char i = 0;
    OledInit();
	while(1)
   {  	
   		if(i==0)
		{
		i=1;
		Displaypictureclear1(5 ,52);
		Displaypicture1(0,0);
		Displaypicture1(0,104);
		}
		else
		{
		i=0;
		Displaypictureclear1(0,0);
		Displaypictureclear1(0,104);
		Displaypicture1(5,52);
		}
	    DisplayChinese(0,60,5);	//爱
		DisplayChinese(3,30,0);	//赵
		DisplayChinese(3,60,1);	//雅
		DisplayChinese(3,90,2);	//琼
		DisplayChinese(6,0,4);	//我
		DisplayChinese(6,112,6);//你
		//Display8x16Str(3,85, "!!!!");
        Delay(10);
    }
}
