#ifndef __JQ8900_H_
#define __JQ8900_H_

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
void SendData(uint addr);
void Musicselect(void);

//---加入全局变量--//
extern uchar song;
#endif