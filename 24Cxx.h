

#ifndef __24Cxx_H__
	#define __24Cxx_H__
	
	#define I2C_24cx 0xa0
	#define I2C_2405 0x64

	sbit _24_SDA = P0^0;
	sbit _24_SCL = P4^6;
	sbit _24_WP = P4^5;
	
	extern unsigned int type_I2C;//=512;
	extern unsigned int add_end_I2C;//=0xffff;
	extern unsigned int PageLen_I2C;//=128;
	
	uint8 readMem(uint16 page_start ,uint8 c_addr);
	void read_mul(uint16 page_start, uint8 c_addr, uint8 *message_dat,uint16 Len);
	void writeMem(uint16 page_start, uint8 c_addr ,uint8 message_in);
	void write_mul(uint16 page_start, uint8 c_addr ,uint8 *message_dat,uint8 Len);  //页写,会溢出
	void fast_write_mul(uint16 page_start,uint8 c_addr,unsigned char *message_dat,uint16 Len);//快速写I2C,防溢出
	
#endif
