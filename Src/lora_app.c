#include "lora_app.h"
#include "loralora_driver.h"
#include "Driver_IIC_SHT30.h"
#include "sys_timer.h"
#include "string.h"
#include "mk110_product.h"
#include <stdio.h>

//要求：初始化LORA，EU868 DR1入网，每隔10s发送一帧温湿度数据

typedef enum
{	
	LORA_AT_CHECK,
    LORA_GET_MAC,
    LORA_WORK_MODE_STATE,
    LORA_JOIN_MODE_STATE,
    LORA_REGION_STATE,
    LORA_CLASS_STATE,
    LORA_DEVEUI_STATE,
    LORA_APPEUI_STATE,
    LORA_APPKEY_STATE,
    LORA_APPSKEY_STATE,
    LORA_NWKSKEY_STATE,
    LORA_DEVADDR_STATE,
    LORA_CH_STATE,
    LORA_ADR_STATUS_STATE,
    LORA_ADR_PARAMS_STATE,
    LORA_TX_POWER_STATE,
    LORA_DR_STATE,
    LORA_DUTYCYCLE_STATE,
    LORA_RX2_CH_STATE,
    LORA_RX1_DL_STATE,
    LORA_JOIN_DL_STATE,
    LORA_DWELL_TIME_STATE,
    LORA_MC_STATE,
    LORA_UNC_REPEAT_STATE,//LORAWAN非确认帧
    LORA_LORA_TX_HEX_STATE,//LORA发送十六进制
    LORA_LORA_TX_STR_STATE,//LORA发送字符串
    LORA_PARAMS_SET_SUCCESS,
    LORA_PARAMS_SET_FALSE,
    LORA_WAITTING,
}LORA_PARMAS_SET_TYPE;

typedef enum
{
	LORA_PARAMS_SET_STATE,
	LORA_JOINNET_STATE,
	LORA_JOINNET_CHECK_STATE,
	LORA_TEMHUM_DATA_STATE,
	LORA_DATA_SENDING_STATE,
	LORA_DATA_SENDING_INTERVAL_STATE,
	LORA_WAITING_STATE,
}LORA_SENDING_STATE;

#define DATA_SENDING_INTERVAL	10*1000
#define WAITING_INTERVAL	1*1000

#define AT_TIMEOUT             100
#define LORA_SEND_TIMEOUT             5000

#define RESPECT_REC		"OK"


LORAWAN_PARAMS lorawan_params;

void init_default_lora_params(void)
{
	memset(&lorawan_params, 0, sizeof(LORAWAN_PARAMS));
	
	sprintf(lorawan_params.work_mde, WORK_MODE);
	sprintf(lorawan_params.join_mde, JOIN_MODE);
	sprintf(lorawan_params.region, REGION);
	sprintf(lorawan_params.lora_class, CLASE);
	memcpy(lorawan_params.deveui, get_deveui_params_set(),23);
	sprintf(lorawan_params.appeui, LORA_APPEUI_STR);
	sprintf(lorawan_params.appkey, LORA_APPKEY_STR);
	memcpy(&lorawan_params.devaddr,get_devaddr_params_set(),11);
	sprintf(lorawan_params.adr_enable, ADR_ENABLE);
	sprintf(lorawan_params.tx_power, TX_POWER);
	sprintf(lorawan_params.dr, DR_TYPE);
	sprintf(lorawan_params.ch_dutycycle, CH_DUTYCYCLE);
	lorawan_params.app_dutycycle = APP_DUTYCYCLE;
	lorawan_params.dwell_up = DWELL_UP;
	lorawan_params.dwell_down = DWELL_DOWN;
	sprintf(lorawan_params.lora_unc_repeat, UNC_REPEAT);
}


static uint8_t lora_params_state = LORA_AT_CHECK;
void lora_params_state_init(void)
{
	lora_params_state = LORA_AT_CHECK;
}
ACK lora_params_set(void)
{
    static uint32_t rtc = 0;
    static ACK return_value = ACK_IDLE;
    static uint8_t last_state = 0;
    switch(lora_params_state)
    {
		case LORA_AT_CHECK:
			return_value = at_system_command_exe(AT_CHECK_EXE, AT_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_GET_MAC;
				LOG_LUO("get AT succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_AT_CHECK;
                rtc = get_rtc();
				LOG_LUO("get AT false\r\n");
            }
			break;
        case LORA_GET_MAC:
            return_value = lora_mac_get();
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_WORK_MODE_STATE;
				init_default_lora_params();
				LOG_LUO("get mac succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_GET_MAC;
                rtc = get_rtc();
				LOG_LUO("get mac false\r\n");
            }
            break;
        case LORA_WORK_MODE_STATE:
            return_value = set_lora_params(LORA_WORK_MODE, (uint8_t *)lorawan_params.work_mde, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_JOIN_MODE_STATE;
				LOG_LUO("get LORA_WORK_MODE_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_WORK_MODE_STATE;
                rtc = get_rtc();
            }
            break;
        case LORA_JOIN_MODE_STATE:
            return_value = set_lora_params(LORA_JOIN_MODE, (uint8_t *)lorawan_params.join_mde, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_REGION_STATE;
				LOG_LUO("get LORA_JOIN_MODE_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_JOIN_MODE_STATE;
                rtc = get_rtc();
            }
            break;
        case LORA_REGION_STATE:
            return_value = set_lora_params(LORA_REGION, (uint8_t *)lorawan_params.region, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_CLASS_STATE;
				LOG_LUO("get LORA_REGION_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_REGION_STATE;
                rtc = get_rtc();
            }           
            break;
        case LORA_CLASS_STATE:
            return_value = set_lora_params(LORA_CLASS, (uint8_t *)lorawan_params.lora_class, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_DEVEUI_STATE;
				LOG_LUO("get LORA_CLASS_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_CLASS_STATE;
                rtc = get_rtc();
            }               
            break;
        case LORA_DEVEUI_STATE:
            return_value = set_lora_params(LORA_DEVEUI, (uint8_t *)lorawan_params.deveui, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_APPEUI_STATE;
				LOG_LUO("get LORA_DEVEUI_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_DEVEUI_STATE;
                rtc = get_rtc();
            }       
            break;
        case LORA_APPEUI_STATE:
            return_value = set_lora_params(LORA_APPEUI, (uint8_t *)lorawan_params.appeui, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_APPKEY_STATE;
				LOG_LUO("get LORA_APPEUI_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_APPEUI_STATE;
                rtc = get_rtc();
            }   
            break;
        case LORA_APPKEY_STATE:
            return_value = set_lora_params(LORA_APPKEY, (uint8_t *)lorawan_params.appkey, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_APPSKEY_STATE;
				LOG_LUO("get LORA_APPKEY_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_APPKEY_STATE;
                rtc = get_rtc();
            }   
            break;
        case LORA_APPSKEY_STATE:
            /*return_value = set_lora_params(LORA_APPSKEY, (uint8_t *)lorawan_params.appkey, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_NWKSKEY_STATE;
				LOG_LUO("get LORA_APPSKEY_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_APPSKEY_STATE;
                rtc = get_rtc();
            }*/
			lora_params_state = LORA_NWKSKEY_STATE;
            break;
        case LORA_NWKSKEY_STATE:
            /*return_value = set_lora_params(LORA_NWKSKEY, (uint8_t *)lorawan_params.appkey, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_DEVADDR_STATE;
				LOG_LUO("get LORA_NWKSKEY_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_NWKSKEY_STATE;
                rtc = get_rtc();
            }   */
			lora_params_state = LORA_DEVADDR_STATE;
            break;
        case LORA_DEVADDR_STATE:
			/*
            return_value = set_lora_params(LORA_DEVADDR, (uint8_t *)lorawan_params.devaddr, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_CH_STATE;
				LOG_LUO("get LORA_DEVADDR_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_DEVADDR_STATE;
                rtc = get_rtc();
            }   */
			lora_params_state = LORA_CH_STATE;
            break;
        case LORA_CH_STATE://不建议配置
            lora_params_state = LORA_ADR_STATUS_STATE;
            break;
        case LORA_ADR_STATUS_STATE:
            return_value = set_lora_params(LORA_ADR_STATUS, (uint8_t *)lorawan_params.adr_enable, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_ADR_PARAMS_STATE;
				LOG_LUO("get LORA_ADR_STATUS_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_ADR_STATUS_STATE;
                rtc = get_rtc();
            }
            break;
        case LORA_ADR_PARAMS_STATE:
            lora_params_state = LORA_TX_POWER_STATE;
            break;
        case LORA_TX_POWER_STATE:
            return_value = set_lora_params(LORA_TX_POWER, (uint8_t *)lorawan_params.tx_power, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);//EU868最大TX
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_DR_STATE;
				LOG_LUO("get LORA_TX_POWER_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_TX_POWER_STATE;
                rtc = get_rtc();
            }
            break;
        case LORA_DR_STATE:
            return_value = set_lora_params(LORA_DR, (uint8_t *)lorawan_params.dr, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);//固定DR 1
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_DUTYCYCLE_STATE;
				LOG_LUO("get LORA_DR_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_DR_STATE;
                rtc = get_rtc();
            }
            break;
        case LORA_DUTYCYCLE_STATE:
            return_value = set_lora_dutycycle_params((uint8_t *)lorawan_params.ch_dutycycle, lorawan_params.app_dutycycle);//关闭ch duty， app duty为0
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_RX2_CH_STATE;
				LOG_LUO("get LORA_DUTYCYCLE_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_DUTYCYCLE_STATE;
                rtc = get_rtc();
            }
            break;
        case LORA_RX2_CH_STATE:
            lora_params_state = LORA_RX1_DL_STATE;
            break;
        case LORA_RX1_DL_STATE:
            lora_params_state = LORA_JOIN_DL_STATE;
            break;
        case LORA_JOIN_DL_STATE:
            lora_params_state = LORA_DWELL_TIME_STATE;
            break;
        case LORA_DWELL_TIME_STATE:
            /*return_value = set_lora_dwelltime_params(lorawan_params.dwell_up, lorawan_params.dwell_down);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_MC_STATE;
				LOG_LUO("get LORA_DWELL_TIME_STATE succss");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_DWELL_TIME_STATE;
                rtc = get_rtc();
            }*/
			lora_params_state = LORA_MC_STATE;
            break;
        case LORA_MC_STATE:
            lora_params_state = LORA_UNC_REPEAT_STATE;
            break;
        case LORA_UNC_REPEAT_STATE:
            return_value = set_lora_params(LORA_UNC_REPEAT, (uint8_t *)lorawan_params.lora_unc_repeat, REC_TIMEOUT, (uint8_t *)RESPECT_REC, 1);
            if(return_value == ACK_SUCCESS)
            {
                lora_params_state = LORA_PARAMS_SET_SUCCESS;
				LOG_LUO("get LORA_UNC_REPEAT_STATE succss\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                lora_params_state = LORA_WAITTING;
                last_state = LORA_UNC_REPEAT_STATE;
                rtc = get_rtc();
            }
            break;
        case LORA_PARAMS_SET_SUCCESS:
            return ACK_SUCCESS;
        case LORA_PARAMS_SET_FALSE:
            break;
        case LORA_WAITTING:
            if(timepassed_rtc(rtc, REC_TIMEOUT))
            {
                lora_params_state = last_state;
            }
            break;
    }
	return ACK_WAITING;
}

#define htonl(x)    ( (x<<24 &0xff000000)|(x<<8 & 0x00ff0000)|(x>>8 &0x0000ff00)|(x>>24 &0x000000ff))
#define TEM_HUM_PORT 5
static uint8_t send_data_char[250] = {0};
void change_send_format_hex(int16_t tem, int16_t hum)
{
	uint8_t temp[4];
	temp[0] = tem/1000;
	send_data_char[0] = temp[0] + '0';
	temp[1] = (tem - temp[0] *1000)/100;
	send_data_char[1] = temp[1] + '0';
	temp[2] = (tem - temp[0] *1000 - temp[1]*100)/10;
	send_data_char[2] = temp[2] + '0';
	temp[3] = (tem - temp[0] *1000 - temp[1]*100 - temp[2]*10);
	send_data_char[3] = temp[3] + '0';
	temp[0] = hum/1000;
	send_data_char[4] = temp[0] + '0';
	temp[1] = (hum - temp[0] *1000)/100;
	send_data_char[5] = temp[1] + '0';
	temp[2] = (hum - temp[0] *1000 - temp[1]*100)/10;
	send_data_char[6] = temp[2] + '0';
	temp[3] = (hum - temp[0] *1000 - temp[1]*100 - temp[2]*10);
	send_data_char[7] = temp[3] + '0';
	send_data_char[8] = 0;//组成4字节
    /*memset(&send_data_char[8], 1+'0', 14);//组成11字节
	send_data_char[22] = 0;//为了截取字符串，11字节*/
}
void lora_process(void)
{
    static uint8_t state = LORA_PARAMS_SET_STATE;
    static uint32_t rtc = 0;
    static ACK return_value = ACK_IDLE;
	static int16_t Test_temp;
	static int16_t Test_hum;
	if(get_product_start_flag())
	{
		return;
	}
	uart_process();
    switch(state)
    {
        case LORA_PARAMS_SET_STATE:
            return_value = lora_params_set();
            if(return_value == ACK_SUCCESS)
            {
                state = LORA_JOINNET_STATE;
            }
            break;
        case LORA_JOINNET_STATE:
            return_value = lora_exe_command(JOIN_NET_TYPE);
		    if(return_value == ACK_SUCCESS)
            {
                state = LORA_JOINNET_CHECK_STATE;
            }
			else if(return_value == ACK_FALSE)
			{
				state = LORA_JOINNET_STATE;
			}
            break;
        case LORA_JOINNET_CHECK_STATE:
            if(get_joinnet_status())//收到入网成功的回复
            {
                state = LORA_TEMHUM_DATA_STATE;
				clear_joinnet_status();
            }
			else if(get_re_joinnet_status())
			{
				state = LORA_PARAMS_SET_STATE;
				clear_re_joinnet_status();
				lora_params_state_init();
			}
            break;
		case LORA_TEMHUM_DATA_STATE:
			Read_tempHum_valu();
		    Test_temp = Get_sht30TemValu();
			Test_hum = Get_sht30HumValu();
			change_send_format_hex(Test_temp, Test_hum);
			state = LORA_DATA_SENDING_STATE;
			break;
        case LORA_DATA_SENDING_STATE:
            return_value = lora_send_unconfirm(HEX_TYPE, TEM_HUM_PORT, send_data_char, LORA_SEND_TIMEOUT, (uint8_t *)RESPECT_REC, 1);//发的是字符串，0x12 0x34，发送时是"1234"，发送四个字节应该是8长度
            if(return_value == ACK_SUCCESS)
            {
                state = LORA_DATA_SENDING_INTERVAL_STATE;
                rtc = get_rtc();
				LOG_LUO("========send success=========\r\n");
            }
            else if(return_value == ACK_FALSE)
            {
                state = LORA_WAITING_STATE;
                rtc = get_rtc();
            }
            break;
        case LORA_DATA_SENDING_INTERVAL_STATE:
            if(timepassed_rtc(rtc, DATA_SENDING_INTERVAL))
            {
                state = LORA_TEMHUM_DATA_STATE;
				LOG_LUO("get in the send process3\r\n");
            }
			break;
        case LORA_WAITING_STATE:
            if(timepassed_rtc(rtc, WAITING_INTERVAL))
            {
				LOG_LUO("get in the send process6\r\n");
                state = LORA_TEMHUM_DATA_STATE;
            }
            break;
    }
}


//------------串口回调-------------
void uart_process(void)
{
    if(UsartType1.receive_flag)
    {
		at_command_recv_deal(UsartType1.usartDMA_rxBuf, UsartType1.rx_len);
		
		UsartType1.usartDMA_rxBuf[UsartType1.rx_len] = 0;//DMA给出的数据清零,方便打印字符串
        LOG_LUO("rec buflen %d buf: %s\r\n",UsartType1.rx_len, UsartType1.usartDMA_rxBuf);
        UsartType1.receive_flag = 0;
    }
}




