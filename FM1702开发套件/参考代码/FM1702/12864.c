#include<reg52.h>

#define uchar unsigned  char
#define uint  unsigned  int
sbit RS=P1^5;
sbit RW=P1^6;
sbit E=P1^7;                        
sbit PSB=P1^4;   //硬件上的串口,并口选择位,此开发板用的是并口,此位置1为并口




uchar code table_1[]={"校园一卡通研发"};
uchar code table_2[]={"卡号:"};
uchar code table_3[]={"消费:"};
uchar code table_4[]={"余额:"};

uchar code table_5[]={"."};
uchar code table_6[]={"元"};

uchar code table_7[]={"("};
uchar code table_8[]={")"};

uchar code table_9[]={"                "};

uchar code table_a[]={"广州恒天欢迎您"};
uchar code table_b[]={"请刷卡"};
uchar code table_c[]={"密码错!"};
uchar code table_d[]={"请接射频卡模块!"};
/////延时函数//////////////////////////////////
void delay1(unsigned int z)
{
	unsigned int x,y;
	for(x=z;x>0;x--)
	for(y=110;y>0;y--);	
}
///////////////////////////////////////////////
//////写命令////////////////////////////////////
void WriteCommand(uchar a)
{
	 E=0;  //1读取信息；1-0下降沿执行指令
	 RS=0;  //0=写指令；1=写数据
	 RW=0;  //0=写指令或数据；1=读LCD信息
	 E=1;  //读信息
	 P2=a;  //写指令
	 delay1(5);           //延时 
	 E=0;    //执行指令
	 delay1(5);           //延时 
}
//////////////////////////////////////////////////


//////写数据//////////////////////////////////////
void WriteData(uchar b)  
{

     E=0;   //执行指令
	 RS=1;   //写数据
	 RW=0;   //写数据
	 E=1;   //读取信息
	 P2=b;   //写数据
	 delay1(5);      //延时 
	 E=0;     //执行指令
	 delay1(5);      //延时 
}
//////////////////////////////////////////////////



///////初始化////////////////////////////////////
void initial_LCD()      
{   

   	 PSB=1;      //选择为并口
	 WriteCommand(0x01);  //清屏
	 WriteCommand(0x30);  //功能设置。8字节界面，基本指令集。（0011 x0xx）
	 WriteCommand(0x0c);  //显示设置。整体显示，光标不显示，光标不反向。（0000 1110）
	 WriteCommand(0x06);   //输入方式设置。光标右移，地址位加1；整体右移。（0000 0110)
	// WriteCommand(0x14);   //光标右移。
}
/////////////////////////////////////////////////



///////要显示的内容//////////////////////////////

void DispalyLcd()
{
	 unsigned char i;
	 WriteCommand(0x81);  		//页地址,从80开始,详情参见资料//
	 for (i=1;i<=14;i++)		//第一行显示的是7个汉字,1个汉字为2个字节,所以这里要发送14个字节//
	 WriteData(table_1[i-1])  ;   

	 WriteCommand(0x90);  		//页地址,从92开始,详情参见资料//
	 for (i=1;i<=5;i++)		
	 WriteData(table_2[i-1])  ; 


	 WriteCommand(0x88);  		//页地址,从88开始,详情参见资料//
	 for (i=1;i<=5;i++)		    //第三行
	 WriteData(table_3[i-1])  ; 


	 WriteCommand(0x98);  	
	 for (i=1;i<=16;i++)		
	 WriteData(table_9[i-1])  ; 




	 WriteCommand(0x98);  		//页地址,从88开始,详情参见资料//
	 for (i=1;i<=5;i++)		
	 WriteData(table_4[i-1])  ; 

}
/////////////////////////////////////////////////////

//////////数值的转换格式  \ 显示数值 ////////////////

void LCD_Data(uchar LCDbyte )
{
	 unsigned char count;
	 count=(LCDbyte&0xf0)>>4;
	 if (count>0x9) WriteData(count%9+0x40);
	 else WriteData(count+0x30);
	 count=LCDbyte&0x0f;
	 if (count>9) WriteData(count%9+0x40);
	 else WriteData(count+0x30);
}
/////////////////////////////////////////////////////

void displaymun(uchar temp1,uchar temp2,uchar temp3,uchar temp4,uchar temp5,uchar buff1,uchar buff2,uchar buff3)
{
		uchar i;
		WriteCommand(temp1);
		LCD_Data(buff1); 

		WriteCommand(temp2);
		LCD_Data(buff2); 


		 WriteCommand(temp4);  	//小数点
		 for (i=1;i<=1;i++)		
		 WriteData(table_5[i-1])  ;

		WriteCommand(temp3);
		LCD_Data(buff3); 

		 WriteCommand(temp5);  	//元
		 for (i=1;i<=2;i++)		
		 WriteData(table_6[i-1])  ; 
	
}

void dis_Request1()
{
		uchar i;
		 WriteCommand(0x9b); 
		 for (i=1;i<=1;i++)		
		 WriteData(table_7[i-1])  ;

		 WriteCommand(0x9c); 
		 for (i=1;i<=1;i++)		
		 WriteData(table_8[i-1])  ;
}

void dis_Request2()
{
		uchar i;
		 WriteCommand(0x9a); 
		 for (i=1;i<=1;i++)		
		 WriteData(table_7[i-1])  ;

		 WriteCommand(0x9d); 
		 for (i=1;i<=1;i++)		
		 WriteData(table_8[i-1])  ;
}

void dis_Request3()
{
		uchar i;
		 WriteCommand(0x99); 
		 for (i=1;i<=1;i++)		
		 WriteData(table_7[i-1])  ;

		 WriteCommand(0x9e); 
		 for (i=1;i<=1;i++)		
		 WriteData(table_8[i-1])  ;
}
void DispalyLcd1()
{
	 unsigned char i;
	 WriteCommand(0x98);  	
	 for (i=1;i<=16;i++)		
	 WriteData(table_9[i-1])  ; 
}


void DispalyLcd_home()
{
	 unsigned char i;
	 WriteCommand(0x81);  	//dis  广州恒天欢迎你
	 for (i=1;i<=14;i++)	
	 WriteData(table_a[i-1])  ;   

 

	 WriteCommand(0x93);  		//dis:请刷卡
	 for (i=1;i<=6;i++)		
	 WriteData(table_b[i-1])  ; 

	 
}

void dis_passerror()
{
	 unsigned char i;

	 WriteCommand(0x88);  	
	 for (i=1;i<=16;i++)		
	 WriteData(table_9[i-1])  ; 
	 DispalyLcd1();

	 WriteCommand(0x9a);  		
	 for (i=1;i<=7;i++)		
	 WriteData(table_c[i-1])  ; 	
}

void DispalyLcd_RF()
{
	 unsigned char i;
	 WriteCommand(0x80);  	
	 for (i=1;i<=15;i++)	
	 WriteData(table_d[i-1])  ;   

	 
}
