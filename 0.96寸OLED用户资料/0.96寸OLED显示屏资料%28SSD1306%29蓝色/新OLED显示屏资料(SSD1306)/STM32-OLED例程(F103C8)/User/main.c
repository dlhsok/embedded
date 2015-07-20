/************************************************************************************
*  Copyright (c), 2015, HelTec Automation Technology co.,LTD.
*            All rights reserved.
*
* File name: main.c
* Project  : HelTec
* Processor: STM32F103C8T6
* Compiler : MDK for ARM - 4.72.1.0
* 
* Author : Aaron.Lee
* Version: 1.02
* Date   : 2014.5.25
* Email  : leehunter8801@gmail.com
* Modification: none
*
* Description: 接线方式
*          OLED    STM32
*
* 					VCC -- +3.3V~5V
* 					GND -- GND
*						CLK -- PA7
*					 MOSI -- PA6
*						CS  -- PA5
*						D/C -- PA4
*
* Others: none;
*
* Modify: Change RST into CS
*
* Function List: 1. void main(void);//主函数
*
*************************************************************************************/
#include "stm32f10x.h"
#include "OLED.h"
#include "delay.h"
//#include "codetab.h"

int main(void)
{
	unsigned char i;
	extern const unsigned char BMP1[];
	
	OLED_Init();
	while(1)
	{
//		OLED_Fill(0xff);
//		DelayS(2);
//		OLED_CLS();
		for(i=0; i<8; i++)//通过点阵显示汉字 -- i表示子表中汉字的位置
		{
			OLED_16x16CN(i*16,0,i);
		 	OLED_16x16CN(i*16,2,i+8);
		 	OLED_16x16CN(i*16,4,i+16);
		 	OLED_16x16CN(i*16,6,i+24);
		}
		DelayS(2);
		OLED_CLS();
		
		OLED_6x8Str(0,0,"HelTec Automation");
		OLED_6x8Str(0,1,"www.heltec.cn");
		OLED_8x16Str(0,2,"2013-2-21");
		OLED_8x16Str(10,4,"OLED");
		DelayS(4);
		OLED_CLS();
		
		OLED_BMP(0,0,128,8,(unsigned char *)BMP1);
		DelayS(4);
	}
}
