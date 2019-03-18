/*************************************************************/
//2014.07.15修改版
/*************************************************************/
#include "mifare_card.h"
#include "fm175xx.h"
#include "type_a.h" 
#include "main.h"
/*****************************************************************************************/
/*名称：Mifare_Auth																		 */
/*功能：Mifare_Auth卡片认证																 */
/*输入：mode，认证模式（0：key A认证，1：key B认证）；sector，认证的扇区号（0~15）		 */
/*		*mifare_key，6字节认证密钥数组；*card_uid，4字节卡片UID数组						 */
/*输出:																					 */
/*		OK    :认证成功																	 */
/*		ERROR :认证失败																	 */
/*****************************************************************************************/
 unsigned char Mifare_Auth(unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid)
{
	unsigned char data send_buff[12],rece_buff[1],result;
	unsigned int data rece_bitlen;
	if(mode==0x0)
		send_buff[0]=0x60;//60 kayA认证指令
	if(mode==0x1)
		send_buff[0]=0x61;//61 keyB认证指令
  	send_buff[1]=sector*4;//认证扇区的块0地址
	send_buff[2]=mifare_key[0];
	send_buff[3]=mifare_key[1];
	send_buff[4]=mifare_key[2];
	send_buff[5]=mifare_key[3];
	send_buff[6]=mifare_key[4];
	send_buff[7]=mifare_key[5];
	send_buff[8]=card_uid[0];
	send_buff[9]=card_uid[1];
	send_buff[10]=card_uid[2];
	send_buff[11]=card_uid[3];

	Pcd_SetTimer(1);
	result =Pcd_Comm(MFAuthent,send_buff,12,rece_buff,&rece_bitlen);//Authent认证

	if (result==OK)
		{
		if(Read_Reg(Status2Reg)&0x08)//判断加密标志位，确认认证结果
			return OK;
		else
			return ERROR;
		}
	return ERROR;

}
/*****************************************************************************************/
/*名称：Mifare_Blockset									 */
/*功能：Mifare_Blockset卡片数值设置							 */
/*输入：block，块号；*buff，需要设置的4字节数值数组					 */
/*											 */
/*输出:											 */
/*		OK    :设置成功								 */
/*		ERROR :设置失败								 */
/*****************************************************************************************/
 unsigned char Mifare_Blockset(unsigned char block,unsigned char *buff)
 {
  unsigned char data block_data[16],result;
	block_data[0]=buff[3];
	block_data[1]=buff[2];
	block_data[2]=buff[1];
	block_data[3]=buff[0];
	block_data[4]=~buff[3];
	block_data[5]=~buff[2];
	block_data[6]=~buff[1];
	block_data[7]=~buff[0];
   	block_data[8]=buff[3];
	block_data[9]=buff[2];
	block_data[10]=buff[1];
	block_data[11]=buff[0];
	block_data[12]=block;
	block_data[13]=~block;
	block_data[14]=block;
	block_data[15]=~block;
  result= Mifare_Blockwrite(block,block_data);
  return result;
 }
/*****************************************************************************************/
/*名称：Mifare_Blockread																 */
/*功能：Mifare_Blockread卡片读块操作													 */
/*输入：block，块号（0x00~0x3F）；buff，16字节读块数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockread(unsigned char block,unsigned char *buff)
{	
	unsigned char data send_buff[2],result;
	unsigned int data rece_bitlen;
	Write_Reg(TxModeReg,0x80);
	Write_Reg(RxModeReg,0x80);
    Write_Reg(BitFramingReg,0x00);
	Pcd_SetTimer(1);
	send_buff[0]=0x30;//30 读块指令
	send_buff[1]=block;//块地址
	result =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);//
	if ((result!=0 )|(rece_bitlen!=16*8)) //接收到的数据长度为16
		return ERROR;
	return OK;
}
/*****************************************************************************************/
/*名称：mifare_blockwrite																 */
/*功能：Mifare卡片写块操作																 */
/*输入：block，块号（0x00~0x3F）；buff，16字节写块数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockwrite(unsigned char block,unsigned char *buff)
{	
	unsigned char data result,send_buff[16],rece_buff[1];
	unsigned int data rece_bitlen;
	Pcd_SetTimer(5);
	send_buff[0]=0xA0;//A0 写块指令
	send_buff[1]=block;//块地址
  
  	result =Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);//
	if ((result!=OK)|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
	return(ERROR);
	
	Pcd_SetTimer(10);

	result =Pcd_Comm(Transceive,buff,16,rece_buff,&rece_bitlen);//
	if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A)) //如果未接收到0x0A，表示无ACK
		return ERROR;
	return OK;
}
/*****************************************************************************************/
/*名称：																				 */
/*功能：Mifare 卡片增值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节增值数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockinc(unsigned char block,unsigned char *buff)
{	
	unsigned char data result,send_buff[2],rece_buff[1];
	unsigned int data rece_bitlen;
	Pcd_SetTimer(5);
	send_buff[0]=0xc1;// C1 增值指令
	send_buff[1]=block;//块地址
	result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
	if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
		return ERROR;
	Pcd_SetTimer(5);
	Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);

	return result;
}
/*****************************************************************************************/
/*名称：mifare_blockdec																	 */
/*功能：Mifare 卡片减值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节减值数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockdec(unsigned char block,unsigned char *buff)
{	
	unsigned char data result,send_buff[2],rece_buff[1];
	unsigned int data rece_bitlen;
	Pcd_SetTimer(5);
	send_buff[0]=0xc0;//
	send_buff[1]=block;//块地址
	result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
	if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
		return ERROR;
	Pcd_SetTimer(5);
	Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);

	return result;
}
/*****************************************************************************************/
/*名称：mifare_transfer																	 */
/*功能：Mifare 卡片transfer操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Transfer(unsigned char block)
{	
	unsigned char data result,send_buff[2],rece_buff[1];
	unsigned int data rece_bitlen;
	Pcd_SetTimer(5);
	send_buff[0]=0xb0;//
	send_buff[1]=block;//块地址
	result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
	if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
		return ERROR;
	return result;
}
/*****************************************************************************************/
/*名称：mifare_restore																	 */
/*功能：Mifare 卡片restore操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/

unsigned char Mifare_Restore(unsigned char block)
{	
	unsigned char data result,send_buff[4],rece_buff[1];
	unsigned int data rece_bitlen;
	Pcd_SetTimer(5);
	send_buff[0]=0xc2;//
	send_buff[1]=block;//块地址
	result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
	if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
		return ERROR;
	Pcd_SetTimer(5);
	send_buff[0]=0x00;
	send_buff[1]=0x00;
	send_buff[2]=0x00;
	send_buff[3]=0x00;
	Pcd_Comm(Transceive,send_buff,4,rece_buff,&rece_bitlen);
	return result;
}