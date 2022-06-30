#ifndef USERAPP_SHT30_H
#define USERAPP_SHT30_H
#include "stdbool.h"
#endif

#ifdef	USERAPP_SHT30_C


#else /*LIB_IO***********************************************************************/

extern void ReadTemHumTaskInit(void);

//读取温湿度值
extern void Read_tempHum_valu(void);

extern void ReadTemHumTask(void);

//清除sht30温湿度值
extern void Clr_sht30TemHumValue(void);

extern int Get_sht30HumValu(void);

extern int Get_sht30TemValu(void);

extern void sht30TimeCntCallBack(void);

//获取温湿度初始化完成标志
extern bool getReadTempHumResult(void);

#endif

