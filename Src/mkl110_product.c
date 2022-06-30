#include "mk110_product.h"
#include "gpio.h"
#include "string.h"
#include "hw.h"
#include "Driver_IIC_SHT30.h"
#include "sys_timer.h"




void TestGPIO_OutPut_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = PA4_Pin|PA5_Pin|PA6_Pin|PA7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PB6_Pin|PB7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PA2_Pin|PA3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



  /*Configure GPIO pin Output Level */
//   HAL_GPIO_WritePin(GPIOA, PA2_Pin, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(GPIOA, PA3_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOA, PA4_Pin, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOA, PA5_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOA, PA6_Pin, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOA, PA7_Pin, GPIO_PIN_RESET);
//   HAL_GPIO_WritePin(GPIOB, PA6_Pin, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(GPIOB, PA7_Pin, GPIO_PIN_RESET);
}

void TestGPIO_InPut_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = PA4_Pin|PA5_Pin|PA6_Pin|PA7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PB6_Pin|PB7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PA2_Pin|PA3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



  /*Configure GPIO pin Output Level */
//   HAL_GPIO_WritePin(GPIOA, PA2_Pin, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(GPIOA, PA3_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOA, PA2_Pin, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOA, PA3_Pin, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOB, PA6_Pin, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOB, PA7_Pin, GPIO_PIN_RESET);
//   HAL_GPIO_WritePin(GPIOB, PA6_Pin, GPIO_PIN_SET);
//   HAL_GPIO_WritePin(GPIOB, PA7_Pin, GPIO_PIN_RESET);


}

 uint8_t product_test_flag = 0;
//static uint8_t product_success_flag = 0;
uint8_t set_product_flag(void)
{
    product_test_flag = 0;
	return product_test_flag;
}

uint8_t get_product_flag(void)
{
	return product_test_flag;
}

//指示灯设置函数
//指示灯设置函数
void set_led_state(uint8_t     led_state)
{
	LOG_LUO("set led state is %d\r\n",led_state);
	switch(led_state)
	{
		case LED_STATE_SYS_OPEN:
			  /*Configure GPIO pin Output Level */
            HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_SET);
			break;
		case LED_STATE_CONNECTED:
            HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_RESET);
			break;
		case LED_STATE_PRODUCT_PROCESS:
			HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);
		    delay_ms(200);
		    HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_SET);
			delay_ms(200);
		    HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_RESET);
			break;
		case LED_STATE_PRODUCT_SUCCESS:
			HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_RESET);
			break;
		case LED_STATE_PRODUCT_FALSE:
			HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);
            //HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_RESET);
			break;
		case LED_STATE_PRODUCT_KEY2_PRESS:
            HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
		    delay_ms(200);
		    HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
		    delay_ms(200);
		    HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
		    delay_ms(200);
		    HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
            delay_ms(200);
		    HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
		    delay_ms(200);
		    HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
            break;
        case LED_STATE_SENDDATA:
			HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
		    delay_ms(200);
		    HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
		    break;
		case LED_GREEN_FLASH:
			HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);
		    delay_ms(200);
		    HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_SET);
			delay_ms(200);
		    HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);
			break;
		case LED_TWO_ON:
			HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_SET);
			break;
	}

}

//每个步骤超时失败
void product_test_process(void)
{
	static  uint8_t state = 0;
    static int16_t Test_temp;
	static int16_t Test_hum;
	product_check();
	if(get_product_start_flag() == 0)
	{
		return;
	}
	switch(state)
	{
		case 0:
			set_led_state(LED_STATE_PRODUCT_PROCESS);
			LOG_LUO("product product_test_process start\r\n");
			state = 1;
			break;
		case 1:
//			 ReadTemHumTaskInit();//已被初始化
             set_led_state(LED_STATE_PRODUCT_PROCESS);
             LOG_LUO("------>ReadTemHumTaskInit......\r\n");
             delay_ms(100);
        	 Read_tempHum_valu();
		     Test_temp=Get_sht30TemValu();
			 Test_hum = Get_sht30HumValu();
			if(Test_temp != 0 || Test_hum != 0)
			{
				state = 2;
				//TestGPIO_OutPut_Init();
				LOG_LUO("product ReadTemHum success\r\n");
                //delay_ms(100);   
			}
			else
			{
				state = 3;
				LOG_LUO("product ReadTemHum false\r\n");
			}
			break;
		case 2:
			state = 4;
//			product_success_flag = 1;
			LOG_LUO("product  success\r\n");
			set_led_state(LED_TWO_ON);
			break;
		case 3:
			state = 4;
			LOG_LUO("product  false\r\n");
			set_led_state(LED_STATE_PRODUCT_FALSE);
			break;
		case 4:        
			break;
	}
}

static uint8_t product_start_flag = 0;
uint8_t get_product_start_flag(void)
{
	return product_start_flag;
}
void product_check(void)
{
    static uint8_t state = 0;
	static uint32_t rtc = 0;
    switch(state)
    {
        case 0:
			state  = 1;
			set_led_state(LED_STATE_SYS_OPEN);
            break;
        case 1:
            //按键按下
			if(HAL_GPIO_ReadPin(GPIOA,KEY_Pin)==0)//默认情况下为1
			{
				//按键防抖
				delay_ms(20);
				if(HAL_GPIO_ReadPin(GPIOA,KEY_Pin)==0)
				{
					LOG_LUO("press the first time\r\n");
					state = 2;
					rtc = get_rtc();
				}
			}
            break;
        case 2:
			//按键释放
			if(HAL_GPIO_ReadPin(GPIOA,KEY_Pin)==1)//释放情况下为1
			{
				LOG_LUO("rease the key\r\n");
				state = 3;
				product_start_flag = 1;
			}
			if(timepassed_rtc(rtc, KEY_TIMEOUT))
			{
				state = 3;
			}
            break;
		case 3:
			break;
    }
}

//------------------------------
void test(void)
{
	static uint8_t state = 0;
	switch(state)
	{
		case 0:
			state = 1;
			//TestGPIO_InPut_Init();
			break;
		case 1:
			state = HAL_GPIO_ReadPin(GPIOA,KEY_Pin);
			LOG_LUO("product  state %d\r\n",state);
			break;
		
	}
}








