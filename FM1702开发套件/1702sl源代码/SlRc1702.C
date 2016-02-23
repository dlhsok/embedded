#include "reg52.h"
#include <intrins.h>
#include <string.h>
#include "main.h"
#include "slrc1702.h"


#define FSD 64

extern struct TranSciveBuffer{unsigned char MfCommand;
                              unsigned int  MfLength;
                              unsigned char MfData[64];
                             };
/////////////////////////////////////////////////////////////////////
//复位并初始化fm1702
//
/////////////////////////////////////////////////////////////////////
char PcdReset()
{
   char status = MI_OK;
   char n = 0xFF;
   unsigned int i = 3000;

    DelayMs(100);
    DelayMs(100);
	DelayMs(100);
    DelayMs(100);
    DelayMs(100);
    DelayMs(100);
    DelayMs(100);

   RC1702_RST=0;
   DelayMs(50);
   DelayMs(50);

   RC1702_RST=1;
   DelayMs(5);
   DelayMs(50);
   DelayMs(50);
   DelayMs(50);
   DelayMs(50);
   DelayMs(50);
   DelayMs(50);
   RC1702_RST=0;
   DelayMs(5);

   while (i!=0 && n)
   {
      n = ReadRawRC(RegCommand);
      i--;
   }

   if (i != 0)
   {
      WriteRawRC(RegPage,0x80);
      n = 0x80;
      while ( (i!=0) && (n&0x80) )
      {
          n = ReadRawRC(RegCommand);
          i--;
      }
      if (i==0 || (n&0xFF))
      {   status = MI_RESETERR;   }
   }
   else
   {    status = MI_RESETERR;     }
   
   if (status == MI_OK)
   {    WriteRawRC(RegPage,0x0);  }
   
   return status;
}

//////////////////////////////////////////////////////////////////////
//设置1702的工作方式 
//////////////////////////////////////////////////////////////////////
char PcdConfigISOType(unsigned char type)
{
   
   if (type == 'A')                    //ISO14443_A
   { 
       ClearBitMask(RegControl,0x08);

       WriteRawRC(RegClockQControl,0x0);
       WriteRawRC(RegClockQControl,0x40);
       Delay_50us(2);                   // wait approximately 100 us - calibration in progress
       ClearBitMask(RegClockQControl,0x40);
       WriteRawRC(RegBitPhase,0xAD);	
       WriteRawRC(RegRxThreshold,0xFF);
       WriteRawRC(RegRxControl2,0x01);      

       WriteRawRC(RegTimerControl,0x02);   
       WriteRawRC(RegIRqPinConfig,0x03); 
	    M500PcdRfReset(1);

       PcdSetTmo(106);
       DelayMs(1);
       PcdAntennaOn();

   }

   else{ return -1; }
   return MI_OK;
}
///////////////////////////////////////////////////////////////////////
// Reset Rf Card
///////////////////////////////////////////////////////////////////////
char M500PcdRfReset(unsigned char ms)
{
    char status = MI_OK;
	
    if(ms)
    {
        ClearBitMask(RegTxControl,0x03); 
         DelayMs(2);                
        SetBitMask(RegTxControl,0x03); 
    }
    else
    {
        ClearBitMask(RegTxControl,0x03);  
    }
    return status;
}


/////////////////////////////////////////////////////////////////////
//读1702寄存器位
//input:Address=寄存器地址
//ouput:读出的值
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
     unsigned char i, ucAddr;
     unsigned char ucResult=0;

     RC1702_SCK = 0;
     RC1702_NSS = 0;
     ucAddr =((Address<<1)&0x7E)|0x80;

     for(i=8;i>0;i--)
     {
         RC1702_MOSI = ((ucAddr&0x80)==0x80);
         RC1702_SCK = 1;
         ucAddr <<= 1;
         RC1702_SCK = 0;
     }

     for(i=8;i>0;i--)
     {
         RC1702_SCK = 1;
         ucResult <<= 1;
         ucResult|=(bit)RC1702_MISO;
         RC1702_SCK = 0;
     }

	 RC1702_NSS = 1;
	 RC1702_SCK = 1;
     return ucResult;
}


/////////////////////////////////////////////////////////////////////
//写1702寄存器
//input:Address=寄存器地址
//      value=要写入的值
/////////////////////////////////////////////////////////////////////

void WriteRawRC(unsigned char Address, unsigned char value)
{  
    unsigned char i, ucAddr;

    RC1702_SCK = 0;
    RC1702_NSS = 0;
    ucAddr = ((Address<<1)&0x7E);

    for(i=8;i>0;i--)
    {
        RC1702_MOSI = ((ucAddr&0x80)==0x80);
        RC1702_SCK = 1;
        ucAddr <<= 1;
        RC1702_SCK = 0;
    }

    for(i=8;i>0;i--)
    {
        RC1702_MOSI = ((value&0x80)==0x80);
        RC1702_SCK = 1;
        value <<= 1;
        RC1702_SCK = 0;
    }
        RC1702_NSS = 1;
        RC1702_SCK = 1;
}


/////////////////////////////////////////////////////////////////////
//置1702寄存器位
//input:reg=寄存器地址
//      mask=置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)  
{
   char tmp = 0x0;
   tmp = ReadRawRC(reg);
   WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//清1702寄存器位
//input:reg=寄存器地址
//      mask=清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)  
{
   char tmp = 0x0;
   tmp = ReadRawRC(reg);
   WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//设置1702定时器
//input:tmolength=设置值
/////////////////////////////////////////////////////////////////////
void PcdSetTmo(unsigned char tmoLength)
{
   switch(tmoLength)
   {  
      case 0:                         // (0.302 ms) FWI=0
         WriteRawRC(RegTimerClock,0x07); // TAutoRestart=0,TPrescale=128
         WriteRawRC(RegTimerReload,0x21);// TReloadVal = 'h21 =33(dec) 
         break;
      case 1:                         // (0.604 ms) FWI=1
         WriteRawRC(RegTimerClock,0x07); // TAutoRestart=0,TPrescale=128
         WriteRawRC(RegTimerReload,0x41);// TReloadVal = 'h41 =65(dec) 
         break;
      case 2:                         // (1.208 ms) FWI=2
         WriteRawRC(RegTimerClock,0x07); // TAutoRestart=0,TPrescale=128
         WriteRawRC(RegTimerReload,0x81);// TReloadVal = 'h81 =129(dec) 
         break;
      case 3:                         // (2.416 ms) FWI=3
         WriteRawRC(RegTimerClock,0x09); // TAutoRestart=0,TPrescale=4*128
         WriteRawRC(RegTimerReload,0x41);// TReloadVal = 'h41 =65(dec) 
         break;
      case 4:                         // (4.833 ms) FWI=4
         WriteRawRC(RegTimerClock,0x09); // TAutoRestart=0,TPrescale=4*128
         WriteRawRC(RegTimerReload,0x81);// TReloadVal = 'h81 =129(dec) 
         break;
      case 5:                         // (9.666 ms) FWI=5
         WriteRawRC(RegTimerClock,0x0B); // TAutoRestart=0,TPrescale=16*128
         WriteRawRC(RegTimerReload,0x41);// TReloadVal = 'h41 =65(dec) 
         break;
      case 6:                         // (19.33 ms) FWI=6
         WriteRawRC(RegTimerClock,0x0B); // TAutoRestart=0,TPrescale=16*128
         WriteRawRC(RegTimerReload,0x81);// TReloadVal = 'h81 =129(dec) 
         break;
      case 7:                         // (38.66 ms) FWI=7
         WriteRawRC(RegTimerClock,0x0D); // TAutoRestart=0,TPrescale=64*128
         WriteRawRC(RegTimerReload,0x41);// TReloadVal = 'h41 =65(dec) 
         break;
      case 8:                         // (77.32 ms) FWI=8
         WriteRawRC(RegTimerClock,0x0D); // TAutoRestart=0,TPrescale=64*128
         WriteRawRC(RegTimerReload,0x81);// TReloadVal = 'h81 =129(dec) 
         break;
      case 9:                         // (154.6 ms) FWI=9
         WriteRawRC(RegTimerClock,0x0F); // TAutoRestart=0,TPrescale=256*128
         WriteRawRC(RegTimerReload,0x41);// TReloadVal = 'h41 =65(dec) 
         break;
      case 10:                        // (309.3 ms) FWI=10
         WriteRawRC(RegTimerClock,0x0F); // TAutoRestart=0,TPrescale=256*128
         WriteRawRC(RegTimerReload,0x81);// TReloadVal = 'h81 =129(dec) 
         break;
      case 11:                        // (618.6 ms) FWI=11
         WriteRawRC(RegTimerClock,0x13); // TAutoRestart=0,TPrescale=4096*128
         WriteRawRC(RegTimerReload,0x11);// TReloadVal = 'h21 =17(dec) 
         break;
      case 12:                        // (1.2371 s) FWI=12
         WriteRawRC(RegTimerClock,0x13); // TAutoRestart=0,TPrescale=4096*128
         WriteRawRC(RegTimerReload,0x21);// TReloadVal = 'h41 =33(dec) 
         break;
      case 13:                        // (2.4742 s) FWI=13
         WriteRawRC(RegTimerClock,0x13); // TAutoRestart=0,TPrescale=4096*128
         WriteRawRC(RegTimerReload,0x41);// TReloadVal = 'h81 =65(dec) 
         break;
      case 14:                        // (4.9485 s) FWI=14
         WriteRawRC(RegTimerClock,0x13); // TAutoRestart=0,TPrescale=4096*128
         WriteRawRC(RegTimerReload,0x81);// TReloadVal = 'h81 =129(dec) 
         break;
      case 15:                        // (4.9485 s) FWI=14
         WriteRawRC(RegTimerClock,0x9); // TAutoRestart=0,TPrescale=4096*128
         WriteRawRC(RegTimerReload,0x0ff);// TReloadVal = 'h81 =129(dec) 
         break;
      default:                       // 
         WriteRawRC(RegTimerClock,0x19); // TAutoRestart=0,TPrescale=128
         WriteRawRC(RegTimerReload,tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
         break;
   }     
   WriteRawRC(RegTimerControl,0X06);
}

/////////////////////////////////////////////////////////////////////
//通过1702和ISO14443卡通讯
//input: pi->MfCommand = RC1702命令字
//       pi->MfLength  = 发送的数据长度
//       pi->MfData[]  = 发送数据
//output:status        = 错误字
//       pi->MfLength  = 接收的数据BIT长度
//       pi->MfData[]  = 接收数据
/////////////////////////////////////////////////////////////////////
char PcdComTransceive(struct TranSciveBuffer *pi)
{
   bit recebyte = 0;
   char status;
   unsigned char irqEn   = 0x00;
   unsigned char waitFor = 0x00;
   unsigned char lastBits;
   unsigned char n;
   unsigned int i;
   switch (pi->MfCommand)
   {
      case PCD_IDLE:
         irqEn   = 0x00;
         waitFor = 0x00;
         break;
      case PCD_WRITEE2:
         irqEn   = 0x11;
         waitFor = 0x10;
         break;
      case PCD_READE2:
         irqEn   = 0x07;
         waitFor = 0x04;
         recebyte=1;
         break;
      case PCD_LOADCONFIG:
      case PCD_LOADKEYE2:
      case PCD_AUTHENT1:
         irqEn   = 0x05;
         waitFor = 0x04;
         break;
      case PCD_CALCCRC:
         irqEn   = 0x11;
         waitFor = 0x10;
         break;
      case PCD_AUTHENT2:
         irqEn   = 0x04;
         waitFor = 0x04;
         break;
      case PCD_RECEIVE:
         irqEn   = 0x06;
         waitFor = 0x04;
         recebyte=1;
         break;
      case PCD_LOADKEY:
         irqEn   = 0x05;
         waitFor = 0x04;
         break;
      case PCD_TRANSMIT:
         irqEn   = 0x05;
         waitFor = 0x04;
         break;
      case PCD_TRANSCEIVE:
         irqEn   = 0x3D;
         waitFor = 0x04;
         recebyte=1;
         break;
      default:
         pi->MfCommand = MI_UNKNOWN_COMMAND;
         break;
   }
   
   if (pi->MfCommand != MI_UNKNOWN_COMMAND)
   {
      WriteRawRC(RegPage,0x00);
      WriteRawRC(RegInterruptEn,0x7F);
      WriteRawRC(RegInterruptRq,0x7F);
      WriteRawRC(RegCommand,PCD_IDLE);
      SetBitMask(RegControl,0x01);
      WriteRawRC(RegInterruptEn,irqEn|0x80);
      for (i=0; i<pi->MfLength; i++)
      {
         WriteRawRC(RegFIFOData, pi->MfData[i]);
      }
      WriteRawRC(RegCommand, pi->MfCommand);
      i = 0x3500;
      do
      {
         n = ReadRawRC(RegInterruptRq);
         i--;
      }
      while ((i!=0) && !(n&irqEn&0x20) && !(n&waitFor));
      status = MI_COM_ERR;
      if ((i!=0) && !(n&irqEn&0x20))
      {
         if (!(ReadRawRC(RegErrorFlag)&0x17))
         {
            status = MI_OK;
            if (recebyte)
            {
              	n = ReadRawRC(RegFIFOLength);
              	lastBits = ReadRawRC(RegSecondaryStatus) & 0x07;
                if (lastBits)
                {
                   pi->MfLength = (n-1)*8 + lastBits;
                }
                else
                {
                   pi->MfLength = n*8;
                }
                if (n == 0)
                {
                   n = 1;
                }
                for (i=0; i<n; i++)
                {
                    pi->MfData[i] = ReadRawRC(RegFIFOData);
                }
            }
         }
		 else if (ReadRawRC(RegErrorFlag)&0x01)
         {
		    status = MI_COLLERR;
            if (recebyte)
            {
              	n = ReadRawRC(RegFIFOLength);
              	lastBits = ReadRawRC(RegSecondaryStatus) & 0x07;
                if (lastBits)
                {
                   pi->MfLength = (n-1)*8 + lastBits;
                }
                else
                {
                   pi->MfLength = n*8;
                }
                if (n == 0)
                {
                   n = 1;
                }
                for (i=0; i<n; i++)
                {
                    pi->MfData[i+1] = ReadRawRC(RegFIFOData);
                }
            }
			pi->MfData[0]=ReadRawRC(0x0B);
         }

      }
      else if (n & irqEn & 0x20)
      {   status = MI_NOTAGERR;   }
      else if (!(ReadRawRC(RegErrorFlag)&0x17))
      {   status = MI_ACCESSTIMEOUT;   }
      else
      {   status = MI_COM_ERR;    }
      
      WriteRawRC(RegInterruptEn,0x7F);
      WriteRawRC(RegInterruptRq,0x7F);
      SetBitMask(RegControl,0x04);           // stop timer now
      WriteRawRC(RegCommand,PCD_IDLE); 
   }
   return status;
}

/*
/////////////////////////////////////////////////////////////////////
//读fm1702slEEPROM
//input :startaddr=EEPROM地址（低位在前）
//       length=读字节数
//output:readdata=读出的数据
/////////////////////////////////////////////////////////////////////
char PcdReadE2(unsigned int startaddr,unsigned char length,unsigned char *readdata)
{
    char status;
    struct TranSciveBuffer MfComData;
    struct TranSciveBuffer *pi;
    pi = &MfComData;

    MfComData.MfCommand = PCD_READE2;
    MfComData.MfLength  = 3;
    MfComData.MfData[0] = startaddr&0xFF;
    MfComData.MfData[1] = (startaddr >> 8) & 0xFF;
    MfComData.MfData[2] = length;

    status = PcdComTransceive(pi);

    if (status == MI_OK)
    {   memcpy(readdata, &MfComData.MfData[0], length);    }
    return status;
}
*/

/////////////////////////////////////////////////////////////////////
//写fm1702slEEPROM
//input :startaddr=EEPROM地址（低位在前）
//       length=写字节数
//       writedata=要写入的数据
/////////////////////////////////////////////////////////////////////
char PcdWriteE2(unsigned int startaddr,unsigned char length,unsigned char *writedata)
{
    char status;
    struct TranSciveBuffer MfComData;    
    struct TranSciveBuffer *pi;
    pi = &MfComData;

    MfComData.MfCommand = PCD_WRITEE2;
    MfComData.MfLength  = length+2;
    MfComData.MfData[0] = startaddr&0xFF;
    MfComData.MfData[1] = (startaddr >> 8) & 0xFF;
    memcpy(&MfComData.MfData[2], writedata, length);    

    status = PcdComTransceive(pi);
    return status;
}

/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
char PcdAntennaOn()
{
    unsigned char i;
    i = ReadRawRC(RegTxControl);
    if (i & 0x03)
    {   return MI_OK;	}
    else
    {
        SetBitMask(RegTxControl, 0x03);
        return MI_OK;
    }
}

