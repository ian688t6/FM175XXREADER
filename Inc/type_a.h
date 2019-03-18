#ifndef TYPE_A_H
#define TYPE_A_H

struct PICC_A_STR
{
unsigned char ATQA[2];
unsigned char UID_len;
unsigned char UID[12];
unsigned char SAK;
};
extern struct PICC_A_STR PICC_A;


extern unsigned char TypeA_Request(unsigned char *pTagType);
extern unsigned char TypeA_WakeUp(unsigned char *pTagType);
extern unsigned char TypeA_Anticollision(unsigned char selcode,unsigned char *pSnr);
extern unsigned char TypeA_Select(unsigned char selcode,unsigned char *pSnr,unsigned char *pSak);
extern unsigned char TypeA_Halt(void);
extern unsigned char TypeA_CardActivate(struct PICC_A_STR *type_a_card);
#endif