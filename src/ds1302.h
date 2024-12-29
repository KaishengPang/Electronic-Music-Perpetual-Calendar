#ifndef __DS1302_H_
#define __DS1302_H_

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

//---定义ds1302使用的IO口---//

//---定义全局函数---//
void Ds1302Write(uchar addr, uchar dat);
uchar Ds1302Read(uchar addr);
void Ds1302Init();
void Ds1302ReadTime();
void dataProcess();
void dataChange(void);
void setClock(void);
uchar alarmDetect(void);
void blue2time(uchar flag, uchar pos);
void blue2clocktime(uchar flag, uchar pos);
//---加入全局变量--//
extern uchar TIME[7];	//加入全局变量
extern uchar displayData[16];
extern uchar displayData2[16];
extern uchar buffer[7];
extern uchar clocktime[7];

#endif