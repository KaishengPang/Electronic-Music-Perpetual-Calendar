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

//ũ�����������
void turn_lunar_calendar(uint year, uchar month, uchar day, uchar *lunar);
uchar judgeSolar(uint year,uchar month,uchar day);
uchar calculateWeekDay(uint year, uchar month, uchar day);

// ����ũ�����һЩ�ַ�����
uchar code TianGan[20] = "���ұ����켺  ���ɹ�";
uchar code DiZhi[24] = "�ӳ���î������δ�����纥";
uchar code month_lunar[24] = "�������������߰˾�ʮ����";
uchar code first_day[6] = "��ʮإ";
uchar code second_day[20] = "һ�����������߰˾�ʮ";
uchar code Solarterms[96] = "С����������ˮ���ݴ���������������С��â������С��������ﴦ���¶��ֺ�¶˪������Сѩ��ѩ����";
uchar code WeekDays[14] = "һ������������";

void main()
{
	// ��ʼ������
	Uart_Init_blue();
	// ��ʼ����Ļ
	Lcm_Init();
	// ��ʼ��ʱ��
	Ds1302Init();
	
	while(1)
		{
			// ��������
			uchar key;// �����洢��ֵ
			uchar clockvo[16] = "                ";// �����ݴ水��sw4���ֵ�16λ����ʱ���ַ���
			
			// ���displaydata1��2����displaydata�ڵ�һ�еڶ�����ʾ
			dataProcess();
			Display_String(1, displayData);
			Display_String(2, displayData2);
			
			// dht11��ʾ
			Display_String(3, DHT11_receive());
			
			// ռλ
			
			Display_String(4,"KS CALENDAR 2024");
			
			// �������
			key = key_scan();
			
			if (key == 1)
			{
				// �ٻ��˵�
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
					Display_String(1, "����ʱ��:       ");
					Display_String(2, clockvo);
					Display_String(4, "���� K4 ������ҳ");
					key = key_scan();
					if (key == 4)
					{
						break;
					}
				}
			}
						////����ʱ����
			
			if(alarmDetect())
			{
				SendData(0x0a);
				SendData(0x01);
				SendData(0x05);
				SendData(0x0c);//�������
				SendData(0x0a);
				SendData(song);
				SendData(0x0b);
				while(1)
					{
						Display_String(1, "    ��������    ");
						Display_String(2, "!!!!!!!!!!!!!!!!");
						Display_String(3, "!!!!!!!!!!!!!!!!");
						Display_String(4, "���� K4 ������ҳ");
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
						Display_String(1, "  ����!!����!!  ");
						Display_String(2, "  ���ƻ���!!    ");
						Display_String(3, "  ����SW1 ������");
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
	uchar month[2] = "��";
	uchar result;
	uchar weekday;
	uchar weekinfo[16] = "                ";
  uchar week[2] = "��";
	uchar threee[2] = "��";
	uchar ten[2] = "ʮ";
  Lcm_Init();
	
	while(1)
	{
		//dataprocess ���ڶ�ȡTIME������displaydata
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
			lunarinfo[i] = TianGan[2 * (TianGan_t - 1) + i];//���
			lunarinfo[i + 2] = DiZhi[2 * (DiZhi_t - 1) + i];//��֧
			lunarinfo[i + 4] = month_lunar[2 * (lunar[0] - 1) + i-1];//����
			lunarinfo[i + 6] = month[i];//���¡�
			if(lunar[1] <= 10)
			{
				lunarinfo[i + 8] = first_day[i];//��ʮإ
				lunarinfo[i + 10] = second_day[2 * (lunar[1] - 1) + i];//1-10
			}
			else if(lunar[1] <= 19)
			{
				lunarinfo[i + 8] = first_day[i + 2];//��ʮإ
				lunarinfo[i + 10] = second_day[2 * (lunar[1] - 10 - 1) + i];//1-10
			}
			else if(lunar[1] <= 29)
			{
				lunarinfo[i + 8] = first_day[i + 4];//��ʮإ
				lunarinfo[i + 10] = second_day[2 * (lunar[1] - 20 - 1) + i];//1-10
			}		
			else if(lunar[1] == 30)
			{
				lunarinfo[i + 8] = threee[i];//��ʮ
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
			Display_String(1, "K2ѡ��    K1����");
			Display_String(2, "1.������ʾ      ");
			Display_String(3, "2.�޸�ʱ��      ");
			Display_String(4, "3.��������      ");
		}
		else if (flag == 2)
		{
			Display_String(1, "K2ѡ��    K1����");
			Display_String(2, "1.������ʾ    ѡ");
			Display_String(3, "2.�޸�ʱ��      ");
			Display_String(4, "3.��������      ");
		}
		else if (flag == 3)
		{
			Display_String(1, "K2ѡ��    K1����");
			Display_String(2, "1.������ʾ      ");
			Display_String(3, "2.�޸�ʱ��    ѡ");
			Display_String(4, "3.��������      ");
		}
		else if (flag == 4)
		{
			Display_String(1, "K2ѡ��    K1����");
			Display_String(2, "1.������ʾ      ");
			Display_String(3, "2.�޸�ʱ��      ");
			Display_String(4, "3.��������    ѡ");
		}
		else if (flag == 5)
		{
			Display_String(1, "1.������ʾ      ");
			Display_String(2, "2.�޸�ʱ��      ");
			Display_String(3, "3.��������      ");
			Display_String(4, "4.��������    ѡ");
		}
		else if (flag == 6)
		{
			Display_String(1, "2.�޸�ʱ��      ");
			Display_String(2, "3.��������      ");
			Display_String(3, "4.��������      ");
			Display_String(4, "5.��ȡ����    ѡ");
		}
		else if (flag == 7)
		{
			Display_String(1, "3.��������      ");
			Display_String(2, "4.��������      ");
			Display_String(3, "5.��ȡ����      ");
			Display_String(4, "6.��������    ѡ");
		}
		else if (flag == 8)
		{
			Display_String(1, "4.��������      ");
			Display_String(2, "5.��ȡ����      ");
			Display_String(3, "6.��������      ");
			Display_String(4, "7.����        ѡ");
		}
	}
}

void getweather()
{
	uchar key;
	Lcm_Init();
	while(1)
	{
		Display_String(1, "�������ֻ�App   ");
		if(weather == 25)
		{
			Display_String(3, "��������        ");
		}
		else if(weather == 26)
		{
			Display_String(3, "��������        ");
		}
		else if(weather == 27)
		{
			Display_String(3, "��������        ");
		}
		else if(weather == 28)
		{
			Display_String(3, "������        ");
		}
		else if(weather == 29)
		{
			Display_String(3, "����  �Ϸ�      ");
		}
		else if(weather == 30)
		{
			Display_String(3, "����            ");
		}
		else if(weather == 31)
		{
			Display_String(3, "ǿ����          ");
		}
		else if(weather == 32)
		{
			Display_String(3, "������          ");
		}
		else if(weather == 33)
		{
			Display_String(3, "ǿ������        ");
		}
		else if(weather == 34)
		{
			Display_String(3, "С��            ");
		}
		else if(weather == 35)
		{
			Display_String(3, "����            ");
		}
		else if(weather == 36)
		{
			Display_String(3, "����            ");
		}
		else if(weather == 37)
		{
			Display_String(3, "����            ");
		}
		else if(weather == 38)
		{
			Display_String(3, "����          ");
		}
		else if(weather == 39)
		{
			Display_String(3, "  ��            ");
		}
		else if(weather == 50)
		{
			Display_String(3, "  ��            ");
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
		Display_String(1, "    ��������    ");
		if(weather == 50)
		{
		Display_String(3, "    ��������    ");
		}
		else if(weather == 51)
		{
		Display_String(3, "    ��������    ");
		}
		else if(weather == 52)
		{
		Display_String(3, "  ��ҵ��ֹ����  ");
		}
		else if(weather == 53)
		{
		Display_String(3, "    ʵ������    ");
		}
		key = key_scan();
		if(key == 4)
		{
			return;
		}
	}
}
