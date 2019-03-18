
#ifndef __RC522_H
#define __RC522_H


#define s8  char
#define u8 unsigned char
#define u16 unsigned int


sbit     NSS  = P1^2;			
sbit     SCK  = P1^5;
sbit     MOSI = P1^3;
sbit     MISO = P1^4;
sbit     MFRST= P3^3;

 
//通讯缓存最大长度
#define MAXRLEN 					64

//MF522命令字
#define PCD_IDLE              		0x00               //取消当前命令
#define PCD_AUTHENT           		0x0E               //验证密钥
#define PCD_RECEIVE           		0x08               //接收数据
#define PCD_TRANSMIT          		0x04               //发送数据
#define PCD_TRANSCEIVE        		0x0C               //发送并接收数据
#define PCD_RESETPHASE        		0x0F               //复位
#define PCD_CALCCRC           		0x03               //CRC计算

//Mifare_One卡片命令字
#define PICC_REQIDL           		0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           		0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        		0x93               //防冲撞
#define PICC_ANTICOLL2        		0x95               //防冲撞
#define PICC_AUTHENT1A        		0x60               //验证A密钥
#define PICC_AUTHENT1B        		0x61               //验证B密钥
#define PICC_READ             		0x30               //读块
#define PICC_WRITE            		0xA0               //写块
#define PICC_DECREMENT        		0xC0               //扣款
#define PICC_INCREMENT        		0xC1               //充值
#define PICC_RESTORE          		0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         		0xB0               //保存缓冲区中数据
#define PICC_HALT             		0x50               //休眠

//MF522 FIFO长度定义
#define DEF_FIFO_LENGTH       		64                 //FIFO size=64byte

//MF522寄存器定义
// PAGE 0
#define     RFU00                 	0x00    //保留
#define     CommandReg            	0x01    //启动和停止命令的执行
#define     ComIEnReg             	0x02    //中断请求传递的使能和禁能控制位
#define     DivlEnReg             	0x03    //中断请求传递的使能和禁能控制位
#define     ComIrqReg             	0x04    //包含中断请求标志
#define     DivIrqReg             	0x05	//包含中断请求标志
#define     ErrorReg              	0x06    //错误标志,指示执行的上个命令的错误状态
#define     Status1Reg            	0x07    //包含通信的状态标志
#define     Status2Reg            	0x08    //包含接收器和发射器的状态标志
#define     FIFODataReg           	0x09    //64字节FIFO缓冲区的输入和输出
#define     FIFOLevelReg          	0x0A	//指示FIFO中存储的字节数
#define     WaterLevelReg         	0x0B    //定义FIFO下溢和上溢报警的FIFO深度
#define     ControlReg            	0x0C    //不同的控制寄存器
#define     BitFramingReg         	0x0D    //面向位的帧的调节
#define     CollReg               	0x0E    //RF接口上检测到的第一个位冲突的位的位置
#define     RFU0F                 	0x0F    //保留
// PAGE 1     
#define     RFU10                 	0x10    //保留
#define     ModeReg               	0x11    //定义发送和接收的常用模式
#define     TxModeReg             	0x12    //定义发送过程的数据传输速率
#define     RxModeReg             	0x13    //定义接收过程中的数据传输速率
#define     TxControlReg          	0x14    //控制天线驱动器管教TX1和TX2的逻辑特性
#define     TxAutoReg             	0x15    //控制天线驱动器的设置
#define     TxSelReg              	0x16    //选择天线驱动器的设置
#define     RxSelReg              	0x17    //选择内部的接收器设置
#define     RxThresholdReg        	0x18    //选择位译码器的阈值
#define     DemodReg              	0x19    //定义解调器的设置
#define     RFU1A                 	0x1A    //保留
#define     RFU1B                 	0x1B    //保留
#define     MifareReg             	0x1C    //控制ISO 14443/MIFARE模式中106kbit/s的通信
#define     RFU1D                 	0x1D    //保留
#define     RFU1E                 	0x1E    //保留
#define     SerialSpeedReg        	0x1F    //选择串行UART接口的速率
// PAGE 2    
#define     RFU20                 	0x20    //保留
#define     CRCResultRegM         	0x21    //显示CRC计算的实际MSB值
#define     CRCResultRegL         	0x22    //显示CRC计算的实际LSB值
#define     RFU23                 	0x23    //保留
#define     ModWidthReg           	0x24    //控制MODWIDTH的设置
#define     RFU25                 	0x25    //保留
#define     RFCfgReg              	0x26    //配置接收器增益
#define     GsNReg                	0x27    //选择天线驱动器管脚TX1和TX2的调制电导
#define     CWGsCfgReg            	0x28    //选择天线驱动器管脚TX1和TX2的调制电导
#define     ModGsCfgReg           	0x29	  //选择天线驱动器管脚TX1和TX2的调制电导
#define     TModeReg              	0x2A    //定义内部定时器的设置
#define     TPrescalerReg         	0x2B    //定义内部定时器的设置
#define     TReloadRegH           	0x2C    //描述16位长的定时器重装值
#define     TReloadRegL           	0x2D    //描述16位长的定时器重装值
#define     TCounterValueRegH     	0x2E    //显示16位长的实际定时器值
#define     TCounterValueRegL     	0x2F    //显示16位长的实际定时器值
// PAGE 3      
#define     RFU30                 	0x30    //保留
#define     TestSel1Reg           	0x31    //常用测试信号的配置
#define     TestSel2Reg           	0x32    //常用测试信号的配置和PRBS控制
#define     TestPinEnReg          	0x33    //D1-D7输出驱动器的使能管脚(注:仅用于串行接口)
#define     TestPinValueReg       	0x34    //定义D1-D7用作I/O总线时的值
#define     TestBusReg            	0x35    //显示内部测试总线的状态
#define     AutoTestReg           	0x36    //控制数字自测试
#define     VersionReg            	0x37    //显示版本
#define     AnalogTestReg         	0x38    //控制管脚AUX1和AUX2
#define     TestDAC1Reg           	0x39    //定义TestDAC1的测试值
#define     TestDAC2Reg           	0x3A    //定义TestDAC2的测试值
#define     TestADCReg            	0x3B    //显示ADCI 和Q通道的实际值
#define     RFU3C                 	0x3C    //保留
#define     RFU3D                 	0x3D    //保留
#define     RFU3E                 	0x3E    //保留
#define     RFU3F		          	0x3F    //保留

//和MF522通讯时返回的错误代码
#define 	MI_OK					(0)		  //返回成功
#define 	MI_NOTAGERR				(-1)	  //返回目标错误
#define 	MI_ERR					(-2)	  //返回错误

//文件线路保护方式
#define		MOD_NOPROTECT			0x00	//不使用线路保护
#define		MOD_CRYPT				0xC0	//线路保护且加密

//CPU卡通讯返回状态字
#define		ST_OK					0x9000	//正确执行
#define		ST_ERR					0xFFFF	//返回错误
#define		ST_RETDATWRG			0x6281	//返回数据错误
#define		ST_VERIFY				0x6283	//选择文件无效,文件或密钥校验错误
#define		ST_STNCHG				0x6400	//状态标志未改变
#define		ST_EEPWRG				0x6581	//写EEPROM不成功
#define		ST_LENWRG				0x6700	//错误长度
#define		ST_CLAPTWRG				0x6900	//CLA与线路保护要求不匹配
#define		ST_INVALID				0x6901	//无效的状态
#define		ST_COMMAND				0x6981	//命令与文件结构不相容
#define		ST_SECURE				0x6982	//不满足安全状态
#define		ST_KEYLOCK				0x6983	//密钥锁死
#define		ST_USECON				0x6985	//使用条件不满足
#define		ST_NSPACK				0x6987	//无安全报文
#define		ST_SPDATWRG				0x6988	//安全报文数据项不正确
#define		ST_DATPRWRG				0x6A80	//数据域参数错误
#define		ST_FUNWRG				0x6A81	//功能不支持或卡中无MF或卡片锁定
#define		ST_NOFILE				0x6A82	//文件未找到
#define		ST_NORECORD				0x6A83	//记录未找到
#define		ST_NOSPACE				0x6A84	//文件无足够空间
#define		ST_P1P2WRG				0x6A86	//参数P1,P2错误,文件已存在
#define		ST_NOKEY				0x6A88	//密钥未找到
#define		ST_OFFSETWRG			0x6B00	//在达到Le/Lc字节之前文件结束,偏移量错误
#define		ST_CLAWRG				0x6E00	//无效的CLA
#define		ST_DATWRG				0x6F00	//数据无效
#define		ST_MACWRG				0x9302	//MAC错误
#define		ST_APPLOCK				0x9303	//应用已被锁定
#define		ST_SUMLACK				0x9401	//金额不足
#define		ST_NOKEY2				0x9403	//密钥未找到
#define		ST_MACINVAL			    0x9406	//所需的MAC不可用

//密钥类型
#define		INTERNALKEY			0x34	//内部认证密钥
#define		PROTECTKKEY			0x36	//文件线路保护密钥
#define		EXTERNALKEY			0x39	//外部认证密钥
/*
extern u8 ReadACard;			//读到卡标志
extern u8 fcnt;					//蜂鸣器和LED响应时间
extern u8 Key[6];				//密钥
   */
void PcdInit(void); 
void PcdSwitchPCB(void);
s8 PcdConfigISOType(u8 type);//
void WriteRawRC(u8 Address, u8 value);//
u8 ReadRawRC(u8 Address);
void SetBitMask(u8 reg,u8 mask);
void ClearBitMask(u8 reg,u8 mask);
void PcdAntennaOff(void);
void PcdAntennaOn(void);
s8 PcdComCmd(u8 Command, u8 *pDataIn, u8 InLenByte, u8 *pDataOut, u16  *pOutLenBit);
s8 PcdReset(void);
void CalulateCRC(u8 *pIndata,u8 len,u8 *pDataOut);
s8 PcdHalt(void);
s8 PcdAnticoll(u8 *pSnr);
s8 PcdRequest(u8 req_code,u8 *pTagType);
s8 PcdRats(u8 * DataOut,u8 * Len);
u16 PcdEraseDF(void);
s8 PcdEraseCheck(u8* pDataOut,u8 *Len);
u16 PcdCreateFile(u8 *fileid, u8 Lc, u8 *pDataIn);
u16 PcdGetChallenge(u8* pRan);
u16 PcdExAuth(u8 keyflag,u8 *pRan);
u16 PcdSelectFile(u8* pDataIn, u8* pDataOut,u8 * Len);
u16 PcdReadBinary(u8 offset, u8 Len, u8* pDataOut);
u16 PcdUpdateBinary(u8 offset, u8 Len, u8* pDataIn);
u16 PcdSetKey(u8 Keysign, u8 Len, u8* pDataIn);
u16 GetCard(u8 Reqcode, u8* pTagType, u8* pSnr);
u16 CardReset(u8 * Data_Out,u8 *  Len);
void Request_loop();
u8 Pcd_Cmd(u8* pDataIn, u8  In_Len, u8* pDataOut,u8 * Out_Len);
s8 PcdEraseCheck(u8* pDataOut,u8 *Len);
#endif


