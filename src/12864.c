# include "lcd.h"
# include <string.h>
#define LCMdata P0
#define Busy 0x80
#define Busyt 0x01

sbit RS = P2 ^ 0;
sbit RW = P2 ^ 1;
sbit E = P2 ^ 2;
sbit REST = P2 ^ 3;

// bb c switch_bit相互配合实现DB和P0逆序输入输出
// P0.0 - DB7
// P0.1 - DB6
// P0.2 - DB5
// .......
uchar bdata BB;  //在位寻址区定义字符变量B
sbit B7=BB^7;
sbit B6=BB^6;
sbit B5=BB^5;
sbit B4=BB^4;
sbit B3=BB^3;
sbit B2=BB^2;
sbit B1=BB^1;
sbit B0=BB^0;
uchar bdata C;  //在位寻址区定义字符变量C
sbit C7=C^7;
sbit C6=C^6;
sbit C5=C^5;
sbit C4=C^4;
sbit C3=C^3;
sbit C2=C^2;
sbit C1=C^1;
sbit C0=C^0;


void Switch_bit(uchar InChar)
{
		C = InChar;
		B0=C7;
		B1=C6;
		B2=C5;
		B3=C4;
		B4=C3;
		B5=C2;
		B6=C1; 
		B7=C0;
}

// 在line行显示16位字符串，莫要使用strcat等string.h库中的函数拼字符串，51吃不消，极易出现bug
void Display_String(uchar line, uchar *string)
{
    uchar addr, i;
    if (line == 1)
        addr = 0x80;
    else if (line == 2)
        addr = 0x90;
    else if (line == 3)
        addr = 0x88;
    else if (line == 4)
        addr = 0x98;
    Wr_Command(addr, 1);
    for (i = 0; i < 16; i++)
    {
        Wr_Data(*string++);
    }
}

void Wr_Data(uchar wrdata) 
{
    RDbf();
    RS = 1;
    RW = 0;
    E = 1;
	  Switch_bit(wrdata);
    LCMdata = BB;
    E = 0;
}

// when you wanna write on the screen, utilize this fun first
void Wr_Command(uchar wrcommand, busyc)
{
    if (busyc)
        RDbf();
    RS = 0;
    RW = 0;
    E = 1;
		Switch_bit(wrcommand);
    LCMdata = BB;
    E = 0;
}

void RDbf(void)
{
    while (1)
    {
        RS = 0;
        RW = 1;
        E = 0;
        LCMdata = 0xFF;
        E = 1;
        if ((LCMdata & Busyt) == 0)
            break;
    }
}

void Lcm_Init(void)
{
    Delay5Ms();
    REST = 1;
    REST = 0;
    REST = 1;
    Wr_Command(0x30, 0);
    Delay5Ms();
    Wr_Command(0x30, 0); 
    Delay5Ms();
    Wr_Command(0x0C, 1);
    Delay5Ms();
    Wr_Command(0x01, 1);
    Delay5Ms();
    Delay5Ms();
    Wr_Command(0x06, 1);
    Delay5Ms();
}

void Delay5Ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 54;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}

