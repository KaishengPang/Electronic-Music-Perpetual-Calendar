#ifndef __BLUETOOTH_H_
#define __BLUETOOTH_H_

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


//---定义全局函数---//
void Uart_Init_blue(void);
void PostChar(uchar character);
//---加入全局变量--//
extern uchar weather;

#endif