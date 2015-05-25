/*****************************************************************
*                                                                *
* ME850单片机开发系统扩展应用 －  收音模块演示程序               *
*                                                                *
* MCU 芯片: AT89S52      工作频率: 11.0592MHz                    *
*                                                                *
* 版本：V1.0 (2011/01/15)                                        *
* 作者：gguoqing  (Email: gguoqing@willar.com)                   *
* 网站：www.sofi-tech.com (硕飞科技)                             *
*       www.mcusj.com (伟纳单片机世界)                           *
* 邮箱：willar@tom.com                                           *
*                                                                *
*【版权】Copyright(C) 深圳硕飞科技有限公司  All Rights Reserved  *
*【声明】此程序仅用于学习与参考，引用请注明版权和作者信息！      *
*                                                                *
******************************************************************
*                                                                *
*  K1--- 向上搜索电台 K2--- 向下搜索电台                         *
*  K3--- 音量加       K4--- 音量减                               *
*                                                                *
*****************************************************************/

#include <reg52.h>
#include <intrins.h>
#include "lcd.h"
#include "IIC.h"
#include "delay.h"

sbit  K1 = P1^4;
sbit  K2 = P1^5;
sbit  K3 = P1^6;
sbit  K4 = P1^7;

unsigned long frequency;

// RDA5807 寄存器
unsigned char  RDA_reg_data[8] =
{
   0xd0,0x00,  // 02H
   0x00,0x00,  // 03H
   0x00,0x40,  // 04H
   0x90,0x88,  // 05H
};

unsigned char code  cdis1[ ] = {"FM STEREO RADIO "};
unsigned char code  cdis2[ ] = {"FM       VOL    "};

char code reserve[3]_at_ 0x3b; //保留0x3b开始的3个字节

/**********************************************************

 连续写寄存器子函数

**********************************************************/
void RDA5807_write_reg(void)
{
   uchar i;

   I2C_start();
    // 收音模块写入操作
   I2C_write_byte(0x20);
    // 寄存器连续写操作
   for(i=0; i<8; i++)
   {
     I2C_write_byte(RDA_reg_data[i]);
   }
   I2C_stop();
}

/**********************************************************

 连续读寄存器子函数

**********************************************************/
void RDA5807_read_reg(uchar *reg_buf)
{
   I2C_start();

    // 收音模块读取操作
   I2C_write_byte(0x21);

    // 寄存器连续读操作
   reg_buf[0] = I2C_read_byte(I2C_ACK);
   reg_buf[1] = I2C_read_byte(I2C_ACK);
   reg_buf[2] = I2C_read_byte(I2C_ACK);
   reg_buf[3] = I2C_read_byte(I2C_NACK);

   I2C_stop();
}

/**********************************************************

 模块上电初始化子函数

**********************************************************/
void RDA5807_power(void)
{
   delayms(50);

    // 发送软件复位指令
   RDA_reg_data[0] = 0x00;
   RDA_reg_data[1] = 0x02;
   RDA5807_write_reg();

   delayms(10);

    // 收音模块默认参数
   RDA_reg_data[0] = 0xd0;
   RDA_reg_data[1] = 0x01;
   RDA5807_write_reg();
}
/**********************************************************

 功能描述：收音模块自动寻台模式

**********************************************************/
void RDA5807_FM_seek(void)
{
   uint chan;
   uchar  reg_data[4] = {0x00, 0x00, 0x00, 0x00};

   RDA_reg_data[3] &= ~(1 << 4);      //调谐禁用

   // 内部自动寻台使能
   RDA_reg_data[0] |=  (1 << 0);      //SEEK位置1
   RDA5807_write_reg();

    // 等待STC 标志置位
   while(0 == (reg_data[0] & 0x40))
   {
     delayms(20);
      // 读取内部状态
     RDA5807_read_reg(reg_data);
   }
    // 获取当前工作频点
   chan = reg_data[0] & 0x03;
   chan = reg_data[1] | (chan << 8);
   chan = chan << 6;

    // 保存当前工作频点
   RDA_reg_data[2] = (chan >> 8) & 0xff;
   RDA_reg_data[3] = (chan & 0xff);
}

/**********************************************************

 频率显示子函数

**********************************************************/
void  show_frequency(void)
{ 
   unsigned char i,display[5];
   unsigned int  temp;

   temp = (RDA_reg_data[2]*256)+(RDA_reg_data[3]&0xc0);	 //计算
   temp = temp>>6;
   frequency = (unsigned long)(100*temp+87000)/100;

   for(i=0; i<5; i++)  // 清显存单元
   display[i] = 0x00;

   display[0] = (frequency)/1000 ;     //数据转换
   display[1] = (frequency%1000)/100;
   display[2] = (frequency%100)/10;
   display[3] = 0x2e;                  //小数点
   display[4] = (frequency%10);

   if(display[0] == 0)
   { 
     display[0] = display[1]+0x30;
     display[1] = display[2]+0x30;
     display[2] = display[3];
     display[3] = display[4]+0x30;
     display[4] = 0x20;
   }
   else
   { 
     display[0] += 0x30;
     display[1] += 0x30;
     display[2] += 0x30;
     display[4] += 0x30;
   }

   lcd_pos_xy(3,2);                      //频率显示
   lcd_wdat(display[0]);
   lcd_wdat(display[1]);
   lcd_wdat(display[2]);
   lcd_wdat(display[3]);
   lcd_wdat(display[4]);
}

/**********************************************************

 音量显示子函数

**********************************************************/
void show_volume()
{
   unsigned char temp,display[2];

   temp = RDA_reg_data[7] & 0x0f; //取音量值

   display[0] = temp/10;
   display[1] = temp%10;

   if(display[0] == 0)            //如果高位为0
   { 
     display[0] = display[1];     //低位显存内容进入高位显存
     display[1] = 0x20;           //低位不显示
   }
   else 
   {
     display[1] += 0x30;
   }
   display[0] += 0x30;

   lcd_pos_xy(13,2);              //音量值显示
   lcd_wdat(display[0]);
   lcd_wdat(display[1]); 
}

/**********************************************************

 主函数

**********************************************************/
void  main(void)
{  
   P0 = 0xff;
   P1 = 0xff;
   P2 = 0xff;

   lcd_init(); 
   lcd_w_string(0,1,cdis1,0);
   lcd_w_string(0,2,cdis2,0);

   RDA5807_power();

   while(1)
   {
     if(K1 == 0)
     {
       delayms(20);
       if(K1 == 0)
       {
         RDA_reg_data[0] |= (1 << 1); 	//SEEK UP	 
         RDA5807_FM_seek();		  
         while(K1 == 0);
       }
     }

     if(K2 == 0)
     {
       delayms(20);
       if(K2 == 0)
       {
         RDA_reg_data[0] &= ~(1 << 1);  //SEEK DOWN		 
         RDA5807_FM_seek();
         while(K2 == 0);
       }
     }

     if(K3 == 0)
     {
       delayms(20);
       if(K3 == 0)
       {
         if((RDA_reg_data[7] & 0x0f) < 0x0f)
         {
           RDA_reg_data[0] = 0xd0;
           RDA_reg_data[1] = 0x01;
           RDA_reg_data[3] &= ~(1 << 4);

           RDA_reg_data[7]++;	 // 音量递增
           RDA5807_write_reg();
		   while(K3 == 0);
         }
       }
     }

     if(K4 == 0)
     {
       delayms(20);
       if(K4 == 0)
       {
         if((RDA_reg_data[7] & 0x0f) > 0x00)
         {
           RDA_reg_data[0] = 0xd0;
           RDA_reg_data[1] = 0x01;
           RDA_reg_data[3] &= ~(1 << 4);
           
           RDA_reg_data[7]--;	 // 音量递减
           RDA5807_write_reg();
           while(K4 == 0);
         }
       }
     }

     show_volume();
     show_frequency();
   }
}

/*********************************************************/

