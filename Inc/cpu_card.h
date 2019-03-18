#ifndef CPU_CARD_H
#define CPU_CARD_H 1

struct ATS_STR
{
unsigned char TL;
unsigned char T0;
unsigned char TA1;
unsigned char TB1;
unsigned char TC1;
unsigned char FSCI;//FSCI=T0&0x0F
unsigned char FWI;//FWI=(TB1&0xF0)>>4;
unsigned char SFGI;//SFGI=T0&0x0F
};

struct CPU_CARD_STR
{
unsigned char PCB;
unsigned char CID;
unsigned char WTXM;
struct ATS_STR ATS;
};

extern struct CPU_CARD_STR CPU_CARD;

extern void CPU_PCB(void);
extern unsigned char CPU_BaudRate_Set(unsigned char mode);
extern unsigned char CPU_TPDU(unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_Rats(unsigned char param1,unsigned char param2,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_PPS(unsigned char pps0,unsigned char pps1,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_Random(unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_I_Block(unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_R_Block(unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_S_Block(unsigned char pcb_byte,unsigned char *rece_len,unsigned char *rece_buff);

#endif