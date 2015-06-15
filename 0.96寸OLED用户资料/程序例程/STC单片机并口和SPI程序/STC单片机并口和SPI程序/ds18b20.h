#ifndef __DS18B20_H__
#define __DS18B20_H__
//20090101修改
sbit DQ = P2^4;   //定义通信端口 
bit flag_Negative_number = 0;//负数标志


unsigned char wl=0,wh=0;
//float  temper=0;
unsigned int temper=0;

//晶振22MHz  
void delay_18B20(unsigned int i)
{
 	while(i--);
}



//读一个字节  
ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat=0;
	for (i=8;i>0;i--)
	 {
		  DQ = 0; // 给脉冲信号
		  dat>>=1;
		  DQ = 1; // 给脉冲信号
		  if(DQ)
		  dat|=0x80;
		  delay_18B20(60);
	 }
 	return(dat);
}

//写一个字节  
WriteOneChar(unsigned char dat)
{
 unsigned char i=0;
 for (i=8; i>0; i--)
 {
  DQ = 0;
  DQ = dat&0x01;
  delay_18B20(60);
  DQ = 1;
  dat>>=1;
 }
}

//初始化函数
Init_DS18B20(void) 
  {
	 //unsigned char x=0;
	 DQ = 1;          //DQ复位
	 delay_18B20(5);  //稍做延时
	 DQ = 0;          //单片机将DQ拉低
	 delay_18B20(150); //精确延时 大于 480us
	 DQ = 1;          //拉高总线
	 delay_18B20(150);	

  }

//读取温度
ReadTemperature(void)
{
  
    EA = 0;		 
    Init_DS18B20();
    WriteOneChar(0xCC); //跳过读序号列号的操作
    WriteOneChar(0x44); // 启动温度转换
    delay_18B20(5000);
   // delay_18B20(5000);

    Init_DS18B20();
    WriteOneChar(0xCC); //跳过读序号列号的操作
    WriteOneChar(0xBE); //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
    wl=ReadOneChar();
    wh=ReadOneChar(); 
    temper=wh;
    temper<<=8;
    temper=temper|wl;	
	 //temper=((wh*256)+wl)*0.0625;	 
    flag_Negative_number = 0;
    if(temper>0x0fff) 
     {
	   temper=~temper+1;
	   flag_Negative_number = 1;
	   //disc(6,2,hen);
      }


     //18b20：t=d*0.0625
    // t = t*5; //有效位到小数点后2位，
	 ///t = temp*0.0625;
    // temper = t*10+0.5;
	// temper = temper*0.0625;
	 
	  temper=((temper+1)*0.0625)*10;
	 
     EA = 1;
   return(temper);	  

}
#endif
