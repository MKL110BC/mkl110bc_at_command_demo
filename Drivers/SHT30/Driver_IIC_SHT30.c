////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Copyright 2016 深圳捷快科技有限公司
// All rights reserved.
//
// Filename     �fDriver_SD.c
// Version      ：1.0
// Author       ：zhangjun
// Accomplished date ： 2016.7.1
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#ifndef LIB_DRIVER_IIC_SHT30
#define LIB_DRIVER_IIC_SHT30
#include "main.h"
#include "gpio.h"
#endif

#define PORT_IIC_SHT30_SDA    GPIOB
#define PORT_IIC_SHT30_SCL    GPIOB

#define PIN_IIC_SHT30_SDA     GPIO_PIN_7
#define PIN_IIC_SHT30_SCL     GPIO_PIN_6

#define SDA_SHT30_H    HAL_GPIO_WritePin(PORT_IIC_SHT30_SDA, PIN_IIC_SHT30_SDA, GPIO_PIN_SET)       
#define SDA_SHT30_L    HAL_GPIO_WritePin(PORT_IIC_SHT30_SDA, PIN_IIC_SHT30_SDA, GPIO_PIN_RESET)       

#define SCL_SHT30_H    HAL_GPIO_WritePin(PORT_IIC_SHT30_SCL, PIN_IIC_SHT30_SCL, GPIO_PIN_SET)       
#define SCL_SHT30_L    HAL_GPIO_WritePin(PORT_IIC_SHT30_SCL, PIN_IIC_SHT30_SCL, GPIO_PIN_RESET) 

#define SDA_SHT30_read    HAL_GPIO_ReadPin(PORT_IIC_SHT30_SDA,PIN_IIC_SHT30_SDA)

#define IIC_DELAY	2


 
//如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
//如果接V3.3,则IIC地址为0X69(不包含最低位).

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	  I2C_SHT30_Init 
// Function capacity：  I2C_SHT30初始化
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
void I2C_SHT30_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct; 

    GPIO_InitStruct.Pin = PIN_IIC_SHT30_SDA;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(PORT_IIC_SHT30_SDA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PIN_IIC_SHT30_SCL;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(PORT_IIC_SHT30_SCL, &GPIO_InitStruct);
    
//    SCL_SHT30_H;
//    SDA_SHT30_H;
	delay_us(IIC_DELAY/2);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	   
// Function capacity：  
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
void SDA_SHT30_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = PIN_IIC_SHT30_SDA;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(PORT_IIC_SHT30_SDA, &GPIO_InitStruct);        
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	   
// Function capacity：  
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
void SDA_SHT30_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = PIN_IIC_SHT30_SDA;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull =GPIO_PULLUP;         //上拉   
    HAL_GPIO_Init(PORT_IIC_SHT30_SDA, &GPIO_InitStruct);
}


//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	  IIC_Start 
// Function capacity：  IIC起始信号
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
unsigned char I2C_SHT30_Start(void)
{
 	SDA_SHT30_OUT();      //sda线输出
	SDA_SHT30_H;	  	  
	SCL_SHT30_H;
	delay_us(IIC_DELAY);
 	SDA_SHT30_L;           
	delay_us(IIC_DELAY);
	SCL_SHT30_L;          //钳住I2C_SHT30总线，准备发送或接收数据 	
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	  IIC_Stop 
// Function capacity：  IIC停止信号
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
void I2C_SHT30_Stop(void)
{
 	SDA_SHT30_OUT();      //sda线输出

	SCL_SHT30_L;
	SDA_SHT30_L;  
 	delay_us(IIC_DELAY);
	
	SCL_SHT30_H;
	SDA_SHT30_H;          
 	delay_us(IIC_DELAY);	         							   	
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	   
// Function capacity：  产生ACK应答
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
void I2C_SHT30_Ack(void)
{
	SCL_SHT30_L;
 	SDA_SHT30_OUT();      //sda线输出
	SDA_SHT30_L;	
	delay_us(IIC_DELAY);
	SCL_SHT30_H;
 	delay_us(IIC_DELAY);
	SCL_SHT30_L;
} 

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	   
// Function capacity：  不产生ACK应答
// Parameter declare：
// parameter 1： void
//	......
// Return value： void
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
void I2C_SHT30_NAck(void)
{
	SCL_SHT30_L;
 	SDA_SHT30_OUT();      //sda线输出
	SDA_SHT30_H;
 	delay_us(IIC_DELAY);
	SCL_SHT30_H;
 	delay_us(IIC_DELAY);
	SCL_SHT30_L;
} 

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	   
// Function capacity：  等待ACK应答
// Parameter declare：
// parameter 1： void
//	......
// Return value： 1有ACK,=0无ACK
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
unsigned char I2C_SHT30_WaitAck(void) 	 
{
    unsigned char ucErrTime = 0;

 	SDA_SHT30_IN();      //SDA设置为输入

	SDA_SHT30_H;
	delay_us(1);	
	SCL_SHT30_H;	
 	delay_us(IIC_DELAY);
	while(SDA_SHT30_read)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			I2C_SHT30_Stop();
			return 1;
		}
	}
	SCL_SHT30_L;
 	delay_us(IIC_DELAY);
	return 0;
}
 
//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	   
// Function capacity：  IIC发送一个字节
// Parameter declare：
// parameter 1： void
//	......
// Return value： 
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
void I2C_SHT30_SendByte(unsigned char SendByte) 
{
    unsigned char i;
 	SDA_SHT30_OUT();      //sda线输出
	
	SCL_SHT30_L;          //拉低时钟开始数据传输	
	delay_us(IIC_DELAY); 
    for(i=0;i<8;i++)
    {  		
        if(SendByte&0x80)
            SDA_SHT30_H;  
        else 
            SDA_SHT30_L;   
        SendByte<<=1;
		
        delay_us(IIC_DELAY/2);				
        SCL_SHT30_H;
		delay_us(IIC_DELAY);		
		SCL_SHT30_L;          //拉低时钟开始数据传输	
		delay_us(IIC_DELAY/2); 		
    }	
}  

//////////////////////////////////////////////////////////////////////////////////////////////
// Function name：	   
// Function capacity：  IIC发送一个字节
// Parameter declare：
// parameter 1： void
//	......
// Return value： 
// Designed idea： 
// Author：	fengchao
// Creation date： 2016.10.27
//////////////////////////////////////////////////////////////////////////////////////////////
unsigned char I2C_SHT30_ReadByte(unsigned char ack)
{
    unsigned char i;
    unsigned char ReceiveByte=0; 
	
 	SDA_SHT30_IN();//SDA设置为输入
	delay_us(IIC_DELAY);
    for(i=0;i<8;i++ )
	{
        SCL_SHT30_L; 
        delay_us(IIC_DELAY);
				SCL_SHT30_H;
        ReceiveByte<<=1;
        if(SDA_SHT30_read)ReceiveByte++;   
        delay_us(IIC_DELAY);
    }

    if (!ack)
        I2C_SHT30_NAck();//发送nACK
    else
        I2C_SHT30_Ack(); //发送ACK	
    return ReceiveByte;
}
