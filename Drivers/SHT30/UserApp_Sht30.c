#ifndef USERAPP_SHT30_C
#define USERAPP_SHT30_C
#include "Driver_IIC_SHT30.h"
#include <stdbool.h>
#include <string.h>
#include "hw.h"
#include "lora_driver.h"
#endif

#define SHT30_DEBUG_IMFO

/*******************************************/
typedef struct
{
	unsigned char  g_sht30_ok;	
	unsigned char  tem_humdatabuff[20];
	unsigned char  iic_return_data[2];
	unsigned char  Sht30RunState;
	
	int16_t Temp_valu;
	uint16_t Hum_valu;
	int16_t Sht30TimeCnt;
}SHT30_TASK_STRUCT;

SHT30_TASK_STRUCT iic_sht30_task;

bool tempAlarmFlag = 0;
bool humAlarmFlag = 0;
bool sht30FirstReadFlag = 0;
bool sht30TempAlarmLoraFlag = 0;
bool sht30HumAlarmLoraFlag = 0;

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	 写寄存器
// Function capacity： 
// Parameter declare：
// parameter 1： 设备地址
// parameter 2： 从机高地址
// parameter 3： 从机低地址
//	......
// Return value： void
// Designed idea： 
// Author：	zhangjun
// Creation date： 2016.7.1
//////////////////////////////////////////////////////////////////////////////////////////////
void sht30_writeReg(unsigned char dev_addr,unsigned char savHigaddr,unsigned char sav_lowaddr)
{
	I2C_SHT30_Start();
	I2C_SHT30_SendByte(dev_addr);
	I2C_SHT30_WaitAck();
	I2C_SHT30_SendByte(savHigaddr);
	I2C_SHT30_WaitAck();
	I2C_SHT30_SendByte(sav_lowaddr);
	I2C_SHT30_WaitAck();
	I2C_SHT30_Stop();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	 读数据
// Function capacity： 
// Parameter declare：
// parameter 1： 设备地址
// parameter 2： 从机高地址
// parameter 3： 从机低地址
// parameter 4： 读取数据长度
// parameter 5： 类型
//	......
// Return value： void
// Designed idea： 
// Author：	zhangjun
// Creation date： 2016.7.1
//////////////////////////////////////////////////////////////////////////////////////////////
void sht30_WR_data(unsigned char dev_addr,unsigned char savHigaddr,unsigned char sav_lowaddr,unsigned char rdatalen,unsigned char if_Rtemp_hum)
{
	I2C_SHT30_Start();
	I2C_SHT30_SendByte(dev_addr);
	I2C_SHT30_WaitAck();
	I2C_SHT30_SendByte(savHigaddr);
	I2C_SHT30_WaitAck();
	I2C_SHT30_SendByte(sav_lowaddr);
	I2C_SHT30_WaitAck();  
	I2C_SHT30_Start();
	I2C_SHT30_SendByte(dev_addr|0x01);// send read commd
    I2C_SHT30_WaitAck();  
	
	if(if_Rtemp_hum==0)
	{
		for(unsigned char i=0;i<rdatalen;i++)
		{
		   iic_sht30_task.iic_return_data[i] = I2C_SHT30_ReadByte(i==(rdatalen-1)?0:1);
		} 
    }
	else
	{
		for(unsigned char i=0;i<rdatalen;i++)
		{		
		   iic_sht30_task.tem_humdatabuff[i] = I2C_SHT30_ReadByte(i==(rdatalen-1)?0:1);
		} 		   
	}
	I2C_SHT30_Stop();
}


//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	 读取温湿度初始化
// Function capacity： 
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	zhangjun
// Creation date： 2016.7.1
//////////////////////////////////////////////////////////////////////////////////////////////
void ReadTemHumTaskInit(void)
{	
	sht30_writeReg(0x88,0x30,0x20);
	//sht30_writeReg(0X88,0x30,0x93);
	sht30_writeReg(0X88,0x2B,0x32);
	
//	sht30_writeReg(0X88,0x20,0x32);
//	sht30_writeReg(0X88,0x20,0x24);
//	sht30_writeReg(0X88,0x20,0x3f);
//	
//	sht30_writeReg(0X88,0x22,0x36);
//	sht30_writeReg(0X88,0x22,0x20);
//	sht30_writeReg(0X88,0x22,0x2b);
//	
//	sht30_writeReg(0X88,0x23,0x34);
//	sht30_writeReg(0X88,0x23,0x22);
//	sht30_writeReg(0X88,0x23,0x29);
//	
	sht30_writeReg(0X88,0x27,0x37);
//	sht30_writeReg(0X88,0x27,0x21);
//	sht30_writeReg(0X88,0x27,0x2a);
//	sht30_writeReg(0X88,0x21,0x2d);

	sht30_WR_data(0X88,0xF3,0x2D,2,0);
	sht30_WR_data(0X88,0xE1,0x1F,2,0);
	sht30_WR_data(0X88,0xE1,0x14,2,0);
	sht30_WR_data(0X88,0xE1,0x09,2,0);
	sht30_WR_data(0X88,0xE1,0x02,2,0);
	
	sht30_WR_data(0X88,0xE0,0x00,6,1);

	memset(&iic_sht30_task.tem_humdatabuff,0,sizeof(iic_sht30_task.tem_humdatabuff));
	memset(&iic_sht30_task.iic_return_data,0,sizeof(iic_sht30_task.iic_return_data));
}

unsigned char crc_high_first(unsigned char *ptr, unsigned char len)
{
    unsigned char i; 
    unsigned char crc=0xff; /* 初始化crc */ 

    while(len--)
    {
        crc ^= *ptr++;  /* 获取待校验的数据 */  
        for (i=8; i>0; --i)   /* 校验一个字节的数据 */  
        { 
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }

    return (crc); 
}



//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	 读取温湿度值
// Function capacity： 
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	zhangjun
// Creation date： 2016.7.1
//////////////////////////////////////////////////////////////////////////////////////////////
void Read_tempHum_valu()
{	
    sht30_WR_data(0X88,0xE0,0x00,6,1);
	
	if((iic_sht30_task.tem_humdatabuff[2]==crc_high_first(iic_sht30_task.tem_humdatabuff,2)) && (iic_sht30_task.tem_humdatabuff[5]==crc_high_first(iic_sht30_task.tem_humdatabuff+3,2))) 
	{
		iic_sht30_task.Temp_valu = (iic_sht30_task.tem_humdatabuff[0]<<8|iic_sht30_task.tem_humdatabuff[1]);
		iic_sht30_task.Temp_valu = ((100 * iic_sht30_task.Temp_valu * 175) / 65535 - 45 * 100);
		
		iic_sht30_task.Hum_valu = (iic_sht30_task.tem_humdatabuff[3]<<8|iic_sht30_task.tem_humdatabuff[4]);
		iic_sht30_task.Hum_valu = (100 * iic_sht30_task.Hum_valu * 100) / 65535;

		sht30FirstReadFlag = 1;
	}
//	User_Protocol_data.Temp = iic_sht30_task.Temp_valu;
//	User_Protocol_data.Hnum = iic_sht30_task.Hum_valu;
     /*睡眠使能*/	
//	sht30_writeReg(0X88,0x30,0x93);
//	sht30_writeReg(0X88,0x30,0xA2);
	
		#ifdef SHT30_DEBUG_IMFO
		LOG_LUO("------>Read_tempHum_valu...Temp = [%2d] Hum = [%2d]...\r\n",iic_sht30_task.Temp_valu,iic_sht30_task.Hum_valu);	
		#endif	
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	 得到sht30温度值
// Function capacity： 
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	zhangjun
// Creation date： 2016.7.1
//////////////////////////////////////////////////////////////////////////////////////////////
int Get_sht30TemValu(void)
{
	return iic_sht30_task.Temp_valu;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	 得到sht30湿度值
// Function capacity： 
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	zhangjun
// Creation date： 2016.7.1
//////////////////////////////////////////////////////////////////////////////////////////////
int Get_sht30HumValu(void)
{
	return iic_sht30_task.Hum_valu;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Function name：	 清除sht30温湿度值
// Function capacity： 
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	zhangjun
// Creation date： 2016.7.1
//////////////////////////////////////////////////////////////////////////////////////////////
void Clr_sht30TemHumValue(void)
{
	iic_sht30_task.Sht30RunState = 0;
	iic_sht30_task.Temp_valu = 0;
	iic_sht30_task.Hum_valu = 0;
}

//获取温湿度初始化完成标志
bool getReadTempHumResult(void)
{
	return sht30FirstReadFlag;
}
