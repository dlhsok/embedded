#include"all.h"

void UartSend(unsigned long c)
{
	UARTCharPut(UART0_BASE,c);				//将字符发送到输出缓存区
}
