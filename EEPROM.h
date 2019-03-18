

#ifndef __EEPROM_H__
  #define __EEPROM_H__

  #define Clock 11059200L//SYS_CLK
  
  #if   Clock<1000000     //<1M
      #define wait_time 7 //µÈ´ýÊ±¼ä
  #elif Clock<2000000     //2M
      #define wait_time 6
  #elif Clock<3000000     //3M
      #define wait_time 5
  #elif Clock<6000000     //6M
      #define wait_time 4
  #elif Clock<12000000    //12M
      #define wait_time 3 
  #elif Clock<20000000    //20M
      #define wait_time 2
  #elif Clock<24000000    //24M
      #define wait_time 1
  #elif Clock<30000000    //30M
      #define wait_time 0	
  #else
      #define wait_time 0
  #endif
  
  #define Byte_Read 	  0x01
  #define Byte_Write  	0x02
  #define Sector_Erase	0x03
  
//   #define ISP_DATA    IAP_DATA
//   #define ISP_ADDRH   IAP_ADDRH
//   #define ISP_ADDRL   IAP_ADDRL
//   #define ISP_CMD     IAP_CMD
//   #define ISP_TRIG    IAP_TRIG
//   #define ISP_CONTR   IAP_CONTR
  
  void E2prom(uchar isp_iap_C,uint Address);

#endif