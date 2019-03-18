//#include "15f.h"
#include "I2C.h"
 #include "intrins.h"
// sbit ISDA=P3^7;//24C
//sbit ISCL=P3^6;

//sbit ISDA=P1^2;//24C
//sbit ISCL=P1^3;

sbit ISDA=P3^5;//24C
sbit ISCL=P3^6;	
#define  	SDA_H  	   ISDA=1
#define  	SDA_L  	   ISDA=0
#define     SCL_H	   ISCL=1
#define     SCL_L	   ISCL=0
#define     SDA_read   ISDA
 
 #define I2C_SLAVE_ADDRESS7	 0xAE	  // 24C08 Block1 address 0xA2
 #define sEE_PAGESIZE    16


  uint8_t	sEEDataNum;

		  
void I2C_delay(void)
{	
//   uint8_t i=10; //??????????,??????5????
//   while(i) 
//   { 
//     i--; 
//   } 
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();//11.0592M时,最少20个
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

uint8_t I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return FALSE;	//SDA线为低电平则总线忙,退出
	SDA_L;
	I2C_delay();
	if(SDA_read) return FALSE;	//SDA线为高电平则总线出错,退出 
	SDA_L;
	I2C_delay();
	return TRUE;
}

void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}


void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}


uint8_t I2C_WaitAck(void) 	 //???:=1?ACK,=0?ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;
      return FALSE;
	}
	SCL_L;
	return TRUE;
}

static void I2C_SendByte(uint8_t SendByte) //????????//
{
    uint8_t i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}

static uint8_t I2C_ReceiveByte(void)  //????????//
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
	  SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}
/**
  * @brief  Wait for EEPROM Standby state
  * @param  None
  * @retval None
  */
void sEE_WaitEEStandbyState(void)
{	ovt=0;
   do {
   if(ovt>60)
	 break;
     I2C_Start();
     I2C_SendByte(I2C_SLAVE_ADDRESS7);
   }while(!I2C_WaitAck());

   I2C_Stop();
}

/**
  * @brief  Writes more than one byte to the EEPROM with a single WRITE cycle.
  * @note   The number of byte can't exceed the EEPROM page size.
  * @param  pBuffer : pointer to the buffer containing the data to be written to 
  *         the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @param  NumByteToWrite : variable holding number of bytes to 
  *         written to the EEPROM. 
  * @retval bool --zhangpei
  */
uint8_t sEE_WritePage(uint8_t *pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
 if(!I2C_Start())return FALSE;
  I2C_SendByte(I2C_SLAVE_ADDRESS7 & 0xFE);//	????+Write
  I2C_WaitAck();


  I2C_SendByte((uint8_t)((WriteAddr & 0xFF00) >> 8));
  I2C_WaitAck();
	
  I2C_SendByte((uint8_t)(WriteAddr & 0x00FF));
  I2C_WaitAck();
	
  while(NumByteToWrite--) {
    I2C_SendByte(*pBuffer);
	I2C_WaitAck();
	pBuffer++;
  }
  I2C_Stop();
  return TRUE;
}
/**
  * @brief  Writes buffer of data to the I2C EEPROM.
  * @param  pBuffer : pointer to the buffer  containing the data to be written 
  *         to the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @param  NumByteToWrite : number of bytes to write to the EEPROM. --zhangpei
  */
uint8_t FM11_WriteE2(uint8_t *pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
   uint8_t NumOfPage = 0, NumOfSingle = 0, count = 0;
  uint16_t Addr = 0;

  Addr = WriteAddr % sEE_PAGESIZE;
  count = sEE_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
  NumOfSingle = NumByteToWrite % sEE_PAGESIZE;

  if(Addr == 0) {
    if(NumOfPage == 0) {
	   sEEDataNum = NumOfSingle;
	   if(!sEE_WritePage(pBuffer, WriteAddr, sEEDataNum)) return FALSE;
	   sEE_WaitEEStandbyState();
	} else {
	  while(NumOfPage--) {
	    sEEDataNum = sEE_PAGESIZE;        
        if(!sEE_WritePage(pBuffer, WriteAddr, sEEDataNum)) return FALSE;
		sEE_WaitEEStandbyState();
		WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;
	  }
	  if(NumOfSingle != 0) {
	    sEEDataNum = NumOfSingle;          
        if(!sEE_WritePage(pBuffer, WriteAddr, sEEDataNum)) return FALSE;
		sEE_WaitEEStandbyState();
	  }
	}
  }	else { // If WriteAddr is not sEE_PAGESIZE aligned
    if(NumOfPage == 0) {
	  if(NumByteToWrite > count) {
	    sEEDataNum = count;
		if(!sEE_WritePage(pBuffer, WriteAddr, sEEDataNum)) return FALSE;
		sEE_WaitEEStandbyState();
		sEEDataNum = (NumByteToWrite - count);          
        /*!< Write the remaining data in the following page */
        if(!sEE_WritePage((uint8_t*)(pBuffer + count), (WriteAddr + count), sEEDataNum)) return FALSE;
		sEE_WaitEEStandbyState();
	  }	else {
		sEEDataNum = NumOfSingle;         
        if(!sEE_WritePage(pBuffer, WriteAddr, sEEDataNum)) return FALSE;
		sEE_WaitEEStandbyState();
	  }
	} else { // If NumByteToWrite > sEE_PAGESIZE
	  NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sEE_PAGESIZE;
      NumOfSingle = NumByteToWrite % sEE_PAGESIZE;

	  if(count != 0)
      {  
        /* Store the number of data to be written */
        sEEDataNum = count;         
        if(!sEE_WritePage(pBuffer, WriteAddr, sEEDataNum)) return FALSE;
		sEE_WaitEEStandbyState();
        WriteAddr += count;
        pBuffer += count;
      }
	  while(NumOfPage--) {
	    sEEDataNum = sEE_PAGESIZE;          
        if(!sEE_WritePage(pBuffer, WriteAddr, sEEDataNum)) return FALSE;
		sEE_WaitEEStandbyState();
		WriteAddr +=  sEE_PAGESIZE;
        pBuffer += sEE_PAGESIZE;  
	  }
	  if(NumOfSingle != 0) {
	    sEEDataNum = NumOfSingle;           
        if(!sEE_WritePage(pBuffer, WriteAddr, sEEDataNum)) return FALSE;
		sEE_WaitEEStandbyState(); 
	  } 
	}
  }
  return TRUE;
}

// Read FM327 Register
uint8_t FM11_ReadReg(uint16_t addr)
{
	uint8_t pd[50];
	uint8_t a;
  if(FM11_ReadE2(pd, addr, 1)) 
	{	
		a=*pd;
	  return a;
	}
  else 
	    return FALSE;
}
// Write FM327 Register
uint8_t FM11_WriteReg(uint16_t addr, uint8_t da)
{
  if(!I2C_Start())return FALSE;
  I2C_SendByte(I2C_SLAVE_ADDRESS7 & 0xFE);// ????+Write
  I2C_WaitAck();
  I2C_SendByte((uint8_t)((addr & 0xFF00) >> 8));
  I2C_WaitAck();
  I2C_SendByte((uint8_t)(addr & 0x00FF));
  I2C_WaitAck();
  I2C_SendByte(da);
  I2C_WaitAck();
  I2C_Stop();
  return TRUE;
}


uint8_t FM11_ReadE2(uint8_t *pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{		
       if(!I2C_Start())
				 return FALSE;
    I2C_SendByte(I2C_SLAVE_ADDRESS7 & 0xFE);//	???? + W
    if(!I2C_WaitAck()){I2C_Stop(); return FALSE;}
	
	I2C_SendByte((uint8_t)((ReadAddr & 0xFF00)>>8));
	I2C_WaitAck();
	
		I2C_SendByte((uint8_t)(ReadAddr & 0x00FF));      
    I2C_WaitAck();
    
		I2C_Start();
    I2C_SendByte(I2C_SLAVE_ADDRESS7 | 0x01);
    I2C_WaitAck();
    
		while(NumByteToRead)
    {
      *pBuffer = I2C_ReceiveByte();
      if(NumByteToRead == 1)I2C_NoAck();
      else I2C_Ack(); 
      pBuffer++;
      NumByteToRead--;
    }
    I2C_Stop();
    return TRUE;
}

// Read FM327 FIFO
uint8_t FM11_ReadFIFO(uint8_t NumByteToRead,uint8_t *pbuf)
{
  if(FM11_ReadE2(pbuf, FM327_FIFO, NumByteToRead)) return TRUE;
  else return FALSE;
}
// WriteFM327 FIFO
uint8_t FM11_WriteFIFO(uint8_t *pbuf, uint8_t NumByteToWrite)
{
  if(sEE_WritePage(pbuf, FM327_FIFO, NumByteToWrite)) return TRUE;
  else return FALSE;
}



void FM11_Init(void)
{
// 	uint8_t val;
		
  //I2C_GPIO_Config();
	FM11_WriteReg(MAIN_IRQ_MASK_REG,0x44);	//MAIN中断配置;接收开始中断
  FM11_WriteReg(FIFO_IRQ_MASK_REG,0x07); //FIFO中断配置
	FM11_WriteReg(NFC_CFG_REG,0x03);	//写NFC配置字(默认支持-4)
	FM11_WriteReg(FIFO_FLUSH_REG,0xFF);		//清fifo寄存器
	
	
	//val = 0xFF &(~MAIN_IRQ_RX_START);
	//FM11_WriteReg(MAIN_IRQ_MASK,val);	//MAIN中断配置;接收开始中断

	//val = 0xFF & (~FIFO_IRQ_WL);		//只开启水平中断
	//FM11_WriteReg(FIFO_IRQ_MASK,val);	//FIFO中断配置

	//FM11_WriteReg(NFC_CFG,0x00);	//写NFC配置字(默认支持-4)
	
// 	RFpdtpReady = OFF;
// 	RFapduReady = OFF;
// 	RFdataLen = 0;
// 	CTdataLen = 0;
// 	CTdataIndex = 0;

// 	RFapduDemo = OFF;
// 	
// 	memset(RFdataBuf,0,BUF_LENTH);
// 	memset(CTdataBuf,0,BUF_LENTH);
}


 /*********************************************************************************************************
** 函数名称:	FM11_RFDataTx
** 函数功能:	RF数据回发
** 输入参数:    ilen:回发数据长度
** 					*ibuf:回发的数据
** 输出参数:    无
** 返回值:      无
*********************************************************************************************************/
void FM11_RFDataTx(uint16_t ilen,uint8_t *ibuf)
{
	uint16_t slen;//,cnt;
	uint8_t *sbuf;
	
	slen = ilen;
	sbuf = ibuf;
// 	cnt = 0;

	if(slen <= 32)
	{
		FM11_WriteFIFO(sbuf,slen);		//write fifo	有多少发多少
		slen = 0;
	}
	else
	{
		FM11_WriteFIFO(sbuf,32);			//write fifo	先发32字节进fifo
		
		slen -= 32;		//待发长度－32
		sbuf += 32;		//待发数据指针+32
	}
	
	FM11_WriteReg(RF_TXEN_REG,0x55);	//写0x55时触发非接触口回发数据

	while(slen>0)
	{		
		if((FM11_ReadReg(FIFO_WORDCNT_REG) & 0x3F )<=8)
		{
			if(slen<=24)
			{
				FM11_WriteFIFO(sbuf,slen);			//write fifo	先发32字节进fifo
				slen = 0;				
			}
			else
			{
				FM11_WriteFIFO(sbuf,24);			//write fifo	先发32字节进fifo
				
				slen -= 24; 	//待发长度－24
				sbuf += 24; 	//待发数据指针+24
			}
		}
		
		FM11_WriteReg(RF_TXEN_REG,0x55);	//写0x55时触发非接触口回发数据
	}

	//while(FM11_ChkIrqInfo(FIFO_IRQ_EMPTY,FIFO_IRQ)==OFF);
	//FM11_ReadReg(MAIN_IRQ);

	//while((FM11_ReadReg(FIFO_WORDCNT) & 0x3F )> 0);	//等待发送完成标志位置起
	
	
	//irq_txdone = 0;
}


