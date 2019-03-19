#ifndef UART_H
#define UART_H 1

#define Frame_Start 0xAA
#define Frame_End 0x55

//extern unsigned char data timer_L,timer_H;
//extern bit sam_timer_flag;

struct uart_buff
 	{
	unsigned char length;
	unsigned char *buff;
	unsigned char chk;
	};

struct status
{
 unsigned char send_finish;		//串口完成发送一帧数据标志位
 unsigned char rece_finish;		//串口完成接收一帧数据标志位
 unsigned char rece_error;			//串口接收出错标志
 unsigned char send_error;			//串口发送出错标志
};	  

extern struct status data uart_status;
extern struct uart_buff send_buff;
extern struct uart_buff rece_buff;
extern unsigned	char uart_data[256];


void send(char sj);
 
void Send_char1(unsigned char Data);//发送单个字节

void Send_string1(unsigned char *pz,unsigned int Len);

void Debug_Send_char1(unsigned char Data);//发送单个字节

void Debug_Send_string1(unsigned char *pz,unsigned int Len);

void Uart1_Init(void);

void Send_char2(unsigned char Data);//发送单个字节

void Send_string2(unsigned char *pz,unsigned int Len);

void Uart2_Init(void);

void Hex_Print(unsigned char uc_data);

void Uart_Send(void);

#endif