#ifndef __PRODUCT__TEST__H
#define __PRODUCT__TEST__H

#include "stdint.h"

#define TEST_IO		15	
uint8_t get_product_flag(void);
void product_test_process(void);
uint8_t set_product_flag(void);
#endif
