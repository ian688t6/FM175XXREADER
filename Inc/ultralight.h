#ifndef ULTRALIGHT_H
#define ULTRALIGHT_H 1


extern unsigned char Ultra_Read(unsigned char page,unsigned char *buff);
extern unsigned char Ultra_Write(unsigned char page,unsigned char *buff);

#endif