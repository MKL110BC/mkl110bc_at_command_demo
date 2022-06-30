#include "sys_timer.h"
#include "stm32l1xx_hal.h"



uint32_t ms_to_rtc(uint32_t ms)
{
	return ( uint32_t )HAL_GetTickFreq()*ms;
}

uint32_t get_rtc(void)
{
	return HAL_GetTick();
}

uint8_t timepassed_rtc(uint32_t rtc,uint32_t ms)
{
	if(((uint32_t)get_rtc()-rtc)>=ms_to_rtc(ms))
		return 1;
	return 0;
}






