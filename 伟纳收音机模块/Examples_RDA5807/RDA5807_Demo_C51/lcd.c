/**********************************************************

File: lcd.c

Compiler:  gguoqing

**********************************************************/
#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "delay.h"
	
#define uchar unsigned char
#define uint  unsigned int

#define DATA_PORT  P0

sbit LCD_RS = P2^0;
sbit LCD_RW = P2^1;
sbit LCD_EN = P2^2;


/*******************************************************************

  检查LCD忙状态  
  lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。

*******************************************************************/ 
bit lcd_busy()
 {
    bit result;
    LCD_RS = 0;
    LCD_RW = 1;
    LCD_EN = 1;
    delayNOP();
    result = (bit)(DATA_PORT&0x80);
    LCD_EN = 0;
    return(result); 
 }

/*******************************************************************

  写指令数据到LCD       
  RS=L，RW=L，E=高脉冲，D0-D7=指令码。
  Check=1，进行忙检测。

*******************************************************************/
void lcd_wcmd(uchar cmd, bit Check)
{   
    if(Check)
    while(lcd_busy());

    LCD_RS = 0;
    LCD_RW = 0;
    LCD_EN = 0;
    delayNOP();
    DATA_PORT = cmd;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0;
}

/*******************************************************************

  写显示数据到LCD
  RS=H，RW=L，E=高脉冲，D0-D7=数据。

*******************************************************************/
void lcd_wdat(uchar dat)
{                          
    while(lcd_busy());      //进行忙检测

    LCD_RS = 1;
    LCD_RW = 0;
    LCD_EN = 0;
    DATA_PORT = dat;
    delayNOP();
    LCD_EN = 1;
    delayNOP();
    LCD_EN = 0;
}

/*******************************************************************

  LCD初始化设定

*******************************************************************/
void lcd_init()
{ 
    delayms(50);
    lcd_wcmd(0x38,0);      //16*2显示，5*7点阵，8位数据
    delayms(5);
    lcd_wcmd(0x38,0);      //不进行忙检测，强制执行。
    delayms(5);
    lcd_wcmd(0x38,0);
    delayms(5);

	lcd_wcmd(0x38,1);      //进行忙检测 
    delayms(5);
    lcd_wcmd(0x0c,1);      //显示开，关光标
    delayms(5);
    lcd_wcmd(0x06,1);      //移动光标
    delayms(5);
    lcd_wcmd(0x01,1);      //清除LCD的显示内容
    delayms(5);
}

/*******************************************************************

  设定显示位置

*******************************************************************/
void lcd_pos_xy(uchar pos_x,uchar pos_y)
{                          
   uchar  pos_temp;

   pos_temp=pos_x & 0x0f;
   pos_y &= 0x0f;

   if(pos_y==0x01)
   pos_temp |= 0x80;
   else 
   if(pos_y==0x02)
   pos_temp |= 0xc0;

   lcd_wcmd(pos_temp,1);
}

/*******************************************************************

  写字符串子函数 

*******************************************************************/
void  lcd_w_string(uchar pos_x,uchar pos_y,uchar code *str,uint times)
{
   uchar m;
   lcd_pos_xy(pos_x,pos_y);      //设定起始位置
   for(m=0; m<16; m++)
   {
     lcd_wdat(str[m]);
	 delayms(times);
   }
}

/*****************************************************************

  pos_x 取值范围： 0 - 15

  pos_y 取值范围： 1 - 2

******************************************************************/
