
#include <reg51.h>
#include <intrins.h>
#include <FM1702.h>
#include   <STDIO.H>


#define uchar unsigned  char
#define uint  unsigned  int

#define TRUE   1
#define FALSE  0


sbit  rst =P0^7;
sbit  cs  =P0^6;
sbit  si  =P0^5;
sbit  sck =P0^4;
sbit  so  =P0^3;

sbit  led =P0^2;
sbit  beep=P1^2;







/////变量定义///////////////

uchar 	RevBuffer[24];		    /* FM1702命令发送接收缓冲区 */
uchar 	UID[5];		            /* 序列号 */
uchar  Secnr;			        /* 块号 */
#define 	m1erea 		0x01     //要操作的扇区号

uint     pay=500;				//刷卡扣取的金额,2个字节,65535
uint     ctime,cctime;
uchar    ctimeH,ctimeL;
uchar lcd[5];
/////变量定义///////////////



////////////////////// 子函声明 /////////////////////////////
uchar SPIRead(char reg_ad);
void SPIWrite(char reg_ad,char reg_data);
void Write_FIFO(uchar count, uchar  *buff);
uchar Clear_FIFO(void);
uchar Judge_Req(uchar  *buff);
uchar Command_Send(uchar count, uchar *buff, uchar Comm_Set);
uchar Request(uchar mode);
void delay(uint x);
uchar Read_FIFO(uchar *buff);
//void Save_UID(uchar row, uchar col, uchar length);
uchar AntiColl();
uchar Check_UID();
//void Set_BitFraming(uchar row, uchar col);
uchar Select_Card();
char M500HostCodeKey(  uchar *uncoded, uchar *coded);
uchar Load_key_CPY(uchar *buff);
void keyto();
uchar block_numset(uchar block_num);
uchar MIF_Write(uchar *buff, uchar Block_Adr);
uchar Read_E2(uchar lsb, uchar msb, uchar count, uchar *buff);
uchar CRCrdadandset(uchar * buff,uchar RW);

void initial_LCD();
void DispalyLcd();
void LCD_Data(uchar LCDbyte );
void WriteCommand(uchar a);
void displayUid();
void displaymun(uchar temp1,uchar temp2,uchar temp3,uchar temp4,uchar temp5,uchar buff1,uchar buff2,uchar buff3);

void dis_Request1();
void dis_Request2();
void dis_Request3();


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////





void Init_FM1702()
{	
	uchar temp;
	uint	i;
	sck = 1;
	so = 1;
	si = 1;
	rst = 1;			            /* FM1702复位 */
	for(i = 0; i < 0xff; i++)
	{
		_nop_();                     /*等待约140ms, 11.0592*/
	}
	rst = 0;
	for(i = 0; i < 0xff; i++)      
	{
		_nop_();
	}
     ///////////  init_spi  ///////////////////////////////////////////////	
	while(temp = SPIRead(Command) != 0)	/* 等待Command = 0,FM1702复位成功 */
	{
		_nop_();
	}
////////////////////////////////////////////////////////////////////////////
    SPIWrite(Page_Sel,0x80);                 //往PAG里面写0X80初始化SPI接口
	for(i = 0; i < 0xff; i++) /* 延时 */
	{
		if(temp=SPIRead(Command) == 0x00)	//如果Command为0表示初始化SPI接口成功
		{
			SPIWrite(Page_Sel,0x00);        //往PAG里面写0开使用SPI接口
		}
	}
     ////////// init_FM1702 ///////////////////////////////////////
	SPIWrite(InterruptEn,0x7f);	//address 06H  /* 中断使能/禁止寄存器 */off
	SPIWrite(Int_Req,0x7f);     //address 07H    /* 中断请求标识寄存器 */清0
	SPIWrite(TxControl,0x5b);	//address 11H    /* 发送控制寄存器 */ 
	SPIWrite(RxControl2,0x01);  //address 1EH    设置接收控制寄存器
	SPIWrite(RxWait,0x07);		//address 21H    设置接收和发送之间的时间间隔
    SPIWrite(TypeSH,0x00);      //0:TYPEA模式*//* 1:TYPEB模式*//* 2:上海模式*/
}	
////////////////////////////////////////////////////////////////////////////



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//名称: spi_byte_transceive                                                    //
//功能: 该函数实现SPI通讯的数据收发                                            //
//									       //
//输入:                                                                        //
//      发送数据                                                               //
//                                                                             //
//输出:                                                                        //
//      接收数据                                                               //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
char spi_byte_transceive(char sendbyte)
{
  char i,temp;
  for(i=0;i<8;i++)
   {  
    	sck=0;
    	if(sendbyte & 0x80)                //位运算，判断最高位是否为1
     	{ 
      		si=1;
     	}
    	else
     	{ 
      		si=0;
     	}
    	sendbyte <<= 1;    
    	sck=1;
   	 temp <<= 1;
   	 if(so)
    	temp |= 0x01;
   }
    	sck=0;
    	 _nop_();
	 _nop_();
    	si=0;
    	return (temp);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//名称: rc531_register_write                                                   // 
//功能: 该函数实现通过SPI接口对RC531中一个寄存器写入值                         // 
//                                                                             // 
//输入:                                                                        // 
//     RC531目标寄存器地址和写入值                                             // 
//                                                                             // 
//输出:                                                                        // 
//     N/A                                                                     // 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void SPIWrite(char reg_ad,char reg_data)
{   
    sck=0;                               //时钟
    reg_ad <<= 1;
    cs=0;                              //接口选通
    reg_ad &= 0x7F;                      //最高位为0表示写
    spi_byte_transceive(reg_ad);     //写地址
    spi_byte_transceive(reg_data);  //写数据
    cs=1;
    return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//名称: rc531_register_read                                                    // 
//功能: 该函数实现通过SPI接口读取RC531中一个寄存器的值                         // 
//                                                                             // 
//输入:                                                                        // 
//     RC531目标寄存器地址                                                     // 
//                                                                             // 
//输出:                                                                        // 
//     目标寄存器的值                                                          // 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
uchar SPIRead(char reg_ad)
{   char temp;
    sck=0;       //时钟
     _nop_();
     _nop_();
    cs=0;      //接口选通开
    reg_ad <<= 1;                          //reg_ad左移一位付值给reg_ad
    reg_ad |= 0x80;                        //reg_ad跟“0X80”或运算后付值给reg_ad   最高位为1表示读
    spi_byte_transceive(reg_ad);
    temp=spi_byte_transceive(0x00);
    cs=1;
    return (temp);
}


/****************************************************************/
/*名称: Request */
/*功能: 该函数实现对放入FM1702操作范围之内的卡片的Request操作*/
/*输入: mode: ALL(监测所以FM1702操作范围之内的卡片) */
/* STD(监测在FM1702操作范围之内处于HALT状态的卡片) */
/*输出: FM1702_NOTAGERR: 无卡*/
/* FM1702_OK: 应答正确*/
/* FM1702_REQERR: 应答错误*/
/****************************************************************/
uchar Request(uchar mode)
{	
	unsigned char temp;	

	RevBuffer[0] = mode;
	SPIWrite(0x11,0x58);//关发射控制
	SPIWrite(0x11,0x5b);//开发射控制

	SPIWrite(0x0f,0x07);          //address 0FH  调整位的帧格式	
	temp = SPIRead(0x09);
    temp&=0xf7;
	SPIWrite(0x09,temp);
	SPIWrite(0x22,0x03);
	temp = Command_Send(1, RevBuffer, Transceive);
	if(temp == FALSE)
	return (1);	
	Read_FIFO(RevBuffer);    //读取FIFO里面的数据及数据长度	
	temp = Judge_Req(RevBuffer); //对卡片复位应答信号的判断返回值是否正确
	if(temp == 1)
	return (0);							
	return (1);

}

/****************************************************************/
/*名称: Command_Send */
/*功能: 该函数实现向FM1702发送命令集的功能*/
/*输入: count, 待发送命令集的长度*/
/* buff, 指向待发送数据的指针*/
/* Comm_Set, 命令码*/
/*输出: TRUE, 命令被正确执行*/
/* FALSE, 命令执行错误*/
/****************************************************************/
uchar Command_Send(uchar count, uchar *buff, uchar Comm_Set)
{
	uint		j;
	uchar  temp;
	
	SPIWrite(Command,0x00);
	Clear_FIFO();
	Write_FIFO(count, buff);   //把26H写入FIFO

	
	SPIWrite(Command,Comm_Set);		/* 命令执行 */
	for(j = 0; j < RF_TimeOut; j++) /* 检查命令执行否 */
	{
		temp = SPIRead(Command);
		if(temp == 0x00)//如果TEMP为真执行IF语句，否则退出IF语句
		{
			return TRUE;//退出FOR语句（带参数返回）
		}
	}
	return FALSE;
}


/****************************************************************/
/*名称: Clear_FIFO */
/*功能: 该函数实现清空FM1702中FIFO的数据*/
/*输入: N/A */
/*输出: TRUE, FIFO被清空*/
/* FALSE, FIFO未被清空*/
/****************************************************************/
uchar Clear_FIFO(void)
{
	uchar	temp;
	uint	i;
	temp = SPIRead(Control);			/* 清空FIFO */
	temp = (temp | 0x01);
	SPIWrite(Control,temp);
	for(i = 0; i < RF_TimeOut; i++) /* 检查FIFO是否被清空 */
	{
		temp = SPIRead(FIFO_Length);
		if(temp == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}


/****************************************************************/
/*名称: Write_FIFO */
/*功能: 该函数实现向FM1702的FIFO中写入x bytes数据*/
/*输入: count, 待写入字节的长度*/
/* buff, 指向待写入数据的指针*/
/*输出: N/A */
/****************************************************************/
void Write_FIFO(uchar count, uchar *buff)
{
	uchar	i;	
	for(i = 0; i < count; i++)
	{
		SPIWrite(FIFO,*(buff + i));
	}
}

/****************************************************************/
/*名称: Read_FIFO */
/*功能: 该函数实现从FM1702的FIFO中读出x bytes数据*/
/*输入: buff, 指向读出数据的指针*/
/*输出: N/A */
/****************************************************************/
uchar Read_FIFO(uchar *buff)
{
	uchar	temp;
	uchar	i;
	
	temp = SPIRead(FIFO_Length);//读取FIFO里有多少字节。
	if(temp == 0)
	{
		return 0;
	}

	if(temp >= 24)		
	{
	
		temp = 24;	
	}

	for(i = 0; i < temp; i++)
	{
		*(buff + i) = SPIRead(FIFO);//读取FIFO里面的数据放在指针处
	}

	return temp;
}


/****************************************************************/
/*名称: Judge_Req */
/*功能: 该函数实现对卡片复位应答信号的判断*/
/*输入: *buff, 指向应答数据的指针*/
/*输出: TRUE, 卡片应答信号正确*/
/* FALSE, 卡片应答信号错误*/
/****************************************************************/
uchar Judge_Req(uchar  *buff)
{	
	uchar	temp1, temp2;	
	temp1 = *buff;
	temp2 = *(buff + 1);
	if((temp1 == 0x02) || (temp1 == 0x04) || (temp1 == 0x05) || (temp1 == 0x53) || (temp1 == 0x03))
	{                        //以上为或运算，只要有一条件为真，则运行IF语句。
		if (temp2 == 0x00)   //否则退出IF语句
		{
			return TRUE;
		}
	}
	return FALSE;
}

/****************************************************************/
/*名称: AntiColl */
/*功能: 该函数实现对放入FM1702操作范围之内的卡片的防冲突检测*/
/*输入: N/A */
/*输出: FM1702_NOTAGERR: 无卡*/
/* FM1702_BYTECOUNTERR: 接收字节错误*/
/* FM1702_SERNRERR: 卡片序列号应答错误*/
/* FM1702_OK: 卡片应答正确*/
/****************************************************************/
uchar AntiColl()
{
	unsigned char	temp;
	unsigned char	i;
	RevBuffer[0] = RF_CMD_ANTICOL;    //写寻卡通讯命令93H
	RevBuffer[1] = 0x20;              //写寻卡通讯命令20H
	SPIWrite(0x22,0x03); //address 22H  选择数据校验种类和类型
	temp = Command_Send(2, RevBuffer, Transceive);//Transceive=0X1E命令
	while(1)
	{
		if(temp == FALSE)
		{
			return(1);   //无卡
		}	  
		temp = SPIRead(0x04);   //读0x04里的返回卡号长度
		if(temp == 0)
		{
			return FM1702_BYTECOUNTERR; //接收字节长度错误
		}
		Read_FIFO(RevBuffer);           //读取卡号  					
		for(i = 0; i < temp; i++)
		{
			UID[i] = RevBuffer[i]; //把临时寄存器中的卡号放在UID专用寄存器中
		}		
		temp = Check_UID();			/* 校验收到的UID */
		if(temp == FALSE)
		{
			return(FM1702_SERNRERR);
		}
		return(0);
	}
		return(0);


}

/****************************************************************/
/*名称: Check_UID */
/*功能: 该函数实现对收到的卡片的序列号的判断*/
/*输入: N/A */
/*输出: TRUE: 序列号正确*/
/* FALSE: 序列号错误*/
/****************************************************************/
uchar Check_UID()
{
	
	uchar	temp;
	uchar	i;
	
	temp = 0x00;
	for(i = 0; i < 5; i++)
	{
		temp = temp ^ UID[i];//对卡号异或运算，即二数相同为0，不同为1
	}						//正确结果为0

	if(temp == 0)
	{
		return TRUE;
	}

	return FALSE;
}

/****************************************************************/
/*名称: Select_Card */
/*功能: 该函数实现对放入FM1702操作范围之内的某张卡片进行选择*/
/*输入: N/A */
/*输出: FM1702_NOTAGERR: 无卡*/
/* FM1702_PARITYERR: 奇偶校验错*/
/* FM1702_CRCERR: CRC校验错*/
/* FM1702_BYTECOUNTERR: 接收字节错误*/
/* FM1702_OK: 应答正确*/
/* FM1702_SELERR: 选卡出错*/
/****************************************************************/
uchar Select_Card()
{
	unsigned char	temp, i;	

	RevBuffer[0] = RF_CMD_SELECT;//写0x93命令时关闭发送以及接收CRC校验以及开启奇校验
	RevBuffer[1] = 0x70; //当发送PICC_REQSTD 命令时RegBitFraming 要装入0x07
	for(i = 0; i < 5; i++)
	{
		RevBuffer[i + 2] = UID[i];  //把5个卡序列号转放入缓冲区
	}
	SPIWrite(0x22,0x0f);//选择数据校验的种类和模式	
	temp = Command_Send(7, RevBuffer, Transceive);//Transceive=1EH（接收命令）
	if(temp == FALSE)			
	{
		return(1);
	}
	else
	{
		Read_FIFO(RevBuffer);	/* 从FIFO中读取应答信息, 读取卡片容量*/
		temp = *RevBuffer;			//返回卡片容量S50卡:08,S70卡:18

		if((temp == 0x18) || (temp == 0x08) || (temp == 0x28) || (temp == 0x53))	/* 判断应答信号是否正确 */
			return(0);
		else
			return(FM1702_SELERR);
	}
}

/****************************************************************/
/*名称: Load_keyE */
/*功能: 该函数实现把E2中密码存入FM1702的keyRevBuffer中*/
/*输入: Secnr: EE起始地址*/
/*输出: True: 密钥装载成功*/
/* False: 密钥装载失败*/
/****************************************************************/
uchar Load_key_CPY(uchar *buff)
{
	char  status;
    uchar coded_keys[12];
	uchar	temp;
	M500HostCodeKey(buff,coded_keys);////////////////	
	temp=Command_Send(0x0c,coded_keys,LoadKey);/* LoadKey将密钥从FIFO缓存复制到KEY缓存 0x19*/
	if(temp == FALSE)       //0x0c为12个字节长度 //coded_keys为指向地址寄存器
	{
		return FM1702_LOADKEYERR;
	}

	status=(SPIRead(ErrorFlag)) & 0x40;//判断loadkey执行是否正确
	if (status==0x40)
    return FM1702_AUTHERR;
    return FM1702_OK;
}


// 转换密钥格式
///////////////////////////////////////////////////////////////////////
char M500HostCodeKey(  uchar *uncoded, uchar *coded)   
{
    uchar cnt = 0;
    uchar ln  = 0;     
    uchar  hn  = 0;      
    for (cnt = 0; cnt < 6; cnt++)
    {
        ln = uncoded[cnt] & 0x0F;
        hn = uncoded[cnt] >> 4;
        coded[cnt * 2 + 1] = (~ln << 4) | ln;
        coded[cnt * 2 ] = (~hn << 4) | hn;
    }
    return 0;
}


/****************************************************************/
/*名称: Authentication */
/*功能: 该函数实现密码认证的过程*/
/*输入: UID: 卡片序列号地址*/
/* SecNR: 扇区号*/
/*输出: FM1702_NOTAGERR: 无卡*/
/* FM1702_PARITYERR: 奇偶校验错*/
/* FM1702_CRCERR: CRC校验错*/
/* FM1702_OK: 应答正确*/
/* FM1702_AUTHERR: 权威认证有错*/
/****************************************************************/
uchar Authentication(uchar *UID, uchar SecNR)
{	
	unsigned char i;
	unsigned char temp, temp1;	
	RevBuffer[0] = RF_CMD_AUTH_LA;   //密码A   
	RevBuffer[1] = SecNR;  
	for(i = 0; i < 4; i++)
	RevBuffer[2 + i] = UID[i];//把序列号放入缓冲区是否为同一张卡	  
	SPIWrite(0x22,0x0f);  //否则验证不通过

//************ Authent1=0x0c 验证命令认证过程第1步 ****************//	
	temp = Command_Send(6, RevBuffer, Authent1);
	if(temp == FALSE)   			
	return FM1702_AUTHERR;	

//************ Authent2=0x14 验证命令认证过程第2步 ****************//
	temp = Command_Send(0, RevBuffer, Authent2);//如果密码错则第二步不通过
	if(temp == FALSE)			
	return FM1702_AUTHERR;
	temp1 = SPIRead(0x09);//读控制标识寄存器为0X08时表示加密单元打开，
	temp1 = temp1 & 0x08;	//通过后该位内部置1 （CRYPTO1ON）
	if(temp1 == 0x08)
	return (0);
	return FM1702_AUTHERR;
}


/****************************************************************/
/*名称: MIF_Read */
/*功能: 该函数实现读MIFARE卡块的数值*/
/*输入: buff: 缓冲区首地址*/
/* Block_Adr: 块地址*/
/*输出: FM1702_NOTAGERR: 无卡*/
/* FM1702_PARITYERR: 奇偶校验错*/
/* FM1702_CRCERR: CRC校验错*/
/* FM1702_BYTECOUNTERR: 接收字节错误*/
/* FM1702_OK: 应答正确*/
/****************************************************************/
uchar MIF_READ(uchar *buff, uchar Block_Adr)
{
	unsigned char temp;
	SPIWrite(0x22,0x0f);
	buff[0] = RF_CMD_READ;   //0x30
	buff[1] = Block_Adr;
	temp = Command_Send(2, buff, Transceive);
	if(temp == 0)
	{			

		return (1);
	}

	temp = SPIRead(0x04);
	if(temp == 0x10)	
	{
		Read_FIFO(buff);

		return (0);
	}

	return (1);
}


/****************************************************************/
/*名称: MIF_Write */
/*功能: 该函数实现写MIFARE卡块的数值*/
/*输入: buff: 缓冲区首地址*/
/* Block_Adr: 块地址*/
/*输出: FM1702_NOTAGERR: 无卡*/
/* FM1702_BYTECOUNTERR: 接收字节错误*/
/* FM1702_NOTAUTHERR: 未经权威认证*/
/* FM1702_EMPTY: 数据溢出错误*/
/* FM1702_CRCERR: CRC校验错*/
/* FM1702_PARITYERR: 奇偶校验错*/
/* FM1702_WRITEERR: 写卡块数据出错*/
/* FM1702_OK: 应答正确*/
/****************************************************************/
uchar MIF_Write(uchar *buff, uchar Block_Adr)
{
	unsigned char	temp;
	unsigned char	*F_buff;

	SPIWrite(0x23,0x63);
	SPIWrite(0x12,0x3f);
	F_buff = buff + 0x10;
	SPIWrite(0x22,0x07);    /* Note: this line is for 1702, different from RC500*/
	*F_buff = RF_CMD_WRITE;
	*(F_buff + 1) = Block_Adr;
	temp = Command_Send(2, F_buff, Transceive);
	if(temp == FALSE)
	{
		return(1);
	}

	temp = SPIRead(0x04);
	if(temp == 0)
	{
		return(FM1702_BYTECOUNTERR);
	}

	Read_FIFO(F_buff);
	temp = *F_buff;
	switch(temp)
	{
	case 0x00:	return(FM1702_NOTAUTHERR);	
	case 0x04:	return(FM1702_EMPTY);
	case 0x0a:	break;
	case 0x01:	return(FM1702_CRCERR);
	case 0x05:	return(FM1702_PARITYERR);
	default:	return(FM1702_WRITEERR);
	}

	temp = Command_Send(16, buff, Transceive);
	if(temp == TRUE)
	{
		return(0);
	}
	else
	{
		temp = SPIRead(0x0a);
		if((temp & 0x02) == 0x02)
			return(FM1702_PARITYERR);
		else if((temp & 0x04) == 0x04)
			return(FM1702_FRAMINGERR);
		else if((temp & 0x08) == 0x08)
			return(FM1702_CRCERR);
		else
			return(FM1702_WRITEERR);
	}
}


/****************************************************************/
/*名称: Read_E2 */
/*功能: 该函数实现从FM1702的EE中读出数据*/
/*输入: lsb, EE地址(低字节) */
/* msb, EE地址(高字节) */
/* count, 待读出数据EE的字节个数*/
/* buff, 指向待读出数据的指针*/
/*输出: TRUE, EE数据正确读出*/
/* FALSE, EE数据读出有误*/
/****************************************************************/
uchar Read_E2(uchar lsb, uchar msb, uchar count, uchar *buff)
{
	
	uchar	temp;
	
	*buff = lsb;
	*(buff + 1) = msb;
	*(buff + 2) = count;
	temp = Command_Send(3, buff, ReadE2);
	Read_FIFO(buff);
	if(temp != FALSE) 
	return(TRUE);
	return(FALSE);
}

/****************************************************************/
/*名称: Write_E2 */
/*功能: 该函数实现向FM1702的EE中写入数据*/
/*输入: lsb, EE地址(低字节) */
/* msb, EE地址(高字节) */
/* count, 待写入数据EE的字节个数*/
/* buff, 指向待写入数据的指针*/
/*输出: TRUE, EE数据正确写入*/
/* FALSE, EE数据写入有误*/
/****************************************************************/
uchar Write_E2( uchar lsb,uchar msb,uchar count, uchar *buff)
{
	uchar temp, i;
	for(i = 0; i < count; i++)
	{
		*(buff + count - i + 1) = *(buff - i + count);//指向长的高位，拷贝数值，结果后推2BYTE
	}

	*buff =lsb;
	*(buff+1) =msb; 
	temp = Command_Send(count + 2, buff, WriteE2);
 	if(temp != TRUE)
	{
		return FALSE;
	}

	temp = SPIRead(SecondaryStatus);
	temp = temp & 0x40;
	if(temp == 0x40)//等于0X40是表示E2擦写过程结束
	{
		SPIWrite(Command,0x00);			//added 2006/03/15
		return TRUE;
	}

	return FALSE;
}


void keyto()//A密码的加密格式：00 4个字节的序列号 块号（扇区号），共6个字节的密钥
{

	unsigned char i;

	RevBuffer[4]=block_numset(3);
	for(i = 0; i < 7; i++)
	RevBuffer[5 + i] = 0xff;		//默认密码
	
}	


uchar block_numset(uchar block_num)
{ 	
	unsigned char temp,i;
	i=m1erea;
	temp=block_num;
	while(i)
	{
		temp=temp+4;
		i--;
	}
	return temp;
}





///////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void init()
{

    	TMOD=0x21;		   //设T0为方式1，GATE=1；
		SCON=0x50;
		TH1=0xFD;
		TL1=0xFD;
		TH0=0;
		TL0=0; 
		TR0=1;  
		ET0=1;             //允许T0中断
		TR1=1;			   //开启定时器
		TI=1;
		EA=1;			   	//开启总中断
		
ES = 1;
RI = 1;








		P0=0xff;
}


void delay(unsigned int  x)
{
 	unsigned char k;
   	while(x--)                      //延时大约x毫秒钟
   	{
     	for(k=0;k<125;k++);
	}
}




void displayUid()
{
		WriteCommand(0x93);
		LCD_Data(UID[0]); //4个字节的序列号
		WriteCommand(0x94);
		LCD_Data(UID[1]); 
		WriteCommand(0x95);
		LCD_Data(UID[2]); 
		WriteCommand(0x96);
		LCD_Data(UID[3]); 

}

void setpay(uint ctime12)
{
	lcd[4]=ctime12/10000;//一些十进制的转换  65535
	lcd[3]=ctime12%10000/1000;
	lcd[2]=ctime12%1000/100;

	lcd[1]=ctime12%100/10;
	lcd[0]=ctime12%10;

	lcd[3]<<=4;
	lcd[3]=	lcd[3]+	lcd[2];

	lcd[1]<<=4;
	lcd[1]=	lcd[1]+	lcd[0];

	
}


unsigned char Card_Mun()
{
	unsigned char st,i,temp;
	st= AntiColl();
	if(st!=0)				 
    return (1);
	
	st=Select_Card();  //选择卡片
	if(st!=0)
    return (1);
 


					DispalyLcd();
					displayUid();
					beep=0;
					led=0;
					delay(100);
					beep=1;
					led=1;








	keyto();
	st = Load_key_CPY(&RevBuffer[5]);
	if(st!=0)
    return (1);	
		       
	st = Authentication(UID, RevBuffer[4]);
	if(st!=0)
    return (2);

	return (0);
}


void main()
{
	uchar secNR;
	uchar i;
	uchar lsb;
	uchar msb;
	uchar count;
	uchar stauts;
	int temp;

  	init();
	initial_LCD(); //显示屏初始化
	DispalyLcd_RF();
	printf("--------------------------------------------\n");
	printf("--------------------------------------------\n");
	printf("--------广州恒天单片机射频卡测试实验--------\n");
	printf("---------------    请刷卡!    --------------\n");
	printf("--------------------------------------------\n");
	printf("--------------------------------------------\n");
	while(1)
	{
			Init_FM1702();
			WriteCommand(0x01);  //清屏
			DispalyLcd_home();
			while (1)
		   {
				
					do
					{

							for(i=0;i<4;i++)
							{
								delay(100);
								if(i==0)
								{
									dis_Request1();//dis()
									stauts = Request(RF_CMD_REQUEST_STD);
									if(stauts==0)
									{break;}
								}
								if(i==1)
								{
									dis_Request2();//dis(())
									stauts = Request(RF_CMD_REQUEST_STD);
									if(stauts==0)
									{break;}								
								}
								if(i==2)
								{
									dis_Request3();//dis((()))
									stauts = Request(RF_CMD_REQUEST_STD);
									if(stauts==0)
									{break;}
								}
								if(i==3)
								{
									DispalyLcd1();//清dis((()))
									stauts = Request(RF_CMD_REQUEST_STD);
									if(stauts==0)
									{break;}
								}
								
							}

					}while(stauts);
		
				
					stauts=Card_Mun();//读取卡号	
					if(stauts!=0)
					{
						if(stauts==2)
						{
							dis_passerror();
							do{stauts = Request(RF_CMD_REQUEST_STD); }
							while(!stauts);
						}

						break;	//返回的值错误则退出
					}

				/////读数据块的操作///////////
				secNR=block_numset(1);      //操作数据块1
				stauts=MIF_READ(RevBuffer,secNR);  //读出数据块0的数据放在RevBuffer里面
				if(stauts!=0)
				break;	//返回的值错误则退出
				///////////////////////////////

		
		
				ctimeH=RevBuffer[2];//卡内块0中的第一个字节为金额高位
				ctimeL=RevBuffer[3];//卡内块0中的第二个字节为金额低位

				ctime=ctimeH;//操作金额的格式转换
				ctime<<=8;
				ctime=ctime+ctimeL;
				
	
				ctime=ctime-pay;//每刷一次卡扣掉卡内5元，pay为存放要扣金额的值
				cctime=ctime;	


				RevBuffer[3]=ctime;//把低字节写入缓存器中
				ctime=ctime&0xff00;	
				ctime>>=8;
				RevBuffer[2]=ctime;//把高字节写入缓存器中
		



				/////写数据块的操作////////////
				secNR=block_numset(1);      //操作数据块1
				stauts=MIF_Write(RevBuffer,secNR); //写块
				if(stauts!=0)
				break;	//返回的值错误则退出
				///////////////////////////////
		

		
		
		
				/////扣完款之后把卡余额写回卡的块0中后在显示屏显示出卡余额 ////
				setpay(pay);	//显示消费的金额
				displaymun(0x8b,0x8c,0x8e,0x8d,0x8f,lcd[4],lcd[3],lcd[1]);
		
				setpay(cctime);	//显示卡余额
				displaymun(0x9b,0x9c,0x9e,0x9d,0x9f,lcd[4],lcd[3],lcd[1]);//前5个字节为要显示的12864地址，后三个字节为显示的金额值 
				/////////////////////////////////////////////////////////////
				beep=0;
				led=0;
				delay(100);
				beep=1;
				led=1;
				delay(100);
				beep=0;
				led=0;
				delay(100);
				beep=1;
				led=1;


		
				do{stauts = Request(RF_CMD_REQUEST_STD); }//扣款成功后等待把卡移开感应区
				while(!stauts);
				WriteCommand(0x01);  //清屏
				DispalyLcd_home();
				  

			/////以下是对FM1702内部存储器的操作,可把一些重要数据存在里面而掉电不丢失///////
			
		/*
				////////////////写数据到1702的芯片内部E2PROM的地址里面/////////////////////
				lsb=0x50;		//1702的芯片内部E2PROM的地址:0x0050
				msb=0;		
				RevBuffer[0]=0xa1;
				RevBuffer[1]=0xb2;
				RevBuffer[2]=0xc3;
				RevBuffer[3]=0xd4;
				count=4;		//待写入数据EE的字节个数//		
				Write_E2(lsb,msb, count, RevBuffer);
				delay(200);
				////////////////////////////////////////////////////////////////////////////
		
		
				////////////////读1702的芯片内部E2PROM的地址里面的数据/////////////////////////////
				lsb=0x50;			//读E2PROM的地址低位
				msb=0x0;			//地址高位
				count=4;			//要读的个数
				Read_E2(lsb, msb, count, RevBuffer);  //读出的数据放在RevBuffer里面
				////////////////////////////////////////////////////////////////////////////////////
		*/
			
		    }
	 }
}







