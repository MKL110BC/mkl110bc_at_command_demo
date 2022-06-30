#ifndef __SYS_TIMER__H__
#define __SYS_TIMER__H__

#include "stdint.h"
uint32_t get_rtc(void);

uint8_t timepassed_rtc(uint32_t rtc,uint32_t ms);


#endif

