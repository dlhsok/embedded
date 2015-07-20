#ifndef _oled_H_
#define _oled_H_

#define uint unsigned int
#define uchar unsigned char
	
#define LCD_CS_PORT (0)
#define LCD_CS_PIN	(16)
#define LCD_MOSI_PORT (0)
#define LCD_MOSI_PIN	(18)
#define LCD_DC_PORT (0)
#define LCD_DC_PIN	(22)
#define LCD_CLK_PORT (0)
#define LCD_CLK_PIN	(15)


//#define  LCD_CS_1();      (	SPI0drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE));   
//#define  LCD_CS_0();      (	SPI0drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE));
#define  LCD_CS_1();      (	GPIO_PinWrite(LCD_CS_PORT, LCD_CS_PIN, 1));   
#define  LCD_CS_0();      (	GPIO_PinWrite(LCD_CS_PORT, LCD_CS_PIN, 0));

#define  LCD_MOSI_1();      (	GPIO_PinWrite(LCD_MOSI_PORT, LCD_MOSI_PIN, 1));   
#define  LCD_MOSI_0();      (	GPIO_PinWrite(LCD_MOSI_PORT, LCD_MOSI_PIN, 0));

#define  LCD_CLK_1();      (	GPIO_PinWrite(LCD_CLK_PORT, LCD_CLK_PIN, 1));   
#define  LCD_CLK_0();      (	GPIO_PinWrite(LCD_CLK_PORT, LCD_CLK_PIN, 0));

#define  LCD_DC_1();      (GPIO_PinWrite(LCD_DC_PORT, LCD_DC_PIN, 1));   
#define  LCD_DC_0();      (GPIO_PinWrite(LCD_DC_PORT, LCD_DC_PIN, 0));
void OLED_Init(void) ;                             //初始化
void LCD_Fill(uchar bmp_dat);				      //全屏点亮
void LCD_P6x8Str(uchar x,uchar y,uchar ch[]);    //ASIIC
void LCD_shuzi(uchar x,uchar y,uchar ch);	    //显示数字
#endif
