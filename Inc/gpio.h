/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
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
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define G_LED_Pin GPIO_PIN_11
#define G_LED_GPIO_Port GPIOA
#define B_LED_Pin GPIO_PIN_12
#define B_LED_GPIO_Port GPIOA
#define KEY_Pin GPIO_PIN_15
#define KEY_GPIO_Port GPIOA
#define Test_Pin GPIO_PIN_0
#define Test_GPIO_Port GPIOA


#define PA2_Pin GPIO_PIN_2
#define PA3_Pin GPIO_PIN_3
#define PA4_Pin GPIO_PIN_4
#define PA5_Pin GPIO_PIN_5
#define PA6_Pin GPIO_PIN_6
#define PA7_Pin GPIO_PIN_7


#define PB6_Pin GPIO_PIN_6
#define PB7_Pin GPIO_PIN_7


/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define LED_STATE_SYS_OPEN	 			1
#define	LED_STATE_PRODUCT_PROCESS		2
#define LED_STATE_PRODUCT_SUCCESS		3
#define LED_STATE_PRODUCT_FALSE			4
#define LED_STATE_PRODUCT_KEY1_PRESS	5
#define LED_STATE_PRODUCT_KEY2_PRESS	6
#define	LED_STATE_CONNECTED			    7
#define	LED_STATE_SENDDATA			    8
#define	LED_GREEN_FLASH				    9
#define	LED_TWO_ON				    10


/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
void delay_us(uint32_t i);
void delay_ms(uint32_t i);
int Find_String( unsigned char *source_string,unsigned char *expect_string, unsigned int length );
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
