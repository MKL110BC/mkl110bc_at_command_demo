#ifndef __MK110_PRODUCT__H__
#define __MK110_PRODUCT__H__

#include "stdint.h"

#define KEY_TIMEOUT	10*1000

void set_led_state(uint8_t     led_state);
void product_test_process(void);
uint8_t get_product_start_flag(void);
void test(void);
void product_check(void);



#endif

