//串口收发程序

#include "STC15F2K60S2.h"
#include "pin.h"
#include "uart.h"
#include "main.h"
				 
#define UART_BAUD 38400//38400	 //115200bps
#define ETU_WIDTH 97 //103us
#define SYS_CLK 22118400 //33.1776Mhz

 struct status data uart_status;
 struct uart_buff send_buff;
 struct uart_buff rece_buff;
 unsigned char uart_data[256];
/*********************************************/
/*函数名：	    timer_uart_init
/*功能：	    时钟与串口初始化
				初始化串口0为19200BPS
				1个停止位，8个数据位，无校验位
				接收中断允许
/*输入参数：	无
/*返回值：	    无
/*********************************************/
void Uart1_Init(void)
{
	SCON=0x50;				    				//SM0:0 SM1:1 SM2:0 REN:1 TB8:0 RB8:0 TI:0 RI:0
	PCON=0x00;									//SMOD=0  
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式

	TL1=(65536-((long)SYS_CLK/4/(long)UART_BAUD));	  
	TH1=(65536-((long)SYS_CLK/4/(long)UART_BAUD))>>8;

	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1

//	INT1=1;//配置中断脚为高电平
//	IT1=1;//仅触发下降沿
//	EX1=1;//中断使能

	ES=1;					    				//开串口0中断
	EA=1;					    				//开总中断
}

void Debug_Send_char1(unsigned char Data)//发送单个字节
{
	;
// 	ES=0;		//关串口中断
// 	CON1=1;
//   TI=0;
// /***/
//   if(SM0)
//   {
//     ACC=Data;
//     TB8=P;//偶校验
//   }
// /***/  
//   SBUF=Data;
//   while(!TI);
//   TI=0;
// 	CON1=0;
// 	ES=1;		//开串口中断
}
void Debug_Send_string1(unsigned char *pz,unsigned int Len)  //从pz开始发送number位
{
  if(Len==0)
    while(*pz)
    {
      if(*pz=='\n')
        Debug_Send_char1('\r');
      Debug_Send_char1(*pz++);
    }
	else
    while(Len--)
      Debug_Send_char1(*pz++);
}

void Send_char1(unsigned char Data)//发送单个字节
{
	ES=0;		//关串口中断
	CON1=1;
  TI=0;
/***/
  if(SM0)
  {
    ACC=Data;
    TB8=P;//偶校验
  }
/***/  
  SBUF=Data;
  while(!TI);
  TI=0;
	CON1=0;
	ES=1;		//开串口中断
}
void Send_string1(unsigned char *pz,unsigned int Len)  //从pz开始发送number位
{
  if(Len==0)
    while(*pz)
    {
      if(*pz=='\n')
        Send_char1('\r');
      Send_char1(*pz++);
    }
	else
    while(Len--)
      Send_char1(*pz++);
}

void Send_char2(unsigned char Data)//发送单个字节
{
	IE2 &= ~0x01;		//关串口中断
  S2CON &= ~S2TI;         //清除S2TI位
  S2BUF=Data;
  while(!(S2CON & S2TI));
  S2CON &= ~S2TI;         //清除S2TI位
	IE2 |= 0x01;		//开串口中断
}
void Send_string2(unsigned char *pz,unsigned int Len)  //从pz开始发送number位
{
  if(Len==0)
    while(*pz)
    {
      if(*pz=='\n')
        Send_char2('\r');
      Send_char2(*pz++);
    }
	else
    while(Len--)
      Send_char2(*pz++);
}
void Uart2_Init(void)
{
  P_SW2 &= ~S2_S0;            //S2_S0=0 (P1.0/RxD2, P1.1/TxD2)
//  P_SW2 |= S2_S0;             //S2_S0=1 (P4.6/RxD2_2, P4.7/TxD2_2)

//#if (PARITYBIT == NONE_PARITY)
  S2CON = 0x50;               //8位可变波特率
//#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
//  S2CON = 0xda;               //9位可变波特率,校验位初始为1
//#elif (PARITYBIT == SPACE_PARITY)
//  S2CON = 0xd2;               //9位可变波特率,校验位初始为0
//#endif

  T2L = (65536 - (SYS_CLK/4/115200));   //设置波特率重装值0xe8;//
  T2H = (65536 - (SYS_CLK/4/115200))>>8;//0xff;//
  AUXR |= 0x14;                //T2为1T模式, 并启动定时器2
  IE2 |= 0x01;                 //使能串口2中断
  EA = 1;
}
void Hex_Print(unsigned char uc_data)
{
	unsigned char HEX[]="0123456789ABCDEF";
	Send_char1(HEX[(uc_data>>4)&0x0f]);
	Send_char1(HEX[(uc_data>>0)&0x0f]);
}

void Uart_Send(void)
{
	int i = 0;
	
	Send_char1('\r');
	Send_char1('\n');
	for (i = 0; i < send_buff.length; i ++) {
		Hex_Print(send_buff.buff[i]);
	}
	Send_char1('\r');
	Send_char1('\n');
	
	return;
}
