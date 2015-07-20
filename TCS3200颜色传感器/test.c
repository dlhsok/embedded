#include<reg52.h>

#define uchar   unsigned char

#define uint    unsigned int

sbit    S0=P1^0;  //S0,S1为设置输出的占空比

sbit    S1=P1^1;

sbit    S2=P1^3;  //S2,S3为设置虑光的模式

sbit    S3=P1^4;

sbit    OE=P1^2; //使能



void RS232_init()

{

  TMOD=0X20;    //定时器1工作在方式1

  SCON=0x50;

  PCON=0X80;   //

  TH1=TL1=0XFF;//22.1184M晶振，波特率设置为115200

  TR1=1;

  TI=0;

  RI=0;


}

void send_char(uchar a) //发送数据

{

    SBUF=a;

    while(TI==0);

    TI=0;

}

uchar get_char()   //接收数据

{

    while(RI==0);

    RI=0;

return SBUF;

}


uint color_display(uchar m)

{

       uint time,a;

       a=m;

       TMOD=0x61; //计数器1，定时器0工作在16位方式

       TH0=TL0=0; //从零开始计数，定时

       TH1=TL1=0;

       S0=1;

       S1=1;

       S2=m&0x01;

       S3=m&0x02;

       OE=0;

       TR0=TR1=1;

       while(TL1<250);

       TR1=TR0=0;

       OE=1;

       time=TH0*256+TL0;

       return(time);


}


void main()

{

    uint temp;

    RS232_init();

    OE=1;

    while(1)

    {

   

        temp=color_display(get_char());

        RS232_init();

        send_char(temp/1000);     

        send_char(temp%1000/100); 

        send_char(temp%100/10);

        send_char(temp%10);

   

    }

       

}