
#include <reg52.h>
#include "delay.h"
#include "IIC.h"

sbit SDA = P3^4;
sbit SCL = P3^5;
	
/**********************************************************

 启动子程序

 在 SCL 高电平期间 SDA 发生负跳变

**********************************************************/
void I2C_start()
{
   SDA = 1;
   SCL = 1;
   delayNOP();
   SDA = 0;
   delayNOP();
   SCL = 0;
}

/**********************************************************

 停止子函数

 在 SCL 高电平期间 SDA 发生正跳变

**********************************************************/
void I2C_stop()
{
   SDA = 0;
   SCL = 1;
   delayNOP();
   SDA = 1;
   delayNOP();
}

/**********************************************************

 发送应答位子函数

 在 SDA 低电平期间 SCL 发生一个正脉冲

**********************************************************/
/*
void  iic_ack()
{
   SDA = 0;
   SCL = 1;
   delayNOP();
   SCL = 0;
   NOP;
   SDA = 1;
}
*/
/**********************************************************

 发送非应答位子函数

 在 SDA 高电平期间 SCL 发生一个正脉冲

**********************************************************/
/*
void  iic_noack()
{
   SDA = 1;
   SCL = 1;
   delayNOP();
   SCL = 0;
   delayNOP();
   SDA = 0;
}
*/
/**********************************************************

 应答位检测子函数

**********************************************************/
/*
bit iic_testack()
{
   bit ack_bit;

   SDA = 1;          //置SDA为输入方式
   SCL = 1;
   delayNOP();
   ack_bit = SDA;
   SCL = 0;
   NOP;
   return ack_bit;  //返回应答位
}
*/
/**********************************************************

 发送一个字节子程序

**********************************************************/
unsigned char I2C_write_byte(unsigned char indata)
{
    unsigned char i, ack;

    // I2C 发送8 位数据
   for (i=0; i<8; i++)
   {
     // 高在前低在后
     if (indata & 0x80)
      SDA = 1;
     else
      SDA = 0;

     // 发送左移一位
     indata <<= 1;
     delayNOP();
     SCL = 1;
     delayNOP();
     SCL = 0;
   }

    // 设置SDA为输入
   SDA =1;;
   delayNOP();

    // 读取从机应答状态
    SCL = 1;

    delayNOP();

    if(SDA)
    {
      ack = I2C_NACK;
    }
    else
    {
      ack = I2C_ACK;
    }
    SCL = 0;

    return ack;
}

/**********************************************************

  读一个字节子程序

**********************************************************/
unsigned char I2C_read_byte(unsigned char ack)
{
    unsigned char i, data1 = 0;

    // SDA 设置输入方向
    SDA = 1;

    // I2C 接收8位数据
    for(i = 8; i > 0; i--)
    {
      data1 <<= 1;
      SCL = 1;
      delayNOP();

        // 高在前低在后
        if (SDA)
        {
          data1++;
		}
        SCL = 0;
        delayNOP();
    }

    // 主机发送应答状态
    if(ack == I2C_ACK)
      SDA = 0;
    else
      SDA = 1;

    delayNOP();
    SCL = 1;
    delayNOP();
    SCL = 0;

    return data1;
}

/*********************************************************/