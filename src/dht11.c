#include <reg52.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit Data = P1^0;


void DHT11_delay_us(uchar n)
{
    while (--n)
        ;
}

void DHT11_delay_ms(uint z)
{
    uint i, j;
    for (i = z; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

void Delay30us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 80;
	while (--i);
}

void DHT11_start()
{
    Data	= 1;
    DHT11_delay_us(2);
	
    Data = 0;
    DHT11_delay_ms(25);

    Data = 1;
    DHT11_delay_us(30);

}

uchar DHT11_rec_byte()
{
    uchar i, dat = 0;
    for (i = 0; i < 8; i++)
    {
				while(Data);
        while (!Data);
        DHT11_delay_us(8);
        dat <<= 1;
        if (Data == 1)
            dat += 1;
        while (Data);
    }
    return dat;
}

// dht11不稳定，容易乱码
// 接收dht11发来的数据并格式化为lcd12864可以显示的16位字符串格式
uchar* DHT11_receive()
{
    uchar R_H, R_L, T_H, T_L, RH, RL, TH, TL, revise, rec_dat[16];
    DHT11_start();
    if (Data == 0)
    {
        while (Data == 0);
        DHT11_delay_us(50);
        R_H = DHT11_rec_byte();
        R_L = DHT11_rec_byte();
        T_H = DHT11_rec_byte();
        T_L = DHT11_rec_byte();
        revise = DHT11_rec_byte();
        DHT11_delay_us(25);

        if ((R_H + R_L + T_H + T_L) == revise)
        {
            RH = R_H;
            RL = R_L;
            TH = T_H;
            TL = T_L;
        }
        rec_dat[0] = 'R';
        rec_dat[1] = 'H';
				rec_dat[2] = ':';
        rec_dat[3] = '0' + (RH / 10);
        rec_dat[4] = '0' + (RL % 10);
				rec_dat[5] = '%';
        rec_dat[6] = ' ';
			  rec_dat[7] = ' ';
				rec_dat[8] = 'T';
			  rec_dat[9] = 'E';
			  rec_dat[10] = 'M';
				rec_dat[11] = ':';
        rec_dat[12] = '0' + (TH / 10);
        rec_dat[13] = '0' + (TL % 10);
				rec_dat[14] = '`';
				rec_dat[15] = 'C';
    }
		return rec_dat;
}

