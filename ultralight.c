
#include "pin.h"
#include "fm175xx.h"
#include "ultralight.h"
#include "type_a.h"
#include "main.h"

/*****************************************************************************************/
/*名称：ultra_read																		 */
/*功能：ultralight卡片读块																 */
/*输入：	page：块地址（0~15）												 */
/*输出：	buff：块数据指针（数据长度为4字节）											 */
/*       	OK: 应答正确                                                              	 */
/*	 		ERROR: 应答错误																 */
/*****************************************************************************************/
unsigned char Ultra_Read(unsigned char page,unsigned char *buff)
{
	unsigned char data ret;
	unsigned char data send_byte[2];
	unsigned int data rece_bitlen;
	Write_Reg(TxModeReg,0x80);
	Write_Reg(RxModeReg,0x80);
    Write_Reg(BitFramingReg,0x00);
	send_byte[0]=0x30;
	send_byte[1]=page;
	Pcd_SetTimer(1);
	ret=Pcd_Comm(Transceive,send_byte,2,buff,&rece_bitlen);
	return ret;
}
/*****************************************************************************************/
/*名称：ultra_write																		 */
/*功能：ultralight卡片写块																 */
/*输入：	page：页地址（0~15）												 */
/*输出：	buff：写块数据指针（4字节）													 */
/*       	OK: 应答正确                                                              	 */
/*	 		ERROR: 应答错误																 */
/*****************************************************************************************/
unsigned char Ultra_Write(unsigned char page,unsigned char *buff)
{
	unsigned char data ret;
	unsigned char data send_byte[6],rece_byte[1];
	unsigned int data rece_bitlen;
	send_byte[0]=0xA2;
	send_byte[1]=page;
	send_byte[2]=buff[0];
	send_byte[3]=buff[1];
	send_byte[4]=buff[2];
	send_byte[5]=buff[3];
	Pcd_SetTimer(10);
	
	ret=Pcd_Comm(Transceive,send_byte,6,rece_byte,&rece_bitlen);

	if ((ret!=0 )|(rece_byte[0]!=0x0A))	//如果未接收到0x0A，表示无ACK
		return ERROR;
	return OK;
}


