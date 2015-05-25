/**********************************************************

File: lcd.h

Compiler:  gguoqing

**********************************************************/
#ifndef  __lcd_H__             //条件汇编指令
#define  __lcd_H__             //避免重复调用

#define uchar unsigned char
#define uint  unsigned int

extern  bit lcd_busy();
extern  void lcd_wcmd(uchar cmd,bit Check);
extern  void lcd_wdat(uchar dat);
extern  void lcd_init();
extern  void lcd_pos_xy(uchar pos_x,uchar pos_y);
extern  void lcd_w_string(uchar pos_x,uchar pos_y,uchar code *str,uint times);


#endif                      //条件汇编结束

/**********************************************************/
