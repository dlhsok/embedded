//  包含必要的头文件
#include  "all.h"



extern int a;

//  防止JTAG失效
extern void jtagWait(void);

//  系统时钟初始化
extern void clockInit(void);



extern void PortInit(void);
extern void SetSclk(unsigned char val);
extern void SetMosi(unsigned char val);
extern void uDelay(unsigned char l);

extern void Delay(unsigned char n);
extern void WriteCommand(unsigned char Data);
extern void WriteData(unsigned char Data);
extern void SetStartColumn(unsigned char d);
extern void SetAddressingMode(unsigned char d);
extern void SetColumnAddress(unsigned char a, unsigned char b);
extern void SetPageAddress(unsigned char a, unsigned char b);
extern void SetStartLine(unsigned char d);
extern void SetContrastControl(unsigned char d);
extern void Set_Charge_Pump(unsigned char d);
extern void Set_Segment_Remap(unsigned char d);
extern void Set_Entire_Display(unsigned char d);
extern void Set_Inverse_Display(unsigned char d);
extern void Set_Multiplex_Ratio(unsigned char d);
extern void Set_Display_On_Off(unsigned char d)	;
extern void SetStartPage(unsigned char d);
extern void Set_Common_Remap(unsigned char d);
extern void Set_Display_Offset(unsigned char d);
extern void Set_Display_Clock(unsigned char d);
extern void Set_Precharge_Period(unsigned char d);
extern void Set_Common_Config(unsigned char d);
extern void Set_VCOMH(unsigned char d);
extern void Set_NOP(void);
extern void FillArea(unsigned char spage, unsigned char epage,
                 unsigned char scolumn, unsigned char ecolumn,
                 unsigned char pt);
extern void CheckOutline(void);
extern void OledInit(void);
extern void Display8x16Str(unsigned char page, unsigned char column, unsigned char *str);
extern void DisplayChinese(unsigned char page, unsigned char column, unsigned int location);
extern void TestMain(void);
