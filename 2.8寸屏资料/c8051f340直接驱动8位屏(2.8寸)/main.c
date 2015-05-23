/*******************************************************************************
*  Copyright (C) 2010 - All Rights Reserved
*		
* 软件作者:	骑飞家族
* 版权所有: 骑飞电子	
* 创建日期:	2010年6月10日 
* 软件历史:	2010年11月2日修改
* Version:  1.0 
* Demo 淘宝地址：http://store.taobao.com/shop/view_shop.htm?asker=wangwang&shop_nick=qifeidianzi
**********************************************************************************************************************************************
懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒
懒懒懒懒懒懒懒懒懒懒懒一懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒困一懒懒懒懒懒懒懒懒懒懒懒懒懒懒一一一一一一一懒懒懒懒懒懒懒
懒懒困一一一一懒一一一一一一二懒懒懒懒困一一一一一一一一懒懒懒懒懒懒懒懒懒四厲懒懒一一懒懒懒一懒懒懒懒懒懒懒懒懒一一一一四四一一一懒懒懒懒懒懒
懒懒懒懒懒懒一懒懒懒一二一懒懒懒懒懒懒一一一一四厲二一四懒一一懒懒懒懒懒四一一一一一一一一一一一懒懒懒懒懒懒懒懒四懒懒四一一一一厲懒懒懒懒懒懒
懒懒懒四厲厲一懒懒厲懒懒懒四懒懒懒懒懒懒懒懒懒懒懒困一懒一一懒懒懒懒懒懒懒一一厲厲一一厲厲厲一一懒懒懒懒懒懒厲厲厲厲厲厲一一厲厲厲懒懒懒懒懒懒
懒懒懒一懒二一一一一一一一一一一一懒懒懒懒懒懒懒懒二一一困懒懒懒懒懒懒懒懒一一一一一一一一一一四懒懒懒懒懒一一一一一一一一一一一一一一一懒懒懒
懒懒懒一一一一一厲二一一厲一懒厲懒懒懒懒懒懒懒懒懒一一一一一懒懒懒懒懒懒懒一一懒懒一一懒懒懒一困懒懒懒懒懒一一四懒懒懒懒一一懒懒困一一四懒懒懒
懒懒懒懒懒懒懒一困一懒一厲一懒懒懒懒懒懒懒懒懒懒懒困一懒懒一一懒懒懒懒懒懒一一一一一一一一一一厲懒懒懒懒懒懒懒懒懒懒懒懒一一厲懒懒懒懒懒懒懒懒
懒二一一一懒四困厲一一一厲一懒懒懒懒懒懒懒懒懒懒懒懒一一懒懒懒懒懒懒懒懒懒懒懒懒懒四一厲懒懒懒懒懒懒一懒懒懒懒懒懒懒懒懒一一厲懒懒懒懒懒懒懒懒
懒懒懒懒懒懒一懒懒懒懒懒一一懒懒懒懒懒懒懒懒懒懒懒懒厲一一一一一一困懒懒懒懒懒懒懒懒一一一一一一一一一懒懒懒懒懒困懒懒厲一一懒懒懒懒懒懒懒懒懒
懒懒懒懒一一一懒懒困一一一一懒懒懒懒懒懒懒懒懒懒懒懒懒懒一一一一一懒懒懒懒懒懒懒懒懒懒一一一一一一一懒懒懒懒懒懒懒一一一一四懒懒懒懒懒懒懒懒懒
懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒懒
**********************************************************************************************************************************************/
//
#include<TFT.h>
#include<delay.h>
#include<C8051F340.h>


void OSCILLATOR_Init (void);
void Port_Init(void);
void Ext_Interrupt_Init (void);

unsigned char Vsync_Flag=0;

void main(void)
{		
   int i,j;
	PCA0MD &= ~0x40;      //看门狗禁止                
  	OSCILLATOR_Init();    //时钟初始化
	Port_Init();          //端口初始化 	             
	P0MDOUT&=~0xC0;       //设置VCYNC、HREF端口为输入模式
	P0MDIN|=0xC0;
	P0|=0xC0;


	delay(10);

	Tft_Init();          //TFT初始化
	TftClear(RED);       //清屏

	TftWR_Str(20,100,"C8051F340 TFT Test ",RED,BLACK);
	delay_ms(5000);
	delay_ms(5000);
	delay_ms(5000);
	while(1)
	{
		
		delay_ms(5000);
		TftClear(BLACK);
		delay_ms(5000);
		TftClear(BLUE);
		delay_ms(5000);
		TftClear(RED);
		delay_ms(5000);
		TftClear(GREEN);
		delay_ms(5000);
		TftClear(CYAN);
		delay_ms(5000);
		TftClear(MAGENTA);
		delay_ms(5000);
		TftClear(YELLOW);
		delay_ms(5000);
		TftClear(WHITE);
	
	}/**/

}

void OSCILLATOR_Init (void)
{
    int i = 0;
    FLSCL     = 0x90;
    CLKMUL    = 0x80;
    for (i = 0; i < 20; i++);    // Wait 5us for initialization
    CLKMUL    |= 0xC0;
    while ((CLKMUL & 0x20) == 0);
    CLKSEL    = 0x03;
    OSCICN    = 0x83;            
}

//-------------------------
// Port_Init
//-------------------------
// Port Initialization
// - Configure the Crossbar and GPIO ports.

void Port_Init(void)
{  
	P4MDOUT=0xFF;
	P3MDOUT=0xFF;
	P2MDOUT=0xFF;
	P0MDOUT=0xFF;
	P1MDOUT   = 0xFF;

	XBR0      = 0x01;
	XBR1      = 0x40;

} 


