////指定PCD接收缓冲值
//#ifndef FSDI
//#define FSDI 4
//#endif
///////////////////////////////////////////////////////////////////////
////89C52特殊寄存器定义
///////////////////////////////////////////////////////////////////////
//sfr16    RCAP2LH    =    0xCA;
//sfr16    T2LH       =    0xCC;


/////////////////////////////////////////////////////////////////////
//端口定义
/////////////////////////////////////////////////////////////////////



//sbit  RC1702_RST   =P0^7;
//sbit  RC1702_NSS   =P0^6;
//sbit  RC1702_MOSI  =P0^5;
//sbit  RC1702_SCK   =P0^4;
//sbit  RC1702_MISO  =P0^3;
//
//sbit  led =P0^2;
//sbit  BELL=P1^2;
#include "RST.h"
#include "NSS.h"
#include "MOSI.h"
#include "SCK.h"
#include "MISO.h"

extern LDD_TDeviceData *bitRSTPtr;
extern LDD_TDeviceData *bitNSSPtr;
extern LDD_TDeviceData *bitMOSIPtr;
extern LDD_TDeviceData *bitSCKPtr;
extern LDD_TDeviceData *bitMISOPtr;

#define RC1702_RST   =P0^7;
#define RC1702_NSS   =P0^6;
#define RC1702_MOSI  =P0^5;
#define RC1702_SCK   =P0^4;
#define RC1702_MISO  =P0^3;

#define led ;
#define BELL ;

struct TranSciveBuffer{unsigned char MfCommand;
                       unsigned int  MfLength;
                       unsigned char MfData[64];
                      };

/////////////////////////////////////////////////////////////////////
//1702的EEPROM定义
/////////////////////////////////////////////////////////////////////
#define  ICDEV_RC500_ADR 0x30
/////////////////////////////////////////////////////////////////////
//延时常数及波特率定义
/////////////////////////////////////////////////////////////////////


#define OSC_FREQ               22118400L     // 11059200L

#define BOUND115200             256 - (OSC_FREQ/192L)/115200L   // 255
#define BOUND57600              256 - (OSC_FREQ/192L)/57600L    // 254
#define BOUND38400              256 - (OSC_FREQ/192L)/38400L    // 253
#define BOUND28800              256 - (OSC_FREQ/192L)/28800L    // 252
#define BOUND19200              256 - (OSC_FREQ/192L)/19200L    // 250
#define BOUND14400              256 - (OSC_FREQ/192L)/14400L    // 248
#define BOUND9600               256 - (OSC_FREQ/192L)/9600L     // 244
#define  BOUND4800       256 - 24
#define RCAP2_50us             65536L - OSC_FREQ/240417L
#define RCAP2_1ms              65536L - OSC_FREQ/12021L
#define RCAP2_10ms             65536L - OSC_FREQ/1200L
#define RCAP2_1s             65536L - OSC_FREQ/12L
/////////////////////////////////////////////////////////////////////
//和上位机的通讯错误代码定义
/////////////////////////////////////////////////////////////////////
#define  FAULT10  10                                      //通用错误
#define  FAULT11  11                                      //不支持该命令
#define  FAULT12  12                                      //命令参数错
#define  FAULT13  13                                      //无卡
#define  FAULT14  14                                      //射频基站损坏

#define  FAULT20  20                                      //寻卡失?
#define  FAULT21  21                                      //卡复位失败
#define  FAULT22  22                                      //密码验证失败
#define  FAULT23  23                                      //读卡失败
#define  FAULT24  24                                      //写卡失败



#define mifare1			    1
#define mifarepro		    2
#define mifarelight		    3
#define unknowncard		    4


/////////////////////////////////////////////////////////////////////
//函数原型
/////////////////////////////////////////////////////////////////////
void InitializeSystem();                                   
void Rc1702Ready();                                         
//void AnswerCommandOk();
//void AnswerCommandOk1();
void AnswerOk();          
//void AnswerErr(char faultcode);                            
void DelayMs(unsigned int j);                                      
void Delay_50us(unsigned char j);                                  

void ComSetBound();
//void ComGetHardModel();
//void ComM1702PcdConfigISOType();
//void ComPcdAntenna();
void ComRequestA();
void ComAnticoll();
void ComSelect();
void ComHlta();
void ComAuthentication();
void ComM1Read();
void ComM1Write();
//void ComM1Initval();
//void ComM1Readval();
void ComM1Decrement();
//void ComM1Increment();
//void ComM1Restore();
//void ComM1Transfer();

//void ComTypeARst();
//void ComTypeACOS();
//void ComUL_PcdAnticoll();
//void ComUL_PcdWrite();
void Comsond();
void ComM500PcdLoadKeyE2();
void ComM500PiccAuthE2();



