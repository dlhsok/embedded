/**********************************************************

 file: IIC.H

**********************************************************/
#ifndef  __IIC_H__             //条件汇编指令
#define  __IIC_H__             //避免重复调用

#define  I2C_ACK     1
#define  I2C_NACK    0
	
extern  void  I2C_start();
extern  void  I2C_stop();
//extern  void  iic_init();
//extern  void  iic_ack();
//extern  void  iic_noack();
//extern  bit   iic_testack();
extern  unsigned char  I2C_read_byte(unsigned char ack);
extern  unsigned char  I2C_write_byte(unsigned char indata);

#endif                      //条件汇编结束

/**********************************************************/