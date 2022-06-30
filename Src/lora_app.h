#ifndef __LORA_APP__H_
#define __LORA_APP__H_

#include "stdint.h"


#define WORK_MODE	"LORAWAN"
#define JOIN_MODE	"OTAA"
#define REGION		"US915"//"EU868"
#define CLASE		"A"
#define ADR_ENABLE		"OFF"
#define TX_POWER		"0"
#define DR_TYPE			"0"//"1"
#define CH_DUTYCYCLE		"OFF"
#define APP_DUTYCYCLE		0
#define DWELL_UP		1
#define DWELL_DOWN		1
#define UNC_REPEAT		"1"
#define LORA_APPEUI_STR		"70:B3:D5:7E:D0:02:6B:87"
#define LORA_APPKEY_STR		"2B:7E:15:16:28:AE:D2:A6:AB:F7:15:88:09:CF:4F:3C"

typedef struct
{
	char work_mde[10];
	char join_mde[5];
	char region[7];
	char lora_class[2];
	char deveui[25];
	char appeui[25];
	char appkey[50];
	char devaddr[12];
	char adr_enable[5];
	char tx_power[3];
	char dr[3];
	char ch_dutycycle[5];
	uint8_t app_dutycycle;
	uint8_t dwell_up;
	uint8_t dwell_down;
	char lora_unc_repeat[5];
	
}LORAWAN_PARAMS;


void uart_process(void);

void lora_process(void);


#endif

