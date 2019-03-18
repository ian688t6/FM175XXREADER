#include <intrins.h>
#include "STC15F2K60S2.h"
#include "main.h"
#include "pcd.h"
#include "spi.h"
#include "uart.h"
#include <string.h>

#define u8 unsigned char
#define u16 unsigned int

u8 fcnt;				//响应时间
unsigned char idata PCB = 0x0A;					//CPU卡APDU指令分组号

void delay_us(unsigned int x)
{
	while(x)
	{
		_nop_();
		x--;
	}
		
}

						
//*************************************************************************
// 函数名	：RC522_Init
// 描述		：初始化引脚
// 入口		：无
// 出口		：无
// 返回		：无
//*************************************************************************
void PcdInit(void)
{		
	PcdReset();
	PcdConfigISOType('A');
	fcnt = 0;
}

//*************************************************************************
// 函数名	：PcdSwitchPCB(void)
// 描述		：切换分组号
// 入口		：
// 出口		：
// 返回		：成功返回MI_OK
//*************************************************************************

void PcdSwitchPCB(void)
{
	switch(PCB)
	{
		case 0x00:
			PCB=0x0A;
			break;
		case 0x0A:
			PCB=0x0B;
			break;
		case 0x0B:
			PCB=0x0A;
			break;
		default:
			PCB=0x0A;
			break;
	}
}

//*************************************************************************
// 函数名	：PcdConfigISOType(u8 type)
// 描述		：设置RC522的工作方式
// 入口		：type[IN]:卡片类型
// 出口		：无
// 返回		：成功返回MI_OK
//*************************************************************************
s8 PcdConfigISOType(u8 type)
{	
	s8 status = MI_ERR;
	if(type == 'A')
	{
		status = MI_OK;
		ClearBitMask(Status2Reg,0x08);	//状态2寄存器
		WriteRawRC(ModeReg,0x3D);			//和Mifare卡通讯,CRC初始值0x6363
		WriteRawRC(RxSelReg,0x86);		//选择内部接收器设置,内部模拟部分调制信号,发送数据后,延时6个位时钟,接收	 10
		WriteRawRC(RFCfgReg,0x7F);		//配置接收器,48dB最大增益  
	    WriteRawRC(TReloadRegL,0x30);   	//定时器的低8位数据 
	    WriteRawRC(TReloadRegH,0x00);		//定时器的高8位数据,实际值是0xD3E这部分主要是设置定时器寄存器
    	WriteRawRC(TModeReg,0x8D); 		//实际值是0X0D3E这部分主要是设置定时器寄存器TPrescaler 分频数为0xD0	  //8D
		WriteRawRC(TPrescalerReg,0x3E);	
		delay_us(1000);
		PcdAntennaOn();
	}
	return status;
}

//*************************************************************************
// 函数名	：WriteRawRC
// 描述		：写RC522寄存器
// 入口		：Address[IN]:寄存器地址
// 			  value:写入数值
// 出口		：无
// 返回		：无
//*************************************************************************
void WriteRawRC(u8 Address, u8 value)
{  
    u8 i, ucAddr;

    SCK = 0;
    NSS = 0;
    ucAddr = ((Address<<1)&0x7E);

    for(i=0;i<8;i++)								//先发送地址
    {	 
		MOSI = ((ucAddr&0x80)==0x80);
        SCK = 1;
        ucAddr <<= 1;
        SCK = 0;	   
    }

    for(i=0;i<8;i++)
    {	
        MOSI = ((value&0x80)==0x80);				//再发送操作数
        SCK = 1;
        value <<= 1;
        SCK = 0;
    }
    NSS = 1;
    SCK = 1;
}
//*************************************************************************
// 函数名	：ReadRawRC
// 描述		：读RC522寄存器
// 入口		：Address[IN]:寄存器地址
// 出口		：无
// 返回		：读出的值
//*************************************************************************
u8 ReadRawRC(u8 Address)
{

     u8 i, ucAddr;
     u8 ucResult=0;

     SCK = 0;
     NSS = 0;
     ucAddr = ((Address<<1)&0x7E)|0x80; 

     for(i=0;i<8;i++)
     {
		MOSI = ((ucAddr&0x80)==0x80);
		SCK = 1;
		ucAddr <<= 1;
		SCK = 0;
     }

     for(i=0;i<8;i++)
     {
		SCK = 1;
		ucResult <<= 1;
		ucResult|=MISO&0x01;
		SCK = 0;
     }

     NSS = 1;
     SCK = 1;
     return ucResult;
}

//*************************************************************************
// 函数名	：SetBitMask
// 描述		：位置1
// 入口		：reg[IN]:寄存器地址
// 			  mask[IN]:置位值
// 出口		：无
// 返回		：无
//*************************************************************************
void SetBitMask(u8 reg,u8 mask)  
{
    u8 tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

//*************************************************************************
// 函数名	：ClearBitMask
// 描述		：位置0
// 入口		：reg[IN]:寄存器地址
// 			  mask[IN]:置位值
// 出口		：无
// 返回		：无
//*************************************************************************
void ClearBitMask(u8 reg,u8 mask)  
{
    u8 tmp = 0x00;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask); 					 	// clear bit mask
} 
//*************************************************************************
// 函数名	：PcdAntennaOff
// 描述		：关闭天线
// 入口		：无
// 出口		：无
// 返回		：无
//*************************************************************************
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);					//禁止Tx1RFEn,Tx2RFEn
}

//*************************************************************************
// 函数名	：PcdAntennaOn  
// 描述		：开启天线,每次启动或关闭天险发射之间应至少有1ms的间隔
// 入口		：无
// 出口		：无
// 返回		：无
//*************************************************************************
void PcdAntennaOn(void)
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);						//读取出发送控制寄存器
    if (!(i & 0x03))									//如果未开启,则
    {
        SetBitMask(TxControlReg, 0x03);					//开启Tx1RFEn,Tx2RFEn
    }
}

//*************************************************************************
// 函数名	：PcdComMF522  
// 描述		：通过RC522和ISO14443卡通讯
// 入口		：Command[IN]:RC522命令字
// 			  pDataIn[IN]:通过RC522发送到卡片的数据
// 			  InLenByte[IN]:发送数据的字节长度
// 			  *pOutLenBit[OUT]:返回数据的位长度
// 出口		：pDataOut[OUT]:接收到的卡片返回数据
// 返回		：无
//*************************************************************************
s8 PcdComCmd(u8 Command, u8 *pDataIn, u8 InLenByte, u8 *pDataOut, u16  *pOutLenBit)
{
    s8 status = MI_ERR;
    u8 irqEn   = 0x00;
    u8 waitFor = 0x00;
    u8 lastBits;
    u8 n;
    u16 i;
    switch(Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;							// 接受到数据及命令执行完毕	
          break;
       default:
         break;
    }
    WriteRawRC(ComIEnReg,irqEn|0x80);			    // 容许除定时器中断请求以为得所有中断请求
    ClearBitMask(ComIrqReg,0x80);					// 屏蔽位清除
    WriteRawRC(CommandReg,PCD_IDLE);				// 取消当前命令
    SetBitMask(FIFOLevelReg,0x80);					// 清除FIFO中的读写指针

    for (i=0; i<InLenByte; i++)
    {   
		WriteRawRC(FIFODataReg, pDataIn[i]);    		//数据写入FIFO
	}
    WriteRawRC(CommandReg, Command);					//写入命令,将缓冲区中的数据发送到天线,并激活自动接收器
   
    if (Command == PCD_TRANSCEIVE)						//如果命令为0C
    {    
		SetBitMask(BitFramingReg,0x80);  				//相当于启动发送STARTSENG
	}
    i = 2000;											//根据时钟频率调整，操作M1卡最大等待时间=600,操作CPU卡最大等待时间=1200
	do 
    {
         n = ReadRawRC(ComIrqReg);						//读取中断标志,检查数据返回
         i--;
    }
    while ((i!=0) && !(n&0x20) && !(n&waitFor));		// 定时器未超时,没有错误,0x01,0x30
    ClearBitMask(BitFramingReg,0x80);					// 相当于清除发送STARTSENG

    if (i!=0)											// 定时时间到，i，没有递减到0
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B))				// 判断有无出现错误标志	 Buffer溢出,位冲突,接收CRC错误,奇偶校验错误,
         {	
             status = MI_OK;							// 初始化状态 
		 	 if (n & irqEn & 0x01)						// 若是PCD_TRANSCEIVE, irq = 0x77,  定时器为0中断产生,定时器为0时为错误
			 {   
				status = MI_NOTAGERR;   				// 搜索不到卡
			 }
             if (Command == PCD_TRANSCEIVE)				// 如果是发送接收指令
             { 
               	n = ReadRawRC(FIFOLevelReg);			// 读取接收到的数据的字节数
              	lastBits = ReadRawRC(ControlReg) & 0x07;// 2-0:RxLastBits,显示接收到最后一个字节的位数
                if (lastBits)							// 若该位为0，最后整个字节有效
                {   
					*pOutLenBit = (n-1)*8 + lastBits;   //pOutLenBit记录总共收到的位数
				}
                else
                {   
					*pOutLenBit = n*8;   				//接收完整位数
				}
                if (n == 0)								//假如没有中断产生
                {   
					n = 1;   							//n置1
				}
                if (n > MAXRLEN)						// 一次最大能接受到的字节数
                {   
					n = MAXRLEN;   						//超出最大长度,只接受最大长度的值
				}
                for (i=0; i<n; i++)
                {   
					pDataOut[i] = ReadRawRC(FIFODataReg); //从FIFO读取数据   
				}
            }
         }
         else
         {   
		 	status = MI_ERR;   							//有错误
		 }
   }
   SetBitMask(ControlReg,0x80);           	//停止定时器
   WriteRawRC(CommandReg,PCD_IDLE); 		//清空指令
   return status;								//返回状态
} 

//*************************************************************************
// 函数名	：PcdReset
// 描述		：复位RC522
// 入口		：无
// 出口		：无
// 返回		：成功返回MI_OK
//*************************************************************************
s8 PcdReset(void)
{
    MFRST=0;
	P16 = 0;
	delay_us(1000);
    MFRST=1;
	P16 = 1;
	delay_us(1000);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_us(100);
	WriteRawRC(ModeReg,0x3D);            	//和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TModeReg,0x8D);			 	//定时器模式寄存器,定时器减值计数
    WriteRawRC(TReloadRegL,0x30);        	//定时器的低8位数据 
    WriteRawRC(TReloadRegH,0x00);			//定时器的高8位数据
    WriteRawRC(TPrescalerReg,0x3E);		 	//实际值是0X0D3E这部分主要是设置定时器寄存器
	WriteRawRC(TxAutoReg,0x40);			 	//必须要,设置逻辑1,强制100%ASK调制
	PCB=0x00;
    return MI_OK;						 	//定时器时间6.78M/TPrescaler(ms)
}

//*************************************************************************
// 函数名	：CalulateCRC
// 描述		：用MF522计算CRC16函数
// 入口		：pIndata[IN]:需要计算的数据
// 			  len[IN]:数据长度
// 出口		：pDataOut[OUT]:输出结果的两个字节数组
// 返回		：无
//*************************************************************************
void CalulateCRC(u8 *pIndata,u8 len,u8 *pDataOut)
{
    u8 i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);   //取消当前命令
    SetBitMask(FIFOLevelReg,0x80);	   //FlushBuffer 清除ErrReg 的标志位
    for (i=0; i<len; i++)
    	WriteRawRC(FIFODataReg, *(pIndata+i));
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));		//当CRCIRq 所有数据被处理完毕该位置位
    pDataOut[0] = ReadRawRC(CRCResultRegL);
    pDataOut[1] = ReadRawRC(CRCResultRegM);
}


//*************************************************************************
// 函数名	：PcdSelect
// 描述		：验证卡片密码
// 入口		：pSnr[IN]:卡片序列号，4字节
// 出口		：无
// 返回		：成功返回MI_OK
//*************************************************************************
s8 PcdSelect(u8 *pSnr)
{
    s8 status;
    u8 i;
    u16 unLen;
    u8 ucComMF522Buf[MAXRLEN]; 		
    
    ClearBitMask(Status2Reg,0x08);				// 清空校验成功标志	

	memset(ucComMF522Buf, 0x00, MAXRLEN);

    ucComMF522Buf[0] = PICC_ANTICOLL1;				// 防冲突
    ucComMF522Buf[1] = 0x70;						// 发送7字节 
    ucComMF522Buf[6] = 0;							//ID校验清0
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);				// 保存卡ID
    	ucComMF522Buf[6]  ^= *(pSnr+i);				// 计算校验值
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);	// 生成发送内容的CRC校验,保存到最后两个字节
    status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);	// 发送选卡,及CRC校验
    
    if ((status != MI_OK) || (unLen != 0x18))		// 返回结果正确, 并且长度为24位, 3字节,(1)卡内存储器大小+(2)CRC
	{
    	status = MI_ERR;							// 错误
	}
    return status;									// 返回结果
}
//*************************************************************************
// 函数名	：PcdAnticoll
// 描述		：防冲撞
// 入口		：pSnr[OUT]:卡片序列号，4字节
// 出口		：无
// 返回		：成功返回MI_OK
//*************************************************************************
s8 PcdAnticoll(u8 *pSnr)
{
    s8 status;
    u8 i,snr_check=0;
    u16  unLen;
    u8 ucComMF522Buf[MAXRLEN]; 
   
    ClearBitMask(Status2Reg,0x08);			// 清空校验成功标志	
    WriteRawRC(BitFramingReg,0x00);			// 最后一个字节发送所有数据
    ClearBitMask(CollReg,0x80);				// CollRegCollReg	0冲突结束后冲突位被置零
 
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
    ucComMF522Buf[0] = PICC_ANTICOLL1;			// 防冲突指令，所有位在接收到冲突后将清除
    ucComMF522Buf[1] = 0x20;					// 发送2个字节

    status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)						
    {
    	 for (i=0; i<4; i++)					// 接受卡ID
         {   
             *(pSnr+i)  = ucComMF522Buf[i];		// 0-3:ID
             snr_check ^= ucComMF522Buf[i];		// 校验ID
         }
         if (snr_check != ucComMF522Buf[i])		// 4:校验值
		 { 	 
         	status = MI_ERR;					// 校验出错
		 }
    }
	else
	{
		status = MI_ERR;
	}
    SetBitMask(CollReg,0x80);					// CollRegCollReg	在106kbps良好的防冲突情况下该位置1
	return status;								// 返回结果
}
//*************************************************************************
// 函数名	：PcdRequest
// 描述		：寻卡
// 入口		：req_code[IN]:寻卡方式, 0x52 = 寻感应区内所有符合14443A标准的卡,0x26 = 寻未进入休眠状态的卡
// 			 
// 出口		： pTagType[OUT]：卡片类型代码
//			      0x4400 = Mifare_UltraLight 
//			      0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)CPU卡
//                0x4403 = Mifare_DESFire
// 返回		：成功返回MI_OK
//*************************************************************************
s8 PcdRequest(u8 req_code,u8 *pTagType)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 

	PcdReset();
	
	ClearBitMask(Status2Reg,0x08);			// 清空校验成功标志,清除MFCrypto1On位
	WriteRawRC(BitFramingReg,0x07);			// StartSend =0;RxAlign=0定义最后一个字节发送的位数,发送7个位
	SetBitMask(TxControlReg,0x03);			// 两天线发射信号,Tx1RFEn,Tx2RFEn置1
 
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
	ucComMF522Buf[0] = req_code;				//寻卡方式,所有卡还是其他什么卡

	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);// 将收到的卡片类型号保存

	if(status == MI_OK)
	{
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}	 
	else
	{
		status = MI_ERR;
	}
	return status;								//返回结果
}

//*************************************************************************
// 函数名	：PcdRats
// 描述		：转入APDU命令格式
// 入口		： 无
// 出口		： DataOut 输出的数据，	Len 输出数据的长度
// 返回		：成功返回MI_OK
//*************************************************************************

s8 PcdRats(u8 * DataOut,u8 * Len)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);	// 清空校验成功标志,清除MFCrypto1On位

	memset(ucComMF522Buf, 0x00, MAXRLEN);

	ucComMF522Buf[0] = 0xE0;		
	ucComMF522Buf[1] = 0x51;				

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);	// 生成发送内容的CRC校验,保存到最后两个字节
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,4,DataOut,&unLen);// 将收到的卡片类型号保存

	if (status == MI_OK)
	{
		Len[0]= unLen/8-2;
		status = MI_OK;
	}	 
	else
		status = MI_ERR;	 
	return status;								//返回结果
}

//*************************************************************************
// 函数名	：GetCard()
// 描述		：IC卡检测,并返回数据
// 入口		：req_code[IN]:寻卡方式, 0x52 = 寻感应区内所有符合14443A标准的卡,0x26 = 寻未进入休眠状态的卡
// 出口		：pTagType[OUT]：卡片类型代码
//			      	0x4400 = Mifare_UltraLight 
//			      	0x0400 = Mifare_One(S50)
//                	0x0200 = Mifare_One(S70)
//               	0x0800 = Mifare_Pro(X)
//                	0x4403 = Mifare_DESFire
//			  		snr[OUT]:IC卡ID
// 返回		：成功返回 ST_OK
//*************************************************************************
u16 GetCard(u8 Reqcode, u8* pTagType, u8* pSnr)
{
	s8 status ;
	status = PcdRequest(Reqcode, pTagType);  				//寻卡
	if(status)
	{
		status = PcdRequest(Reqcode, pTagType);

		if(status)
		{
			return ST_ERR;
		}
	}

	status = PcdAnticoll(pSnr);								//防冲撞, 获取ID
	if(status)
	{
		return ST_ERR;
	}

	status = PcdSelect(pSnr);								//选择卡片
	if(status)
	{
		return ST_ERR;
	}
	return ST_OK;

}
//*************************************************************************
// 函数名	：CpuReset()
// 描述		：CPU卡专用复位
// 入口		：无
// 出口		：Data_Out 输出的复位数据信息及长度
// 返回		：成功返回9000
//*************************************************************************

u16 CardReset(u8 * Data_Out,u8 *  Len)
{
	s8 status = MI_OK;	
	status = PcdRats(Data_Out,Len);										//卡片复位
	if(status)
	{
		return ST_ERR;
	}
	else

		return ST_OK;
	
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//*************************************************************************
// 函数名	：Pcd_Cmd
// 描述		：执行上位机发来的指令
// 入口		：pDataIn : 要执行的指令 In_Len  指令数据长度					
// 出口		：pDataOut：输出执行后的返回数据   Out_Len输出的数据长度
// 返回		：MI_OK
//*************************************************************************
 
u8 Pcd_Cmd(u8* pDataIn, u8  In_Len, u8* pDataOut,u8 * Out_Len)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	u8 i;

	ClearBitMask(Status2Reg,0x08);					// 清空校验成功标志,清除MFCrypto1On位
	memset(ucComMF522Buf, 0x00, MAXRLEN);

 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;


	ucComMF522Buf[2] = pDataIn[0];				// CLA
	ucComMF522Buf[3] = pDataIn[1];				// INS 			
	ucComMF522Buf[4] = pDataIn[2];				// P1						 
	ucComMF522Buf[5] = pDataIn[3];				// P2					
	ucComMF522Buf[6] = pDataIn[4];				// LEN	
	Hex_Print(In_Len);
	Hex_Print(ucComMF522Buf[0]);
	Hex_Print(ucComMF522Buf[1]);
	Hex_Print(ucComMF522Buf[2]);
	Hex_Print(ucComMF522Buf[3]);
	Hex_Print(ucComMF522Buf[4]);
	Hex_Print(ucComMF522Buf[5]);
	Hex_Print(ucComMF522Buf[6]);
	
	Send_char1('\r');
	Send_char1('\n');

	for(i=0;i<ucComMF522Buf[6];i++)				//DATA
	{
		ucComMF522Buf[7+i] = pDataIn[5+i];	  

	}
										
    CalulateCRC(ucComMF522Buf,In_Len+2,&ucComMF522Buf[In_Len+2]);	// 生成发送内容的CRC校验,保存到最后两个字节
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,In_Len+4,pDataOut,&unLen); 							
	if (status == MI_OK)
	{
		Out_Len[0] = unLen/8-4;	 //接收到数据的长度,不要前面和后面的各两个字节，才是返回的有用数据						
		return MI_OK;
	}
	else
	{
	    Out_Len[0] = unLen/8-4;
		if((pDataOut[2]==0x90)&&(pDataOut[3]==0x00))
		return MI_OK;
		else
		return MI_ERR;

	}

}

//*************************************************************************
// 函数名	：PcdGetChallenge
// 描述		：读取随机数
// 入口		：无			  		
// 出口		： pRan: 4个字节的随机数
// 返回		：成功返回9000
//*************************************************************************
 
u16 PcdGetChallenge(u8* pRan)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 i,ucComMF522Buf[MAXRLEN]; 
	u8 st[2];
	u16 sst=0;

	ClearBitMask(Status2Reg,0x08);		//清空校验成功标志,清除MFCrypto1On位
 
	memset(ucComMF522Buf, 0x00, MAXRLEN);
	
 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x00;			// CLA
	ucComMF522Buf[3] = 0x84;			// INS - get random	
	ucComMF522Buf[4] = 0x00;			// P1
	ucComMF522Buf[5] = 0x00;			// P2
	ucComMF522Buf[6] = 0x04;			// Le - 随机数的长度=4个字节	

    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);	// 生成发送内容的CRC校验,保存到最后两个字节
	
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);// 将收到的卡片类型号保存 	
	
	if (status == MI_OK)
	{
		st[0] = ucComMF522Buf[6];
		st[1] = ucComMF522Buf[7]; 

		sst=st[0];
		sst=(sst<<8)|st[1];

		if(sst==0x9000)
		{
			for(i = 0;i<4;i++)
				*(pRan+i) = ucComMF522Buf[2+i];
	
		}
		return sst;
	}
	else
		return ST_ERR;
}

//*************************************************************************
// 函数名	：PcdSelectFile
// 描述		：选择文件
// 入口		：ChooseType : 选择方式
//					Lc : 根据选择方式而定数据长度
//					pDataIn : 文件标识符或者DF 名称
// 出口		：	    pDataOut 输出数据   Len输出的数据长度
// 返回		：成功返回9000
//*************************************************************************
// 
u16 PcdSelectFile(u8* pDataIn, u8* pDataOut,u8 * Len)
{
	s8 status =MI_ERR;  
	u16 unLen;
	u8 ucComMF522Buf[MAXRLEN]; 
	u8 st[2];
	u16 sst=0;

	ClearBitMask(Status2Reg,0x08);					// 清空校验成功标志,清除MFCrypto1On位
	memset(ucComMF522Buf, 0x00, MAXRLEN);

 	PcdSwitchPCB();
	
	ucComMF522Buf[0] = PCB;
	ucComMF522Buf[1] = 0x01;
	ucComMF522Buf[2] = 0x00;				// CLA
	ucComMF522Buf[3] = 0xA4;				// INS-Select File			
	ucComMF522Buf[4] = 0x00;				// P1						 
	ucComMF522Buf[5] = 0x00;				// P2					
	ucComMF522Buf[6] = 0x02;				// Lc								
	ucComMF522Buf[7] = pDataIn[0];			//写入文件名
	ucComMF522Buf[8] = pDataIn[1];									
    CalulateCRC(ucComMF522Buf,9,&ucComMF522Buf[9]);	// 生成发送内容的CRC校验,保存到最后两个字节
	status = PcdComCmd(PCD_TRANSCEIVE,ucComMF522Buf,11,pDataOut,&unLen);// 将收到的卡片类型号保存								
	if (status == MI_OK)
	{
		st[0] = pDataOut[unLen/8-4];   //执行的返回状态
		st[1] = pDataOut[unLen/8-3];

		sst=st[0];
		sst=(sst<<8)|st[1];

		Len[0] = unLen/8-4;

		return sst;

	}
	else
		return ST_ERR;
}

