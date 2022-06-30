/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include "string.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();


  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = G_LED_Pin|B_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = Test_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Test_GPIO_Port, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);

}

/* USER CODE BEGIN 2 */
void delay_us(uint32_t i)
{
//    uint32_t temp;
//    SysTick->LOAD=4*i;         //设置重装计数器，32MHz时钟
//    SysTick->CTRL=0X01;         //使能，减到零是无动作，采用外部时钟源
//    SysTick->VAL=0;                //清零计数器
//    do
//    {
//        temp=SysTick->CTRL;           //读取当前值
//    }
//    while((temp&0x01)&&(!(temp&(1<<16))));     //等待时间到达
//    SysTick->CTRL=0;    //关闭计数器
//    SysTick->VAL=0;        //清空计数器
	HAL_Delay(i);
}

void delay_ms(uint32_t i)
{
//    uint32_t temp;
//    SysTick->LOAD=16000*i;         //设置重装计数器，32MHz时钟
//    SysTick->CTRL=0X01;         //使能，减到零是无动作，采用外部时钟源
//    SysTick->VAL=0;                //清零计数器
//    do
//    {
//        temp=SysTick->CTRL;           //读取当前值
//    }
//    while((temp&0x01)&&(!(temp&(1<<16))));     //等待时间到达
//    SysTick->CTRL=0;    //关闭计数器
//    SysTick->VAL=0;        //清空计数器
    HAL_Delay(i);
}

int Find_String( unsigned char *source_string,unsigned char *expect_string, unsigned int length )
{
    unsigned int expect_length;
	unsigned int match_position;
	unsigned int i;

	expect_length = strlen( (const char *)expect_string  );
	match_position = 0;
    for(i=0;i<length;i++)
    {
		if( source_string[i] == expect_string[match_position] )
		{
			match_position++;
		}
		else
		{
			if(match_position != 0)
			{
				match_position = 0;
				if( source_string[i] == expect_string[match_position] )
					match_position++;
			}
		}
   	    if( match_position == expect_length )
            return (i+1-match_position);
    }
    return -1;
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
