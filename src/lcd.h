#ifndef __LCD_H_
#define __LCD_H_

//---包含头文件---//
#include<reg52.h>
#include<intrins.h>

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

//---定义jq8900使用的IO口---//

 
//---定义全局函数---//
void Display_String(uchar line, uchar *string);
void Lcm_Init(void);
void Wr_Command(uchar wrcommand, busyc);
void Wr_Data(uchar wrdata);
void RDbf(void);
void Delay5Ms(void);
void Switch_bit(uchar InChar);
//---加入全局变量--//

#endif