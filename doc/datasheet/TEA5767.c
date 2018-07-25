#ifndef __I2C_Driver_H
#define __I2C_Driver_H
#define uchar	unsigned char
#define uint	unsigned short int
#define ulint	unsigned int

#define delay_nus 2
/* IO�������� */
#define SDA_IN()	{ GPIOB->CRH &= 0XFFFF0FFF; GPIOB->CRH |= 8 << 12; } /*  */
#define SDA_OUT()	{ GPIOB->CRH &= 0XFFFF0FFF; GPIOB->CRH |= 3 << 12; }

/* IO�������� */
#define IIC_SCL		PBout( 10 ) /* SCL */
#define IIC_SDA		PBout( 11 ) /* SDA */
#define READ_SDA	PBin( 11 ) /* ����SDA */

/* IIC���в������� */
void IIC_Init( void );                          /* ��ʼ��IIC��IO�� */
void IIC_Start( void );                         /* ����IIC��ʼ�ź� */
void IIC_Stop( void );                          /* ����IICֹͣ�ź� */
void IIC_Send_Byte( uchar txd );                /* IIC����һ���ֽ� */
uchar IIC_Read_Byte( unsigned char ack );       /* IIC��ȡһ���ֽ� */
uchar IIC_Wait_Ack( void );                     /* IIC�ȴ�ACK�ź� */
void IIC_Ack( void );                           /* IIC����ACK�ź� */
void IIC_NAck( void );                          /* IIC������ACK�ź� */
void IIC_Write_One_Byte( uchar daddr, uchar addr, uchar data );
uchar IIC_Read_One_Byte( uchar daddr, uchar addr );
void Write_TEA5767( void );
void Read_TEA5767( void );
void TEA5767( void );
void Radio_Set_Frequency( void );
void Radio_Manual_Search( void );
void Radio_Search( void );
void Radio_Power_Display( void );
void Radio_Frequency_Display( void );
#endif

�ļ� �� I2C_Driver.c
#include "I2C_Driver.h"
#include "GUI.h"
#include "enhance.h"

uchar ack_cnt = 0;
void delay_us( uint t )
{
	t = 72 * t;
	while ( t-- > 0 )
		;
}


/* ��ʼ��IIC */
void IIC_Init( void )
{
	RCC->APB2ENR	|= 1 << 3;      /* ��ʹ������IO PORTBʱ�� */
	GPIOB->CRH	&= 0XFFFF00FF;  /* PB10/11 ������� */
	GPIOB->CRH	|= 0X00003300;
	GPIOB->ODR	|= 3 << 10;     /* PB10/11 ����� */
}


/* ����IIC��ʼ�ź� */
void IIC_Start( void )
{
	SDA_OUT();                      /* sda����� */
	IIC_SDA = 1;
	IIC_SCL = 1;
	delay_us( delay_nus );
	IIC_SDA = 0;                    /* START:when CLK is high,DATA change form high to low */
	delay_us( delay_nus );
	IIC_SCL = 0;                    /* ǯסI2C���ߣ�׼�����ͻ�������� */
}


/* ����IICֹͣ�ź� */
void IIC_Stop( void )
{
	SDA_OUT();                      /* sda����� */
	IIC_SCL = 0;
	IIC_SDA = 0;                    /* STOP:when CLK is high DATA change form low to high */
	delay_us( delay_nus );
	IIC_SCL = 1;
	IIC_SDA = 1;                    /* ����I2C���߽����ź� */
	delay_us( delay_nus );
}


/*
 * �ȴ�Ӧ���źŵ���
 * ����ֵ��1������Ӧ��ʧ��
 *        0������Ӧ��ɹ�
 */
uchar IIC_Wait_Ack( void )
{
	uchar ucErrTime = 0;
	SDA_IN(); /* SDA����Ϊ���� */
	IIC_SDA = 1; delay_us( delay_nus );
	IIC_SCL = 1; delay_us( delay_nus );
	while ( READ_SDA )
	{
		ucErrTime++;
		if ( ucErrTime > 250 )
		{
			IIC_Stop();
			return(1);
		}
	}
	IIC_SCL = 0; /* ʱ�����0 */
	ack_cnt++;
	return(0);
}


/* ����ACKӦ�� */
void IIC_Ack( void )
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us( delay_nus );
	IIC_SCL = 1;
	delay_us( delay_nus );
	IIC_SCL = 0;
}


/*������ACKӦ�� */
void IIC_NAck( void )
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us( delay_nus );
	IIC_SCL = 1;
	delay_us( delay_nus );
	IIC_SCL = 0;
}


/*
 * IIC����һ���ֽ�
 * ���شӻ�����Ӧ��
 * 1����Ӧ��
 * 0����Ӧ��
 */
void IIC_Send_Byte( uchar txd )
{
	uchar t;
	SDA_OUT();
	IIC_SCL = 0;                    /* ����ʱ�ӿ�ʼ���ݴ��� */
	for ( t = 0; t < 8; t++ )
	{
		IIC_SDA = (txd & 0x80) >> 7;
		txd	<<= 1;
		delay_us( delay_nus );  /* ��TEA5767��������ʱ���Ǳ���� */
		IIC_SCL = 1;
		delay_us( delay_nus );
		IIC_SCL = 0;
		delay_us( delay_nus );
	}
}


/* ��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK */
uchar IIC_Read_Byte( unsigned char ack )        /*  */
{
	unsigned char i, receive = 0;
	SDA_IN();                               /* SDA����Ϊ���� */
	for ( i = 0; i < 8; i++ )
	{
		delay_us( 2 );
		IIC_SCL = 1;
		receive <<= 1;
		delay_us( 2 );
		if ( READ_SDA )
			receive++;
		IIC_SCL = 0;
	}
	if ( !ack )
		IIC_NAck();     /* ����nACK */
	else
		IIC_Ack();      /* ����ACK */
	return(receive);
}


/* /////////////TEA5767///////////////////// */
#define W_TEA5767	0xc0
#define R_TEA5767	0xc1
unsigned char	Radio_Read_Buf[5]	= "12345";
unsigned char	Radio_Write_Buf[5]	= { 0x2a, 0xb6, 0x40, 0x17, 0x40 }; /* Ҫд��TEA5767������ */
#define Max_frequency	108000
#define Min_frequency	87500
#define Mute_On		1
#define Mute_Off	0
unsigned long		Frequency_Write		= 93600;
unsigned long		Frequency_Read		= 0;
unsigned long		pll			= 0;
unsigned char		Search_Mode_Flag	= 0, Radio_On_Flag = 0; /* Search_Over=0; */
static unsigned char	Radio_Search_Flag	= 0;
unsigned char		Radio_Mute		= Mute_Off;
void Write_TEA5767( void )
{
	uchar i = 0;
	delay_us( 0 );
	IIC_Start();
	IIC_Send_Byte( W_TEA5767 );
	if ( !IIC_Wait_Ack() )
	{
		for ( i = 0; i < 5; i++ )
		{
			IIC_Send_Byte( Radio_Write_Buf[i] );
			if ( IIC_Wait_Ack() )
			{
				GUI_DispStringAt( "Write buffer Failed !", 10, 100 );
				break;
			}
		}
	}else  {
		GUI_DispStringAt( "Failed !", 10, 80 );
	}
	IIC_Stop();
}


void Read_TEA5767( void )
{
	uchar i = 0;
	IIC_Start();
	IIC_Send_Byte( 0xc1 );
	if ( !IIC_Wait_Ack() )                                                                          /* chip addr of read data */
	{
		for ( i = 0; i < 5; i++ )
			Radio_Read_Buf[i] = (i == 4) ? IIC_Read_Byte( 0 ) : IIC_Read_Byte( 1 );         /* the last byte with ACK. */
		IIC_Stop();
/* GUI_DispStringAt("Read TEA5767 Completed !",10,100); */
	}else IIC_Stop();

	pll = (Radio_Read_Buf[0] & 0x3f) * 256 + Radio_Read_Buf[1] + 1;                                 /* //!!!!!    +1 */
	if ( Radio_Write_Buf[2] & 0x10 )
	{
		Frequency_Read = pll * 8192 - 225000;
	}else  {
		Frequency_Read = pll * 8192 + 225000;
	}
}


void Radio_Set_Frequency( void )
{
	if ( Radio_Write_Buf[2] & 0x10 )
	{
		pll = ( ( (Frequency_Write * 1000) + 225000) * 4) / 32768;
	}   else{
		pll = ( ( (Frequency_Write * 1000) - 225000) * 4) / 32768;      /* ����Ϊ�ͱߴ����շ�ʽ */
	}
	Radio_Write_Buf[0]	= pll / 256;
	Radio_Write_Buf[1]	= pll % 6;
	Radio_Write_Buf[0]	&= 0x3F;                                        /*������������ */
	if ( Radio_Mute == Mute_On )
		Radio_Write_Buf[0] |= 0x80;                                     /*  */
	else
		Radio_Write_Buf[0] &= 0x7f;                                     /*  */

	Radio_Write_Buf[2]	= 0x00;
	Radio_Write_Buf[3]	= 0x12;
	Radio_Write_Buf[4]	= 0x00;
	Write_TEA5767();
}


void Radio_Manual_Search( void )
{
	if ( Search_Mode_Flag )
	{
		Frequency_Write += 100;                  /* ����100KHZ */
		if ( Frequency_Write > Max_frequency )
		{
			Frequency_Write = Min_frequency;
		}
	}else  {
		Frequency_Write -= 100;
		if ( Frequency_Write < Min_frequency )
		{
			Frequency_Write = Max_frequency;
		}
	}
	Radio_Set_Frequency();
}


void Radio_Search( void )
{
	switch ( Radio_Search_Flag )
	{
	case 0:
	{
		Radio_Mute = Mute_On;       /* ������ */
		Radio_Manual_Search();
		Radio_Search_Flag = 1;
	}
	case 1:
	{
		unsigned char Radio_If;
/* _delay_ms(100); */
		Read_TEA5767();
		Radio_If = Radio_Read_Buf[2] & 0x7f;
		if ( (Radio_Read_Buf[3] >> 4) > 4 )                     /* ADC����4ֹͣ */
		{
			if ( (Radio_If > 0x30) && (Radio_If < 0x3a) )   /* �Ƚ�IF */
			{
				Radio_On_Flag		= 0;            /* �ر����� */
				Radio_Write_Buf[0]	&= 0x7f;        /* �����ر� */
				Write_TEA5767();
			}
			Radio_Search_Flag = 0;
		}else  {                                                /* ADCС��4���� */
			Radio_Search_Flag = 0;
		}
		Radio_Frequency_Display();
		Radio_Power_Display();
	}
	}
}


void Radio_Power_Display( void )
{
	unsigned char Temp;
	Temp = (Radio_Read_Buf[3] >> 4);        /* RF�ź�ǿ��ADC */
	GUI_GotoXY( 140, 0 );
	GUI_DispString( "RF:" );
	GUI_DispDec( Temp, 2 );
	Temp = (Radio_Read_Buf[2] & 0x7f);      /* ��Ƶ�����������ȷ��гʱ�ڡ�0x31,0x3e��֮�� */
	GUI_GotoXY( 200, 0 );
	GUI_DispString( "IF:" );
	GUI_DispHex( Temp, 2 );
	if ( (Temp >= 0x31) && (Temp <= 0x3e) )
		GUI_DispStringAt( "Good", 200, 20 );
	else
		GUI_DispStringAt( "Bad ", 200, 20 );
}


void Radio_Frequency_Display( void )
{
	char Display_Bit[9] = "000.0MHz ";
	Frequency_Read	= Frequency_Read / 1000;
	Display_Bit[0]	= Frequency_Read / 100000;
	if ( Display_Bit[0] == 0 )
	{
		Display_Bit[0] = '0';
	}else  {
		Display_Bit[0] += 0x30;
	}
	Display_Bit[1]	= (Frequency_Read / 10000) + 0x30;
	Display_Bit[2]	= (Frequency_Read / 1000) + 0x30;
	Display_Bit[4]	= (Frequency_Read / 100) + 0x30;

	GUI_DispStringAt( Display_Bit, 140, 20 );
}


void TEA5767( void )
{
	static uchar i = 0;
	if ( !i )
	{
		IIC_Init();
		delay_us( 0 );
		IIC_Stop();
		i++;
		Frequency_Write = 93600;
		Radio_Set_Frequency();
	}
	Read_TEA5767();
	Radio_Frequency_Display();
	Radio_Power_Display();

	GUI_GotoXY( 10, 0 );
	GUI_DispString( "W:" );
	TFT_DispHexStringAt( 6 * 2 + 10, 0, Radio_Write_Buf, 5 );
	GUI_GotoXY( 10, 20 );
	GUI_DispString( "R:" );
	TFT_DispHexStringAt( 6 * 2 + 10, 20, Radio_Read_Buf, 5 );
/* GUI_DispDecAt(ack_cnt,10,40,3); */
}