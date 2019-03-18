/*************************************************************/
//2014.07.15修改版
/*************************************************************/
#include "main.h"
#include "FM175XX.h"
#include "type_a.h"
#include "STC15F2K60S2.h"
#include "pin.h"
#include "uart.h"

struct PICC_A_STR PICC_A;   
/****************************************************************************************/
/*名称：TypeA_Request																	*/
/*功能：TypeA_Request卡片寻卡															*/
/*输入：																				*/
/*       			    			     												*/
/*	       								 												*/
/*输出：																			 	*/
/*	       	pTagType[0],pTagType[1] =ATQA                                         		*/					
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_Request(unsigned char *pTagType)
{
	unsigned char data result,send_buff[1],rece_buff[2];
	unsigned int data rece_bitlen;
	Clear_BitMask(TxModeReg,0x80);//关闭TX CRC
	Clear_BitMask(RxModeReg,0x80);//关闭RX CRC
	Set_BitMask(RxModeReg, 0x08);//关闭位接收
	Clear_BitMask(Status2Reg,0x08);
	Write_Reg(BitFramingReg,0x07);
	send_buff[0] = 0x26;
	Pcd_SetTimer(1);
	result = Pcd_Comm(Transceive,send_buff,1,rece_buff,&rece_bitlen);
	if ((result == OK) && (rece_bitlen == 2*8))
	{    
		*pTagType     = rece_buff[0];
		*(pTagType+1) = rece_buff[1];
	}
 	return result;
}
/****************************************************************************************/
/*名称：TypeA_WakeUp																	*/
/*功能：TypeA_WakeUp卡片寻卡															*/
/*输入：																				*/
/*       			    			     												*/
/*	       								 												*/
/*输出：																			 	*/
/*	       	pTagType[0],pTagType[1] =ATQA                                         		*/					
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_WakeUp(unsigned char *pTagType)
{
	unsigned char data result,send_buff[1],rece_buff[2];
	unsigned int data rece_bitlen;  
	Clear_BitMask(TxModeReg,0x80);//关闭TX CRC
	Clear_BitMask(RxModeReg,0x80);//关闭RX CRC
	Set_BitMask(RxModeReg, 0x08);//关闭位接收
	Clear_BitMask(Status2Reg,0x08);
	Write_Reg(BitFramingReg,0x07);
 	send_buff[0] = 0x52;
   	Pcd_SetTimer(1);
	result = Pcd_Comm(Transceive,send_buff,1,rece_buff,&rece_bitlen);

	if ((result == OK) && (rece_bitlen == 2*8))
	{    
		*pTagType     = rece_buff[0];
		*(pTagType+1) = rece_buff[1];
	}
	return result;
}
/****************************************************************************************/
/*名称：TypeA_Anticollision																*/
/*功能：TypeA_Anticollision卡片防冲突													*/
/*输入：selcode =0x93，0x95，0x97														*/
/*       			    			     												*/
/*	       								 												*/
/*输出：																			 	*/
/*	       	pSnr[0],pSnr[1],pSnr[2],pSnr[3]pSnr[4] =UID                            		*/					
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_Anticollision(unsigned char selcode,unsigned char *pSnr)
{
    unsigned char data result,i,send_buff[2],rece_buff[5];
    unsigned int data rece_bitlen;
	Clear_BitMask(TxModeReg,0x80);
	Clear_BitMask(RxModeReg,0x80);
    Clear_BitMask(Status2Reg,0x08);
    Write_Reg(BitFramingReg,0x00);
    Clear_BitMask(CollReg,0x80);
 
    send_buff[0] = selcode;
    send_buff[1] = 0x20;
    Pcd_SetTimer(1);
    result = Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);

    if (result == OK)
    {
    	 for (i=0; i<5; i++)
         	*(pSnr+i)  = rece_buff[i];
         if (pSnr[4] != (pSnr[0]^pSnr[1]^pSnr[2]^pSnr[3]))
    		result = ERROR;    
    }
  return result;
}
/****************************************************************************************/
/*名称：TypeA_Select																	*/
/*功能：TypeA_Select卡片选卡															*/
/*输入：selcode =0x93，0x95，0x97														*/
/*      pSnr[0],pSnr[1],pSnr[2],pSnr[3]pSnr[4] =UID 			    			     	*/
/*	       								 												*/
/*输出：																			 	*/
/*	       	pSak[0],pSak[1],pSak[2] =SAK			                            		*/					
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_Select(unsigned char selcode,unsigned char *pSnr,unsigned char *pSak)
{
    unsigned char data result,i,send_buff[7],rece_buff[5];
    unsigned int data rece_bitlen;
	Write_Reg(BitFramingReg,0x00);
  	Set_BitMask(TxModeReg,0x80);
	Set_BitMask(RxModeReg,0x80);
    Clear_BitMask(Status2Reg,0x08);
	
	send_buff[0] = selcode;
    send_buff[1] = 0x70;
    
    for (i=0; i<5; i++)
       	send_buff[i+2] = *(pSnr+i);
    
   	Pcd_SetTimer(1);
    result = Pcd_Comm(Transceive,send_buff,7,rece_buff,&rece_bitlen);
    if (result == OK)
    	*pSak=rece_buff[0]; 
	return result;
}
/****************************************************************************************/
/*名称：TypeA_Halt																		*/
/*功能：TypeA_Halt卡片停止																*/
/*输入：																				*/
/*       			    			     												*/
/*	       								 												*/
/*输出：																			 	*/
/*	       											                            		*/					
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_Halt(void)
{
    unsigned char data result,send_buff[2],rece_buff[1];
	unsigned int data rece_bitlen;
    send_buff[0] = 0x50;
    send_buff[1] = 0x00;
   
   	Write_Reg(BitFramingReg,0x00);
  	Set_BitMask(TxModeReg,0x80);
	Set_BitMask(RxModeReg,0x80);
    Clear_BitMask(Status2Reg,0x08);
	Pcd_SetTimer(1);
    result = Pcd_Comm(Transmit,send_buff,2,rece_buff,&rece_bitlen);
    return result;
}
/****************************************************************************************/
/*名称：TypeA_CardActivate																*/
/*功能：TypeA_CardActivate卡片激活														*/
/*输入：																				*/
/*       			    			     												*/
/*	       								 												*/
/*输出：	pTagType[0],pTagType[1] =ATQA 											 	*/
/*	       	pSnr[0],pSnr[1],pSnr[2],pSnr[3]pSnr[4] =UID 		                   		*/
/*	       	pSak[0],pSak[1],pSak[2] =SAK			                            		*/					
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_CardActivate(struct PICC_A_STR *type_a_card)
{
	unsigned char data result;
	result=TypeA_Request(type_a_card->ATQA);//寻卡 Standard	 send request command Standard mode
	if (result==ERROR)
		return ERROR;
	if 	((type_a_card->ATQA[0]&0xC0)==0x00)	//一重UID
	{
		result=TypeA_Anticollision(0x93,type_a_card->UID);//1级防冲突
		if (result==ERROR)
			return ERROR;
					
		result=TypeA_Select(0x93,type_a_card->UID,&type_a_card->SAK);//1级选卡
		if (result==ERROR)
			return ERROR;
		type_a_card->UID_len=4;
	}

	if 	((type_a_card->ATQA[0]&0xC0)==0x40)	//二重UID
	{
		result=TypeA_Anticollision(0x93,type_a_card->UID);//1级防冲突
		if (result==ERROR)
			return ERROR;
					
		result=TypeA_Select(0x93,type_a_card->UID,&type_a_card->SAK);//1级选卡
		if (result==ERROR)
			return ERROR;
				
		result=TypeA_Anticollision(0x95,type_a_card->UID+4);//2级防冲突
		if (result==ERROR)
			return ERROR;
				
		result=TypeA_Select(0x95,type_a_card->UID+4,&type_a_card->SAK);//2级选卡
		if (result==ERROR)
			return ERROR;
		type_a_card->UID_len=8;
	}

	if 	((type_a_card->ATQA[0]&0xC0)==0x80)	//三重UID
	{
		result=TypeA_Anticollision(0x93,type_a_card->UID);//1级防冲突
		if (result==ERROR)
			return ERROR;
				
		result=TypeA_Select(0x93,type_a_card->UID,&type_a_card->SAK);//1级选卡
		if (result==ERROR)
			return ERROR;
				
		result=TypeA_Anticollision(0x95,type_a_card->UID+4);//2级防冲突
		if (result==ERROR)
			return ERROR;
		
		result=TypeA_Select(0x95,type_a_card->UID+4,&type_a_card->SAK);//2级选卡
		if (result==ERROR)
			return ERROR;
			
		result=TypeA_Anticollision(0x97,type_a_card->UID+8);//3级防冲突
		if (result==ERROR)
			return ERROR;
			
		result=TypeA_Select(0x97,type_a_card->UID+8,&type_a_card->SAK);//3级选卡
		if (result==ERROR)
			return ERROR;
		type_a_card->UID_len=12;
	}
	return result;
}
