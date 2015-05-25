
/**********************************************************

 file: delay.h

**********************************************************/
#ifndef  __delay_h__             //条件汇编指令
#define  __delay_h__             //避免重复调用

#include <intrins.h>
	
#define  NOP  _nop_()

extern  void delayNOP();
extern  void delayms( unsigned int ms); 

#endif                           //条件汇编结束

/**********************************************************/
