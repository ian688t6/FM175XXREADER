/*************************************************************/
//2014.06.03修改版
/*===========================================================*/
#include "STC15F2K60S2.h"
#include "intrins.h"
#include "pin.h"
//#include "fm175xx.h"
//#include "spi.h"
//#include "type_a.h"
//#include "type_b.h"
//#include "ultralight.h"
#include "mifare_card.h"
#include "uart.h"
//#include "cpu_card.h"
#include "main.h"
#include "pcd.h"
#include "uart_app.h"
//#include "24c64.h"
#include "I2C.h"
#include "BT.h"
#include "EEPROM.h"
#include "24Cxx.h"
#include "2405.h"

uchar xdata receive_data1[rs_Max_Len1]={0};
uint receive_number1=0;//
uint R_ACC1;//=0;
uchar UART1_rs[9];

uchar xdata receive_data2[rs_Max_Len2]={0};
uint receive_number2=0;//
uint R_ACC2;//=0;
uchar UART2_rs[9];

uchar S_Yadd=0;
uchar S_Madd=0;
uchar S_LSH=0;

uchar C_add=0x0a;
uchar idata MCU_ID[7];
	
unsigned  char ovt=0;

// unsigned char bjid[12]={0,0,0,0};

unsigned char mima[6]={0xff,0xff,0xff,0xff,0xff,0xff} ;
unsigned char mdata[16*16+1]="操作员: ";
unsigned char mima2[6]={0xc8,0xf0,0xb7,0xe5,0x18,0x13} ;
unsigned char rxbuf[33] ;
//uchar ly[33] = "蓝牙周";//{0xc0,0xb6,0xd1,0xc0,0xd6,0xdc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
#define open_box_cmd_add 0x0000 //
uchar open_box_cmd[33];// = "直接盒";//{0xd6,0xb1,0xbd,0xd3,0xba,0xd0};//,0x05,0x06,0x07,0x08,0x09,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x01,0x02,0x1f};
uchar ty[100];
uchar SN=0;
uchar _1a_flag=0;//发送就绪
uchar _1a_flag_17xx_EN=0;
uchar _1a_flag_17xx=0;
uchar _1a_flag_11xx=0;
uchar Time0_=0;//=0x00;	  //定时器中断累加器
	
bit GPRS_flag=0;

uchar OPEN_EN_17xx=0;//0x00:关闭,不可开箱  0x01:开启,可以开箱
uchar OPEN_EN_11xx=0;

uchar ICC_type=0;//卡类型

uchar User_type=0;//用户类型 0:点钞员   1:复合员

uchar Hex[]="0123456789ABCDEF";
	
unsigned char go_flag=0;			//串口数据状态标志位

void Io_Init();
void Delay_100us(unsigned int delay_time);
void Variable_Init(void);

void WDT_rest(void)
{
	WDT_CONTR=0x35;
}
uchar X_or(uchar *pz,uint Len)//pz异或首地址,Len数据长度(不大于65535),带回异或值
{
  uchar uXor=0;//
  while(Len--)
  {
    uXor^=*pz++;
  }
  return uXor;
}
unsigned int crc16_char(unsigned int gx, unsigned char Data)//OK 
{
//x^16+x^15+x^2+x^0=1 1000 0000 0000 0101=0x18005 
  unsigned int  crc=0;
  unsigned char i;
	for(i=0;i<8;i++)
	{
		if((crc^((unsigned int)(Data)<<8))&0x8000) //反转时 crc&0x0001
			crc=(crc<<=1)^gx; //反转时 crc=(crc>>=1)^gx
		else
			crc<<=1; //反转时 crc>>=1
		Data<<=1;
	}
  return (crc);
}
unsigned int crc16_string(unsigned int gx, unsigned char *pt, unsigned int Len)//OK
{
  unsigned int  crc=0;
  while(Len--)
  {	
    crc=(crc<<8)^crc16_char(gx,(crc>>8)^*pt++);//右移
//    crc=(crc>>8)^crc16_char(gx,(crc&0xff)^*pt++);//左移
//    crc=GetCRC16(crc,*pt++);//查表
  }	
  return crc;
}
void Delay(unsigned int t)
{
	unsigned int x,y;
	for(x=t;x>0;x--)
		for(y=110;y>0;y--);
}
#if 0
unsigned char wrm1(char key,char *miyao,char bank,char sector,char *sj) //写M1卡，传入的三个参数key:A 0秘钥认证还是B 1秘钥认证；
//miyao:认证的密码。6字节
//bank：块号
//sector:扇区号
//sj:需要写入的数据 16字节
{		//send(0x33);
	if(bank>3 || sector > 15)
		return 0;
	bank += 4*sector;
	Set_Rf(3);   //turn on radio 打开射频输出
	Pcd_ConfigISOType(0);
		// send(0x44);
	send_buff.buff[0]=TypeA_CardActivate(&PICC_A);	 //激活选卡
	   	// send(0x55);
	if(send_buff.buff[0]==OK)
	{ 
		if(Mifare_Auth(key,sector,miyao,PICC_A.UID)==OK)//认证 第二个扇区的密码
		{ 
			if( Mifare_Blockwrite(bank,sj)==OK)
			{
				Set_Rf(0);
				return 1;
			}
			else
			{
				Set_Rf(0);
				return 0;
			}
		}
		else
		{
			Set_Rf(0);  //关闭射频输出
			return 0;
		}
	}
	else
	{
		Set_Rf(0);  //关闭射频输出
	 // send(0x66);
		return 0;
	}
}           																

char rem1(char key,unsigned char *miyao,char bank,char sector,char *js)//功能：读M1卡;参数：key:认证模式，miyao 6字节密码，bank 块号，sector:扇区号
{
	if(bank>3 || sector > 15)
		return 0;
	bank += 4*sector;
	Set_Rf(3);   //turn on radio
	Pcd_ConfigISOType(0);
	send_buff.buff[0]=TypeA_CardActivate(&PICC_A);
	if(send_buff.buff[0]==OK)
	{
		if(Mifare_Auth(key,sector,miyao,PICC_A.UID)==OK)//认证 第二个扇区的密码
		{
			if(Mifare_Blockread(bank,js)==OK)
			{
				Set_Rf(0);
				return 1;
			}
			else
			{
				Set_Rf(0);
				return 0;
			}
		}
		else
		{
			Set_Rf(0);
			return 0;
		}
	}
	else
	{
		Set_Rf(0);
		return 0;
	}
}
unsigned char rem1_sector(char key,unsigned char *miyao,char bank,char sector,char *js,unsigned int Len)//可以跨块,可以跨扇区,但要各扇区密码相同
{
	unsigned int i;
	for(i=0;i<Len/16;i++)
	{
		if(0 == rem1(key,miyao,(bank+i)%4,sector+(bank+i)/4,js+16*i))
		{
			break;
		}
	}
	if((Len/16) == i)
	{
		if(Len%16)
		{
			return(rem1(key,miyao,(bank+i)%4,sector+(bank+i)/4,js+16*i));
		}
		return 1;
	}
	else
	{
		return 0;
	}
}
#endif

void UART_(unsigned char *receive_data,unsigned char *buf,unsigned char uartx)
{
  uchar Data;
  uint receive_number,R_ACC;
	uint rece_len;
  switch(uartx)
  {
   #if UART1==1
    case 1:
    Data=SBUF;
    receive_number=receive_number1;
	rece_len = rece_buff.length;
    R_ACC=R_ACC1;
		//Send_char1(Data);
    break;
   #endif
   
   #if UART2==1
    case 2:
    Data=S2BUF;
    receive_number=receive_number2;
    R_ACC=R_ACC2;
		//Send_char1(Data);return;
		/**/
		if(0 == AT_BT)
		{
			receive_data[receive_number++]=Data;
			receive_number2=receive_number;
			buf[RH_ID]=1;
			buf[RB_ID]=0;
			buf[Time0_COM]=0x00;
			return;
		}
		/**/
    break;
   #endif
    
   #if UART3==1
    case 3:
    Data=S3BUF;
    receive_number=receive_number3;
    R_ACC=R_ACC3;
    break;
   #endif
    
   #if UART4==1
    case 4:
    Data=S4BUF;
    receive_number=receive_number4;
    R_ACC=R_ACC4;
    break;
   #endif
   
   default:return;
  }

start_uart:
  rece_buff.buff[rece_len] = Data;
  receive_data[receive_number] = rece_buff.buff[rece_len]; 
  buf[R_XOR]^=receive_data[receive_number];
  if(buf[R_ACCfinish]==0)
    R_ACC+=receive_data[receive_number];
  receive_number++;
	rece_len ++;
  if(1 == receive_number)
  {
    buf[R_XOR]=receive_data[0]^0x00;
    R_ACC=receive_data[0]+0x00;
    buf[R_ACCfinish]=0;
    buf[RH_ID]=1;
    buf[RB_ID]=0;
    buf[Time0_COM]=0x00;
    switch(receive_data[0])
    {    
      case 0x1e:buf[_XY_]=0x1e;break;
      default:buf[RH_ID]=0;receive_number=0;break;
    }
  }
  else if(2 == receive_number)
  {
    if(0x1e == receive_data[0] && 0x60 == receive_data[1])
    { 
      buf[_XY_]=0x1e;
    }
    else
    {
      receive_number=0;
      goto start_uart;
    }
  }
  else if(0x1e == buf[_XY_])//1e 60
  {
	#if 0
    if(3 == receive_number)
    {
      /***
      if(receive_data[2]!=0xff&&receive_data[2]!=C_add)//判断消息的目标地址
      {
        receive_number=0x00;
        goto start_uart;
      }
      ***/
    }
    else if(4 == receive_number)
    {
      /**
      if((receive_data[2]==0xff||receive_data[2]==C_Address3||receive_data[2]==C_Address4)&&receive_data[3]==0x03)//屏蔽中控器发出的部分数据
      {
        receive_number=0x00;
        goto start_uart;
      }
      **/
    }
    else if(5 == receive_number)
    {
      /**if(
        receive_data[4]!=0x01&&//查找设备
        receive_data[4]!=0x02&&//校时
        receive_data[4]!=0x05&&//应答
        receive_data[4]!=0x06&&//下载
        receive_data[4]!=0x07  //实时
        )//判断是否是所需要的消息
      {
        receive_number=0x00;
        goto start_uart;
      }**/
    }
  #endif
		if(receive_number>9)//是否接收到最小长度
    {
      if((((uint)(receive_data[6])<<8)+receive_data[7]==receive_number-10))//判断长度是否正确
      {
        if(buf[R_XOR]==0x1f)//包尾与XOR是否正确
        {
					buf[RB_ID]=1;
        }
        else
        {
          Send_char1(buf[R_XOR]^0x1f^receive_data[receive_number-2]);
          buf[RB_ID]=0;
        }
        buf[RH_ID]=0;
        receive_number=0;
        buf[Time0_COM]=0x00;
      }
      else
      {
        buf[RB_ID]=0;
      }
    }
  }
  else
  {
    receive_number=0;
  }

  switch(uartx)
  {
   #if UART1==1
    case 1:
    if(buf[RB_ID]==1)
      ES=0;//不接收
    receive_number1=receive_number;
	rece_buff.length=rece_len;
    R_ACC1=R_ACC;
    break;
   #endif
   
   #if UART2==1
    case 2:
    if(buf[RB_ID]==1)
      IE2 &= ~0x01;//不接收
    receive_number2=receive_number;
    R_ACC2=R_ACC;
    break;
   #endif
   
   #if UART3==1
    case 3:
    if(buf[RB_ID]==1)
      IE2 &= ~0x08;//不接收
    receive_number3=receive_number;
    R_ACC3=R_ACC;
    break;
   #endif
   
   #if UART4==1
    case 4:
    if(buf[RB_ID]==1)
      IE2 &= ~0x10;//不接收
    receive_number4=receive_number;
    R_ACC4=R_ACC;
    break;
   #endif

    default:return;
  }
}

void uart1_inte() interrupt 4	//point,data_length
{
  if(TI)
    TI=0;
  if(RI)
  {  //bz=1;
    RI=0;
		if(receive_number1>=rs_Max_Len1)
			receive_number1=0;
		UART_(receive_data1,UART1_rs,1);
		uart_status.rece_finish=True;
		uart_status.rece_error=False;
	}
}

void uart2_inte() interrupt 8
{
	if (S2CON & S2RI)
  {
    S2CON &= ~S2RI;         //清除S2RI位
    //P0 = S2BUF;             //P0显示串口数据
    //P2 = (S2CON & S2RB8);   //P2.2显示校验位
		if(receive_number2>=rs_Max_Len2)
			receive_number2=0;
    UART_(receive_data2,UART2_rs,2);
  }
  if (S2CON & S2TI)
  {
    S2CON &= ~S2TI;         //清除S2TI位
  }
}

void fmq()
{
	P34=0;
	Delay(2000);
	P34=1;
}

void Timer0Init(void)		//50毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x4C;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
  ET0=1;
  EA=1;//开总中断
}
void time0(void) interrupt 1  //50ms中断
{
	Time0_++;

	if(UART1_rs[RH_ID])
	{
		UART1_rs[Time0_COM]++;
		if(UART1_rs[Time0_COM]>=20)//50ms*20
		{
			Send_char1((receive_number1>>8)&0xff);
			Send_char1((receive_number1>>0)&0xff);
			//Send_string1(receive_data1,receive_number1);
			UART1_rs[RB_ID]=0;
			UART1_rs[RH_ID]=0;
			receive_number1=0;
			UART1_rs[Time0_COM]=0x00;
			Send_char1(0xE1);
		}
	}
	if(UART2_rs[RH_ID])
	{
		UART2_rs[Time0_COM]++;
		if(UART2_rs[Time0_COM]>=20)//50ms*20
		{
			Send_char1((receive_number2>>8)&0xff);
			Send_char1((receive_number2>>0)&0xff);
			//Send_string1(receive_data2,receive_number2);
			UART2_rs[RB_ID]=0;
			UART2_rs[RH_ID]=0;
			receive_number2=0;
			UART2_rs[Time0_COM]=0x00;
			Send_char1(0xE2);
		}
	}
		
	if(1 == STAT_BT)
	{
		GPRS_flag=1;
	}
	else
	{
		GPRS_flag=0;
		if(1 == OPEN_EN_17xx)
			OPEN_EN_17xx=0;
		if(1 == OPEN_EN_11xx)
			OPEN_EN_11xx=0;
	}
	
//	OPEN_EN_17xx=1;
//	OPEN_EN_11xx=1;
	
  if(Time0_%10 == 0)
  {
    not_LED;
		if(1 == STAT_BT)
		{
			if(0 == User_type)//点钞员
			{
				Close_LED3;
				if((0 == OPEN_EN_17xx) && (0 == OPEN_EN_11xx))
					Open_LED2;
				else
					not_LED2;
			}
			else if(1 == User_type)//复合员
			{
				Close_LED2;
				if(0 == OPEN_EN_17xx)
					Open_LED3;
				else
					not_LED3;
			}
		}
		else
		{
			Open_LED2;
			Open_LED3;
		}
  }
	
	if(Time0_>=200)	 
    Time0_=0x00;
	
	ovt++;
	if(ovt>80)
	 ovt=0;
}

unsigned char UARTn(unsigned char *r_data ,unsigned char BJXY , unsigned char uartx)
{
  uint i,j;
  uchar m,n;
  uchar r_Hadd;
  r_Hadd=r_data[2];
  F0=0;
  if(0x1e == BJXY)
  {
    i=8;
		if(2 == uartx)
		{
			Send_string1(r_data,((uint)(r_data[6])<<8)+r_data[7]+10);
		}
    switch (r_data[4])
    {           //1e 60 90 60 01 90 00 00 1f 1f
      case 0x01://1E 60 05 01 01 05 00 00 7e 1f硬件版本,软件版本
      {
        r_data[i++]=0x09,r_data[i++]=0x00,r_data[i++]=0x04;
        r_data[i++]='V',r_data[i++]='e',r_data[i++]='r';
        
        r_data[i++]=YJBan3_Ben3;//硬件版本
				if(r_Hadd==C_add&&r_data[3]==0xff)
				{
          r_data[i++]='M',r_data[i++]='C',r_data[i++]='U',r_data[i++]='_',r_data[i++]='I',r_data[i++]='D';
					for(m=0;m<7;m++)
				  {
            r_data[i++]=MCU_ID[m];
				  }
				}
				r_data[10]=i-11;
        r_data[i++]=0x0A,r_data[i++]=0x00,r_data[i++]=0x05;
        r_data[i++]=RJBan3_Ben31;r_data[i++]=RJBan3_Ben32;r_data[i++]=RJBan3_Ben33;r_data[i++]=RJBan3_Ben34;r_data[i++]=RJBan3_Ben35;//软件版本

	      switch(uartx)
        {
         #if UART1==1
          case 1:
            UART1_rs[finish01]=1;
          break;
         #endif
          
         #if UART2==1
          case 2:
            UART2_rs[finish01]=1;
          break;
         #endif
         
         #if UART3==1
          case 3:
            UART3_rs[finish01]=1;
          break;
         #endif
         
         #if UART4==1
          case 4:
            UART4_rs[finish01]=1;
          break;
         #endif
         
         default:return 2;
        }
				if(r_Hadd==0xff&&r_data[3]==0x01)
				{
          r_data[2]=r_data[3];
					r_data[3]=0x10;
					r_data[4]=0x05;//目的地址与源地址对调,消息帧为0x05
		      r_data[6]=0x00/*(i-8)>>8*/,r_data[7]=i-8;//写入数据长度
		      r_data[i++]=X_or(r_data,i/*((uint)(r_data[6])<<8)+r_data[7]+8*/);//计算异或值,
					r_data[i++]=0x1f;//写入包尾
		      switch(uartx)
          {
           #if UART1==1
            case 1:
              Send_string1(r_data,i);//发送应答
            break;
           #endif
            
           #if UART2==1
            case 2:
              Send_string2(r_data,i);//发送应答
            break;
           #endif
           
           #if UART3==1
            case 3:
              Send_string3(r_data,i);//发送应答
            break;
           #endif
           
           #if UART4==1
            case 4:
              Send_string4(r_data,i);//发送应答
            break;
           #endif
           
           default:return 2;
          }
					return 1;
				}
      }
        break;
			
      case 0x05://消息回复
			{
				//00 01 02 03 04 05 06 07 08 09 10 11 12 13 14
				//1e 60 05 03 05 05 00 04 0c 00 01 01 xor 1f
				//1E 60 05 03 05 03 00 04 0C 00 01 01 76 1F
				//1e 60 05 03 05 05 00 00 xor 1f
				//1E 60 05 01 05 05 00 00 7a 1f
				//
				if(/**/r_data[2]==S_Yadd&&//上次发送的源地址,此次收到的目标地址
					 r_data[3]==S_Madd&&//上次发送的目标地址,此次收到的源地址
					 r_data[5]==S_LSH&&//流水号
					 /**/
					 (
					 (r_data[6]==0x00&&r_data[7]==0x00)||//简化回复成功
					 (r_data[6]==0x00&&r_data[7]==0x04&&r_data[8]==0x0c&&r_data[9]==0x00&&r_data[10]==0x01&&r_data[11]==0x01)||//标准回复成功
					 (r_data[8]==0x09&&r_data[8+r_data[10]+3]==0x0a)||//硬件版本,软件版本
					 0
					 )&&
					 1
					)
				{
					switch(uartx)
					{
					 #if UART1==1
						case 1:
							UART1_rs[finish05]=1;
						break;
					 #endif
						
					 #if UART2==1
						case 2:
							UART2_rs[finish05]=1;
							_1a_flag=0;
						break;
					 #endif
					 
					 #if UART3==1
						case 3:
							UART3_rs[finish05]=1;
						break;
					 #endif
					 
					 #if UART4==1
						case 4:
							UART4_rs[finish05]=1;
						break;
					 #endif
					 
					 default:return 2;
					}
				}
				return 1;
				//r_data[2]=0xFF;
			}
        break;
			
      case 0x06://消息帧,数据下载
			{
				//1e 60 30 01 06 30 00 03 00 00 00 7A 1f
				//1E 60 10 01 06 01 00 3C 11 00 39 00 00 01 02 05 41 32 31 C2 B7 B9 AB BD BB B3 B5 B3 B5 CC E5 B9 E3 B8 E6 20 20 20 20 20 D5 D0 D7 E2 B5 E7 BB B0 A3 BA 20 20 20 20 20 20 20 31 33 35 32 35 37 32 34 31 31 38 3D 1F
				if(0x00 == r_data[8]) //程序下载,除源地址外其它必须相吻合
				{
					if(r_data[5]==C_add&&r_data[6]==0x00&&r_data[7]==0x03&&r_data[9]==0x00&&r_data[10]==0x00)
						F0=1;
					else
						F0=0;
				}
//00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20
//1e 60 0a 01 06 01 00 09 6f 00 06 d6 b1 bd d3 ba d0 71 1f //直接盒
//1e 60 0a 01 06 01 00 09 6f 00 06 c0 b6 d1 c0 d6 dc 00 1f //蓝牙周
				else if('o' == r_data[8])//open_box_cmd 命令中不能有0x00与0xff
				{
					E2prom(Sector_Erase,open_box_cmd_add);
					for(j=0;j<(((uint)(r_data[9])<<8)+r_data[10])%33;j++)
					{
						open_box_cmd[j]=ISP_DATA=r_data[11+j];
						E2prom(Byte_Write,open_box_cmd_add+j);
					}
					ISP_DATA='\0';
					E2prom(Byte_Write,open_box_cmd_add+j);
				}
			}
        break;

			case 0x08:
			{
				if(0x3d == r_data[8])
				{
/**
00 01 02 03 04 05 06 07 08 09 10 11 12 13
1E 60 0A 01 08 06 00 04 3d 00 01 00 43 1F//关闭蓝牙开箱器
1E 60 0A 01 08 06 00 04 3d 00 01 01 42 1F//:开启蓝牙开箱器
1.	蓝牙开箱器开机默认为关闭状态
2.	在蓝牙每一次连接后,平板发送开启命令
3.	开箱后蓝牙开箱器自动关闭
4.	平板上点提交数据成功后再次发送开启命令
5.	蓝牙断开连接后,开箱器自动变为关闭状态
**/
					Send_char1(r_data[8]);
					Send_char1(r_data[11]);
					Send_char1(_1a_flag_17xx);
					Send_char1(_1a_flag_11xx);
					if(0 == r_data[11])
					{
						if(1 == _1a_flag_17xx)
						{
							_1a_flag_17xx=0;
							OPEN_EN_17xx=r_data[11];
						}
						if(1 == _1a_flag_11xx)
						{
							_1a_flag_11xx=0;
							OPEN_EN_11xx=r_data[11];
						}
					}
					else
					{
						OPEN_EN_17xx=r_data[11];
						OPEN_EN_11xx=r_data[11];
					}
				}
				else if(0x3f == r_data[8])
				{
/**
1E 60 0A 01 08 06 00 04 3f 00 01 00 40 1F//点钞员
1E 60 0A 01 08 06 00 04 3f 00 01 01 40 1F//复合员
**/
					User_type=r_data[11];
					if(0 == User_type)
						Send_string1("\n点钞员\n",0);
					else if(1 == User_type)
						Send_string1("\n复合员\n",0);
				}
			}
				break;

			default:
			{
				r_data[i++]=0x0c;//应答数据帧
				r_data[i++]=0x00,r_data[i++]=0x01;//长度
				r_data[i++]=0x03;//命令重发
			}
				break;
    }
    if(r_data[2]==C_add||(r_Hadd!=0xff&&r_data[3]==0x01))//C_add)//判断是否应答
    {
      r_data[2]=r_data[3];
			r_data[3]=r_Hadd;
			r_data[4]=0x05;//目的地址与源地址对调,消息帧为0x05
      r_data[6]=0x00/*(i-8)>>8*/,r_data[7]=i-8;//写入数据长度
      r_data[i++]=X_or(r_data,i/*((uint)(r_data[6])<<8)+r_data[7]+8*/);
			r_data[i++]=0x1f;//计算异或值,写入包尾
      switch(uartx)
      {
       #if UART1==1
        case 1:
          Send_string1(r_data,i);//发送应答
        break;
       #endif
        
       #if UART2==1
        case 2:
					Send_string1(r_data,i);
          Send_string2(r_data,i);//发送应答
        break;
       #endif
       
       #if UART3==1
        case 3:
          Send_string3(r_data,i);//发送应答
        break;
       #endif
       
       #if UART4==1
        case 4:
          Send_string4(r_data,i);//发送应答
        break;
       #endif
       
       default:return 2;
      }
#ifndef MCU_AT89Sxx
      if(F0)
      {
        Time0_=0;//
        while(Time0_<10);
        ISP_CONTR=0x60;//复位到ISP区
      }
#endif
      return r_data[i-2];
    }
    else
    {
      if(i>8)
        return r_data[i-1];
      else 
        return 1;
    }
  }
  else
	{
		return 0;
	}
}
void rest_FM17xx(void)
{
	NPD=0;//1.1/1.2
	P16=0;//1.0
	Delay(100);
	NPD=1;
	P16=1;
	Delay(100);
}

void Main(void)
{
//	unsigned int i,j;
//	uchar m;
//	uchar idata *ipt=0xf1;
//	uchar code *cpt=0xbff9;
//	bit FM17xx=0;
//	bit FM11xx=0;
//	uchar FM17_Err=0;
//	uchar FM17_Err2=0;
//	uchar FM11_Err=0;
//	uchar FM11_War=0;
//	uchar Send_buf[100];
//	uchar open_box_old_cmd[32]= "蓝牙周";
//	uchar open_box_old_cmd[32]= "直接盒";
//	uint crc;
	
	Io_Init();
	Variable_Init();
//	SPI_Init();
	Uart1_Init();
	Send_string1("\n编译时间2019年3月26日\n",0);

	PcdInit();
	PcdAntennaOff(); 
	PcdAntennaOn(); 
//	Uart2_Init();
//	Timer0Init();
//	FM11_Init();
//	BT_Init();
	
	WDT_rest();
	
  	while (1) {
		WDT_rest();
		Uart_App();
	
	}
	return;
}


/*****************************/
/*函数介绍：	管脚初始化程序
/*输入参数：	无
/*输出参数：	无
/*返回值：		无
/*占用资源：	无
/*****************************/
void Io_Init()
{
	P0 = P1 = P2 = P3 = 0xFF;
	P0M1=0x00	;
	P0M0=0x00	;
	P1M1=0x00	;
	P1M0=0x00	;
	P2M1=0x00	;
	P2M0=0x00	;
	P3M1=0x00	;
	P3M0=0x00	;
	P4M1=0x00	;
	P4M0=0x00	;
	
	P0M0 |= (1<<2);//P0^2;//42Pin
	REST_BT=0;
	
	P0M0 |= (1<<4);//P0^4;//44Pin
	AT_BT=1;
	
	P1M0 |= (1<<2);//P1.2
	SPI_CS = 1;
	
	P1M0 |= (1<<3);
	P1M0 |= (1<<4);
	P1M0 |= (1<<5);
	
	P1M0 |= (1<<6);
	P16=1;
	
	P3M0 |= (1<<3);
	NPD=1;//P3.3
	
	LED = 0;// P2^5;
	P2M0 |= (1<<5);
	
	LED2 = 0;// P2^6;
	P2M0 |= (1<<6);
	
	LED3 = 0;// P2^7;
	P2M0 |= (1<<7);
	
	CON1=0;// P1^7;
	P1M0 |= (1<<7);
	
}
/*************************/
/*函数介绍：	延时函数
/*输入参数：	delay_time
/*输出参数：	无
/*返回值：		无
/*占用资源：	无
/*************************/
void Delay_100us(unsigned int delay_time)		//0.1ms*delay_time
{
	unsigned int i;
	for(i=0;i<delay_time;i++)
	{
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); 
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_();	_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_();	_nop_(); _nop_(); _nop_(); _nop_();	_nop_(); _nop_();
	}
}

void Variable_Init(void)
{
	send_buff.buff=uart_data;
	rece_buff.buff=uart_data+128;
	uart_status.rece_finish=False;
	uart_status.send_finish=False;
	uart_status.rece_error=False;
	uart_status.send_error=False;
}


