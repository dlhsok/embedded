#include "LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "PIN_DEF.H"
#include "RC522.H"
//#include "UART0_Func.c"
#include "ctype.h" 
#include "BoardConfig.h"

const PIN RF_LPCTL = {2,2};				// P2.3 射频卡休眠控制 ------RST  
const PIN RF_SS	 = {2,2};												// p2.7 射频卡从机选择(SS)---SDA
const PIN RF_SCLK  = {2,2};                 // p2.6 射频卡数据时钟输出(SCLK)
const PIN RF_DATA_OUT  = {2,2};             // p2.5 射频卡数据输出(MOSI)
const PIN RF_DATA_IN  = {2,2};              // p2.1 射频模块输入(MISO) 

unsigned char UID[5],Temp[4]                                       ;
unsigned char RF_Buffer[18]                                        ;
unsigned char Password_Buffer[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}   ; // Mifare One ????
unsigned char des_on       = 0                                     ; // DES????

void Delay(unsigned int time)
{
  unsigned int i,k                             ;
  for(i=0;i<255;i++)
    for(k=0;k<time;k++);  
}

void Auto_Reader(void)
{
  while(1)
  {
    if(PcdRequest(0x52,Temp)==MI_OK)
    {
      if(Temp[0]==0x04&&Temp[1]==0x00)  
          PutString("MFOne-S50");
        else if(Temp[0]==0x02&&Temp[1]==0x00)
          PutString("MFOne-S70");
        else if(Temp[0]==0x44&&Temp[1]==0x00)
          PutString("MF-UltraLight");
        else if(Temp[0]==0x08&&Temp[1]==0x00)
          PutString("MF-Pro");
        else if(Temp[0]==0x44&&Temp[1]==0x03)
          PutString("MF Desire");
        else
          PutString("Unknown");
      if(PcdAnticoll(UID)==MI_OK)
      { 
        PutString0("Card Id is:");
        tochar(UID[0]);
        tochar(UID[1]);
        tochar(UID[2]);
        tochar(UID[3]);
        PutString0('\n');                              //发送换行指令
                        
        RED_LED_ON                                            ;
        Delay(200)                                           ;
        RED_LED_OFF                                           ;
        Delay(200)                                           ;
      }
    }
  else GRE_LED_OFF                                            ;
  } 
}

void Find_Card(void)
{
    if(PcdRequest(0x52,Temp)==MI_OK)
    {
      if(Temp[0]==0x04&&Temp[1]==0x00)  
          PutString("MFOne-S50");
        else if(Temp[0]==0x02&&Temp[1]==0x00)
          PutString("MFOne-S70");
        else if(Temp[0]==0x44&&Temp[1]==0x00)
          PutString("MF-UltraLight");
        else if(Temp[0]==0x08&&Temp[1]==0x00)
          PutString("MF-Pro");
        else if(Temp[0]==0x44&&Temp[1]==0x03)
          PutString("MF Desire");
        else
          PutString("Unknown");
        PutString("SUCCESS!");
    }
    else PutString("Faile!");                                             
}

void Init_Port(void)
{
	PIN_Configure(RF_LPCTL.Portnum, RF_LPCTL.Pinnum, PIN_FUNC_0, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	GPIO_SetDir(RF_LPCTL.Portnum, RF_LPCTL.Pinnum, GPIO_DIR_OUTPUT);
	PIN_Configure(RF_SS.Portnum, RF_SS.Pinnum, PIN_FUNC_0, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	GPIO_SetDir(RF_SS.Portnum, RF_SS.Pinnum, GPIO_DIR_OUTPUT);
	PIN_Configure(RF_SCLK.Portnum, RF_SCLK.Pinnum, PIN_FUNC_0, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	GPIO_SetDir(RF_SCLK.Portnum, RF_SCLK.Pinnum, GPIO_DIR_OUTPUT);
	PIN_Configure(RF_DATA_OUT.Portnum, RF_DATA_OUT.Pinnum, PIN_FUNC_0, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	GPIO_SetDir(RF_DATA_OUT.Portnum, RF_DATA_OUT.Pinnum, GPIO_DIR_OUTPUT);
	PIN_Configure(RF_DATA_IN.Portnum, RF_DATA_IN.Pinnum, PIN_FUNC_0, PIN_PINMODE_PULLUP, PIN_PINMODE_NORMAL);
	GPIO_SetDir(RF_DATA_IN.Portnum, RF_DATA_IN.Pinnum, GPIO_DIR_INPUT);

}
/*******************************************
函数名称：HandleConfigMenu
功    能：处理PC的配置函数
参    数：inputvalue--接收到的来自PC机的字符
返回值  ：无
********************************************/
void HandleConfigMenu(uchar inputvalue)
{
    switch(toupper(inputvalue)) 
    {
    case 'A':
              Auto_Reader();
              break;
    case 'F':
              Find_Card();
              break;
    default:
              DisplayConfigMenu();  
    }
}

int main()
{
	uchar i;
  PcdReset();//复位RC522
  PcdAntennaOn();//开启天线发射 
  DisplayConfigMenu();
  while(1)
  {
        Send1Char('>');
        Send1Char('\n');
        i=Get1Char();
        HandleConfigMenu(i);
  }
}
