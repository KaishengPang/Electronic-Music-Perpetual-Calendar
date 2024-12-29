#include"ds1302.h"
#include"lcd.h"
sbit DSIO=P1^3;
sbit RST=P1^4;
sbit SCLK=P1^2;

//---DS1302写入和读取时分秒的地址命令---//
//---秒分时日月周年 最低位读写位;-------//
uchar code READ_RTC_ADDR[7] = {0x81, 0x83, 0x85, 0x87, 0x89, 0x8b, 0x8d};
uchar code WRITE_RTC_ADDR[7] = {0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c};

//---DS1302时钟初始化2024年7月4日15点00分00秒。---//
//---存储顺序是秒分时日月周年,存储格式是用BCD码---//
uchar TIME[7] = {0x00, 0x00, 0x17, 0x04, 0x07, 0x00, 0x24};
//                0秒   1分  2时   3日   4月   5周   6年
uchar displayData[16];//在12864显示的第一行
uchar displayData2[16];//在12864显示的第二行
uchar time1[16];
uchar time2[16];
uchar buffer[7];
uchar clocktime[7];
/*******************************************************************************
* 函 数 名         : Ds1302Write
* 函数功能		   : 向DS1302命令（地址+数据）
* 输    入         : addr,dat
* 输    出         : 无
*******************************************************************************/
uchar key_scan();
void Display_String(uchar line, uchar *string);
void Delay5Ms(void);
void Add(uchar dateBCD, uchar symbol);

void Ds1302Write(uchar addr, uchar dat)
{
	uchar n;
	RST = 0;
	_nop_();

	SCLK = 0;//先将SCLK置低电平。
	_nop_();
	RST = 1; //然后将RST(CE)置高电平。
	_nop_();

	for (n=0; n<8; n++)//开始传送八位地址命令
	{
		DSIO = addr & 0x01;//数据从低位开始传送
		addr >>= 1;
		SCLK = 1;//数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();
	}
	for (n=0; n<8; n++)//写入8位数据
	{
		DSIO = dat & 0x01;
		dat >>= 1;
		SCLK = 1;//数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;
		_nop_();
	}	
		 
	RST = 0;//传送数据结束
	_nop_();
}

/*******************************************************************************
* 函 数 名         : Ds1302Read
* 函数功能		   : 读取一个地址的数据
* 输    入         : addr
* 输    出         : dat
*******************************************************************************/

uchar Ds1302Read(uchar addr)
{
	uchar n,dat,dat1;
	RST = 0;
	_nop_();

	SCLK = 0;//先将SCLK置低电平。
	_nop_();
	RST = 1;//然后将RST(CE)置高电平。
	_nop_();

	for(n=0; n<8; n++)//开始传送八位地址命令
	{
		DSIO = addr & 0x01;//数据从低位开始传送
		addr >>= 1;
		SCLK = 1;//数据在上升沿时，DS1302读取数据
		_nop_();
		SCLK = 0;//DS1302下降沿时，放置数据
		_nop_();
	}
	_nop_();
	for(n=0; n<8; n++)//读取8位数据
	{
		dat1 = DSIO;//从最低位开始接收
		dat = (dat>>1) | (dat1<<7);
		SCLK = 1;
		_nop_();
		SCLK = 0;//DS1302下降沿时，放置数据
		_nop_();
	}

	RST = 0;
	_nop_();	//以下为DS1302复位的稳定时间,必须的。
	SCLK = 1;
	_nop_();
	DSIO = 0;
	_nop_();
	DSIO = 1;
	_nop_();
	return dat;	
}

/*******************************************************************************
* 函 数 名         : Ds1302Init
* 函数功能		   : 初始化DS1302.
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Ds1302Init()
{
	uchar n;
	Ds1302Write(0x8E,0X00);		 //禁止写保护，就是关闭写保护功能
	for (n=0; n<7; n++)//写入7个字节的时钟信号：分秒时日月周年
	{
		Ds1302Write(WRITE_RTC_ADDR[n], TIME[n]);	
	}
	Ds1302Write(0x8E,0x80);		 //打开写保护功能
}

/*******************************************************************************
* 函 数 名         : Ds1302ReadTime
* 函数功能		   : 读取时钟信息
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void Ds1302ReadTime()
{
	uchar n;
	for (n=0; n<7; n++)//读取7个字节的时钟信号：分秒时日月周年
	{
		TIME[n] = Ds1302Read(READ_RTC_ADDR[n]);
	}
		
}
//将数据转化为可显示在12864上的字符串
//void dataProcess(void)
void dataProcess(void)
{
	uchar i;
	Ds1302ReadTime();
	//年的第1位
	displayData[0] = '2';
	//年的第2位
	displayData[1] = '0';
	//年的第3位
	displayData[2] = TIME[6] / 16 + '0';
	//年的第4位
	displayData[3] = TIME[6] % 16 + '0';
	
	displayData[4] = '-';
	//月 首位
	displayData[5] = TIME[4] / 16 + '0';
	//月 尾位
	displayData[6] = TIME[4] % 16 + '0';
	
	displayData[7] = '-';
	//日 首位
	displayData[8] = TIME[3] / 16 + '0';
	//日 尾位
	displayData[9] = TIME[3] % 16 + '0';
	
	for (i=10; i<=15; i++)
	{
		displayData[i] = ' ';
	}
	///////////////////
	////换行
	///////////////////
	
	//小时 首位
	displayData2[0] = TIME[2] / 16 + '0';
	//小时 尾位
	displayData2[1] = TIME[2] % 16 + '0';
	
	displayData2[2] = ':';
	//分钟 首位
	displayData2[3] = TIME[1] / 16 + '0';
	//分钟 尾位
	displayData2[4] = TIME[1] % 16 + '0';
	
	displayData2[5] = ':';
	//秒 首位
	displayData2[6] = TIME[0] / 16 + '0';
	//秒 尾位
	displayData2[7] = TIME[0] % 16 + '0';
	// 分隔
	displayData2[8] = ' ';
	
	if (displayData2[0] == '0')
	{
		displayData2[9] = 'A';
	}
	else if (displayData2[0] == '1')
	{
		if (displayData2[1] >= '2')
		{
			displayData2[9] = 'P';
		}
		else
		{
			displayData2[9] = 'A';
		}
	}
	else
	{
		displayData2[9] = 'P';
	}
	

	displayData2[10] = 'M';
	for (i=11; i<=15; i++)
	{
		displayData2[i] = ' ';
	}
}


void dataProcess_new(void)
{
	uchar i;
	//年的第1位
	time1[0] = '2';
	//年的第2位
	time1[1] = '0';
	//年的第3位
	time1[2] = buffer[6] / 16 + '0';
	//年的第4位
	time1[3] = buffer[6] % 16 + '0';
	
	time1[4] = '-';
	//月 首位
	time1[5] = buffer[4] / 16 + '0';
	//月 尾位
	time1[6] = buffer[4] % 16 + '0';
	
	time1[7] = '-';
	//日 首位
	time1[8] = buffer[3] / 16 + '0';
	//日 尾位
	time1[9] = buffer[3] % 16 + '0';
	
	for (i=10; i<=15; i++)
	{
		time1[i] = ' ';
	}
	///////////////////
	////换行
	///////////////////
	
	//小时 首位
	time2[0] = buffer[2] / 16 + '0';
	//小时 尾位
	time2[1] = buffer[2] % 16 + '0';
	
	time2[2] = ':';
	//分钟 首位
	time2[3] = buffer[1] / 16 + '0';
	//分钟 尾位
	time2[4] = buffer[1] % 16 + '0';
	
	time2[5] = ':';
	//秒 首位
	time2[6] = buffer[0] / 16 + '0';
	//秒 尾位
	time2[7] = buffer[0] % 16 + '0';
	// 分隔
	time2[8] = ' ';
	
	if (time2[0] == '0')
	{
		time2[9] = 'A';
	}
	else if (time2[0] == '1')
	{
		if (time2[1] >= '2')
		{
			time2[9] = 'P';
		}
		else
		{
			time2[9] = 'A';
		}
	}
	else
	{
		time2[9] = 'P';
	}
	

	time2[10] = 'M';
	for (i=11; i<=15; i++)
	{
		time2[i] = ' ';
	}
	
}


void dataChange(void)
{
	uchar n, key, flag = 0;
//	uchar buffer[7] = {0x00, 0x00, 0x15, 0x06, 0x06, 0x00, 0x22};
//                      0秒   1分   2时   3日   4月    5周   6年	
	//time1：年 月 日 显示形式
	//time2: 时 分 秒 显示形式
	//读取7个字节的时钟信号：分秒时日月周年
	Ds1302ReadTime();
	for (n=0; n<7; n++)
	{
		buffer[n] = TIME[n];
	}
	Lcm_Init();
	while(1)
	{
		if (flag == 0)
		{
			Display_String(1, "修改--年        ");
		}
		else if(flag == 1)
		{
			Display_String(1, "修改--月        ");
		}
		else if(flag == 2)
		{
			Display_String(1, "修改--日        ");
		}
		else if(flag == 3)
		{
			Display_String(1, "修改--时        ");
		}
		else if(flag == 4)
		{
			Display_String(1, "修改--分        ");
		}
		else if(flag == 5)
		{
			Display_String(1, "修改--秒        ");
		}
		
		dataProcess_new();
		Display_String(2, time1);
		Display_String(3, time2);
		Display_String(4, "1Flag2Add3Bub4RE");
		
		key = key_scan();
		// 修改flag
		if (key == 1){
			flag ++;
			if (flag == 6)flag = 0;
		}
		else if (key == 2){
			uchar date, n;
			if (flag == 0)
			{
				n = 6;
			}
			else if (flag == 1)
			{
				n = 4;
			}
			else if (flag == 2)
			{
				n = 3;
			}
			else if (flag == 3)
			{
				n = 2;
			}
			else if (flag == 4)
			{
				n = 1;
			}
			else if (flag == 5)
			{
				n = 0;
			}
			date = buffer[n] + 1;
			buffer[n] = date;
		}
		else if (key == 3){
			uchar date, n;
			if (flag == 0)
			{
				n = 6;
			}
			else if (flag == 1)
			{
				n = 4;
			}
			else if (flag == 2)
			{
				n = 3;
			}
			else if (flag == 3)
			{
				n = 2;
			}
			else if (flag == 4)
			{
				n = 1;
			}
			else if (flag == 5)
			{
				n = 0;
			}
			date = buffer[n] - 1;
			buffer[n] = date;
		}
		else if (key == 4){break;}
	}

	Ds1302Write(0x8E,0X00);//禁止写保护，就是关闭写保护功能
	for (n=0; n<7; n++)//写入7个字节的时钟信号：分秒时日月周年
	{
		Ds1302Write(WRITE_RTC_ADDR[n], buffer[n]);
	}
	Ds1302Write(0x8E,0x80);//打开写保护功能
}

void setClock(void)
{
	uchar n, key, flag = 0, i;
//	uchar buffer[7] = {0x00, 0x00, 0x15, 0x06, 0x06, 0x00, 0x22};
//                      0秒   1分   2时   3日   4月    5周   6年	
	//time1：年 月 日 显示形式
	//time2: 时 分 秒 显示形式
	//读取7个字节的时钟信号：分秒时日月周年
	Ds1302ReadTime();
	for (n=0; n<7; n++)
	{
		buffer[n] = TIME[n];
	}
	Lcm_Init();
	while(1)
	{
		if (flag == 0)
		{
			Display_String(1, "闹钟修改--年    ");
		}
		else if(flag == 1)
		{
			Display_String(1, "闹钟修改--月    ");
		}
		else if(flag == 2)
		{
			Display_String(1, "闹钟修改--日    ");
		}
		else if(flag == 3)
		{
			Display_String(1, "闹钟修改--时    ");
		}
		else if(flag == 4)
		{
			Display_String(1, "闹钟修改--分    ");
		}
		else if(flag == 5)
		{
			Display_String(1, "闹钟修改--秒    ");
		}
		
		dataProcess_new();
		Display_String(2, time1);
		Display_String(3, time2);
		
		key = key_scan();
		// 修改flag
		if (key == 1){
			flag ++;
			if (flag == 6)flag = 0;
		}
		else if (key == 2){
			uchar date, n;
			if (flag == 0)
			{
				n = 6;
			}
			else if (flag == 1)
			{
				n = 4;
			}
			else if (flag == 2)
			{
				n = 3;
			}
			else if (flag == 3)
			{
				n = 2;
			}
			else if (flag == 4)
			{
				n = 1;
			}
			else if (flag == 5)
			{
				n = 0;
			}
			date = buffer[n] + 1;
			buffer[n] = date;
		}
		else if (key == 3){
			uchar date, n;
			if (flag == 0)
			{
				n = 6;
			}
			else if (flag == 1)
			{
				n = 4;
			}
			else if (flag == 2)
			{
				n = 3;
			}
			else if (flag == 3)
			{
				n = 2;
			}
			else if (flag == 4)
			{
				n = 1;
			}
			else if (flag == 5)
			{
				n = 0;
			}
			date = buffer[n] - 1;
			buffer[n] = date;
		}
		else if (key == 4){break;}
	}
	
	for (i = 0; i < 8; i++)
	{
		clocktime[i] = buffer[i];
	}
}

uchar alarmDetect(void)
{
	uchar i, Alarm = 0;
	Ds1302ReadTime();
	for (i = 0; i < 3; i++)
	{
		if (clocktime[i] == TIME[i])Alarm++;
	}
	if(Alarm == 3)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
void blue2time(uchar flag, uchar pos)
{
		uchar i, n, a;
	  Ds1302ReadTime();
		for(i=0;i<8;i++)
		{
			buffer[i] = TIME[i];
		}
		a = buffer[pos] + 2 * flag - 1;

		buffer[pos] = a;
		Ds1302Write(0x8E,0X00);//禁止写保护，就是关闭写保护功能
		for (n=0; n<7; n++)//写入7个字节的时钟信号：分秒时日月周年
		{
			Ds1302Write(WRITE_RTC_ADDR[n], buffer[n]);
		}
		Ds1302Write(0x8E,0x80);//打开写保护功能
}
void blue2clocktime(uchar flag, uchar pos)
{
	uchar buffer;
	buffer = clocktime[pos] + 2 * flag - 1;
	clocktime[pos] = buffer;
}