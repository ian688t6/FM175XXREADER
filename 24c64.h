#include <intrins.h>

		 /*
sbit  ISDA=P3^5;//24C
sbit  ISCL=P3^4;			//24c64
sbit  IWP=P3^6;	   
			*/

 sbit ISDA=P3^7;//24C
sbit ISCL=P3^6;		
sbit IWP=P3^5;	


#define uchar unsigned char 
#define uint unsigned int 
#define ulong unsigned long


bit bdata SystemError;
void Delay16(unsigned int i)
{
	while(i--);
}

void I2cWait(void)		//33.8688,5.137us
{ 
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();	
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();
      _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();	
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();
 }

/*****************************************
i2c start
condition SDA 1-0 while SCL=1
******************************************/
void I2cStart(void)
{
    ISDA=1;		 //时钟保持高,数据线从高到低一次跳变,IIC通信开始
    ISCL=1;
    I2cWait();
    ISDA=0;
    I2cWait();
    ISCL=0;
}

/*****************************************
I2c sotp
condition SDA=0-1 while SCL=1
******************************************/
void I2cStop(void)//时钟保持高,数据线从低到高一次跳变,IIC通信结束
{
    ISDA=0;
	ISCL=1;
    I2cWait();
    ISDA=1;
    I2cWait();
    ISCL=0;
}


/*****************************************
I2c Wait
Wait for some time to get proper I2C timing
******************************************/
/*void I2cWait(void)		//22.1184,5.4us
{ 
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();
}*/	  
 	


void slave_ACK(void)//从机发送应答位
 {
   ISDA=0;
   ISCL=1;
   I2cWait();
   ISDA=1;
   ISCL=0;
 }
/*
void slave_NOACK(void)////从机发送非应答位,迫使数据传输结束
 {
   ISDA=1;
   ISCL=1;
   I2cWait();
   ISDA=0;
   ISCL=0;
 }

void check_ACK(void)  //主机应答位检查
 {
   ISDA=1;
   ISCL=1;
   F0=0;
   if(ISDA==1)	   //
    F0=1;
   ISCL=0;      
 }
  */
 /*
void IICSendByte(unsigned char ch)//发送一字节
 {
   uchar idata n=8;
   while(n--)
    {
	  if(ch&0x80==0x80)
	   {
	     ISDA=1;
         ISCL=1;
		 I2cWait();
		 ISDA=0;
		 ISCL=0;
		}
	  else
       {
	     ISDA=0;
		 ISCL=1;
		 I2cWait();
		 ISCL=0;
		}
	  ch=ch<<1;
	}
  }
	   */
uchar IICCreciveByte(void)	//接收一个字节
{
   uchar idata n=8;
   uchar tdata;
   while(n--)
    {
	  ISDA=1;
	  ISCL=1;
	  tdata=tdata<<1;
	  if(ISDA==1)
	   tdata=tdata|0x01;
	  else
	   tdata=tdata&0xfe;
	  ISCL=0;
	 }
	return(tdata);
}
/*void writeNByte(uchar slave_add,uchar n) //发送N个字节
 {
   uchar idata send_da,i=0;
   I2cStart();
   IICSendByte(slave_add);
   check_ACK();
   if(F0==1)
    {
	  SystemError=1;  //非应答表明器件错误或已坏
	  return;
	 }
   while(n--)
    {
	  send_da=sendbuf[i++];
	  IICSendByte(send_da);
	  check_ACK();
      if(F0==1)
       {
	     SystemError=1;  //非应答表明器件错误或已坏
	     return;
	    }
	  }
	I2cStop();
  }
void receiveNByte(uchar idata slave_add,uchar n) //接收N个字节
 {
   uchar idata receive_da,i=0;
   I2cStart();
   IICSendByte(slave_add);
   check_ACK();
   if(F0==1)
    {
	  SystemError=1;  //非应答表明器件错误或已坏
	  return;
	 } 
   while(n--)
    {
	  receive_da=IICCreciveByte();
	  receivebuf[i++]=receive_da;
	  slave_ACK();
	 }
   slave_NOACK();
   I2cStop();
 }	*/
	   
/*****************************************
I2c Init
Initialize I2C interface
Release I2c BUS
******************************************/
/*void I2cInit(void)
{
    ISDA=1;
    ISCL=1;
}		*/


/*****************************************
I2c SentByte
master transfer data to slave and return acknowledge bit
don't include<intrins.h>
******************************************/

uchar I2cSentByte(uchar bytedata)
{
    uchar i;
    uchar ack;

    for(i=0;i<8;i++)
    {
       if(bytedata & 0x80)
           ISDA=1;
       else
           ISDA=0;
       bytedata<<=1;
       I2cWait();
 
       ISCL=1;
       I2cWait();
       ISCL=0;
       I2cWait();
    }

    ISDA=1;	   
   // I2cWait();
    ISCL=1;
    I2cWait();
    ack=ISDA;
    ISCL=0;
    I2cWait();
    return ack;
}
	 

/*****************************************
I2c ReceiveByte
slave trransfer data to master
******************************************/
/*uchar I2cReceiveByte(void)
{
    uchar i;
    uchar bytedata=0;

    for(i=0;i<8;i++)
    {
        ISCL=1;
        I2cWait();
        bytedata<<=1;
        if(ISDA) 
        bytedata|=0x01;
        I2cWait(); 
        ISCL=0;
   
    }
    return bytedata;
}

   */
/*****************************************
I2c SendAcknowledge
Master send acknowledge bit to slave
acknowledge="0",non-acknowledge="1"
******************************************/
/*void SendAcknowledge(bit ack)
{
    ISDA=ack;
    ISCL=1;
    I2cWait();
    ISCL=0;
}
   */
/*****************************************
24c64 project
******************************************/

/*****************************************
24c64 WriteByte
addr:0-8192
value:数据
******************************************/
void _24c64WriteByte(uint addr,uchar value)
{
    IWP=0;
    I2cStart();
    I2cSentByte(0xA0);

    I2cSentByte(addr>>8);   //送高位地址
    I2cSentByte(addr&0xff); //送地位地址
    I2cSentByte(value);
    I2cStop();


	Delay16(300);

	IWP=1;
} 
/*void _24c64WriteByte(uint addr,uchar value)
{
    I2cStart();
    IICSendByte(0xA0);
	check_ACK();
    if(F0==1)
     {
	  SystemError=1;  //非应答表明器件错误或已坏
	  return;
	  }
    IICSendByte(addr>>8);   //送高位地址
    IICSendByte(addr&0xff); //送地位地址
    if(F0==1)
     {
	  SystemError=1;  //非应答表明器件错误或已坏
	  return;
	  }
    IICSendByte(value);
    if(F0==1)
     {
	  SystemError=1;  //非应答表明器件错误或已坏
	  return;
	  }
    I2cStop();
	Delay16(2000);
}  */


/*****************************************
24c64 WriteMulti
page:0-255
count:要写入的数个数
******************************************/
	   
//uchar w24c64WriteMulti(uint addr,uchar count,uchar xdata *SenBuf)  //暂时不用
//{
//    uchar xdata i ;
//	uchar xdata arc;
//	IWP=0;
//    I2cStart();
//    I2cSentByte(0xa0);
//
//    I2cSentByte(addr>>8);   //送高位地址
//    I2cSentByte(addr&0xff); //送地位地址
//
//     for(i=0;i<count;i++)
//	  {
//       arc=I2cSentByte(SenBuf[i]);
//	   if(arc!=0)
//	    return(0);
//	  } 
//
//    I2cStop();
////	Delay16(35000);
//	Delay16(2000);
//	IWP=1;
//	return(arc);
//}	
/*****************************************
24c64 ReadByte
addr:0-8192
******************************************/
uchar _24c64ReadByte(uint addr)
  {
    uchar temp;
	uchar k;

	IWP=1;
    I2cStart();
    k=I2cSentByte(0xa0);
	if(k==1)
	 {SystemError=1;}//return;}
    k=I2cSentByte(addr>>8);   //送高位地址
	if(k==1)
	 {SystemError=1;}//return;}
    k=I2cSentByte(addr&0xff); //送地位地址
	if(k==1)
	 {SystemError=1;}//return;}
    I2cStart();
    k=I2cSentByte(0xa1);
	if(k==1)
	 {SystemError=1;}//return;}

    temp=IICCreciveByte();
    slave_ACK();
    //SendAcknowledge(1);
    I2cStop();
	Delay16(10);
    return temp;
  }