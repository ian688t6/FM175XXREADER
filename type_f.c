#include "main.h"
#include "FM175XX.h" 
#include <string.h>	
unsigned char NFCID2[8];


unsigned char TypeF_Polling(unsigned char slot,unsigned char *rece_len,unsigned char *rece_buff)
{
unsigned int rece_bitlen;
unsigned char result;
unsigned char send_buff[6]= {0x06,0x00,0xFF,0xFF,0x00,0x00};

if ((slot!=0x00)&&(slot!=0x01)&&(slot!=0x03)&&(slot!=0x07)&&(slot!=0x0F))
	return ERROR;
else
	send_buff[5]=slot;
	Pcd_SetTimer(10);
    result = Pcd_Comm(Transceive,send_buff,6,rece_buff,&rece_bitlen);
	if (result==OK)
		{
		*rece_len=rece_bitlen/8;
		memcpy(NFCID2,rece_buff+2,8);
		return OK;	
		}
	else
	*rece_len=0;
return ERROR;
}