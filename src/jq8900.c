#include "jq8900.h"

sbit sda = P1 ^ 5;

void Display_String(uchar line, uchar *string);
uchar key_scan();
uchar song;

// 延时子函数
void Delay1500us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 17;
	j = 31;
	do
	{
		while (--j);
	} while (--i);
}

// 延时子函数
void Delay500us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 6;
	j = 93;
	do
	{
		while (--j);
	} while (--i);
}

// 延时子函数
void Delay2ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 22;
	j = 128;
	do
	{
		while (--j);
	} while (--i);
}

// 延时子函数
void Delay4ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 44;
	j = 4;
	do
	{
		while (--j);
	} while (--i);
}

// 以一线通信向jq8900发送指令
// 常见指令 0x11：重复播放
// 0x13：结束播放 0x0a：清除数字 0x0b：选曲确认
void SendData(uint addr) // 发 送 函 数
{
		int i = 0;
		EA = 0; //发 送 时 关 掉 中 断 ， 防 止 中 断 影 响 时 序 
		sda = 1;//开 始 拉 高
		Delay2ms();
		sda = 0; //开 始 引 导 码 
		Delay4ms(); //此 处 延 时 最 少 要 大 于 2 m s ， 此参数延 时 为 3 1 0 m s 
		for( i=0;i<8;i++) //总 共 8 位 数 据 
		{
				sda = 1;
				if(addr & 0x01) //3 : 1 表 示 数 据 位 1 , 每 个位 用 两 个 脉 冲 表 示 
				{
						Delay1500us();
						sda = 0 ;
						Delay500us();
				}
				else //1 ： 3 表 示 数 据 位 0 , 每个 位 用 两 个 脉 冲 表 示 
				{
						Delay500us();
						sda = 0;
						Delay1500us();
				}
				addr >>= 1;
		}
		sda = 1 ;
		EA = 1 ; //恢 复 中 断
}


// 选歌子函数，共六首歌曲，使用jq8900存储卡版本存储在sd卡内,以0000x.mp4命名
// 滚动屏幕显示，通过全局变量song存储歌曲序号，通过senddata（song）选曲播放。
void Musicselect(void)
{
	uchar flag = 1, key;
	while(1)
	{
		Display_String(1,"选择音乐  K3确认");
		if (flag == 1)
		{
			Display_String(2,"1.拯救         *");
			Display_String(3,"2.Whataya Want. ");
			Display_String(4,"3.Wrecking Ball ");
		}
		else if (flag == 2)
		{
			Display_String(2,"1.拯救          ");
			Display_String(3,"2.Whataya Want.*");
			Display_String(4,"3.Wrecking Ball ");
		}
		else if (flag == 3)
		{
			Display_String(2,"1.拯救          ");
			Display_String(3,"2.Whataya Want. ");
			Display_String(4,"3.Wrecking Ball*");
		}
		else if (flag == 4)
		{
			Display_String(2,"2.Whataya Want. ");
			Display_String(3,"3.Wrecking Ball ");
			Display_String(4,"4.思念         *");
		}
		else if (flag == 5)
		{
			Display_String(2,"3.Wrecking Ball ");
			Display_String(3,"4.思念          ");
			Display_String(4,"5.雨天雨天     *");
		}
		else if (flag == 6)
		{
			Display_String(2,"4.思念          ");
			Display_String(3,"5.雨天雨天      ");
			Display_String(4,"6.印第安老斑鸠 *");
		}
		
		key = key_scan();
		if (key == 2)
		{
			flag++;
			if(flag == 7)flag = 1;
		}
		else if (key == 3)
		{
			song = flag;
			break;
		}
	}
}

