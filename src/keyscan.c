#include <reg52.h>
sbit KeyPort0 = P2 ^ 4;
sbit KeyPort1 = P2 ^ 5;
sbit KeyPort2 = P2 ^ 6;
sbit KeyPort3 = P2 ^ 7;
#define uchar unsigned char
void Delay5Ms();

// 按键检测，仅支持单点，不支持连点，多点。返回键值1-4
uchar key_scan()
{
    uchar KeyV;
    KeyPort0 = 1;
    KeyPort1 = 1;
    KeyPort2 = 1;
    KeyPort3 = 1;
    if (KeyPort0 == 0 || KeyPort1 == 0 || KeyPort2 == 0 || KeyPort3 == 0)
    {
        Delay5Ms();
        if (KeyPort0 == 0)
        {
            KeyV = 1;
        }
        else if (KeyPort1 == 0)
        {
            KeyV = 2;
        }
        else if (KeyPort2 == 0)
        {
            KeyV = 3;
        }
        else if (KeyPort3 == 0)
        {
            KeyV = 4;
        }
        else
        {
            KeyV = 0;
        }

        if (KeyV != 0)
            while (KeyPort0 == 0 || KeyPort1 == 0 || KeyPort2 == 0 || KeyPort3 == 0);
						Delay5Ms();
    }
		return KeyV;
}
