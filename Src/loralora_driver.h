#ifndef __LORALORA_DRIVER__H
#define __LORALORA_DRIVER__H

#include "stdint.h"

#define REC_TIMEOUT             500

typedef enum
{	
	UART_PORT0 = 0,
	UART_PORT1 = 1,
	MAX_UART_NUM
}UART_PORT;

typedef enum
{
    ACK_FALSE = 0,
    ACK_SUCCESS = 1,
    ACK_WAITING = 2,
    ACK_IDLE = 3
}ACK;

//----------------1.系统的AT命令----------------
typedef enum
{	
    AT_ATE_INQUIRE,
    AT_VER_INQUIRE,
    AT_BAUD_INQUIRE,
    AT_SLEEP_INQUIRE,
    AT_GPIO_INQUIRE,
}AT_SYSTEM_COMMAND_INQUIRE;

typedef enum
{	
    AT_ATE_SET,
    AT_BAUD_SET,
    AT_SLEEP_SET,
    AT_GPIO_SET,
    AT_PRODUCT_IO_PIN_SET,
}AT_SYSTEM_COMMAND_SET;

typedef enum
{	
	AT_CHECK_EXE,
	AT_RESET_EXE,
	AT_FACTORY_EXE,
}AT_SYSTEM_COMMAND_EXE;
ACK at_system_command_exe(AT_SYSTEM_COMMAND_EXE command_type, uint16_t timeout, uint8_t *expect_rec, uint8_t retry_times);
ACK at_system_command_inquire(AT_SYSTEM_COMMAND_INQUIRE command_type, uint16_t timeout, uint8_t *expect_rec, uint8_t try_times);
//-------------------2.BLE的AT命令------------------------
typedef enum
{	
    BLE_MAC_INQUIRE,
    BLE_NAME_INQUIRE,
    BLE_DATA_INQUIRE,
    BLE_ADV_STATUS_INQUIRE,
    BLE_SCAN_STATUS_INQUIRE,
    BLE_SCAN_NAME_INQUIRE,
    BLE_SCAN_RSSI_INQUIRE,
    BLE_SCAN_MAC_INQUIRE,
}BLE_COMMAND_INQUIRE;

typedef enum
{	
    BLE_NAME_SET,
    BLE_DATA_SET,
    BLE_ADV_STATUS_SET,
    BLE_SCAN_STATUS_SET,
    BLE_SCAN_NAME_SET,
    BLE_SCAN_RSSI_SET,
    BLE_SCAN_MAC_SET,
    BLE_CONNECT_SET,
}BLE_COMMAND_SET;

ACK at_ble_command_inquire(BLE_COMMAND_INQUIRE command_type, uint16_t timeout, uint8_t *expect_rec, uint8_t try_times);
//-------------------3.LORA/LORAWAN的AT命令------------------------
//lora设置参数的类型
typedef enum
{	
    LORA_WORK_MODE,
    LORA_JOIN_MODE,
    LORA_REGION,
    LORA_CLASS,
    LORA_DEVEUI,
    LORA_APPEUI,
    LORA_APPKEY,
    LORA_APPSKEY,
    LORA_NWKSKEY,
    LORA_DEVADDR,
    LORA_CH,
    LORA_ADR_STATUS,
    LORA_ADR_PARAMS,
    LORA_TX_POWER,
    LORA_DR,
    LORA_RX2_CH,
    LORA_RX1_DL,
    LORA_JOIN_DL,
    LORA_DWELL_TIME,
    LORA_MC,
    LORA_UNC_REPEAT,//LORAWAN非确认帧
    LORA_LORA_TX_HEX,//LORA发送十六进制
    LORA_LORA_TX_STR,//LORA发送字符串

    //参数过多，需要换接口实现的
    LORA_DUTYCYCLE,
    LORA_SEND_CHEX,//LORAWAN确认帧
    LORA_SEND_CSTR,
    LORA_SEND_HEX,
    LORA_SEND_STR,
    LORA_LORA_CONFIG,//LORA模式配置

    //只能给查询，不能设置、执行的
    LORA_TX_LEN,//只有查询
    LORA_LORAWAN_STATUS,//入网状态，只有查询

    //执行的指令
    LORA_START_CERT,//LORA认证模式
    LORA_JOINING,//入网请求
    LORA_LINKCHECK,//执行linkcheck
    LORA_DEVTIME,//执行DEVTIME
}LORA_TYPE;

typedef enum
{	
    HEX_TYPE,
    STRING_TYPE,
}LORA_UNCONFIRM_TYPE;

typedef enum
{	
    JOIN_NET_TYPE,
    LINKCHECK_TYPE,
    DEVTIME_TYPE,
}LORA_EXE_TYPE;


uint8_t get_joinnet_status(void);
uint8_t get_re_joinnet_status(void);
void clear_joinnet_status(void);
void clear_re_joinnet_status(void);

ACK lora_mac_get(void);
uint8_t *get_deveui_params_set(void);
uint8_t *get_devaddr_params_set(void);

ACK set_lora_dutycycle_params(uint8_t *ch_params, uint8_t app_duty);
ACK set_lora_dwelltime_params(uint8_t uplink, uint8_t downlink);

ACK set_lora_params(LORA_TYPE type, uint8_t *params, uint16_t timeout, uint8_t *expect_rec, uint8_t try_times);
ACK lora_exe_command(LORA_EXE_TYPE type);
ACK lora_send_unconfirm(LORA_UNCONFIRM_TYPE type, uint16_t port, uint8_t *data, uint16_t timeout, uint8_t *expect_rec, uint8_t try_times);
//-------------------4.WIFI的AT命令------------------------

//-------------------5.GNSS的AT命令------------------------

void at_command_recv_deal(uint8_t *buf,uint16_t len);

#endif

