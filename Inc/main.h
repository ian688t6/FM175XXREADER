
#ifndef MAIN_H
	#define MAIN_H	1
 
	#include "STC15F2K60S2.h"
	
	#define uchar unsigned char
	#define uint unsigned int
	#define ulong unsigned long

	#define uint8 unsigned char 
	#define uint16 unsigned int 
	#define uint32 unsigned long
	
  #define YJBan3_Ben3 0x11	//硬件版本号
  #define RJBan3_Ben31 0x20    //软件版本号
  #define RJBan3_Ben32 0x19    //软件版本号
  #define RJBan3_Ben33 0x01    //软件版本号
  #define RJBan3_Ben34 0x16    //软件版本号
  #define RJBan3_Ben35 0x11    //软件版本号
	
	#define ERROR	1
	#define	OK		0
	#define True	1
	#define False	0
	
	sbit CON1 =P1^7;

	sbit FMCSN=P3^7;

	sbit LED = P2^5;
	#define Open_LED  LED = 1
	#define Close_LED LED = 0
	#define not_LED		LED = !LED
	
	sbit LED2 = P2^6;
	#define Open_LED2  	LED2 = 1
	#define Close_LED2 	LED2 = 0
	#define not_LED2	 	LED2 = !LED2
	
	sbit LED3 = P2^7;
	#define Open_LED3  	LED3 = 1
	#define Close_LED3 	LED3 = 0
	#define not_LED3		LED3 = !LED3
	
	sbit REST_BT 	=	P0^2;//42Pin
	sbit AT_BT 		=	P0^4;//44Pin
	sbit STAT_BT 	=	P0^5;//1Pin
	
	#define R_XOR 0
	#define Time0_COM 1
	#define RB_ID 2		 //接收到包尾标识标志位
	#define RH_ID 3		 //接收到包头标识标志位
	#define _XY_ 4        //本机串口协议标志位
	#define first 5 //
	#define finish05 6
	#define finish01 7
	#define R_ACCfinish 8
	
	#define UART1 1
	#define rs_Max_Len1 200
	extern uchar xdata receive_data1[];//[rs_Max_Len1];
	extern uint receive_number1;//=0
	extern uint R_ACC1;//=0;
	extern uchar UART1_rs[9];
	
	#define UART2 1
	#define rs_Max_Len2 200
	extern uchar xdata receive_data2[];//[rs_Max_Len2];
	extern uint receive_number2;//=0
	extern uint R_ACC2;//=0;
	extern uchar UART2_rs[9];

	#define UART3 0
	#define UART4 0
	
	extern uchar Time0_;//=0;//=0x00;	  //定时器中断累加器

	void Io_Init();
	extern void Delay_100us(unsigned int delay_time);
//extern void delay();

#endif
