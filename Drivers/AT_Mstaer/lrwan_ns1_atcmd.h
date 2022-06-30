/**
  ******************************************************************************
  * @file    lrwan_ns1_atcmd.h
  * @author  MCD Application Team
  * @brief   Header for driver lrwan_ns1_atcmd.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LRWAN_NS1_ATCMD__
#define __LRWAN_NS1_ATCMD__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32l1xx_hal.h"
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/
typedef enum ATGroup
{
  AT_CTRL = 0,
  AT_SET,
  AT_GET,
} ATGroup_t;

typedef enum Marker_s
{
  CTRL_MARKER = 0,
  SET_MARKER,
  GET_MARKER,
} Marker_t;


/****************************************************************************/
/*here we have to include a list of AT cmd by the way of #include<file>     */
/*this file will be preprocessed for enum ATCmd, enum eATerror and AT marker*/
/*define                                                                    */
/****************************************************************************/


#define  AT_ERROR_INDEX
#define  AT_CMD_STRING
#define  AT_CMD_INDEX
#define  AT_CMD_MARKER
#include "atcmd_modem.h"
#include "SEGGER_RTT.h"

/* Private define ------------------------------------------------------------*/
#define DATA_RX_MAX_BUFF_SIZE    300
/*Max size of the received buffer*/
/*to optimize we can match with device key sizeof*/

#define DATA_TX_MAX_BUFF_SIZE    300
/*Max size of the transmit buffer*/
/*it is the worst-case when sending*/
/*a max payload equal to 64 bytes*/

#define ATCTL_CMD_BUF_SIZE              (512)
#define ATCTL_DL_BUF_SIZE               (30)
#define ATCTL_RX_TIMEOUT                (30)       /* 30x10ms */
#define ATCTL_JOIN_TIMEOUT              (6000)       /*800x10ms */
#define ATCTL_CMD_MAX_SIZE              (10)
#define ATCTL_TX_DELAY                  (500)

   

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  ATCTL_MSG_START,
  ATCTL_MSG_TX,
  ATCTL_MSG_WAIT_ACK,
  ATCTL_MSG_RX,
  ATCTL_MSG_DONE,
  ATCTL_MSG_BUSY,
  ATCTL_MSG_LEN,
  ATCTL_MSG_DR,
  ATCTL_MSG_NO_CH,
  ATCTL_MSG_NO_BAND,
  ATCTL_MSG_NO_NET,                   /* Please join network first */
  ATCTL_MSG_JOINED,
  ATCTL_MSG_JOIN_ALREADY,
  ATCTL_MSG_JOIN_FORCE,
  ATCTL_MSG_JOIN_NORMAL,
  ATCTL_MSG_JOIN_FAILED,
  ATCTL_MSG_JOIN_NETID,
  ATCTL_MSG_JOIN_NOT_OTAA,
  ATCTL_MSG_ACK_RECEIVED,
  ATCTL_MSG_RXWIN,
  ATCTL_MSG_MACCMD,
} atctl_msg_sta_t;

typedef union
{
  int err;
  int ver[3];
  struct
  {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
  } rtc;
  float vdd;
  struct
  {
    bool ack;
    uint8_t port;
    uint8_t *buf;
    uint16_t size;
  } msg;
  struct
  {
    atctl_msg_sta_t sta;
    uint32_t netid;
    uint8_t devaddr[4];
  } join;
  struct
  {
    uint8_t addr;
    uint8_t val;
  } eeprom;
  struct
  {
	uint8_t join_std;
    uint8_t region[12]; 
    uint8_t devaddr[4];
    uint8_t deveui[8];
    uint8_t appeui[8];
	uint8_t appkey[16];
	uint8_t nwkskey[16];
    uint8_t appskey[16];  
  } id;
  struct
  {
    uint8_t dr_value;
    uint8_t dr_type;
  } dr;
  int mode;
  struct
  {
    uint8_t chn_h;
    uint8_t chn_l;
	uint8_t chn_max;

  } channel;
  struct
  {
	uint16_t uart_baud;
	uint8_t txlen;
	uint16_t ul_cnt;
    uint16_t dl_cnt;
	uint8_t txp;
	uint8_t dr;
	uint8_t duty;
	uint32_t rx2fq;
	uint8_t rx2dr;
    uint16_t rx1dl;
    uint16_t rx2dl;
    uint16_t jrx1dl;
    uint16_t jrx2dl;
	int8_t snr;
	int16_t rssi;
  } dev_param;
    struct
  {
    uint8_t ate;
    uint8_t cfm;
    uint8_t adr;
    uint8_t test;
	uint8_t atuo_join;
	uint8_t sleep;
  } std;
    struct
  {
	uint8_t std;
	uint16_t cnt; 
    uint8_t devaddr[4];
    uint8_t nwkskey[16];
    uint8_t appskey[16];
  } mc;
   struct
  {
	uint8_t scan_std;
	uint8_t scan_name[10];
	int16_t scan_rssi;
	uint16_t advi;
	uint16_t uuid; 
	uint8_t advd[10];	  
    uint8_t laddr[6];
    uint8_t name[16];
    uint8_t appskey[16];
  } ble;
} atctl_data_t;

typedef atctl_ret_t (*atctl_func)(char *, int, atctl_data_t *);

typedef struct
{
  ATCmd_t cmd;
  char *name;
  atctl_func func;
} atctl_cmd_list_t;

typedef enum
{
  ATCTL_RX_HEAD,
  ATCTL_RX_CMD,
  ATCTL_RX_DONE,
  ATCTL_PARSE_DONE,
} atctl_sta_t;

/*type definition for SENDB command*/
typedef struct sSendDataBinary
{
  char *Buffer;
  uint8_t Num;
  uint8_t Port;
  uint8_t DataSize;
} sSendDataBinary_t, sSendDataString_t;

/* LoRa modem band plans */
typedef enum eBandPlans
{
  EU868,
  US915,
  US915HYBRID,
  CN779,
  EU433,
  AU915,
  AU915OLD,
  CN470,
  AS923,
  KR920,
  IN865,
  CN470PREQUEL,
  STE920,
  BAND_MAX,
} BandPlans_t;

uint16_t at_cmd_vprintf(const char *format, ...);

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* AT printf */
#define AT_VPRINTF(...)    at_cmd_vprintf(__VA_ARGS__)

#define AT_VSSCANF(...)    tiny_sscanf(__VA_ARGS__)

#define DBG_PRINTF(...) 	SEGGER_RTT_printf(0,__VA_ARGS__);//fprintf(stdout, ">>>>>" format "<<<<", ##__VA_ARGS__) 

/* Exported functions ------------------------------------------------------- */

/******************************************************************************
* @brief This function receives response from the slave device and parse
* @param Cmd: command type
* @param *dt: atctl_data_t type
* @retval LoRa return code
******************************************************************************/
atctl_ret_t at_cmd_receive(ATCmd_t Cmd, atctl_data_t *dt,uint16_t response_timeout);

/******************************************************************************
* @brief This function receives response from the slave device
* @param viod
* @retval LoRa return code
******************************************************************************/
atctl_ret_t at_cmd_receive_evt(void);

/**************************************************************
* @brief  Parse the data stored in buffer
* @param  *dt: atctl_data_t
* @retval LoRa return code
**************************************************************/
atctl_ret_t atctl_rx(atctl_data_t *dt, int timeout);

///**************************************************************
//* @brief  Send the command and receive
//* @param  *dt: atctl_data_t
//* @retval LoRa return code
//**************************************************************/
//atctl_ret_t atctl_tx(atctl_data_t *dt, ATCmd_t cmd, char *fmt, ...);

HAL_StatusTypeDef at_printf_send(uint8_t *buf, uint16_t len);

ATEerror_t  Modem_AT_Cmd(ATGroup_t at_group, ATCmd_t Cmd, void *pdata,uint16_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* __LRWAN_NS2_ATCMD__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
