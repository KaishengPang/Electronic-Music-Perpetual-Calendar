# include <reg52.h>
#include "ds1302.h"
#include "bluetooth.h"

void Delay5000ms();
void Delay500ms();
uchar weather;
/*****************************************************************
*接收中断服务子程序
*****************************************************************/
void Uart_Isr(void)  interrupt 4
{
	uchar Recv;
	if(RI==1)
	{
		RI=0;
		Recv = SBUF;
		switch(Recv)
		{
			//收到对应的数据对年月日进行处理
			case 1:blue2time(1, 6);break;
			case 2:blue2time(0, 6);break;
			
			case 3:blue2time(1, 4);break;
			case 4:blue2time(0, 4);break;
			
			case 5:blue2time(1, 3);break;
			case 6:blue2time(0, 3);break;
			
			case 7:blue2time(1, 2);break;
			case 8:blue2time(0, 2);break;
			
			case 9:blue2time(1, 1);break;
			case 10:blue2time(0, 1);break;
			
			case 11:blue2time(1, 0);break;
			case 12:blue2time(0, 0);break;
			
			//闹钟设置的一部分
			case 13:blue2clocktime(1, 6);break;
			case 14:blue2clocktime(0, 6);break;
			
			case 15:blue2clocktime(1, 4);break;
			case 16:blue2clocktime(0, 4);break;
			
			case 17:blue2clocktime(1, 3);break;
			case 18:blue2clocktime(0, 3);break;
			
			case 19:blue2clocktime(1, 2);break;
			case 20:blue2clocktime(0, 2);break;
			
			case 21:blue2clocktime(1, 1);break;
			case 22:blue2clocktime(0, 1);break;
			
			case 23:blue2clocktime(1, 0);break;
			case 24:blue2clocktime(0, 0);break;

		}
		if(Recv >= 25) weather = Recv;
		}
}
/*****************************************************************
*串口初始化
*****************************************************************/
void Uart_Init_blue(void)
{
  SCON = 0x50;         //方式1 8位数据 接收地址甄别禁止 接收使能 0101 0000
	PCON = 0x00;         //波特率不增倍
	TMOD = 0x20;         //T1,8位自动重装
	TH1 = 0xfd;	  				//波特率9600
	TL1 = 0xfd;
	EA   = 1;
	ES   = 1;
	TR1 = 1;            //启动t1
}
// 单片机蓝牙向手机蓝牙发数据。仅用于报警发短信
void PostChar(uchar character)
{
	SBUF = character;
	while(!TI);
	TI = 0;
}
