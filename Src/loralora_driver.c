#include "loralora_driver.h"
#include "string.h"
#include "sys_timer.h"
#include "stdio.h"
#include "hw.h"
/*
*   任务：写一个demo：：EU868 dr1入网，每隔10s发送一组lora数据即可
*/



/*
	    发送格式	                应答格式
查询	AT+<CMD>=?<CR><LF>	        +<CMD>: <PARAM><CR><LF><CR><LF>OK<CR><LF>
设置	AT+<CMD>=<PARAM><CR><LF>	<CR><LF>OK<CR><LF>或<CR><LF>ERROR()<CR><LF>
执行	AT+<CMD><CR><LF>	        <CR><LF>OK<CR><LF>或<CR><LF>ERROR()<CR><LF>
帮助	AT+<CMD>?<CR><LF>	        +<CMD>: <help string><CR><LF>
*/

#define AT_COMMAND_ERROR            "ERROR(-1)"
#define AT_COMMAND_PARAMS_ERROR     "ERROR(-2)"
#define MODULE_BUSY                 "ERROR(-3)"
#define MODULE_NOT_JOINNET          "ERROR(-4)"
#define MODULE_NO_CHANNEL           "ERROR(-5)"
#define LORA_SEND_LENTH_ERROR       "ERROR(-6)"
#define LORA_DUTYCYCLE_WAIT         "ERROR(-7)"

#define UART_REC_SIZE       500

static uint8_t joinnet_status = 0;
static uint8_t re_joinnet_status = 0;
uint8_t get_joinnet_status(void)
{
	return joinnet_status;
}
void clear_joinnet_status(void)
{
	joinnet_status = 0;
}
uint8_t get_re_joinnet_status(void)
{
	return re_joinnet_status;
}
void clear_re_joinnet_status(void)
{
	re_joinnet_status = 0;
}

//-------------------组包使用的字符串------------------
static uint8_t end_string[] = "\r\n";
static uint8_t comma = ',';

//-------------1.系统相关AT设置指令------------------
static uint8_t at_work_test[] = "AT\r\n";
static uint8_t at_reset[] = "AT+RESET\r\n";
static uint8_t at_factory[] = "AT+FACTORY\r\n";
static uint8_t at_ate_inquire[] = "AT+ATE=?\r\n";
static uint8_t at_ate_set[] = "AT+ATE=";//后面接ON或者OFF
static uint8_t at_ver_inquire[] = "AT+VER=?\r\n";
static uint8_t at_baud_inquire[] = "AT+BAUD=?\r\n";
static uint8_t at_baud_set[] = "AT+BAUD=";//9600
static uint8_t at_sleep_inquire[] = "AT+SLEEP=?\r\n";
static uint8_t at_sleep_set[] = "AT+SLEEP=";
//static uint8_t at_gpio_inquire[] = "AT+GPIO=?\r\n";//num为GPIO编号
//static uint8_t at_gpio_inquire_tail[] = "?\r\n";
static uint8_t at_gpio_set[] = "AT+GPIO";//num为GPIO编号
static uint8_t at_product_io_test[] = "AT+PRODUCT_TEST_PIN=";//value为高低电平设置

//-------------2.BLE相关AT设置指令------------------
static uint8_t at_ble_mac_inquire[] = "AT+LADDR=?\r\n";
static uint8_t at_ble_adv_name_inquire[] = "AT+NAME=?\r\n";
static uint8_t at_ble_adv_name_set[] = "AT+NAME=\r\n";//后面应该加设置的广播名称
static uint8_t at_ble_adv_data_inquire[] = "AT+ADVD=?\r\n";
static uint8_t at_ble_adv_data_set[] = "AT+ADVD=\r\n";//后面加上广播数据
static uint8_t at_ble_adv_status_inquire[] = "AT+SET_ADV=?\r\n";
static uint8_t at_ble_adv_status_set[] = "AT+SET_ADV=\r\n";//后面接设置的广播状态 ON OFF
static uint8_t at_ble_scan_status_inquire[] = "AT+SCAN_STD=?\r\n";
static uint8_t at_ble_scan_status_set[] = "AT+SCAN_STD=\r\n";//后面加上设置的扫描状态   ON OFF
static uint8_t at_ble_scan_name_inquire[] = "AT+SCAN_NAME=?\r\n";
static uint8_t at_ble_scan_name_set[] = "AT+SCAN_STD=\r\n";//后面加上设置的扫描过滤广播名      0-11个字符
static uint8_t at_ble_scan_rssi_inquire[] = "AT+SCAN_RSSI=?\r\n";
static uint8_t at_ble_scan_rssi_set[] = "AT+SCAN_RSSI=\r\n";//后面加上设置的扫描过滤RSSI   -127到0
static uint8_t at_ble_scan_mac_inquire[] = "AT+SCAN_MAC=?\r\n";
static uint8_t at_ble_scan_mac_set[] = "AT+SCAN_MAC=\r\n";//后面加上设置的扫描过滤MAC地址   0-6个十六进制MAC
static uint8_t at_ble_connect_set[] = "AT+BLE_CON=\r\n";//后面加上需要连接的蓝牙mac地址

//-------------3.LORA/LORAWAN相关AT设置指令------------------
//static uint8_t at_lora_workmode_inquire[] = "AT+WORK_MODE=?\r\n";
static uint8_t at_lora_workmode_set[] = "AT+WORK_MODE=";
//static uint8_t at_lora_joinmode_inquire[] = "AT+JOIN_MODE=?\r\n";
static uint8_t at_lora_joinmode_set[] = "AT+JOIN_MODE=";
//static uint8_t at_lora_region_inquire[] = "AT+REGION=?\r\n";
static uint8_t at_lora_region_set[] = "AT+REGION=";
//static uint8_t at_lora_class_inquire[] = "AT+CLASS=?\r\n";
static uint8_t at_lora_class_set[] = "AT+CLASS=";
//static uint8_t at_lora_deveui_inquire[] = "AT+DEVEUI=?\r\n";
static uint8_t at_lora_deveui_set[] = "AT+DEVEUI=";
//static uint8_t at_lora_appeui_inquire[] = "AT+APPEUI=?\r\n";
static uint8_t at_lora_appeui_set[] = "AT+APPEUI=";
//static uint8_t at_lora_appkey_inquire[] = "AT+APPKEY=?\r\n";
static uint8_t at_lora_appkey_set[] = "AT+APPKEY=";
//static uint8_t at_lora_appskey_inquire[] = "AT+APPSKEY=?\r\n";
static uint8_t at_lora_appskey_set[] = "AT+APPSKEY=";
//static uint8_t at_lora_nwkskey_inquire[] = "AT+NWKSKEY=?\r\n";
static uint8_t at_lora_nwkskey_set[] = "AT+NWKSKEY=";
//static uint8_t at_lora_devaddr_inquire[] = "AT+DEVADDR=?\r\n";
static uint8_t at_lora_devaddr_set[] = "AT+DEVADDR=";
//static uint8_t at_lora_ch_inquire[] = "AT+CH=?\r\n";
static uint8_t at_lora_ch_set[] = "AT+CH=";
//static uint8_t at_lora_adr_inquire[] = "AT+ADR_STD=?\r\n";
static uint8_t at_lora_adr_set[] = "AT+ADR_STD=";
//static uint8_t at_lora_adr_params_inquire[] = "AT+ADR_PARAM=?\r\n";
static uint8_t at_lora_adr_params_set[] = "AT+ADR_PARAM=";
//static uint8_t at_lora_txpower_inquire[] = "AT+TX_POWER=?\r\n";
static uint8_t at_lora_txpower_set[] = "AT+TX_POWER=";
//static uint8_t at_lora_dr_inquire[] = "AT+DR=?\r\n";
static uint8_t at_lora_dr_set[] = "AT+DR=";
//static uint8_t at_lora_dutycycle_inquire[] = "AT+DUTY_CYCLE=?\r\n";
static uint8_t at_lora_dutycycle_set[] = "AT+DUTY_CYCLE=";
//static uint8_t at_lora_rx2ch_inquire[] = "AT+RX2_CHANNEL=?\r\n";
static uint8_t at_lora_rx2ch_set[] = "AT+RX2_CHANNEL=";
//static uint8_t at_lora_rx1_delay_inquire[] = "AT+RX1DL=?\r\n";
static uint8_t at_lora_rx1_delay_set[] = "AT+RX1DL=";
//static uint8_t at_lora_join_delay_inquire[] = "AT+JOIN_DELAY=?\r\n";
static uint8_t at_lora_join_delay_set[] = "AT+JOIN_DELAY=";
//static uint8_t at_lora_dwelltime_inquire[] = "AT+DWELLTIME=?\r\n";
static uint8_t at_lora_dwelltime_set[] = "AT+DWELLTIME=";
//static uint8_t at_lora_txlen_inquire[] = "AT+DWELLTIME=?\r\n";
//static uint8_t at_lora_mc_inquire[] = "AT+MC=?\r\n";
static uint8_t at_lora_mc_set[] = "AT+MC=";


//static uint8_t at_lora_net_status_inquire[] = "AT+LORAWAN_STATUS=?\r\n";
static uint8_t at_lora_joinnet_exe[] = "AT+JOINING\r\n";
static uint8_t at_lora_linkcheck_exe[] = "AT+LINKCHECK\r\n";
static uint8_t at_lora_devtime_exe[] = "AT+DEVTIME\r\n";

//static uint8_t at_lora_send_chex_set[] = "AT+SEND_CHEX=\r\n";//确认帧十六进制数据
//static uint8_t at_lora_send_cstr_set[] = "AT+SEND_CSTR=\r\n";//确认帧字符串数据

//static uint8_t at_lora_send_unc_repeat_inquire[] = "AT+UNC_REPEAT=?\r\n";//非确认帧重发次数
static uint8_t at_lora_send_unc_repeat_set[] = "AT+UNC_REPEAT=";
static uint8_t at_lora_send_hex_set[] = "AT+SEND_HEX=";//非确认帧十六进制数据
static uint8_t at_lora_send_str_set[] = "AT+SEND_STR=";//非确认帧字符串数据

//lora radio
//static uint8_t at_lora_cfg_inquire[] = "AT+LORA_CONFIG=?\r\n";
//static uint8_t at_lora_cfg_set[] = "AT+LORA_CONFIG=\r\n";
static uint8_t at_lora_lora_txhex_set[] = "AT+LORA_TX_HEX=";
static uint8_t at_lora_lora_txstr_set[] = "AT+LORA_TX_STR=";

//static uint8_t at_lora_cert_exe[] = "AT+START_CERT\r\n";//执行认证模式


//-------------4.WIFI相关AT设置指令------------------
//static uint8_t at_wifi_cfg_inquire[] = "AT+WIFI_CFG=?\r\n";
//static uint8_t at_wifi_cfg_set[] = "AT+WIFI_CFG=<channel_mask>,<type>,<max_result>,<timeout_per_channel>,<timeout_per_scan>\r\n";//设置WIFI参数
//static uint8_t at_wifi_scan_exe[] = "AT+WIFI_SCAN\r\n";//执行WIFI扫描

//-------------5.GNSS相关AT设置指令------------------
//static uint8_t at_gnss_cfg_inquire[] = "AT+GNSS_CFG=?\r\n";
//static uint8_t at_gnss_cfg_set[] = "AT+GNSS_CFG=<scan_mode>,<constellation>,<max_nb_sat>\r\n";//设置GPS参数
//static uint8_t at_gnss_assist_cfg_inquire[] = "AT+GNSS_ASSIST_CFG=?\r\n";
//static uint8_t at_gnss_assist_cfg_set[] = "AT+GNSS_ASSIST_CFG=<lat>,<lon>\r\n";
//static uint8_t at_gnss_scan_exe[] = "AT+GNSS_SCAN\r\n";
//static uint8_t at_gnss_almanac_update_exe[] = "AT+ALMANAC_UPDATE\r\n";

static uint8_t uart_rec_buff[UART_REC_SIZE];//串口接收500字节
//static uint16_t uart_rec_len = 0;

static uint8_t at_command_buff[300];
static uint16_t command_buff_pointer = 0;//指示组包从哪里开始

void clear_uart_buff(void)
{
    memset(uart_rec_buff, 0, UART_REC_SIZE);
//	uart_rec_len = 0;
}
void clear_at_command_buff(void)
{
    command_buff_pointer = 0;
    memset(at_command_buff, 0 ,sizeof(at_command_buff));
}

void clear_all_buff(void)
{
	clear_at_command_buff();//组包发送
    clear_uart_buff();//串口接收处理
}


void combine_command_buff(uint8_t *buff, uint16_t buff_len)
{
	memcpy(at_command_buff+command_buff_pointer, buff, buff_len);
	command_buff_pointer += buff_len;
}
void lora_uart_send(uint8_t *buf,uint16_t buflen)
{
	Usart1SendData_DMA(buf, buflen);
	LOG_LUO("send uart data len %d, time %d, data %s\r\n",buflen, get_rtc(), buf);
}

//组包所需要的的字符转换函数
static uint8_t string[10];
void hex_to_string(uint32_t data_in)
{
    sprintf((char *)string, "%d", data_in);
}

void clear_string(void)
{
    memset(string,0,sizeof(string));
}

//-----------------1.系统相关AT命令----------------------
void choose_system_command_inquire(AT_SYSTEM_COMMAND_INQUIRE command_type)
{
    if(command_type == AT_ATE_INQUIRE)
    {
        combine_command_buff(at_ate_inquire, sizeof(at_ate_inquire)-1);
    }
    else if(command_type == AT_VER_INQUIRE)
    {
        combine_command_buff(at_ver_inquire, sizeof(at_ver_inquire)-1);
    }
    else if(command_type == AT_BAUD_INQUIRE)
    {
        combine_command_buff(at_baud_inquire, sizeof(at_baud_inquire)-1);
    }
    else if(command_type == AT_SLEEP_INQUIRE)
    {
        combine_command_buff(at_sleep_inquire, sizeof(at_sleep_inquire)-1);
    }
    /*else if(command_type == AT_GPIO_INQUIRE)//这个还需要一个GPIO num参数，函数接口不通用
    {

    }*/
}

void choose_system_command_set(AT_SYSTEM_COMMAND_SET command_type)
{
    if(command_type == AT_ATE_SET)
    {
        combine_command_buff(at_ate_set, sizeof(at_ate_set)-1);
    }
    else if(command_type == AT_BAUD_SET)
    {
        combine_command_buff(at_baud_set, sizeof(at_baud_set)-1);
    }
    else if(command_type == AT_SLEEP_SET)
    {
        combine_command_buff(at_sleep_set, sizeof(at_sleep_set)-1);
    }
    else if(command_type == AT_GPIO_SET)
    {
        combine_command_buff(at_gpio_set, sizeof(at_gpio_set)-1);
    }
    else if(command_type == AT_PRODUCT_IO_PIN_SET)
    {
        combine_command_buff(at_product_io_test, sizeof(at_product_io_test)-1);
    }
}

void choose_system_command_exe(AT_SYSTEM_COMMAND_EXE command_type)
{
    if(command_type == AT_CHECK_EXE)
    {
        combine_command_buff(at_work_test, sizeof(at_work_test)-1);
    }
    else if(command_type == AT_RESET_EXE)
    {
        combine_command_buff(at_reset, sizeof(at_reset)-1);
    }
    else if(command_type == AT_FACTORY_EXE)
    {
        combine_command_buff(at_factory, sizeof(at_factory)-1);
    }
}

//解析查询到的数据
void at_system_command_inquire_parse(AT_SYSTEM_COMMAND_INQUIRE command_type)
{
    if(command_type == AT_ATE_INQUIRE)
    {
        
    }
    else if(command_type == AT_VER_INQUIRE)
    {
        
    }
    else if(command_type == AT_BAUD_INQUIRE)
    {
        
    }
    else if(command_type == AT_SLEEP_INQUIRE)
    {
        
    }
    else if(command_type == AT_GPIO_INQUIRE)
    {

    }
}



/*
*   @brief  执行类的exe系统执行命名,参数依次为: 发送的参数类型，超时时间，期待的回复字符串首地址，重传的次数
*/
ACK at_system_command_exe(AT_SYSTEM_COMMAND_EXE command_type, uint16_t timeout, uint8_t *expect_rec, uint8_t try_times)
{
    static uint8_t state = 0;
    static uint32_t timer = 0;
	static uint8_t count = 0;
    switch(state)
    {
        case 0:
			choose_system_command_exe(command_type);
			lora_uart_send(at_command_buff, strlen((char *)at_command_buff));
			timer = get_rtc();
			state = 1;
            break;
        case 1:
            if(strlen((char*)uart_rec_buff) != 0)
            {
                if(strstr((char*)uart_rec_buff, (char *)expect_rec))
                {
                    state = 0;
					count = 0;
                    clear_all_buff();
                    return ACK_SUCCESS;
                }
            }
            if(timepassed_rtc(timer, timeout))
            {
				count++;
				state = 0;
				clear_all_buff();
				if(count == try_times)
				{
					count = 0;
					return ACK_FALSE;
				}
            }
            break;
    }
    return ACK_WAITING;
}

/*
*   @brief  系统查询统一接口
*/
ACK at_system_command_inquire(AT_SYSTEM_COMMAND_INQUIRE command_type, uint16_t timeout, uint8_t *expect_rec, uint8_t try_times)
{
    static uint8_t state = 0;
    static uint32_t timer = 0;
	static uint8_t count = 0;
    switch(state)
    {
        case 0:
			choose_system_command_inquire(command_type);//指令只需要发这个
			lora_uart_send(at_command_buff, strlen((char *)at_command_buff));
			timer = get_rtc();
			state = 1;
            break;
        case 1:
            if(strlen((char*)uart_rec_buff) != 0)
            {
                if(strstr((char*)uart_rec_buff, (char *)expect_rec))//回复OK最后多了一步系统查询解析
                {
                    at_system_command_inquire_parse(command_type);
                    state = 0;
					count = 0;
                    clear_all_buff();
                    return ACK_SUCCESS;
                }
            }
            if(timepassed_rtc(timer, timeout))
            {
				count++;
				state = 0;
				clear_all_buff();
				if(count == try_times)
				{
					count = 0;
					return ACK_FALSE;
				}
            }
            break;
    }
    return ACK_WAITING;
}

//设置类的需要一个个写接口，参数有变化


//-----------------------2.蓝牙功能AT指令---------------------------------
void choose_ble_command_inquire(BLE_COMMAND_INQUIRE command_type)
{
    if(command_type == BLE_MAC_INQUIRE)
    {
        combine_command_buff(at_ble_mac_inquire, sizeof(at_ble_mac_inquire)-1);
    }
    else if(command_type == BLE_NAME_INQUIRE)
    {
        combine_command_buff(at_ble_adv_name_inquire, sizeof(at_ble_adv_name_inquire)-1);
    }
    else if(command_type == BLE_DATA_INQUIRE)
    {
        combine_command_buff(at_ble_adv_data_inquire, sizeof(at_ble_adv_data_inquire)-1);
    }
    else if(command_type == BLE_ADV_STATUS_INQUIRE)
    {
        combine_command_buff(at_ble_adv_status_inquire, sizeof(at_ble_adv_status_inquire)-1);
    }
    else if(command_type == BLE_SCAN_STATUS_INQUIRE)
    {
        combine_command_buff(at_ble_scan_status_inquire, sizeof(at_ble_scan_status_inquire)-1);
    }
    else if(command_type == BLE_SCAN_NAME_INQUIRE)
    {
        combine_command_buff(at_ble_scan_name_inquire, sizeof(at_ble_scan_name_inquire)-1);
    }
    else if(command_type == BLE_SCAN_RSSI_INQUIRE)
    {
        combine_command_buff(at_ble_scan_rssi_inquire, sizeof(at_ble_scan_rssi_inquire)-1);
    }
    else if(command_type == BLE_SCAN_MAC_INQUIRE)
    {
        combine_command_buff(at_ble_scan_mac_inquire, sizeof(at_ble_scan_mac_inquire)-1);
    }
}

void choose_ble_command_set(BLE_COMMAND_SET command_type)
{
    if(command_type == BLE_NAME_SET)
    {
        combine_command_buff(at_ble_adv_name_set, sizeof(at_ble_adv_name_set)-1);
    }
    else if(command_type == BLE_DATA_SET)
    {
        combine_command_buff(at_ble_adv_data_set, sizeof(at_ble_adv_data_set)-1);
    }
    else if(command_type == BLE_ADV_STATUS_SET)
    {
        combine_command_buff(at_ble_adv_status_set, sizeof(at_ble_adv_status_set)-1);
    }
    else if(command_type == BLE_SCAN_STATUS_SET)
    {
        combine_command_buff(at_ble_scan_status_set, sizeof(at_ble_scan_status_set)-1);
    }
    else if(command_type == BLE_SCAN_NAME_SET)
    {
        combine_command_buff(at_ble_scan_name_set, sizeof(at_ble_scan_name_set)-1);
    }
    else if(command_type == BLE_SCAN_RSSI_SET)
    {
        combine_command_buff(at_ble_scan_rssi_set, sizeof(at_ble_scan_rssi_set)-1);
    }
    else if(command_type == BLE_SCAN_MAC_SET)
    {
        combine_command_buff(at_ble_scan_mac_set, sizeof(at_ble_scan_mac_set)-1);
    }
    else if(command_type == BLE_CONNECT_SET)
    {
        combine_command_buff(at_ble_connect_set, sizeof(at_ble_connect_set)-1);
    }
}


//解析查询到的数据
void at_ble_command_inquire_parse(BLE_COMMAND_INQUIRE command_type)
{
    if(command_type == BLE_MAC_INQUIRE)
    {

    }
    else if(command_type == BLE_NAME_INQUIRE)
    {

    }
    else if(command_type == BLE_DATA_INQUIRE)
    {

    }
    else if(command_type == BLE_ADV_STATUS_INQUIRE)
    {

    }
    else if(command_type == BLE_SCAN_STATUS_INQUIRE)
    {

    }
    else if(command_type == BLE_SCAN_NAME_INQUIRE)
    {

    }
    else if(command_type == BLE_SCAN_RSSI_INQUIRE)
    {

    }
    else if(command_type == BLE_SCAN_MAC_INQUIRE)
    {

    }
}

/*
*   @brief  蓝牙相关查询统一接口
*/
ACK at_ble_command_inquire(BLE_COMMAND_INQUIRE command_type, uint16_t timeout, uint8_t *expect_rec, uint8_t try_times)
{
    static uint8_t state = 0;
    static uint32_t timer = 0;
	static uint8_t count = 0;
    switch(state)
    {
        case 0:
			choose_ble_command_inquire(command_type);//指令只需要发这个
			lora_uart_send(at_command_buff, strlen((char *)at_command_buff));
			timer = get_rtc();
			state = 1;
            break;
        case 1:
            if(strlen((char*)uart_rec_buff) != 0)
            {
                if(strstr((char*)uart_rec_buff, (char *)expect_rec))//回复OK最后多了一步ble查询解析
                {
                    at_ble_command_inquire_parse(command_type);
                    state = 0;
					count = 0;
                    clear_all_buff();
                    return ACK_SUCCESS;
                }
            }
            if(timepassed_rtc(timer, timeout))
            {
				count++;
				state = 0;
				clear_all_buff();
				if(count == try_times)
				{
					count = 0;
					return ACK_FALSE;
				}
            }
            break;
    }
    return ACK_WAITING;
}

//-------------------3.LORA/LORAWAN功能AT指令---------------------------------
static uint8_t mac[17] ={0};
static uint8_t deveui_params_set[23] = {0};//23
static uint8_t devaddr_params_set[11] = {0};//11
uint8_t *get_deveui_params_set(void)
{
    return deveui_params_set;
}
uint8_t *get_devaddr_params_set(void)
{
    return devaddr_params_set;
}

ACK lora_mac_get(void)
{
    static uint8_t state = 0;
    static uint32_t timer = 0;
    switch(state)
    {
        case 0:
			combine_command_buff(at_ble_mac_inquire, sizeof(at_ble_mac_inquire)-1);//结尾符
			lora_uart_send(at_command_buff, strlen((char *)at_command_buff));
			timer = get_rtc();
			state = 1;
            break;
        case 1:
            if(strlen((char*)uart_rec_buff) != 0)
            {
                if(strstr((char*)uart_rec_buff, "OK"))//设置成功
                {
                    char *mac_init = NULL;
                    mac_init = strstr((char*)uart_rec_buff, " ")+1;
					memcpy(mac, mac_init, 17);//全存
					memcpy(deveui_params_set, mac, 9);
					sprintf((char *)(deveui_params_set+9),"FF:FF:");
					memcpy(deveui_params_set+15, mac+9, 8);
					
					memcpy(devaddr_params_set, mac+6, 11);
					LOG_LUO("deveui is:%s\r\n",deveui_params_set);
					LOG_LUO("devaddr is:%s\r\n",devaddr_params_set);
                    state = 0;
                    clear_all_buff();
                    return ACK_SUCCESS;
                }
            }
            if(timepassed_rtc(timer, REC_TIMEOUT))
            {
                state = 0;
                clear_all_buff();
                return ACK_FALSE;
            }
            break;
    }
    return ACK_WAITING;
}


//task:使用EU868和DR1完成入网并每30s发送一组温湿度数据
void choose_lora_set_at_string(LORA_TYPE type)
{
    if(type == LORA_WORK_MODE)
    {
        combine_command_buff(at_lora_workmode_set, sizeof(at_lora_workmode_set)-1);
    }
    else if(type == LORA_JOIN_MODE)
    {
        combine_command_buff(at_lora_joinmode_set, sizeof(at_lora_joinmode_set)-1);
    }
    else if(type == LORA_REGION)
    {
        combine_command_buff(at_lora_region_set, sizeof(at_lora_region_set)-1);
    }
    else if(type == LORA_CLASS)
    {
        combine_command_buff(at_lora_class_set, sizeof(at_lora_class_set)-1);
    }
    else if(type == LORA_DEVEUI)
    {
        combine_command_buff(at_lora_deveui_set, sizeof(at_lora_deveui_set)-1);
    }
    else if(type == LORA_APPEUI)
    {
        combine_command_buff(at_lora_appeui_set, sizeof(at_lora_appeui_set)-1);
    }
    else if(type == LORA_APPKEY)
    {
        combine_command_buff(at_lora_appkey_set, sizeof(at_lora_appkey_set)-1);
    }
    else if(type == LORA_APPSKEY)
    {
        combine_command_buff(at_lora_appskey_set, sizeof(at_lora_appskey_set)-1);
    }
    else if(type == LORA_NWKSKEY)
    {
        combine_command_buff(at_lora_nwkskey_set, sizeof(at_lora_nwkskey_set)-1);
    }
    else if(type == LORA_DEVADDR)
    {
        combine_command_buff(at_lora_devaddr_set, sizeof(at_lora_devaddr_set)-1);
    }
    else if(type == LORA_CH)
    {
        combine_command_buff(at_lora_ch_set, sizeof(at_lora_ch_set)-1);
    }
    else if(type == LORA_ADR_STATUS)
    {
        combine_command_buff(at_lora_adr_set, sizeof(at_lora_adr_set)-1);
    }
    else if(type == LORA_ADR_PARAMS)
    {
        combine_command_buff(at_lora_adr_params_set, sizeof(at_lora_adr_params_set)-1);
    }
    else if(type == LORA_TX_POWER)
    {
        combine_command_buff(at_lora_txpower_set, sizeof(at_lora_txpower_set)-1);
    }
    else if(type == LORA_DR)
    {
        combine_command_buff(at_lora_dr_set, sizeof(at_lora_dr_set)-1);
    }
    else if(type == LORA_DUTYCYCLE)
    {
        combine_command_buff(at_lora_dutycycle_set, sizeof(at_lora_dutycycle_set)-1);
    }
    else if(type == LORA_RX2_CH)
    {
        combine_command_buff(at_lora_rx2ch_set, sizeof(at_lora_rx2ch_set)-1);
    }
    else if(type == LORA_RX1_DL)
    {
        combine_command_buff(at_lora_rx1_delay_set, sizeof(at_lora_rx1_delay_set)-1);
    }
    else if(type == LORA_JOIN_DL)
    {
        combine_command_buff(at_lora_join_delay_set, sizeof(at_lora_join_delay_set)-1);
    }
    else if(type == LORA_DWELL_TIME)
    {
        combine_command_buff(at_lora_dwelltime_set, sizeof(at_lora_dwelltime_set)-1);
    }
    else if(type == LORA_MC)
    {
        combine_command_buff(at_lora_mc_set, sizeof(at_lora_mc_set)-1);
    }
    else if(type == LORA_UNC_REPEAT)
    {
        combine_command_buff(at_lora_send_unc_repeat_set, sizeof(at_lora_send_unc_repeat_set)-1);
    }
    else if(type == LORA_LORA_TX_HEX)
    {
        combine_command_buff(at_lora_lora_txhex_set, sizeof(at_lora_lora_txhex_set)-1);
    }
    else if(type == LORA_LORA_TX_STR)
    {
        combine_command_buff(at_lora_lora_txstr_set, sizeof(at_lora_lora_txstr_set)-1);
    }
}

//所有LORA的配置项都可以在这里配置，输入为1.配置类型    2.配置参数的首地址  3.配置参数的长度
ACK set_lora_params(LORA_TYPE type, uint8_t *params, uint16_t timeout, uint8_t *expect_rec, uint8_t try_times)
{
    static uint8_t state = 0;
    static uint32_t timer = 0;
	static uint16_t params_len = 0;
	static uint8_t count = 0;
    switch(state)
    {
        case 0:
			choose_lora_set_at_string(type);//AT指令头
			params_len = strlen((char *)params);
			combine_command_buff(params, params_len);//设置的参数
			combine_command_buff(end_string, sizeof(end_string)-1);//结尾符
			lora_uart_send(at_command_buff, strlen((char *)at_command_buff));
			timer = get_rtc();
			state = 1;
            break;
        case 1:
            if(strlen((char*)uart_rec_buff) != 0)
            {
                if(strstr((char*)uart_rec_buff, (char *)expect_rec))//设置成功
                {
                    state = 0;
					count = 0;
                    clear_all_buff();
                    return ACK_SUCCESS;
                }
            }
            if(timepassed_rtc(timer, timeout))
            {
				count++;
				state = 0;
				clear_all_buff();
				if(count == try_times)
				{
					count = 0;
					return ACK_FALSE;
				}
            }
            break;
    }
    return ACK_WAITING;
}

ACK set_lora_dutycycle_params(uint8_t *ch_params, uint8_t app_duty)
{
    static uint8_t state = 0;
    static uint32_t timer = 0;
	static uint16_t params_len = 0;;
    switch(state)
    {
        case 0:
			combine_command_buff(at_lora_dutycycle_set, sizeof(at_lora_dutycycle_set)-1);
			params_len = strlen((char *)ch_params);
			combine_command_buff(ch_params, params_len);//设置的参数
			combine_command_buff(&comma, 1);
			hex_to_string(app_duty);
			combine_command_buff(string, strlen((char *)string));
			clear_string();
			combine_command_buff(end_string, sizeof(end_string)-1);//结尾符
			lora_uart_send(at_command_buff, strlen((char *)at_command_buff));
			timer = get_rtc();
			state = 1;
            break;
        case 1:
            if(strlen((char*)uart_rec_buff) != 0)
            {
                if(strstr((char*)uart_rec_buff, "OK"))//设置成功
                {
                    state = 0;
                    clear_all_buff();
                    return ACK_SUCCESS;
                }
            }
            if(timepassed_rtc(timer, REC_TIMEOUT))
            {
                state = 0;
                clear_all_buff();
                return ACK_FALSE;
            }
            break;
    }
    return ACK_WAITING;
}

ACK set_lora_dwelltime_params(uint8_t uplink, uint8_t downlink)
{
    static uint8_t state = 0;
    static uint32_t timer = 0;
    switch(state)
    {
        case 0:
			combine_command_buff(at_lora_dwelltime_set, sizeof(at_lora_dwelltime_set)-1);
			hex_to_string(uplink);
			combine_command_buff(string, strlen((char *)string));
			clear_string();
			combine_command_buff(&comma, 1);
			hex_to_string(downlink);
			combine_command_buff(string, strlen((char *)string));
			clear_string();
			combine_command_buff(end_string, sizeof(end_string)-1);//结尾符
			lora_uart_send(at_command_buff, strlen((char *)at_command_buff));
			timer = get_rtc();
			state = 1;
            break;
        case 1:
            if(strlen((char*)uart_rec_buff) != 0)
            {
                if(strstr((char*)uart_rec_buff, "OK"))//设置成功
                {
                    state = 0;
                    clear_all_buff();
                    return ACK_SUCCESS;
                }
            }
            if(timepassed_rtc(timer, REC_TIMEOUT))
            {
                state = 0;
                clear_all_buff();
                return ACK_FALSE;
            }
            break;
    }
    return ACK_WAITING;
}

void choose_lora_exe_command(LORA_EXE_TYPE type)
{
    if(type == JOIN_NET_TYPE)
    {
        combine_command_buff(at_lora_joinnet_exe, sizeof(at_lora_joinnet_exe)-1);
    }
    else if(type == LINKCHECK_TYPE)
    {
        combine_command_buff(at_lora_linkcheck_exe, sizeof(at_lora_linkcheck_exe)-1);
    }
    else if(type == DEVTIME_TYPE)
    {
        combine_command_buff(at_lora_devtime_exe, sizeof(at_lora_devtime_exe)-1);
    }
}

//1.join net  2.linkchec  3.devtime
ACK lora_exe_command(LORA_EXE_TYPE type)
{
	static uint8_t state = 0;
    static uint32_t timer = 0;
	switch(state)
    {
        case 0:
			choose_lora_exe_command(type);
			lora_uart_send(at_command_buff, strlen((char *)at_command_buff));
			timer = get_rtc();
			state = 1;
            break;
        case 1:
            if(strlen((char*)uart_rec_buff) != 0)
            {
                if(strstr((char*)uart_rec_buff, "OK"))//设置成功
                {
                    state = 0;
                    clear_all_buff();
                    return ACK_SUCCESS;
                }
            }
            if(timepassed_rtc(timer, REC_TIMEOUT))
            {
                state = 0;
                clear_all_buff();
                return ACK_FALSE;
            }
            break;
    }
    return ACK_WAITING;

	//clear_all_buff();
}


ACK lora_send_unconfirm(LORA_UNCONFIRM_TYPE type, uint16_t port, uint8_t *data, uint16_t timeout, uint8_t *expect_rec, uint8_t try_times)
{
    static uint8_t state = 0;
    static uint32_t timer = 0;
	static uint8_t count = 0;
	static uint16_t datelen = 0;
    switch(state)
    {
        case 0:
			if(type == HEX_TYPE)
			{
				combine_command_buff(at_lora_send_hex_set, sizeof(at_lora_send_hex_set)-1);
			}
			else if(type == STRING_TYPE)
			{
				combine_command_buff(at_lora_send_str_set, sizeof(at_lora_send_str_set)-1);
			}
			hex_to_string(port);
			combine_command_buff(string, strlen((char *)string));
			clear_string();
			combine_command_buff(&comma, 1);
			datelen = strlen((char *)data);
			combine_command_buff(data, datelen);
			combine_command_buff(end_string, sizeof(end_string)-1);//结尾符
			lora_uart_send(at_command_buff, strlen((char *)at_command_buff));
			timer = get_rtc();
			state = 1;
            break;
        case 1:
            if(strlen((char*)uart_rec_buff) != 0)
            {
                if(strstr((char*)uart_rec_buff, (char *)expect_rec))//发送成功
                {
                    state = 0;
					count = 0;
                    clear_all_buff();
                    return ACK_SUCCESS;
                }
            }
            if(timepassed_rtc(timer, timeout))
            {
				count++;
				state = 0;
				clear_all_buff();
				if(count == try_times)
				{
					count = 0;
					return ACK_FALSE;
				}
            }
            break;
    }
    return ACK_WAITING;   
}



//-------------------4.WIFI功能AT指令---------------------------------

//-------------------5.GNSS功能AT指令---------------------------------


//-------------------6.串口接收处理--------------------
void check_lora_join_state(uint8_t *buf,uint16_t len)
{
	if(strstr((char*)buf, "SUCCESSED"))
	{
		joinnet_status = 1;
		LOG_LUO("JOIN THE NET SUCCESS\r\n");
	}
	else if(strstr((char*)buf, "FAILED"))
	{
		re_joinnet_status = 1;
		LOG_LUO("JOIN THE NET FAILED\r\n");
	}
}
void at_command_recv_deal(uint8_t *buf,uint16_t len)
{
	buf[len] = 0;
	//--------------------------执行入网的回复---------------------------------
    if(strstr((char*)buf, "+JOINING_RESULT:"))
    {
		check_lora_join_state(buf, len);
    }

//--------------------------一般的指令回复---------------------------
    memcpy(uart_rec_buff, buf, len);
//	uart_rec_len = len;

}


