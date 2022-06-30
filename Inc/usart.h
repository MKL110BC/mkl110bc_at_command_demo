/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define RECEIVELEN 640  
#define USART_DMA_SENDING 1//发送完成 
#define USART_DMA_SENDOVER 0//发送未完成

typedef struct  
{  
uint8_t receive_flag:1;//空闲接收标记
uint8_t dmaSend_flag:1;//发送完成标记
uint16_t rx_len;//接收长度
uint8_t usartDMA_rxBuf[RECEIVELEN];//DMA接收缓存
}USART_RECEIVETYPE;  
   
extern USART_RECEIVETYPE UsartType1,UsartType2;

void UsartReceive_IDLE(UART_HandleTypeDef *huart);
int Usart1SendData_DMA(uint8_t *pdata, uint16_t Length);
int Usart2SendData_DMA(uint8_t *pdata, uint16_t Length);
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
