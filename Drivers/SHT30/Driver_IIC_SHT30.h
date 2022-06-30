////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Copyright 2016 深圳捷快科技有限公司
// All rights reserved.
//
// Filename     ：Driver_GPRS.h
// Version      ：1.0
// Author       ：zhangjun
// Accomplished date ： 2016.7.1
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#ifndef Driver_IIC_SHT30_H
#define Driver_IIC_SHT30_H
//*--------------------------------------------------------------------------------------
//*符号定义
//*--------------------------------------------------------------------------------------
#endif 
#define  LIB_DRIVER_IIC_SHT30
#include "hw.h"

#ifdef LIB_DRIVER_IIC_SHT30

//IIC初始化
void I2C_SHT30_Init(void);

//I2C_SHT30启动信号
unsigned char I2C_SHT30_Start(void);

//I2C_SHT30停止信号
void I2C_SHT30_Stop(void);

//IIC产生ACK应答信号
void I2C_SHT30_Ack(void);

//IIC不产生ACK应答信号
void I2C_SHT30_NAck(void);

//IIC等待IIC的ACK应答信号
unsigned char I2C_SHT30_WaitAck(void); 	 

//IIC发送一个字节
void I2C_SHT30_SendByte(unsigned char SendByte) ;

//IIC读取一个字节
unsigned char I2C_SHT30_ReadByte(unsigned char ack);

//SHT30 IIC写一个字节
unsigned char SHT30_WriteByte(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);		     

//SHT30 IIC读一个字节
unsigned char SHT30_ReadByte(unsigned char SlaveAddress,unsigned char REG_Address);

void ReadTemHumTaskInit(void);

void Read_tempHum_valu(void);
int Get_sht30HumValu(void);
int Get_sht30TemValu(void);
#else

//IIC初始化
extern void I2C_SHT30_Init(void);

//I2C_SHT30启动信号
extern unsigned char I2C_SHT30_Start(void);

//I2C_SHT30停止信号
extern void I2C_SHT30_Stop(void);

//IIC产生ACK应答信号
extern void I2C_SHT30_Ack(void);

//IIC不产生ACK应答信号
extern void I2C_SHT30_NAck(void);

//IIC等待IIC的ACK应答信号
extern unsigned char I2C_SHT30_WaitAck(void); 	 

//IIC发送一个字节
extern void I2C_SHT30_SendByte(unsigned char SendByte) ;

//IIC读取一个字节
extern unsigned char I2C_SHT30_ReadByte(unsigned char ack);

//SHT30 IIC写一个字节
extern unsigned char SHT30_WriteByte(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data);	 

//SHT30 IIC读一个字节
extern unsigned char SHT30_ReadByte(unsigned char SlaveAddress,unsigned char REG_Address);

#endif
