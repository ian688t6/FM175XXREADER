

#include "main.h"
#include "intrins.h"
#include "24Cxx.h"

  /****IIC*******/
#define SDA_IO_1()      		_24_SDA=1
#define SDA_IO_0()      		_24_SDA=0
#define SDA_IO_OUT()				_24_SDA=1

#define Read_GPIO_SDA_IO()	_24_SDA
#define SDA_IO_IN()					_24_SDA=1

#define SCL_IO_1()      		_24_SCL=1
#define SCL_IO_0()      		_24_SCL=0

#define _24WP_IO_1()      	_24_WP=1
#define _24WP_IO_0()      	_24_WP=0

unsigned int type_I2C=512;
unsigned int add_end_I2C=0xffff;
unsigned int PageLen_I2C=128;

#define I2Cmode 0  //0,为模拟I2C ; 1,为硬件I2C

#if I2Cmode ==0

uint8 Ack_Err;//测试应答累加数，若此位没有溢出在Stop()中清除此位，否则就保持溢出判无I2C处理
uint8 I2C_Delay_EN=0;//=0;
void delayus(void)
{
	_nop_();
	_nop_();
}
void delay4us(void)//'b'us
{
  if(I2C_Delay_EN)
  {
		delayus();
		delayus();
		delayus();
		delayus();
  }
}
void i2cStart()//开始条件 时钟线保持高电平期间数据线电平从高到低的跳变作为I2C 总线的起始信号
{
	if(Ack_Err<0xFF)
	  Ack_Err++; 
	SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SDA_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
}
void i2cStop()//结束条件	 时钟线保持高电平期间数据线电平从低到高的跳变作为I2C 总线的停止信号
{
	if(Ack_Err<0xFF)
    Ack_Err=0x00; 
	SDA_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
}
void Ack()//主机应答
{
	SDA_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
}
void NoAck()//不应答
{
	SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
}
uint8 TestAck()//从机应答
{
	uint8 ErrorBit;
	SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	SDA_IO_IN();delay4us();//SDA_IO_INIT_IN();
	if(Read_GPIO_SDA_IO())
		ErrorBit=1;//I2C_ACK;
	else
		ErrorBit=0;//I2C_ACK;
	delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	SDA_IO_OUT();//SDA_IO_INIT();
	SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
	if(Ack_Err==0xFF)
		return(0);
	else
		return(ErrorBit);//0);//
}
void writeByte(uint8 Data)//写8位
{
	uint8 temp;
	for(temp=8;temp!=0;temp--)
	{
		if(Data&0x80)
			SDA_IO_1();//SDA_pin=(bit)(input&0x80);
		else
			SDA_IO_0();
		delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
		SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
		Data<<=1;
		SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();		 
	}
}
unsigned char readByte()//读8位
{
	uint8 temp,rbyte=0; 
	for(temp=8;temp!=0;temp--)  
	{ 
		SDA_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
		rbyte<<=1; 
		SCL_IO_1();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
		SDA_IO_IN();delay4us();//SDA_IO_INIT_IN();
		if(Read_GPIO_SDA_IO())//I2C_ACK;
			rbyte|=1;
		delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ ();
		SDA_IO_OUT();//SDA_IO_INIT();
		//rbyte=rbyte|((uchar)(I2C_ACK));delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
		SCL_IO_0();delay4us();//_nop_ ();_nop_ ();_nop_ ();_nop_ (); 
	}
	return(rbyte); 
}
uint8 readMem(uint16 page_start ,uint8 c_addr)
{
  uint8 Data;
  Ack_Err=0x00;
	do
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//将P2,P1,P0与读控制字合并
	}
	while(TestAck());//等待应答
	if(Ack_Err==0xFF)
  {
    i2cStop();
    return 0x77;
  }
  if(c_addr!=I2C_2405)
	{
		writeByte((page_start>>8)&0xff);//写高8位地址
		TestAck();
	}
	writeByte(page_start&0xff);//写低8位地址
	TestAck();
	i2cStart();
//	writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//将P2,P1,P0与读控制字合并
	writeByte(c_addr+1);
	TestAck();
	Data=readByte();
	NoAck();//不发送应答
	i2cStop();//发送停止
  return(Data);
}
void read_mul(uint16 page_start, uint8 c_addr, uint8 *message_dat,uint16 Len)
{
  Ack_Err=0x00;
	if(Len==0)return;
	do
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//将P2,P1,P0与读控制字合并
	}
	while(TestAck());//等待应答
	if(Ack_Err==0xFF)
  {
    i2cStop();
    return;
  }
  if(c_addr!=I2C_2405)
	{
		writeByte((page_start>>8)&0xff);//写高8位地址
		TestAck();
	}
	writeByte(page_start&0xff);//写低8位地址
	TestAck();
	i2cStart();
//	writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//将P2,P1,P0与读控制字合并
	writeByte(c_addr+1);
	TestAck();
	while(Len!=1)
	{
		*message_dat=readByte();
		Ack();//发送应答
		message_dat++;
		Len--;
	}
	*message_dat=readByte();
	NoAck();//不发送应答
	i2cStop();//发送停止
}
void writeMem(uint16 page_start, uint8 c_addr ,uint8 message_in)
{
  if(readMem(page_start,c_addr)==message_in)return;
  
  _24WP_IO_0();
	
  Ack_Err=0x00;
	do
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//将P2,P1,P0与读控制字合并
	}
	while(TestAck());//等待应答      
	if(Ack_Err==0xFF)
  {
    i2cStop();
    return;
  }
  if(c_addr!=I2C_2405)
	{
		writeByte((page_start>>8)&0xff);//写高8位地址
		TestAck();
	}
  writeByte(page_start&0xff);//发送写的起始地址
	TestAck();
  writeByte(message_in);
	TestAck();
  i2cStop();//发送停止位
	
  Ack_Err=0x00;
	do//
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//将P2,P1,P0与读控制字合并
	}
	while(TestAck());//等待应答
	i2cStop();
	
  _24WP_IO_1();
}
void write_mul(uint16 page_start, uint8 c_addr ,uint8 *message_dat,uint8 Len)  //页写,会溢出
{
  uint8 i;
	if(Len==0)return;
  for(i=0;i<Len;)
    if(readMem(page_start+i,c_addr)==message_dat[i])i++;
    else break;
  if(i==Len)return;
  
  _24WP_IO_0();
	
  Ack_Err=0x00;
	do
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//将P2,P1,P0与读控制字合并
	}
	while(TestAck());//等待应答      
	if(Ack_Err==0xFF)
  {
    i2cStop();
    return;
  }
  if(c_addr!=I2C_2405)
	{
		writeByte((page_start>>8)&0xff);//写高8位地址
		TestAck();
	}
  writeByte(page_start&0xff);//发送写的起始地址
	TestAck();
	while(Len--)
	{
		writeByte(*message_dat++);
		TestAck();
	}
  i2cStop();//发送停止位
	
  Ack_Err=0x00;
	do//
	{
		i2cStart();
		writeByte(c_addr);
//		writeByte(((c_Address<<1)&0x0E)|ReadDeviceAdd);//将P2,P1,P0与读控制字合并
	}
	while(TestAck());//等待应答
	i2cStop();
	
  _24WP_IO_1();
}
#endif

#if I2Cmode ==1
                                                                        /* 定义用于和I2C中断传递信息的  */
                                                                        /* 全局变量                     */
volatile uint8_t     I2C_sla;                                           /* I2C器件从地址                */
volatile uint32_t    I2C_suba;                                          /* I2C器件内部子地址            */
volatile uint8_t     I2C_suba_num;                                      /* I2C子地址字节数              */
volatile uint8_t     *I2C_buf;                                          /* 数据缓冲区指针               */
volatile uint32_t    I2C_num;                                           /* 要读取/写入的数据个数        */
volatile uint8_t     I2C_end;                                           /* I2C总线结束标志：结束总线是  */
                                                                        /* 置1                          */
volatile uint8_t     I2C_suba_en;                                       /*  子地址控制。
                                                                         0--子地址已经处理或者不需要子地址
                                                                         1--读取操作
                                                                         2--写操作
                                                                         */

/*********************************************************************************************************
** Function name:       I2C2_IRQHandler
** Descriptions:        硬件I2C中断服务程序。
** input parameters:    无
** Output parameters:   无
** Returned value:      注意处理子地址为2字节的情况。
*********************************************************************************************************/
//void  myI2C2IRQHandler(void)
void  I2C2_IRQHandler(void)
{
	/*
         *读取I2C状态寄存器I2DAT,按照全局变量的设置进行操作及设置软件标志,清除中断逻辑,中断返回
         */
	switch (LPC_I2C2->I2STAT & 0xF8)
	{   /* 
	     *  根据状态码进行相应的处理     
	     */
		case 0x08:/* 已发送起始条件,主发送和主接  */
		{
																									/* 收都有,装入SLA+W或者SLA+R    */
			if(I2C_suba_en == 1)/* SLA+R */							/* 指定子地址读                 */
				LPC_I2C2->I2DAT = I2C_sla & 0xFE;           /* 先写入地址                   */
			else                                        /* SLA+W                        */
				LPC_I2C2->I2DAT = I2C_sla;                  /* 否则直接发送从机地址         */
																									/* 清零SI位                     */
			LPC_I2C2->I2CONCLR = (1 << 3)|                /* SI                           */
												 (1 << 5);                /* STA                          */
		}
    break;
		case 0x10:/*已发送重复起始条件            */
		{
                                                  /* 主发送和主接收都有           */
                                                  /* 装入SLA+W或者SLA+R           */
			LPC_I2C2->I2DAT = I2C_sla;                  	/* 重起总线后，重发从地址       */
			LPC_I2C2->I2CONCLR = 0x28;                  	/* 清零SI,STA                   */
		}
		break;
		case 0x18:
		case 0x28:/* 已发送I2DAT中的数据,已接收ACK*/
		{
			if (I2C_suba_en == 0)
			{
				if (I2C_num > 0)
				{
					LPC_I2C2->I2DAT = *I2C_buf++;
					LPC_I2C2->I2CONCLR = 0x28;                                /* 清零SI,STA                   */
					I2C_num--;
					delay_ms(1);
				}
				else                                                        /* 没有数据发送了               */
				{                                                           /* 停止总线                     */
					LPC_I2C2->I2CONSET = (1 << 4);                            /* STO                          */
					LPC_I2C2->I2CONCLR = 0x28;                                /* 清零SI,STA                   */
					I2C_end = 1;                                            /* 总线已经停止                 */
				}
			}
			if(I2C_suba_en == 1)
			{                                          /* 若是指定地址读,则重新启动总线*/
				if (I2C_suba_num == 2)
				{
					LPC_I2C2->I2DAT = ((I2C_suba >> 8) & 0xff);
					LPC_I2C2->I2CONCLR = 0x28;                                /* 清零SI,STA                   */
					I2C_suba_num--;
					break;
				}
				if(I2C_suba_num == 1)
				{
					LPC_I2C2->I2DAT = (I2C_suba & 0xff);
					LPC_I2C2->I2CONCLR = 0x28;                                /* 清零SI,STA                   */
					I2C_suba_num--;
					break;
				}
				if (I2C_suba_num == 0)
				{
					LPC_I2C2->I2CONCLR = 0x08;
					LPC_I2C2->I2CONSET = 0x20;
					I2C_suba_en = 0;                                        /* 子地址己处理                 */
					break;
				}
			}
			if (I2C_suba_en == 2)
			{                                         /* 指定子地址写,子地址尚未指定, */
                                                                        /* 则发送子地址                 */
				if (I2C_suba_num > 0)
				{
					if (I2C_suba_num == 2)
					{
						LPC_I2C2->I2DAT = ((I2C_suba >> 8) & 0xff);
						LPC_I2C2->I2CONCLR = 0x28;
						I2C_suba_num--;
						break;
					}
					if (I2C_suba_num == 1)
					{
						LPC_I2C2->I2DAT    = (I2C_suba & 0xff);
						LPC_I2C2->I2CONCLR = 0x28;
						I2C_suba_num--;
						I2C_suba_en  = 0;
						break;
					}
				}
			}
		}
		break;
		case 0x40: 
		{			/* 已发送SLA+R,已接收ACK        */
			if (I2C_num <= 1)
			{                                             /* 如果是最后一个字节           */
				 LPC_I2C2->I2CONCLR = 1 << 2;                                 /* 下次发送非应答信号           */
			}
			else
			{
				 LPC_I2C2->I2CONSET = 1 << 2;                                 /* 下次发送应答信号             */
			}
			LPC_I2C2->I2CONCLR = 0x28;                                        /* 清零SI,STA                   */
		}
		break;
		case 0x20:                                                      /* 已发送SLA+W,已接收非应答     */
		case 0x30:                                                      /* 已发送I2DAT中的数据，已接收  */
                                                                        /* 非应答                       */
		case 0x38:                                                      /* 在SLA+R/W或数据字节中丢失仲  */
                                                                        /* 裁                           */
		case 0x48:                                                      /* 已发送SLA+R,已接收非应答     */
		{
			LPC_I2C2->I2CONCLR = 0x28;
			I2C_end = 0xFF;
		}
		break;
		case 0x50:                                                      /* 已接收数据字节，已返回ACK    */
    {
			*I2C_buf++ = LPC_I2C2->I2DAT;
			I2C_num--;
			if (I2C_num == 1)                                               /* 接收最后一个字节             */
			{   
				LPC_I2C2->I2CONCLR = 0x2C;                                    /* STA,SI,AA = 0                */
			} 
			else
			{   
				LPC_I2C2->I2CONSET = 0x04;                                    /* AA=1                         */
				LPC_I2C2->I2CONCLR = 0x28;
			}
		}
		break;
		case 0x58:                                                      /* 已接收数据字节，已返回非应答 */
    {
			*I2C_buf++ = LPC_I2C2->I2DAT;                                     /* 读取最后一字节数据           */
			LPC_I2C2->I2CONSET = 0x10;                                        /* 结束总线                     */
			LPC_I2C2->I2CONCLR = 0x28;
			I2C_end = 1;
		}
		break;
		default:
		break;
	}
}

void I2C2Init(uint32_t ulFi2c2)
{
	LPC_SC->PCONP |= (1UL << 26);                                      /* 打开I2C2模块功率控制位       */
//	LPC_IOCON->P0_27 = 0x01 | (0 << 6)| (1 << 8) | (1 << 9);           /* P0.27配置为SDA0功能          */
//	LPC_IOCON->P0_28 = 0x01 | (0 << 6)| (1 << 8) | (1 << 9);           /* P0.28配置为SCL0功能          */
	
	LPC_PINCON->PINSEL0 &= ~(3UL << ((SDA_IO%16) * 2));
	LPC_PINCON->PINSEL0 |= (2UL << ((SDA_IO%16) * 2));
//	LPC_PINCON->PINMODE0 |= (2UL << ((SDA_IO%16) * 2));
//	LPC_PINCON->PINMODE_OD0 |= (1UL << SDA_IO);
	
	LPC_PINCON->PINSEL0 &= ~(3UL << ((SCL_IO%16) * 2));
	LPC_PINCON->PINSEL0 |= (2UL << ((SCL_IO%16) * 2));
//	LPC_PINCON->PINMODE0 |= (2UL << ((SCL_IO%16) * 2));
//	LPC_PINCON->PINMODE_OD0 |= (1UL << SCL_IO);
	
//	LPC_SC->PCLKSEL1	|= ((1<<20)|(1<<21));
	
	if (ulFi2c2 > 400000) {
			ulFi2c2 = 400000;
	}
	LPC_I2C2->I2SCLH = (((SystemFrequency / ulFi2c2) / 4)+1)/2;                   /* 设置I2C时钟为fi0c            */
	LPC_I2C2->I2SCLL = (SystemFrequency / ulFi2c2) / 4/2;
	NVIC_EnableIRQ(I2C2_IRQn);                                          /* 使能I2C中断                  */
	NVIC_SetPriority(I2C2_IRQn, 3);                                     /* 设置I2C2中断优先级并使能中断 */
	LPC_I2C2->I2CONCLR = 0x2C;
	LPC_I2C2->I2CONSET = 0x40;                                            /* 使能主I2C                    */
}

/*********************************************************************************************************
** Function name:           Wait_I2c_End
** Descriptions:            软件延时，提供给I2C总线等待超时使用
** input parameters:        dly        延时参数，值越大，延时越久
** Output parameters:       NONE
** Returned value:          NONE
*********************************************************************************************************/
uint8_t  Wait_I2c_End(uint32_t  dly)
{
	uint32_t  i;
	if( I2C_end==1 ) return (1);
	for(; dly>0; dly--)
		for(i=0; i<5000; i++)
			if( I2C_end==1 )
				return (1);
	return (0);
}
#if 0
/*********************************************************************************************************
** Function name:       ISendByte
** Descriptions:        向无子地址器件发送1字节数据。
** input parameters:    sla     器件地址
**                      dat     要发送的数据
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
uint8_t  ISendByte(uint8_t sla, uint8_t dat)
{                                                                       /* 参数设置                     */
	I2C_sla     = sla;                                                  /* 写操作的器件地址             */
	I2C_buf     = &dat;                                                 /* 待发送的数据                 */
	I2C_num     = 1;                                                    /* 发送1字节数据                */
	I2C_suba_en = 0;                                                    /* 无子地址                     */
	I2C_end     = 0;
	LPC_I2C2->CONCLR = 0x2C;
	LPC_I2C2->CONSET = 0x60;                                            /* 设置为主机，并启动总线       */
	return( Wait_I2c_End(20));
}
/*********************************************************************************************************
** Function name:       IRcvByte
** Descriptions:        向无子地址器件读取1字节数据。
** input parameters:    sla     器件地址
**                      dat     要发送的数据
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
uint8_t  IRcvByte(uint8_t sla, uint8_t *dat)
{                                                                       /* 参数设置                     */
	I2C_sla     = sla+1;                                                /* 读操作的器件地址             */
	I2C_buf     = dat;
	I2C_num     = 1;
	I2C_suba_en = 0;                                                    /* 无子地址                     */
	I2C_end     = 0;
	LPC_I2C2->CONCLR = 0x2C;
	LPC_I2C2->CONSET = 0x60;                                            /* 设置为主机，并启动总线       */
	return( Wait_I2c_End(20));
}
#endif
/*********************************************************************************************************
** Function name:       readMem
** Descriptions:        从有子地址器件任意地址开始读取N字节数据
** input parameters:    sla         器件地址
**                      suba_type   子地址结构    1－单字节地址    3－8+X结构    2－双字节地址
**                      suba        器件子地址
**                      s           数据接收缓冲区指针
**                      num         读取的个数
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
uint8 readMem(uint16 page_start ,uint8 c_addr)
{
	uint8 Data[1]={0xff};
	read_mul(page_start, c_addr, Data,1);
	return(Data[0]);
}
/*********************************************************************************************************
** Function name:       read_mul
** Descriptions:        从有子地址器件任意地址开始读取N字节数据
** input parameters:    sla         器件地址
**                      suba_type   子地址结构    1－单字节地址    3－8+X结构    2－双字节地址
**                      suba        器件子地址
**                      s           数据接收缓冲区指针
**                      num         读取的个数
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
//uint8_t read_mul(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
void read_mul(uint16 page_start, uint8 c_addr, uint8 *message_dat,uint16 Len)
{
	uint8 suba_type=2;
	
	if (Len > 0)                                                      /* 判断num个数的合法性          */
	{                                                                 /* 参数设置                     */
		if (suba_type == 1)
		{                                                               /* 子地址为单字节               */
			I2C_sla         = c_addr + 1;                                 /* 读器件的从地址，R=1          */
			I2C_suba        = page_start;                                 /* 器件子地址                   */
			I2C_suba_num    = 1;                                        	/* 器件子地址为1字节            */
		}
		if (suba_type == 2)
		{                                                               /* 子地址为2字节                */
			I2C_sla         = c_addr + 1;                                 /* 读器件的从地址，R=1          */
			I2C_suba        = page_start;                                 /* 器件子地址                   */
			I2C_suba_num    = 2;                                        	/* 器件子地址为2字节            */
		}
		if (suba_type == 3)
		{                                                               /* 子地址结构为8+X              */
			I2C_sla         = c_addr + ((page_start >> 7 )& 0x0e) + 1;    /* 读器件的从地址，R=1          */
			I2C_suba        = page_start & 0x0ff;                         /* 器件子地址                   */
			I2C_suba_num    = 1;                                        	/* 器件子地址为8+x              */
		}
		I2C_buf     = message_dat;                                      /* 数据接收缓冲区指针           */
		I2C_num     = Len;                                              /* 要读取的个数                 */
		I2C_suba_en = 1;                                                /* 有子地址读                   */
		I2C_end     = 0;
																																		/* 清除STA,SI,AA标志位          */
		LPC_I2C2->I2CONCLR = (1 << 2)|                                  /* AA                           */
											 (1 << 3)|                                    /* SI                           */
											 (1 << 5);                                    /* STA                          */
																																		/* 置位STA,启动I2C总线          */
		LPC_I2C2->I2CONSET = (1 << 5)|                                  /* STA                          */
											 (1 << 6);                                    /* I2CEN                        */
		Wait_I2c_End(20);																								/* 等待I2C操作完成              */
//		delay_ms(20);
//		delay_ms(20);
//		return( Wait_I2c_End(20));
	}
//	delay_ms(20);
//	return (0);
}

void writeMem(uint16 page_start, uint8 c_addr ,uint8 message_in)
{
	write_mul(page_start, c_addr ,&message_in,1);
}
/*********************************************************************************************************
** Function name:       I2C_WriteNByte
** Descriptions:        向有子地址器件写入N字节数据
** input parameters:    sla         器件地址
**                      suba_type   子地址结构    1－单字节地址    3－8+X结构    2－双字节地址
**                      suba        器件子地址
**                      s           将要写入的数据的指针
**                      num         将要写入的数据的个数
** Output parameters:   返回值为0时表示出错，为1时表示操作正确。
** Returned value:      NONE
*********************************************************************************************************/
//uint8_t write_mul(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num)
void write_mul(uint16 page_start, uint8 c_addr ,uint8 *message_dat,uint8 Len)  //页写,会溢出
{
	uint8 suba_type=2;
	
	if (Len > 0)                                                        /* 如果读取的个数为0，则返回错误*/
	{                                                                   /* 设置参数                     */
		if (suba_type == 1){                                            	/* 子地址为单字节               */
			I2C_sla         = c_addr;                                      	/* 读器件的从地址               */
			I2C_suba        = page_start;                                  	/* 器件子地址                   */
			I2C_suba_num    = 1;                                        		/* 器件子地址为1字节            */
		}

		if (suba_type == 2){                                            	/* 子地址为2字节                */
			I2C_sla         = c_addr;                                      	/* 读器件的从地址               */
			I2C_suba        = page_start;                                  	/* 器件子地址                   */
			I2C_suba_num    = 2;                                        		/* 器件子地址为2字节            */
		}

		if (suba_type == 3){                                            	/* 子地址结构为8+X              */
			I2C_sla         = c_addr + ((page_start >> 7 )& 0x0e);         	/* 读器件的从地址               */
			I2C_suba        = page_start & 0x0ff;                          	/* 器件子地址                   */
			I2C_suba_num    = 1;                                        		/* 器件子地址为8+X              */
		}

		I2C_buf     = message_dat;                                      	/* 数据                         */
		I2C_num     = Len;                                              	/* 数据个数                     */
		I2C_suba_en = 2;                                                	/* 有子地址，写操作             */
		I2C_end     = 0;                                                                        
		LPC_I2C2->I2CONCLR = (1 << 2)|                                    /* 清除STA,SI,AA标志位          */
											 (1 << 3)|
											 (1 << 5);
		LPC_I2C2->I2CONSET = (1 << 5)|                                    /* 置位STA,启动I2C总线          */
											 (1 << 6);                                    	/* I2CEN                        */
		Wait_I2c_End(20);
		delay_ms(20);//此处延时不可以少,否则可能写不成功
//		return( Wait_I2c_End(20));                                      /* 等待I2C操作完成              */
	}
	delay_ms(20);//此处延时不可以少,否则可能写不成功
//	return (0);
}

#endif

void fast_write_mul(uint16 page_start,uint8 c_addr,unsigned char *message_dat,uint16 Len)//快速写I2C,防溢出
{
	uint16 i;
  uint8 m;
	if(page_start+Len>add_end_I2C)
    Len=add_end_I2C-page_start+1;
  m=PageLen_I2C-page_start%PageLen_I2C;
  if(Len>m)//先写满当前页,让地址为下一页的页首
  {
    write_mul(page_start,c_addr,&message_dat[0],m);
    page_start+=m;
    Len-=m;
  }
  else
    m=0;
  for(i=0;i<Len/PageLen_I2C;i++)//写一整页
  {
    write_mul(page_start,c_addr,&message_dat[i*PageLen_I2C+m],PageLen_I2C);
    page_start+=PageLen_I2C;
  }
  Len%=PageLen_I2C;
  if(Len)//写完剩下不足的一页
    write_mul(page_start,c_addr,&message_dat[i*PageLen_I2C+m],Len);
}
