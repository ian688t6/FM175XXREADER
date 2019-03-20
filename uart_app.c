//#include "fm175xx.h"
#include "spi.h"
//#include "type_a.h"
//#include "type_b.h"
//#include "ultralight.h"
#include "mifare_card.h"
#include "uart.h"
//#include "cpu_card.h"
#include "pin.h"
#include "STC15F2K60S2.h"
#include "main.h"
#include "pcd.h"
#include "des.h"
//#include "type_f.h"
#include <string.h>

#define system_info "FM17550 READER IC STC15F2K DEMO V2.0 2014/07/15"
//char sj[]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,}		;
//////////////////CPU卡的指令/////////////////////////////////////////////////////////////////////////////////
unsigned char code Keyfile[7] ={0x3F, 0x00, 0xB0, 0x01, 0xF0, 0xFF, 0xFF};	//建密钥文件
unsigned char code DefaultKey[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  //原始外部密钥
unsigned char code ExKey[5]   ={0x39, 0xF0, 0xF0, 0xAA, 0x55};	     ////外部密钥建立的指令信息 
unsigned char code DFfile[13] ={0x38,0x05,0x20,0xF0,0xF0,0x95,0xFF,0xFF,0x44,0x44,0x46,0x30,0x31};//建立的DF文件							   
unsigned char code EF_KEY_File[7]={0x3F, 0x01, 0x8F, 0x95, 0xF0, 0xFF, 0xFF};//建立EF密钥文件														  
unsigned char code C_KEY[8]={0x3A, 0xF0, 0xEF, 0x44, 0x55, 0x12, 0x34, 0x56};    //增加口令密钥 
unsigned char code EF_File1[7]={0x28, 0x00, 0x1E, 0xF0, 0xF0, 0xFF, 0xFF};//	基本文件EF 建立的二进制文件1   						   
unsigned char code EF_File2[7]={0x28, 0x00, 0x0F, 0xF4, 0xF0, 0xFF, 0xFF};//	基本文件EF 建立的二进制文件2
unsigned char code EF_File3[7]={0x28, 0x01, 0xFF, 0xF0, 0xF0, 0xFF, 0xFF};//	基本文件EF 建立的二进制文件3   该文件空间为0X1FF即 511个字节
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//////////////////文件标识的定义/////////////////////////////////////////////////////////////////////////////////

unsigned char code MF_File_Flag[2] = {0x3f,0x00};       //MF文件标识3F00//
unsigned char code KEY_File_Flag[2] = {0x00,0x00};      //密钥文件标识0000//
unsigned char code KEY_Flag = {0x00};                //密钥标识00,即放在MF下密钥文件里面的密钥，用1个字节表示 //

unsigned char code DF_File_Flag[2] = {0x3f,0x01};       //DF文件标识3F01//
unsigned char code KEY_DF_File_Flag[2] = {0x00,0x01};   //DF下的密钥文件标识0001// 
unsigned char code Keyword_DF_Flag = {0x01};         //密钥标识01,即放在DF下密钥文件里面的密钥，用1个字节表示 //

unsigned char code EF_File3_Flag[2]	= {0x00,0x03};       //EF文件标识0003//
unsigned char code EF_File4_Flag[2]	= {0x00,0x04};       //EF文件标识0004//
unsigned char code EF_File5_Flag[2]	= {0x00,0x05};       //EF文件标识0005//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Uart_App(void)
{
	int i = 0;
	unsigned char tt[2];
	unsigned char rnd_code[8] = {0};
	unsigned char des_code[8] = {0};	
	unsigned char retlen = 0;
	unsigned char rcvlen = 0;	
	unsigned short sst = 0x0000;
	unsigned char HEX[]="0123456789ABCDEF";
	if((uart_status.rece_finish==True)&&(uart_status.rece_error==False) && (rece_buff.length > 0))
	{
		uart_status.rece_finish=0;
		
//		Send_string1("Uart_App\n",0);
		Send_char1(HEX[(rece_buff.length>>4) &0x0f]);
		Send_char1(HEX[(rece_buff.length>>0) &0x0f]);
		Send_char1('\r');Send_char1('\n');
		for (i = 0; i < rece_buff.length; i ++) {
			Send_char1(HEX[(rece_buff.buff[i]>>4)&0x0f]);
			Send_char1(HEX[(rece_buff.buff[i]>>0)&0x0f]);
			Send_char1(' ');
		}
		Send_char1('\r');Send_char1('\n');
		rcvlen = rece_buff.length;
		rece_buff.length = 0;
		switch(rece_buff.buff[0])
		{
		case 0x0://版本确认
	//		send_buff.buff[0]=strlen(system_info);
	//		memcpy(send_buff.buff+1,system_info,send_buff.buff[0]);
	//		send_buff.length=send_buff.buff[0]+1;
			break;
		
		case 0x1://FM17XX 软复位
			
//			send_buff.buff[0]=FM175X_SoftReset();
//			send_buff.length=1;
			break;

		case 0x2://FM17XX 硬复位
			
//			send_buff.buff[0]=FM175X_HardReset();
//			send_buff.length=1;
			break;

		case 0x3://PC读取FM17XX所有地址
//			send_buff.buff[0]=Read_Reg_All(send_buff.buff+1);
//			send_buff.length=65;
			break;

		case 0x4://PC读取FM17XX地址
//			send_buff.buff[0]=OK;
//			send_buff.buff[1]=rece_buff.buff[1];
//			send_buff.buff[2]=Read_Reg(rece_buff.buff[1]);
//			send_buff.length=3;
			break;

		case 0x5://PC写入FM17XX地址
					
//			Write_Reg(rece_buff.buff[1],rece_buff.buff[2]);
//			send_buff.buff[0]=0;
//			send_buff.length=1;
			break;
		
		case 0x6://读取扩展寄存器	
//			send_buff.buff[0]=OK;
//			send_buff.buff[1]=rece_buff.buff[1];
//			send_buff.buff[2]=Read_Ext_Reg(rece_buff.buff[1]);
//			send_buff.length=3;
			break;

		case 0x7://写入扩展寄存器
				
//			send_buff.buff[0]= Write_Ext_Reg(rece_buff.buff[1],rece_buff.buff[2]);
//			send_buff.buff[1]=rece_buff.buff[1];
//			send_buff.length=2;
			break;	
		
		case 0x8://FM175X_HardPowerdown
//			send_buff.buff[0]=FM175X_HardPowerdown();
//			send_buff.length=1;
			break;

		case 0x9://FM175X_SoftPowerdown
//			send_buff.buff[0]=FM175X_SoftPowerdown();
//			send_buff.length=1;
			break;

		case 0xA://REQUEST
				
//			Set_Rf(3);   //turn on radio
//			Pcd_ConfigISOType(0);
//			send_buff.buff[0]=TypeA_Request(PICC_A.ATQA);//寻卡 Standard	 send request command Standard mode
//			send_buff.buff[1]=PICC_A.ATQA[0];
//			send_buff.buff[2]=PICC_A.ATQA[1];
//			send_buff.length=3;

	//	    CPU_TPDU(1,&a,&bb,&cc)	;

			break;

		case 0xB://WAKEUP
//			Set_Rf(3);   //turn on radio
//			Pcd_ConfigISOType(0);
//			send_buff.buff[0]=TypeA_WakeUp(PICC_A.ATQA);
//			send_buff.buff[1]=PICC_A.ATQA[0];
//			send_buff.buff[2]=PICC_A.ATQA[1];
//			send_buff.length=3;
			break;

		case 0xC:// Halt
//			send_buff.buff[0]= TypeA_Halt();
//			send_buff.length=1;
			break;

		case 0x10:
			//寻卡，防冲突，选卡
			if(GetCard(PICC_REQALL, tt, &send_buff.buff[1])!=ST_OK)  //寻卡函数
			{
				Send_char1('e');
				send_buff.buff[0] 	= ERROR;
				send_buff.length 	= 1;
				break;
			}
			
			//寻卡失败返回
			if (CardReset(&send_buff.buff[1], &retlen)!=ST_OK)		  //CPU卡复位函数
			{
				send_buff.buff[0] 	= ERROR;
				send_buff.length 	= 1;
				break;
			}
			send_buff.buff[0] 	= OK;
			send_buff.length = retlen + 1;
			break;
		
		case 0x11://Mifare 认证
			
//			send_buff.buff[0]=Mifare_Auth(rece_buff.buff[1],rece_buff.buff[2],rece_buff.buff+3,PICC_A.UID);	//card authentication
//			send_buff.length=1;
			break;

		case 0x12://Mifare 读块
//			send_buff.buff[0]=Mifare_Blockread(rece_buff.buff[1]+rece_buff.buff[2]*4,send_buff.buff+1);	//bolck read function
//			send_buff.length=17;
			break;

		case 0x13://Mifare 写块

//			send_buff.buff[0]=Mifare_Blockwrite((rece_buff.buff[1]+rece_buff.buff[2]*4),rece_buff.buff+3);	 //block write function
//			send_buff.length=1;
			break;

		case 0x14://Mifare 增值

//			send_buff.buff[0]=	Mifare_Blockinc(rece_buff.buff[1],rece_buff.buff+2);
//			send_buff.length=1;
			break;
																										
		case 0x15://Mifare 减值
//			send_buff.buff[0]=	Mifare_Blockdec((rece_buff.buff[1]+rece_buff.buff[2]*4),rece_buff.buff+3);
//			send_buff.length=1;
			break;

		case 0x16://Mifare transfer
//			send_buff.buff[0]=	Mifare_Transfer(rece_buff.buff[1]);
//			send_buff.length=1;
			break;

		case 0x17://Mifare restore
//			send_buff.buff[0]=	Mifare_Restore((rece_buff.buff[1]+rece_buff.buff[2]*4));
//			send_buff.length=1;
			break;
		 
		case 0x18://Mifare 设置数据块
//			send_buff.buff[0]= Mifare_Blockset(rece_buff.buff[1],rece_buff.buff+2);
//			send_buff.length=1;
			break;
						
		case 0x20://RATS
//			send_buff.buff[0]=CPU_Rats(rece_buff.buff[1],rece_buff.buff[2],&send_buff.buff[1],send_buff.buff+2);
//			send_buff.length=send_buff.buff[1]+2;
			break;
		
		case 0x21://PPS
//			send_buff.buff[0]=CPU_PPS(rece_buff.buff[1],rece_buff.buff[2],&send_buff.buff[1],send_buff.buff+2);
//			send_buff.length=send_buff.buff[1]+2;
			//  CPU_TPDU(1,&a,&bb,&cc)	;
			break;

		case 0x22://高波特率
//			send_buff.buff[0]=CPU_BaudRate_Set(rece_buff.buff[1]);
//			send_buff.length=1;
			break;

		case 0x23://TPDU指令
//			  CPU_TPDU(25,sj,&send_buff.buff[1],send_buff.buff+2);
		//	send_buff.buff[0]=CPU_TPDU(rece_buff.buff[1],rece_buff.buff+2,&send_buff.buff[1],send_buff.buff+2);
		//	send_buff.length=send_buff.buff[1]+2;
			break;
	  
		case 0x24://R_block
//			send_buff.buff[0]=CPU_R_Block(&send_buff.buff[1],send_buff.buff+2);
//			send_buff.length=send_buff.buff[1]+2;
			break;

		case 0x25://S_block
//			send_buff.buff[0]=CPU_S_Block(rece_buff.buff[1],&send_buff.buff[1],send_buff.buff+2);
//			send_buff.length=send_buff.buff[1]+2;
			break;

		case 0x26://I_block
//			send_buff.buff[0]=CPU_I_Block(rece_buff.buff[1],rece_buff.buff+2,&send_buff.buff[1],send_buff.buff+2);
//			send_buff.length=send_buff.buff[1]+2;
			break;
		
		case 0x27://随机数
//			send_buff.buff[0]=CPU_Random(&send_buff.buff[1],send_buff.buff+2);
//			send_buff.length=send_buff.buff[1]+2;
			break;
	   
		case 0x31://FM17550 TYPE B request
//			Set_Rf(3);   //turn on radio
//			Pcd_ConfigISOType(1);
	//		send_buff.buff[0]=TypeB_Request(&send_buff.buff[1],send_buff.buff+2,&PICC_B);
	//		send_buff.length=send_buff.buff[1]+2;
			break;

		case 0x32://FM17550 TYPE B attrib
	//		send_buff.buff[0]=TypeB_Select(PICC_B.PUPI,&send_buff.buff[1],send_buff.buff+2);
	//		send_buff.length=send_buff.buff[1]+2;
			break;

		case 0x33://FM17550 TYPE B halt
	//		send_buff.buff[0]=TypeB_Halt(PICC_B.PUPI,&send_buff.buff[1],send_buff.buff+2);
	//		send_buff.length=send_buff.buff[1]+2;;
			break;
				   
		case 0x34://FM17550 TYPE B get_uid
	//		send_buff.buff[0]=TypeB_GetUID(&send_buff.buff[1],send_buff.buff+2);
	//		send_buff.length=send_buff.buff[1]+2;
			break;

		case 0x35://FM17550 TYPE B wakeup
	//		Set_Rf(3);   //turn on radio
	//		Pcd_ConfigISOType(1);
	//	  	send_buff.buff[0]=TypeB_WUP(&send_buff.buff[1],send_buff.buff+2,&PICC_B);
	//		send_buff.length=send_buff.buff[1]+2;
			break;
		
		case 0x52://ultralight_read
	//		send_buff.buff[0]=Ultra_Read(rece_buff.buff[1],send_buff.buff+1);
	//		send_buff.length=5;
			break;

		case 0x53://ultralight_write
	//		send_buff.buff[0]=Ultra_Write(rece_buff.buff[1],rece_buff.buff+2);
	//		send_buff.length=1;
			break;

		case 0x60:
	//		Set_Rf(3);   //turn on radio
	//		Pcd_ConfigISOType(2);
	//		send_buff.buff[0]=TypeF_Polling(rece_buff.buff[1],&send_buff.buff[1],send_buff.buff+2);
	//		send_buff.length=send_buff.buff[1]+2;
			break;
		
		case 0x61:	/* PCD Command */
			send_buff.buff[0] = Pcd_Cmd(&rece_buff.buff[2], rece_buff.buff[1], &send_buff.buff[1], &retlen);
			if(send_buff.buff[0] != 0)	//1时出错了
			{
				send_buff.length = 1;
				break;						//返回	
			}
			send_buff.length = retlen + 3;
			break;
			
		case 0x62:	/* Pcd Select CPUCard File */
			if (ST_OK != PcdSelectFile(&rece_buff.buff[1], &send_buff.buff[1], &retlen)) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;
				break;
			}
			send_buff.buff[0] = OK;
			send_buff.length  = retlen + 3;
			break;
		
		case 0x63: /* Pcd Auth */
			memset(rnd_code, 0x0, sizeof(rnd_code));
			if (ST_OK != PcdGetChallenge(rnd_code)) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;
			}
			

//			for (i = 0; i < 4; i ++) {
//				Hex_Print(rnd_code[i]);
//			}
//			Send_char1('\r');Send_char1('\n');
			Des(ENCRY, rnd_code, &rece_buff.buff[2], des_code);
//			for (i = 0; i < 8; i ++) {
//				Hex_Print(des_code[i]);
//			}
//			Send_char1('\r');Send_char1('\n');
			if (ST_OK != PcdExAuth(rece_buff.buff[1], des_code)) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;
				break;
			}
			send_buff.buff[0] = OK;
			send_buff.length = 1;
			break;
			
		case 0x64:
			//擦除MF下的所有DF文件
			if(PcdStatus(PcdEraseDF()) != 0)	 
			{
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;
				break;
			}
			send_buff.buff[0] = OK;
			send_buff.length = 1;
			
			break;
		
		case 0x65: //创建文件
			if (0 != PcdStatus(PcdCreateFile(&rece_buff.buff[1], rcvlen - 3, &rece_buff.buff[3]))) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;			
			}
			
			send_buff.buff[0] = OK;
			send_buff.length = 1;
			break;
		
		case 0x66: //设置新秘钥
			if (ST_OK != PcdSetKey(0x00, 0x0d, &rece_buff.buff[1])) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;
				break;
			}
			send_buff.buff[0] = OK;
			send_buff.length = 1;
			
			break;
			
		case 0x67: //写二进制文件
			if (ST_OK != PcdUpdateBinary(rece_buff.buff[1], rece_buff.buff[2], rece_buff.buff[3], &rece_buff.buff[4])) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;			
				break;
			}
			send_buff.buff[0] = OK;
			send_buff.length = 1;
			
			break;
			
		case 0x68: //读二进制文件
			if (ST_OK != PcdReadBinary(rece_buff.buff[1], rece_buff.buff[2], rece_buff.buff[3], &send_buff.buff[0], &send_buff.length)) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;			
			}
			send_buff.length = rece_buff.buff[3] + 2;
			break;
			
		case 0x69: //创建DF二进制
			if (ST_OK != PcdCreateFile(DF_File_Flag, 0x0d, DFfile)) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;
				break;
			}
			
			PcdCreateFile(tt, 0x0d, DFfile);
			if (ST_OK != PcdSelectFile(DF_File_Flag, &send_buff.buff[1], tt)) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;
				break;			
			}
			
			//建立基本文件EF 二进制文件	3 //这里所建的是空间为511字节的文件
			if (ST_OK != PcdCreateFile(EF_File5_Flag, 0x07, EF_File3)) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;
				Send_char1('e');
				break;
			}
	
			
			
			if (ST_OK != PcdCreateFile(KEY_DF_File_Flag, 0x07, EF_KEY_File)) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;

				break;
			}
			
			if (ST_OK != PcdSetKey(Keyword_DF_Flag, 0x08,C_KEY )) {
				send_buff.buff[0] = ERROR;
				send_buff.length = 1;

				break;			
			}
			

			send_buff.buff[0] = OK;
			send_buff.length =  1;
			break;
			
//		case 0x70:
//			if (ST_OK != PcdCreateFile(KEY_DF_File_Flag, 0x07, EF_KEY_File)) {
//				send_buff.buff[0] = ERROR;
//				send_buff.length = 1;
//				break;			
//			}
//			
//			if (ST_OK != PcdSetKey(Keyword_DF_Flag, 0x08, C_KEY)) {
//				send_buff.buff[0] = ERROR;
//				send_buff.length = 1;
//				break;
//			}
//			send_buff.buff[0] = OK;
//			send_buff.length = 1;
//			break;
			
//		case 0x71:
//			   //建立基本文件EF 二进制文件	3 //这里所建的是空间为511字节的文件
//			if (ST_OK != PcdCreateFile(EF_File5_Flag, 0x07, EF_File3)) {
//				send_buff.buff[0] = ERROR;
//				send_buff.length = 1;
//				break;
//			}
//			send_buff.buff[0] = OK;
//			send_buff.length = 1;
//			
//			break;
			
		default:
			send_buff.buff[0] = ERROR;
			send_buff.length=1;
			break;
			}
			Uart_Send();
		} 
  }