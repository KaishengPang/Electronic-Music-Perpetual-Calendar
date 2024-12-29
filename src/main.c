# include <reg52.h>
# include "jq8900.h"
# include "lcd.h"
# include "ds1302.h"
# include "bluetooth.h"
sbit MQ2 = P1 ^ 1;

uchar key_scan();
void callMenu();
void Delay5Ms();
uchar* DHT11_receive();
void Delay30us();
void getweather();
void control();

//农历年相关驱动
void turn_lunar_calendar(uint year, uchar month, uchar day, uchar *lunar);
uchar judgeSolar(uint year,uchar month,uchar day);
uchar calculateWeekDay(uint year, uchar month, uchar day);

// 处理农历年的一些字符数组
uchar code TianGan[20] = "甲乙丙丁戊己  辛壬癸";
uchar code DiZhi[24] = "子丑寅卯辰巳午未申酉戌亥";
uchar code month_lunar[24] = "正二叁四五六七八九十冬腊";
uchar code first_day[6] = "初十廿";
uchar code second_day[20] = "一二叁四五六七八九十";
uchar code Solarterms[96] = "小寒大寒立春雨水惊蛰春分清明谷雨立夏小满芒种夏至小暑大暑立秋处暑白露秋分寒露霜降立冬小雪大雪冬至";
uchar code WeekDays[14] = "一二叁四五六日";

void main()
{
	// 初始化蓝牙
	Uart_Init_blue();
	// 初始化屏幕
	Lcm_Init();
	// 初始化时钟
	Ds1302Init();
	
	while(1)
		{
			// 变量声明
			uchar key;// 用来存储键值
			uchar clockvo[16] = "                ";// 用来暂存按下sw4出现的16位闹钟时间字符串
			
			// 获得displaydata1和2，用displaydata在第一行第二行显示
			dataProcess();
			Display_String(1, displayData);
			Display_String(2, displayData2);
			
			// dht11显示
			Display_String(3, DHT11_receive());
			
			// 占位
			
			Display_String(4,"KS CALENDAR 2024");
			
			// 按键检测
			key = key_scan();
			
			if (key == 1)
			{
				// 召唤菜单
				callMenu();
			}
			else if (key == 2)
			{
				SendData(0x0a);
				SendData(0x01);
				SendData(0x05);
				SendData(0x0c);
				SendData(0x0a);
				SendData(song);
				SendData(0x0b);
			}
			else if (key == 3)
			{
				SendData(0x0a);
				SendData(0x13);
				SendData(0x0b);
			}
			else if (key == 4)
			{
				
				Lcm_Init();
				while(1)
				{
					uchar key;
					clockvo[0] = clocktime[2] / 16 + '0';
					clockvo[1] = clocktime[2] % 16 + '0';
					clockvo[2] = ':';
					clockvo[3] = clocktime[1] / 16 + '0';
					clockvo[4] = clocktime[1] % 16 + '0';
					clockvo[5] = ':';
					clockvo[6] = clocktime[0] / 16 + '0';
					clockvo[7] = clocktime[0] % 16 + '0';
					Display_String(1, "闹钟时间:       ");
					Display_String(2, clockvo);
					Display_String(4, "按下 K4 返回主页");
					key = key_scan();
					if (key == 4)
					{
						break;
					}
				}
			}
						////闹钟时间检测
			
			if(alarmDetect())
			{
				SendData(0x0a);
				SendData(0x01);
				SendData(0x05);
				SendData(0x0c);//提高音量
				SendData(0x0a);
				SendData(song);
				SendData(0x0b);
				while(1)
					{
						Display_String(1, "    闹钟提醒    ");
						Display_String(2, "!!!!!!!!!!!!!!!!");
						Display_String(3, "!!!!!!!!!!!!!!!!");
						Display_String(4, "按下 K4 返回主页");
						key = key_scan();
						if(key == 4)
						{
							SendData(0x0a);
							SendData(0x13);
							SendData(0x0b);
							break;
						}
					}
			}
						if(MQ2 == 0)
			{
				Delay5Ms();
				if(MQ2 == 0)
				{
					SendData(0x0a);
					SendData(0x07);
					SendData(0x0b);
					PostChar(0x01);
					Lcm_Init();
					while(1)
					{
						Display_String(1, "  警报!!警报!!  ");
						Display_String(2, "  疑似火灾!!    ");
						Display_String(3, "  按下SW1 键继续");
						key = key_scan();
						if(key == 1)
						{
							SendData(0x0a);
							SendData(0x13);
							SendData(0x0b);
							break;
						}
					}
				}
			}
		}
}
void Basic(void)
{
	uchar key, i;
	uint year_t;
	uchar month_t, day_t;
	uchar TianGan_t, DiZhi_t;
	uchar lunar[2];
	uchar lunarinfo[16] = "                ";
	uchar month[2] = "月";
	uchar result;
	uchar weekday;
	uchar weekinfo[16] = "                ";
  uchar week[2] = "周";
	uchar threee[2] = "叁";
	uchar ten[2] = "十";
  Lcm_Init();
	
	while(1)
	{
		//dataprocess 用于读取TIME和生成displaydata
		dataProcess();
		year_t = 2000 + TIME[6] / 16 * 10 + TIME[6] % 16;
		month_t = TIME[4] / 16 * 10 + TIME[4] % 16;
		day_t = TIME[3] / 16 * 10 + TIME[3] % 16;
		
		TianGan_t = (year_t - 3) % 10;
		DiZhi_t = (year_t - 3) % 12;
		
		if(TianGan_t==0)TianGan_t=10;
		if(DiZhi_t==0)DiZhi_t=12;
		
		turn_lunar_calendar(year_t, month_t, day_t, lunar);
		result = judgeSolar(year_t, month_t, day_t);
		weekday = calculateWeekDay(year_t, month_t, day_t);
        for (i=0; i<2; i++) 
        {
            weekinfo[i] = week[i];
            weekinfo[i+2] = WeekDays[2*(weekday-1)+2+i];
        }

		for (i=0; i<2; i++)
		{
			lunarinfo[i] = TianGan[2 * (TianGan_t - 1) + i];//天干
			lunarinfo[i + 2] = DiZhi[2 * (DiZhi_t - 1) + i];//地支
			lunarinfo[i + 4] = month_lunar[2 * (lunar[0] - 1) + i-1];//月数
			lunarinfo[i + 6] = month[i];//“月”
			if(lunar[1] <= 10)
			{
				lunarinfo[i + 8] = first_day[i];//初十廿
				lunarinfo[i + 10] = second_day[2 * (lunar[1] - 1) + i];//1-10
			}
			else if(lunar[1] <= 19)
			{
				lunarinfo[i + 8] = first_day[i + 2];//初十廿
				lunarinfo[i + 10] = second_day[2 * (lunar[1] - 10 - 1) + i];//1-10
			}
			else if(lunar[1] <= 29)
			{
				lunarinfo[i + 8] = first_day[i + 4];//初十廿
				lunarinfo[i + 10] = second_day[2 * (lunar[1] - 20 - 1) + i];//1-10
			}		
			else if(lunar[1] == 30)
			{
				lunarinfo[i + 8] = threee[i];//叁十
				lunarinfo[i + 10] = ten[i];
			}			
		}
		if(result)
		{
			lunarinfo[12] = Solarterms[4 * result];
			lunarinfo[13] = Solarterms[4 * result + 1];
			lunarinfo[14] = Solarterms[4 * result + 2];
			lunarinfo[15] = Solarterms[4 * result + 3];
		}
		else
		{
			lunarinfo[12] = ' ';
			lunarinfo[13] = ' ';
			lunarinfo[14] = ' ';
			lunarinfo[15] = ' ';
		}
		
		Display_String(1, displayData);
		Display_String(2, displayData2);
		Display_String(3, lunarinfo);
		Display_String(4, weekinfo);
		
		key = key_scan();
		if(key == 4)
		{
			return;
		}
	}
	
}

uchar calculateWeekDay(uint year, uchar month, uchar day)
{
    uint century;
    uint yearInCentury;
    uchar weekday;
    

    if (month == 1 || month == 2)
    {
        month += 12;
        year--;
    }
    
    century = year / 100;
    yearInCentury = year % 100;
    

    weekday = (day + 2 * month + 3 * (month + 1) / 5 + yearInCentury + yearInCentury / 4 + century / 4 - 2 * century) % 7;
    

    if (weekday < 0) 
    {
        weekday += 7;
    }
    
    return weekday;
}



void callMenu()
{
	uchar flag = 1;
	uchar test = 0x01;
	uchar key0;
	Lcm_Init();
	while(1)
	{
		Delay30us();
		key0 = key_scan();
		if (key0 == 1)
		{
			if (flag == 1)
			{
			}
			else if (flag == 2)
			{
				Basic();
			}
			else if (flag == 3)
			{
				dataChange();
			}
			else if (flag == 4)
			{
				setClock();
			}
			else if (flag == 5)
			{
				Musicselect();
			}
			else if (flag == 6)
			{
				getweather();
			}
			else if (flag == 7)
			{
				control();
			}
			else if (flag == 8)
			{
				return;
			}
			else
			{
			}
		}
		else if (key0 == 2)
		{
			flag++;
			if (flag == 9)flag = 1;
		}
		else if (key0 == 3)
		{
			break;
		}
		if (flag == 1)
		{
			Display_String(1, "K2选中    K1进入");
			Display_String(2, "1.基础显示      ");
			Display_String(3, "2.修改时间      ");
			Display_String(4, "3.设置闹钟      ");
		}
		else if (flag == 2)
		{
			Display_String(1, "K2选中    K1进入");
			Display_String(2, "1.基础显示    选");
			Display_String(3, "2.修改时间      ");
			Display_String(4, "3.设置闹钟      ");
		}
		else if (flag == 3)
		{
			Display_String(1, "K2选中    K1进入");
			Display_String(2, "1.基础显示      ");
			Display_String(3, "2.修改时间    选");
			Display_String(4, "3.设置闹钟      ");
		}
		else if (flag == 4)
		{
			Display_String(1, "K2选中    K1进入");
			Display_String(2, "1.基础显示      ");
			Display_String(3, "2.修改时间      ");
			Display_String(4, "3.设置闹钟    选");
		}
		else if (flag == 5)
		{
			Display_String(1, "1.基础显示      ");
			Display_String(2, "2.修改时间      ");
			Display_String(3, "3.设置闹钟      ");
			Display_String(4, "4.设置铃声    选");
		}
		else if (flag == 6)
		{
			Display_String(1, "2.修改时间      ");
			Display_String(2, "3.设置闹钟      ");
			Display_String(3, "4.设置铃声      ");
			Display_String(4, "5.获取天气    选");
		}
		else if (flag == 7)
		{
			Display_String(1, "3.设置闹钟      ");
			Display_String(2, "4.设置铃声      ");
			Display_String(3, "5.获取天气      ");
			Display_String(4, "6.事项提醒    选");
		}
		else if (flag == 8)
		{
			Display_String(1, "4.设置铃声      ");
			Display_String(2, "5.获取天气      ");
			Display_String(3, "6.事项提醒      ");
			Display_String(4, "7.返回        选");
		}
	}
}

void getweather()
{
	uchar key;
	Lcm_Init();
	while(1)
	{
		Display_String(1, "请连接手机App   ");
		if(weather == 25)
		{
			Display_String(3, "晴朗天气        ");
		}
		else if(weather == 26)
		{
			Display_String(3, "多云天气        ");
		}
		else if(weather == 27)
		{
			Display_String(3, "少云天气        ");
		}
		else if(weather == 28)
		{
			Display_String(3, "晴间多云        ");
		}
		else if(weather == 29)
		{
			Display_String(3, "阴天  南风      ");
		}
		else if(weather == 30)
		{
			Display_String(3, "阵雨            ");
		}
		else if(weather == 31)
		{
			Display_String(3, "强阵雨          ");
		}
		else if(weather == 32)
		{
			Display_String(3, "雷阵雨          ");
		}
		else if(weather == 33)
		{
			Display_String(3, "强雷阵雨        ");
		}
		else if(weather == 34)
		{
			Display_String(3, "小雨            ");
		}
		else if(weather == 35)
		{
			Display_String(3, "中雨            ");
		}
		else if(weather == 36)
		{
			Display_String(3, "大雨            ");
		}
		else if(weather == 37)
		{
			Display_String(3, "暴雨            ");
		}
		else if(weather == 38)
		{
			Display_String(3, "大暴雨          ");
		}
		else if(weather == 39)
		{
			Display_String(3, "  雨            ");
		}
		else if(weather == 50)
		{
			Display_String(3, "  生            ");
		}
		else
		{
		}
		key = key_scan();
		if(key == 4)
		{
			return;
		}
	}
}

void control()
{
	uchar key;
	Lcm_Init();
	while(1)
	{
		Display_String(1, "    事项提醒    ");
		if(weather == 50)
		{
		Display_String(3, "    生日提醒    ");
		}
		else if(weather == 51)
		{
		Display_String(3, "    会议提醒    ");
		}
		else if(weather == 52)
		{
		Display_String(3, "  作业截止提醒  ");
		}
		else if(weather == 53)
		{
		Display_String(3, "    实验提醒    ");
		}
		key = key_scan();
		if(key == 4)
		{
			return;
		}
	}
}
