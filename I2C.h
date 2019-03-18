 #ifndef _I2C_H_
#define _I2C_H_
 // #include "15f.h"
#include "STC15F2K60S2.h"
    #define uint8_t    unsigned char 
 #define uint16_t    unsigned int 

 /* FM327 reg for I2C */
#define FM327_FIFO				0xFFF0
#define FIFO_FLUSH_REG			0xFFF1
#define	FIFO_WORDCNT_REG		0xFFF2
#define RF_STATUS_REG			0xFFF3
#define RF_TXEN_REG				0xFFF4
#define RF_BAUD_REG				0xFFF5
#define RF_RATS_REG				0xFFF6
#define MAIN_IRQ_REG			0xFFF7
#define FIFO_IRQ_REG			0xFFF8
#define AUX_IRQ_REG				0xFFF9
#define MAIN_IRQ_MASK_REG		0xFFFA
#define FIFO_IRQ_MASK_REG		0xFFFB
#define AUX_IRQ_MASK_REG		0xFFFC
#define NFC_CFG_REG				0xFFFD
#define VOUT_CFG_REG			0xFFFE
#define EE_WR_CTRL_REG			0xFFFF

#define MAIN_IRQ_RF_PWON 0x80 
#define MAIN_IRQ_ACTIVE 0x40
#define MAIN_IRQ_RX_START 0x20
#define MAIN_IRQ_RX_DONE 0x10
#define MAIN_IRQ_TX_DONE 0x08
#define MAIN_IRQ_ARBIT 0x04
#define MAIN_IRQ_FIFO 0x02
#define MAIN_IRQ_AUX 0x01


#define FALSE  0
#define TRUE   1

char m24sr_answer_nfc(char x);
void I2C_GPIO_Config(void);
void sEE_WaitEEStandbyState(void);
uint8_t FM11_ReadE2(uint8_t *pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);
uint8_t FM11_WriteE2(uint8_t *pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
uint8_t FM11_ReadReg(uint16_t addr);
uint8_t FM11_WriteReg(uint16_t addr, uint8_t da);
uint8_t FM11_ReadFIFO(uint8_t NumByteToRead,uint8_t *pbuf);
uint8_t FM11_WriteFIFO(uint8_t *pbuf, uint8_t NumByteToWrite);
void FM11_Init(void);
void FM11_RFDataTx(uint16_t ilen,uint8_t *ibuf);
 extern unsigned  char ovt;
#endif
