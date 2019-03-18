
#include "STC15F2K60S2.h"
#include "BT.h"
#include "uart.h"
#include "main.h"

#define upper_lower_EN 0  //0 区分字母大小写   1 不区分字母大小写
uchar strsearch(uchar *ptr2 ,uchar *ptr1_at ,uint Len)//查字符串*ptr2在*ptr1中的位置
{
//*ptr2为目标字符串,必须以'\0'结束
//*ptr1为源字符,长度为Len
//返回:  0  没有找到
//1-255 从第N个字符开始相同
	uint i,j,k;
	uchar Data;
	if(ptr2[0]==0) return(0);
	for(i=0,j=0;i<Len;i++)
	{
		Data=ptr2[j];
	#if 1 == upper_lower_EN
		if(ptr1_at[i]!=Data)
		{
			if(Data>='a'&&Data<='z')Data=Data-'a'+'A';
			else if(Data>='A'&&Data<='Z')Data=Data-'A'+'a';
		}
	#endif
		if(ptr1_at[i] == Data)
		{//第一个字符相同
			for(k=i;k<Len;k++,j++)
			{
				Data=ptr2[j];
				if(Data == '\0')//比较正确
					return(i+1);//返回值是整数，不含0
			#if 1 == upper_lower_EN
				if(ptr1_at[k]!=Data)
				{
					if(Data>='a'&&Data<='z')Data=Data-'a'+'A';
					else if(Data>='A'&&Data<='Z')Data=Data-'A'+'a';
				}
			#endif
				if(ptr1_at[k]!=Data) break;
			}
			j=0;
		}
	}
	return(0);
}

void BT_Init(void)
{
	uchar xdata BT_Name[]="AT+NAMERF-BLE4.0\n";
	
	REST_BT=0;
	Time0_=0;
	while(Time0_<5);
	REST_BT=1;
	Time0_=0;
	while(Time0_<5);
	
	AT_BT=0;
	Time0_=0;
	while(Time0_<5);
	UART2_rs[RH_ID]=0;
	UART2_rs[Time0_COM]=0;
	receive_number2=0;
	receive_data2[0]='\0';
	Send_string1("\nAT+VER\n",0);
	Send_string2("AT+VER\n",0);
	Time0_=0;
	while(Time0_<5);//此处不可大于串口超时时间
	UART2_rs[RH_ID]=0;
	Send_string1(&receive_data2[0],receive_number2);
	receive_number2=0;
	Send_char1('\r');Send_char1('\n');
	
	UART2_rs[RH_ID]=0;
	UART2_rs[Time0_COM]=0;
	receive_number2=0;
	receive_data2[0]='\0';
	Send_string1("\nAT+NAME\n",0);
	Send_string2("AT+NAME\n",0);
	Time0_=0;
	while(strsearch("RF-BLE4.0",receive_data2,receive_number2) == 0)if(Time0_>15)break;
	//+NAME:RF-BLE4.0
	UART2_rs[RH_ID]=0;
	Send_string1(&receive_data2[0],receive_number2);
	receive_number2=0;
	if(Time0_>15)
	{		
		UART2_rs[RH_ID]=0;
		UART2_rs[Time0_COM]=0;
		receive_number2=0;
		receive_data2[0]='\0';
		Send_char1('\r');Send_char1('\n');
		Send_string1(BT_Name,0);
		Send_string2(BT_Name,0);
		Time0_=0;
		while(strsearch("OK",receive_data2,receive_number2) == 0)if(Time0_>15)break;
		UART2_rs[RH_ID]=0;
		Send_string1(&receive_data2[0],receive_number2);
		receive_number2=0;
	}
	
	UART2_rs[RH_ID]=0;
	UART2_rs[Time0_COM]=0;
	receive_number2=0;
	receive_data2[0]='\0';
	Send_string1("\nAT+MAC\n",0);
	Send_string2("AT+MAC\n",0);
	Time0_=0;
	while(Time0_<5);//此处不可大于串口超时时间
	UART2_rs[RH_ID]=0;
	Send_string1(&receive_data2[0],receive_number2);
	receive_number2=0;
	
	UART2_rs[Time0_COM]=0;
	AT_BT=1;
}
