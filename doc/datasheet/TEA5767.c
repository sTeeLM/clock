#ifndef __I2C_Driver_H
#define __I2C_Driver_H
#define uchar	unsigned char
#define uint	unsigned short int
#define ulint	unsigned int

#define delay_nus 2
/* IO方向设置 */
#define SDA_IN()	{ GPIOB->CRH &= 0XFFFF0FFF; GPIOB->CRH |= 8 << 12; } /*  */
#define SDA_OUT()	{ GPIOB->CRH &= 0XFFFF0FFF; GPIOB->CRH |= 3 << 12; }

/* IO操作函数 */
#define IIC_SCL		PBout( 10 ) /* SCL */
#define IIC_SDA		PBout( 11 ) /* SDA */
#define READ_SDA	PBin( 11 ) /* 输入SDA */

/* IIC所有操作函数 */
void IIC_Init( void );                          /* 初始化IIC的IO口 */
void IIC_Start( void );                         /* 发送IIC开始信号 */
void IIC_Stop( void );                          /* 发送IIC停止信号 */
void IIC_Send_Byte( uchar txd );                /* IIC发送一个字节 */
uchar IIC_Read_Byte( unsigned char ack );       /* IIC读取一个字节 */
uchar IIC_Wait_Ack( void );                     /* IIC等待ACK信号 */
void IIC_Ack( void );                           /* IIC发送ACK信号 */
void IIC_NAck( void );                          /* IIC不发送ACK信号 */
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

文件 ： I2C_Driver.c
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


/* 初始化IIC */
void IIC_Init( void )
{
	RCC->APB2ENR	|= 1 << 3;      /* 先使能外设IO PORTB时钟 */
	GPIOB->CRH	&= 0XFFFF00FF;  /* PB10/11 推挽输出 */
	GPIOB->CRH	|= 0X00003300;
	GPIOB->ODR	|= 3 << 10;     /* PB10/11 输出高 */
}


/* 产生IIC起始信号 */
void IIC_Start( void )
{
	SDA_OUT();                      /* sda线输出 */
	IIC_SDA = 1;
	IIC_SCL = 1;
	delay_us( delay_nus );
	IIC_SDA = 0;                    /* START:when CLK is high,DATA change form high to low */
	delay_us( delay_nus );
	IIC_SCL = 0;                    /* 钳住I2C总线，准备发送或接收数据 */
}


/* 产生IIC停止信号 */
void IIC_Stop( void )
{
	SDA_OUT();                      /* sda线输出 */
	IIC_SCL = 0;
	IIC_SDA = 0;                    /* STOP:when CLK is high DATA change form low to high */
	delay_us( delay_nus );
	IIC_SCL = 1;
	IIC_SDA = 1;                    /* 发送I2C总线结束信号 */
	delay_us( delay_nus );
}


/*
 * 等待应答信号到来
 * 返回值：1，接收应答失败
 *        0，接收应答成功
 */
uchar IIC_Wait_Ack( void )
{
	uchar ucErrTime = 0;
	SDA_IN(); /* SDA设置为输入 */
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
	IIC_SCL = 0; /* 时钟输出0 */
	ack_cnt++;
	return(0);
}


/* 产生ACK应答 */
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


/*不产生ACK应答 */
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
 * IIC发送一个字节
 * 返回从机有无应答
 * 1，有应答
 * 0，无应答
 */
void IIC_Send_Byte( uchar txd )
{
	uchar t;
	SDA_OUT();
	IIC_SCL = 0;                    /* 拉低时钟开始数据传输 */
	for ( t = 0; t < 8; t++ )
	{
		IIC_SDA = (txd & 0x80) >> 7;
		txd	<<= 1;
		delay_us( delay_nus );  /* 对TEA5767这三个延时都是必须的 */
		IIC_SCL = 1;
		delay_us( delay_nus );
		IIC_SCL = 0;
		delay_us( delay_nus );
	}
}


/* 读1个字节，ack=1时，发送ACK，ack=0，发送nACK */
uchar IIC_Read_Byte( unsigned char ack )        /*  */
{
	unsigned char i, receive = 0;
	SDA_IN();                               /* SDA设置为输入 */
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
		IIC_NAck();     /* 发送nACK */
	else
		IIC_Ack();      /* 发送ACK */
	return(receive);
}


/* /////////////TEA5767///////////////////// */
#define W_TEA5767	0xc0
#define R_TEA5767	0xc1
unsigned char	Radio_Read_Buf[5]	= "12345";
unsigned char	Radio_Write_Buf[5]	= { 0x2a, 0xb6, 0x40, 0x17, 0x40 }; /* 要写入TEA5767的数据 */
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
		pll = ( ( (Frequency_Write * 1000) - 225000) * 4) / 32768;      /* 本例为低边带接收方式 */
	}
	Radio_Write_Buf[0]	= pll / 256;
	Radio_Write_Buf[1]	= pll % 6;
	Radio_Write_Buf[0]	&= 0x3F;                                        /*不静音不搜索 */
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
		Frequency_Write += 100;                  /* 步进100KHZ */
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
		Radio_Mute = Mute_On;       /* 静音打开 */
		Radio_Manual_Search();
		Radio_Search_Flag = 1;
	}
	case 1:
	{
		unsigned char Radio_If;
/* _delay_ms(100); */
		Read_TEA5767();
		Radio_If = Radio_Read_Buf[2] & 0x7f;
		if ( (Radio_Read_Buf[3] >> 4) > 4 )                     /* ADC大于4停止 */
		{
			if ( (Radio_If > 0x30) && (Radio_If < 0x3a) )   /* 比较IF */
			{
				Radio_On_Flag		= 0;            /* 关闭搜索 */
				Radio_Write_Buf[0]	&= 0x7f;        /* 静音关闭 */
				Write_TEA5767();
			}
			Radio_Search_Flag = 0;
		}else  {                                                /* ADC小于4继续 */
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
	Temp = (Radio_Read_Buf[3] >> 4);        /* RF信号强度ADC */
	GUI_GotoXY( 140, 0 );
	GUI_DispString( "RF:" );
	GUI_DispDec( Temp, 2 );
	Temp = (Radio_Read_Buf[2] & 0x7f);      /* 中频计数结果，正确调谐时在【0x31,0x3e】之间 */
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