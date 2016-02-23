/////////////////////////////////////////////////////////////////////
//函数原型
/////////////////////////////////////////////////////////////////////
//复位1702
char PcdReset();      

//设置1702的工作方式    
char PcdConfigISOType(unsigned char type);                                    

//读1702-EEPROM数据
//char PcdReadE2(unsigned int startaddr,                     
//               unsigned char length,
//               unsigned char *readdata);    

//写数据到1702-EEPROM               
char PcdWriteE2(unsigned int startaddr,
                unsigned char length,
                unsigned char *writedata); 

//开启1702天线发射 
char PcdAntennaOn();   

//关闭1702天线发射                                         
char PcdAntennaOff(); 

//写1702寄存器
void WriteRawRC(unsigned char Address,unsigned char value);

//读1702寄存器                     
unsigned char ReadRawRC(unsigned char Address); 

//置1702寄存器位                                
void SetBitMask(unsigned char reg,unsigned char mask); 

//清1702寄存器位                         
void ClearBitMask(unsigned char reg,unsigned char mask); 

//设置1702定时器                       
void PcdSetTmo(unsigned char tmoLength);                                

//ISO14443通讯函数
char PcdComTransceive(struct TranSciveBuffer *pi);

char M500PcdRfReset(unsigned char ms);

/////////////////////////////////////////////////////////////////////
//1702命令字
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_WRITEE2           0x01               //写EEPROM
#define PCD_READE2            0x03               //读EEPROM
#define PCD_LOADCONFIG        0x07               //调EEPROM中保存的寄存器设置
#define PCD_LOADKEYE2         0x0B               //将EEPROM中保存的密钥调入缓存
#define PCD_AUTHENT1          0x0C               //验证密钥第一步
#define PCD_AUTHENT2          0x14               //验证密钥第二步
#define PCD_RECEIVE           0x16               //接收数据
#define PCD_LOADKEY           0x19               //传送密钥
#define PCD_TRANSMIT          0x1A               //发送数据
#define PCD_TRANSCEIVE        0x1E               //发送并接收数据
#define PCD_RESETPHASE        0x3F               //复位
#define PCD_CALCCRC           0x12               //CRC计算

/////////////////////////////////////////////////////////////////////
//1702 FIFO长度定义
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//1702寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RegPage               0x00    
#define     RegCommand            0x01    
#define     RegFIFOData           0x02    
#define     RegPrimaryStatus      0x03    
#define     RegFIFOLength         0x04    
#define     RegSecondaryStatus    0x05
#define     RegInterruptEn        0x06    
#define     RegInterruptRq        0x07    
// PAGE 1      Control and Status
#define     RegControl            0x09    
#define     RegErrorFlag          0x0A
#define     RegCollpos            0x0B
#define     RegTimerValue         0x0C
#define     RegCRCResultLSB       0x0D
#define     RegCRCResultMSB       0x0E
#define     RegBitFraming         0x0F
// PAGE 2      Transmitter and Coder Control
#define     RegTxControl          0x11
#define     RegCwConductance      0x12
#define     RegModConductance     0x13
#define     RegCoderControl       0x14
#define     RegModWidth           0x15
#define     RegModWidthSOF        0x16
#define     RegTypeBFraming       0x17
// PAGE 3      Receiver and Decoder Control
#define     RegRxControl1         0x19
#define     RegDecoderControl     0x1A
#define     RegBitPhase           0x1B
#define     RegRxThreshold        0x1C
#define     RegBPSKDemControl     0x1D
#define     RegRxControl2         0x1E
#define     RegClockQControl      0x1F
// PAGE 4      RF-Timing and Channel Redundancy
#define     RegRxWait             0x21
#define     RegChannelRedundancy  0x22
#define     RegCRCPresetLSB       0x23
#define     RegCRCPresetMSB       0x24
#define     RegTimeSlotPeriod     0x25
#define     RegMfOutSelect        0x26
#define     RFU27                 0x27
// PAGE 5      FIFO, Timer and IRQ-Pin Configuration
#define     RegFIFOLevel          0x29
#define     RegTimerClock         0x2A
#define     RegTimerControl       0x2B
#define     RegTimerReload        0x2C
#define     RegIRqPinConfig       0x2D
#define     RFU2E                 0x2E
#define     RFU2F                 0x2F
// PAGE 6      RFU
#define     RFU31                 0x31
#define     RFU32                 0x32
#define     RFU33                 0x33
#define     RFU34                 0x34
#define     RFU35                 0x35
#define     RFU36                 0x36
#define     RFU37                 0x37
// PAGE 7      Test Control
#define     RFU39                 0x39  
#define     RegTestAnaSelect      0x3A   
#define     RFU3B                 0x3B   
#define     RFU3C                 0x3C   
#define     RegTestDigiSelect     0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  0x3F

/////////////////////////////////////////////////////////////////////
//和1702通讯时返回的错误代码
/////////////////////////////////////////////////////////////////////
#define MI_OK                          0
#define MI_CHK_OK                      0

#define MI_NOTAGERR                0x21 //   (-1)
#define MI_CHK_FAILED                  (-1)
#define MI_CRCERR                      (-2)
#define MI_CHK_COMPERR                 (-2)
#define MI_EMPTY                       (-3)
#define MI_AUTHERR                     (-4)
#define MI_PARITYERR                   (-5)
#define MI_CODEERR                   0x22 // (-6)
#define MI_SERNRERR                    (-8)
#define MI_KEYERR                      (-9)
#define MI_NOTAUTHERR                0x23//  (-10)
#define MI_BITCOUNTERR              0x24 //  (-11)
#define MI_BYTECOUNTERR                (-12)
#define MI_IDLE                        (-13)
#define MI_TRANSERR                    (-14)
#define MI_WRITEERR                    (-15)
#define MI_INCRERR                     (-16)
#define MI_DECRERR                     (-17)
#define MI_READERR                     (-18)
#define MI_OVFLERR                     (-19)
#define MI_POLLING                     (-20)
#define MI_FRAMINGERR                  (-21)
#define MI_ACCESSERR                   (-22)
#define MI_UNKNOWN_COMMAND             (-23)
#define MI_COLLERR                     (-24)
#define MI_RESETERR                    (-25)
#define MI_INITERR                     (-25)
#define MI_INTERFACEERR                (-26)
#define MI_ACCESSTIMEOUT               (-27)
#define MI_NOBITWISEANTICOLL           (-28)
#define MI_QUIT                        (-30)
#define MI_RECBUF_OVERFLOW             (-50)
#define MI_SENDBYTENR                  (-51)
#define MI_SENDBUF_OVERFLOW            (-53)
#define MI_BAUDRATE_NOT_SUPPORTED      (-54)
#define MI_SAME_BAUDRATE_REQUIRED      (-55)
#define MI_WRONG_PARAMETER_VALUE       (-60)
#define MI_BREAK                       (-99)
#define MI_NY_IMPLEMENTED              (-100)
#define MI_NO_MFRC                     (-101)
#define MI_MFRC_NOTAUTH                (-102)
#define MI_WRONG_DES_MODE              (-103)
#define MI_HOST_AUTH_FAILED            (-104)
#define MI_WRONG_LOAD_MODE             (-106)
#define MI_WRONG_DESKEY                (-107)
#define MI_MKLOAD_FAILED               (-108)
#define MI_FIFOERR                     (-109)
#define MI_WRONG_ADDR                  (-110)
#define MI_DESKEYLOAD_FAILED           (-111)
#define MI_WRONG_SEL_CNT               (-114)
#define MI_WRONG_TEST_MODE             (-117)
#define MI_TEST_FAILED                 (-118)
#define MI_TOC_ERROR                   (-119)
#define MI_COMM_ABORT                  (-120)
#define MI_INVALID_BASE                (-121)
#define MI_MFRC_RESET                  (-122)
#define MI_WRONG_VALUE                 (-123)
#define MI_VALERR                  0x25  //  (-124)
#define MI_COM_ERR                   0x26// (-125)


