/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include "string.h"
#include "hw.h"
#include "Driver_IIC_SHT30.h"





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
static uint8_t product_led_flag = 0;
static uint8_t product_uart_flag = 0;
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
            HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);
			HAL_GPIO_TogglePin(GPIOA, B_LED_Pin);
		    delay_ms(200);
		    HAL_GPIO_TogglePin(GPIOA, B_LED_Pin);
			break;
		case LED_STATE_PRODUCT_SUCCESS:
			HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_RESET);
			break;
		case LED_STATE_PRODUCT_FALSE:
			HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_SET);
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
	}

}


void product_test_process(void)
{
	static  uint8_t state = 0;
	static uint8_t gpio_sta = 0;
    static uint8_t gpio_data[8];
    static uint8_t uart_buf[256];
    static int16_t Test_temp;

    if(UsartType1.receive_flag)
     {
         memcpy(uart_buf, &UsartType1.usartDMA_rxBuf, UsartType1.rx_len);
         Usart1SendData_DMA(uart_buf, UsartType1.rx_len);
         delay_ms(300);
         memset(uart_buf, 0x00, sizeof(uart_buf));
         UsartType1.receive_flag = 0;
         product_uart_flag = 1;
     }
    if(!HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin))
    {
        set_led_state(LED_STATE_PRODUCT_KEY2_PRESS);
        product_led_flag = 1;
    }
    
	switch(state)
	{
		case 0:
			set_led_state(LED_STATE_PRODUCT_PROCESS);
			LOG_LUO("product product_test_process start\r\n");
			state = 1;
			break;
		case 1:
//			 ReadTemHumTaskInit();
             set_led_state(LED_STATE_PRODUCT_PROCESS);
             LOG_LUO("------>ReadTemHumTaskInit......\r\n");
             delay_ms(100);
        	 Read_tempHum_valu();
		     Test_temp=Get_sht30TemValu();
			if(Test_temp>0)
			{
				state = 2;
				TestGPIO_OutPut_Init();
				LOG_LUO("product TestGPIO_Test start\r\n");
                delay_ms(100);   
			}
			break;
		case 2: 
            LOG_LUO("product TestGPIO_Test H------>\r\n");
            set_led_state(LED_STATE_PRODUCT_PROCESS);
            gpio_data[0]=HAL_GPIO_ReadPin(GPIOA,PA2_Pin);
            gpio_data[1]=HAL_GPIO_ReadPin(GPIOA,PA3_Pin);
            gpio_data[2]=HAL_GPIO_ReadPin(GPIOB,PB6_Pin);
            gpio_data[3]=HAL_GPIO_ReadPin(GPIOB,PB7_Pin);
            gpio_sta = gpio_data[0]+gpio_data[1]+gpio_data[2]+gpio_data[3];
            if(gpio_sta==2)
            {
                state = 3;
                 TestGPIO_InPut_Init();
                 delay_ms(100);
            }
            else
                state = 5;
			break;
        case 3: 
            LOG_LUO("product TestGPIO_Test L------>\r\n");
            set_led_state(LED_STATE_PRODUCT_PROCESS);
            gpio_data[0]=HAL_GPIO_ReadPin(GPIOA,PA4_Pin);
            gpio_data[1]=HAL_GPIO_ReadPin(GPIOA,PA5_Pin);
            gpio_data[2]=HAL_GPIO_ReadPin(GPIOA,PA6_Pin);
            gpio_data[3]=HAL_GPIO_ReadPin(GPIOA,PA7_Pin);
            gpio_sta = gpio_data[0]+gpio_data[1]+gpio_data[2]+gpio_data[3];
            if(gpio_sta==2)
            {
                state = 4;
            }
            else
                state = 5;
			break;
		case 4:
			state = 6;
//			product_success_flag = 1;
			LOG_LUO("product  success\r\n");
			set_led_state(LED_STATE_PRODUCT_SUCCESS);
			break;
		case 5:
			state = 6;
			LOG_LUO("product  false\r\n");
			set_led_state(LED_STATE_PRODUCT_FALSE);
			break;
		case 6:        
			break;
	}
    if((product_led_flag==1)&&(product_uart_flag==1)&&(state==6))
    {
        product_test_flag = 1;
        product_led_flag = 0;
        product_uart_flag = 0;
        state = 0;
    }
    else product_test_flag = 0;
}
