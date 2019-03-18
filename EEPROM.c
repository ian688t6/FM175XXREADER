

#include "STC15F2K60S2.h"
#include "intrins.h"
#include "EEPROM.h"

void E2prom(uchar isp_iap_C,uint Address)	//对内部E2prom操作stc89c51地址在0X1000~0x1FFF 4K,stc89c52地址在0X2000~0x33FF 5K,stc89c54地址在0X4000~0xF3FF 45K
                                          //11F16XE 0x0000~b3ff 45K
											//每扇区512字节,建义只用下128字节	
{

	ISP_ADDRH=(uchar)(Address>>8);
	ISP_ADDRL=(uchar)(Address);
	ISP_CONTR=wait_time|0x80;
	ISP_CMD=isp_iap_C;	 //
	ISP_TRIG=isp_trig1;
	ISP_TRIG=isp_trig2;
	_nop_();
	ISP_CONTR=ISP_TRIG=ISP_CMD=0x00; //清除isp_iap操作,以防勿操作
	ISP_ADDRH=ISP_ADDRL=0xFF;
//	EA=1;				 //开中断
}
