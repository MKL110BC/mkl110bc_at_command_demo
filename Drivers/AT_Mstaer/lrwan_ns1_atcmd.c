/**
  ******************************************************************************
  * @file    lrwan_ns1_atcmd.c
  * @author  MCD Application Team
  * @brief   at command API
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

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stm32l1xx_hal.h"
#include <stdio.h>
#include "lrwan_ns1_atcmd.h"
#include "hw.h"

#define ATCTL_WAKEUP    0
#define CMD_DEBUG 1

/*Globle variables------------------------------------------------------------*/
uint32_t record_num = 0;
uint8_t atctl_dl_buf[256];
char LoRa_AT_Cmd_Buff[DATA_TX_MAX_BUFF_SIZE];    /* Buffer used for AT cmd transmission */
char response[DATA_RX_MAX_BUFF_SIZE];   /*not only for return code but also for return value: exemple KEY*/

/* Private functions ---------------------------------------------------------*/
static atctl_ret_t atctl_parse(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_at(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_ver(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_vdd(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_msg(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_join(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_mode(char *buf, int len, atctl_data_t *dt);

static HAL_StatusTypeDef at_cmd_send(uint16_t len);

static uint8_t at_cmd_format(ATCmd_t Cmd, void *ptr, Marker_t Marker);

static atctl_ret_t atctl_statu(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_addr(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_eui(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_key(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_laddr(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_value(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_string(char *buf, int len, atctl_data_t *dt);
static atctl_ret_t atctl_channel(char *buf, int len, atctl_data_t *dt);

ATCmd_t FLAG_Cmd;
/* Private variables ---------------------------------------------------------*/
static uint16_t Offset = 0;   /*write position needed for sendb command*/
//static uint8_t aRxBuffer[5];  /* Buffer used for Rx input character */
static const atctl_cmd_list_t atctl_cmd_list[] =
{
    {AT,             "AT",         			  atctl_at},
    {AT_LADDR,       "LADDR",    				  atctl_laddr},
    {AT_NAME,        "NAME",      			  atctl_string},
    {AT_ADVI,        "ADVI",    					atctl_value},
    {AT_UUID,        "UUID",    		      atctl_value},
    {AT_ADVD,        "ADVD",   					  atctl_string},
    {AT_SCAN_STD,    "SACN_STD",     		  atctl_statu},
    {AT_SCAN_NAME,   "SACN_NAME",    			atctl_string},
    {AT_SCAN_RSSI,   "SACN_RSSI",         atctl_value},

    {AT_SLEEP,       "SLEEP",       		  atctl_statu},
    {AT_BAUD,        "BAUD",        		  atctl_value},
    {AT_ATE,         "ATE",         			atctl_statu},

    {AT_DEVEUI,      "DEVEUI",       		  atctl_eui},
    {AT_APPEUI,      "APPEUI",      		  atctl_eui},
    {AT_APPKEY,      "APPKEY",      		  atctl_key},
    {AT_NWKSKEY,     "NWKSKEY",     		  atctl_key},
    {AT_APPSKEY,     "APPSKEY",     		  atctl_key},
    {AT_DADDR,       "DEVADDR",     		  atctl_addr},
    {AT_JOIN_MODE,   "JOIN_MODE",    		  atctl_mode},
    {AT_CLASS,       "CLASS",     			  atctl_msg},
    {AT_JOINING,     "JOINING",    			  atctl_join},
    {AT_JOIN_STD,    "JOIN_STD",    		  atctl_join},
    {AT_AUTO_JOIN,   "AUTO_JOIN",    		  atctl_statu},
    {AT_NWKID,       "NWKID",     				NULL},

    {AT_TXLEN,       "TXLEN",      		    atctl_value},
    {AT_SENDB,       "SENDB",     		    NULL},
    {AT_SEND,        "SEND",      			  NULL},
    {AT_RECVB,       "RECVB",     			  atctl_msg},
    {AT_RECV,        "RECV",       			  atctl_msg},
    {AT_CFM,         "CONFIRM",        		atctl_statu},
    {AT_UP_CNT,      "UP_CNT",            atctl_value},
    {AT_DOWN_CNT,    "DOWN_CNT",          atctl_value},

    {AT_ADR,         "ADR",          	    atctl_statu},
    {AT_TXP,         "TX_POWR",           atctl_value},
    {AT_DR,      	 "DR",        	      atctl_value},
    {AT_CH,          "CH",         		    atctl_channel},
    {AT_DUTY,    	 "DUTY_CYCLE",        atctl_value},
    {AT_RX2FQ,       "RX2FQ",     	      atctl_value},
    {AT_RX2DR,       "RX2DR",             atctl_value},
    {AT_RX1DL,       "RX1DL",      			  atctl_value},
    {AT_RX2DL,       "DX2DL",       			atctl_value},
    {AT_JN1DL,       "JN_RX1DL",    			atctl_value},
    {AT_JN2DL,       "JN_RX2DL",   			  atctl_value},
    {AT_REGION,      "REGION",            atctl_string},

    {AT_VER,         "VER",          			atctl_ver},
    {AT_SNR,         "SNR",               atctl_value},
    {AT_RSSI,        "RSSI",              atctl_value},
    {AT_BAT,         "BAT",          		  atctl_vdd},

    {AT_MC,          "MC",                atctl_statu},
    {AT_MC_DADDR,    "MC_DEVADDR",        atctl_addr},
    {AT_MC_NSKEY,    "MC_NWKSKEY",        atctl_key},
    {AT_MC_ASKEY,    "MC_APPSKEY",        atctl_key},
    {AT_MC_CNT,      "MC_CNT",            atctl_value},

    {AT_RESET,       "RESET",             atctl_at},
    {AT_FACTORY,     "FACTORY",           atctl_at},

    {AT_TEST,        "TEST",              atctl_statu},
    {AT_TEST_TXLR,   "TEST_TXLORA",       NULL},
    {AT_TEST_RXLR,   "TEST_RXLORA",       NULL},
    {AT_TEST_CFG,    "TEST_CONF",         NULL},
    {AT_TEST_SCAN,   "TEST_SCAN",         NULL},
    {AT_TEST_PIO0,   "TEST_PIO0",         NULL},
    {AT_TEST_PIO1,   "TEST_PIO1",         NULL},
    {AT_TEST_BLE,    "TEST_BLE_CON",      NULL},
};

//static const struct atctl_msg_str
//{
//    const char *info;
//    atctl_msg_sta_t type;
//} atctl_msg_str[] =
//{
//    {"Start",                               ATCTL_MSG_START},
//    {"Done",                                ATCTL_MSG_DONE},
//    {"LoRaWAN modem is busy",               ATCTL_MSG_BUSY},

//    {"TX ",                                 ATCTL_MSG_TX},
//    {"Wait ACK",                            ATCTL_MSG_WAIT_ACK},
//    {"PORT",                                ATCTL_MSG_RX},
//    {"RXWIN",                               ATCTL_MSG_RXWIN},
//    {"MACCMD",                              ATCTL_MSG_MACCMD},

//    {"Network joined",                      ATCTL_MSG_JOINED},
//    {"Joined already",                      ATCTL_MSG_JOIN_ALREADY},
//    {"NORMAL",                              ATCTL_MSG_JOIN_NORMAL},
//    {"FORCE",                               ATCTL_MSG_JOIN_FORCE},
//    {"Join failed",                         ATCTL_MSG_JOIN_FAILED},
//    {"NetID",                               ATCTL_MSG_JOIN_NETID},
//    {"Not in OTAA mode",                    ATCTL_MSG_JOIN_NOT_OTAA},

//    {"Please join network first",           ATCTL_MSG_NO_NET},
//    {"DR error",                            ATCTL_MSG_DR},
//    {"Length error",                        ATCTL_MSG_LEN},
//    {"No free channel",                     ATCTL_MSG_NO_CH},
//    {"No band in",                          ATCTL_MSG_NO_BAND},
//    {"ACK Received",                        ATCTL_MSG_ACK_RECEIVED},
//};

static const struct band_str
{
    BandPlans_t BandPlan;
    const char *info;
} band_str[] =
{
    {EU868,                         "EU868"},
    {US915,                         "US915"},
    {US915HYBRID,                   "US915HYBRID"},
    {CN779,                         "CN779"},
    {EU433,                         "EU433"},
    {AU915,                         "AU915"},
    {AU915OLD,                      "AU915OLD"},
    {CN470,                         "CN470"},
    {AS923,                         "AS923"},
    {KR920,                         "KR920"},
    {IN865,                         "IN865"},
    {CN470PREQUEL,                  "CN470PREQUEL"},
    {STE920,                        "STE920"},
};

static atctl_sta_t atctl_sta;
static char atctl_rx_buf[ATCTL_CMD_BUF_SIZE];
static volatile int16_t atctl_rx_wr_index;
static uint8_t atctl_rx_tmp_buf[ATCTL_CMD_BUF_SIZE];
static volatile int16_t atctl_rx_tmp_wr_index, atctl_rx_tmp_rd_index, atctl_rx_tmp_cnt;

extern atctl_data_t dt;

/**************************************************************
* @brief  Parse the received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_parse(char *buf, int len, atctl_data_t *dt)
{
    int i, cmdlen;
    i = sscanf((char *)buf, "%*s ERROR(%d)", &dt->err);    
//    LOG_LUO("------>atctl_parse error%d %d \r\n", dt->err,i);
    if (0 == strncasecmp("ERROR", (char *)buf, 5))
    {	
        dt->err = buf[7]-0x30;
        LOG_LUO("------>atctl_parse error%d \r\n", dt->err);
        return ATCTL_RET_CMD_ERR;
    }
//	
    if(buf[0]=='+')	
	{
//		LOG_LUO("atctl_join :%s  %d \r\n", buf, len);
		for (i = 0; i < sizeof(atctl_cmd_list) / sizeof(atctl_cmd_list_t); i ++)
		{
			
			cmdlen = strlen(atctl_cmd_list[i].name);
//			LOG_LUO("atctl_join :  %s  %d \r\n", atctl_cmd_list[i].name, cmdlen);
			if (0 == strncasecmp(atctl_cmd_list[i].name, (char *)buf + 1, cmdlen))
			{
				LOG_LUO("------>atctl_parse %s %d \r\n",atctl_cmd_list[i].name,i);
				if (atctl_cmd_list[i].func == NULL)
				{
					return ATCTL_RET_CMD_OK;
				}
				return (atctl_cmd_list[i].func(buf, len, dt));
			}
		}
	}
	else 
	{
		return (atctl_cmd_list[0].func(buf, len, dt));
	}
    return ATCTL_RET_ERR;
}

/**************************************************************
* @brief  Parse the AT command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_at(char *buf, int len, atctl_data_t *dt)
{
//    int i;
//    uint8_t p[2];
	LOG_LUO("-------------atctl_at \r\n ");
	if(Find_String((unsigned char *)buf,(unsigned char *)"OK",2)!= -1)
		return ATCTL_RET_CMD_AT;
//    i = sscanf((char *)buf, "%*s %c%c", &p[0], &p[1]);
//    if (i == 2)
//    {
//        if (p[0] == 'O' && p[1] == 'K')
//        {
//            return ATCTL_RET_CMD_AT;
//        }
//    }
    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT+VER command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_ver(char *buf, int len, atctl_data_t *dt)
{
    int i;
    i = sscanf((char *)buf, "%*s %d.%d.%d", &dt->ver[0], &dt->ver[1], &dt->ver[2]);
    if (i == 3)
    {
        return ATCTL_RET_CMD_VER;
    }
    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT+VDD command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_vdd(char *buf, int len, atctl_data_t *dt)
{
    int i;
    struct
    {
        int integer;
        int fractional;
    } vdd;
    i = sscanf((char *)buf, "%*s %d.%dV", &vdd.integer, &vdd.fractional);
    if (i == 2)
    {
        dt->vdd = (float)vdd.integer + (float)vdd.fractional / 100.0;
        return ATCTL_RET_CMD_VDD;
    }
    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT+MODE command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_mode(char *buf, int len, atctl_data_t *dt)
{
    int i;
    char str_mode[4] = {0};
    char otaa_mode[] = "OTAA";
    char abp_mode[] = "ABP";
    i = sscanf((char *)buf, "%*s %s", str_mode);
    if (i == 1)
    {
        if (!memcmp(otaa_mode, str_mode, strlen(otaa_mode)))
        {
            dt->mode = 1;
        }
        else if (!memcmp(abp_mode, str_mode, strlen(abp_mode)))
        {
            dt->mode = 0;
        }
        return ATCTL_RET_CMD_MODE;
    }
    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_channel(char *buf, int len, atctl_data_t *dt)
{
    int i;
    int p[4];
    i = sscanf((char *)buf, "%*s %d-%d,%d", &p[0], &p[1], &p[2]);
    if (i == 3)
    {
        dt->channel.chn_l = p[0];
        dt->channel.chn_h = p[1];
        dt->channel.chn_max = p[2];
        return ATCTL_RET_CMD_AT;
    }
    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT+xxvalue command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_value(char *buf, int len, atctl_data_t *dt)
{
    int i;
    int atctl_value = 0;

    i = sscanf((char *)buf, "%*s %d", &atctl_value);
    if (i == 1)
    {
        if (0 == strncasecmp(atctl_cmd_list[AT_ADVI].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_ADVI].name)))
            dt->ble.advi = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_UUID].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_UUID].name)))
            dt->ble.uuid = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_SCAN_RSSI].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_SCAN_RSSI].name)))
            dt->ble.scan_rssi = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_BAUD].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_BAUD].name)))
            dt->dev_param.uart_baud = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_TXLEN].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_TXLEN].name)))
            dt->dev_param.txlen = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_UP_CNT].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_UP_CNT].name)))
            dt->dev_param.ul_cnt = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_DOWN_CNT].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_DOWN_CNT].name)))
            dt->dev_param.dl_cnt = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_TXP].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_TXP].name)))
            dt->dev_param.txp = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_DR].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_DR].name)))
            dt->dev_param.dr = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_DUTY].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_DUTY].name)))
            dt->dev_param.duty = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_RX2FQ].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_RX2FQ].name)))
            dt->dev_param.rx2fq = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_RX2DR].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_RX2DR].name)))
            dt->dev_param.rx2dr = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_RX1DL].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_RX1DL].name)))
            dt->dev_param.rx1dl = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_RX2DL].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_RX2DL].name)))
            dt->dev_param.rx2dl = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_JN1DL].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_JN1DL].name)))
            dt->dev_param.jrx1dl = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_JN2DL].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_JN2DL].name)))
            dt->dev_param.jrx2dl = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_SNR].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_SNR].name)))
            dt->dev_param.snr = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_RSSI].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_RSSI].name)))
            dt->dev_param.rssi = atctl_value;
        else if (0 == strncasecmp(atctl_cmd_list[AT_MC_CNT].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_MC_CNT].name)))
            dt->mc.cnt = atctl_value;
        return ATCTL_RET_CMD_DELAY;
    }
    return ATCTL_RET_CMD_OK;
}


static atctl_ret_t atctl_string(char *buf, int len, atctl_data_t *dt)
{
    int i;
    char atctl_buf[20] = {0};
    
    i = sscanf((char *)buf, "%*s %s", atctl_buf);
	if(i==1)
	{
	 if (0 == strncasecmp(atctl_cmd_list[AT_NAME].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_NAME].name)))
	 {
		memset(dt->ble.name,0,sizeof(dt->ble.name));
        memcpy(dt->ble.name,atctl_buf,len-1);	 
	 }
        else if (0 == strncasecmp(atctl_cmd_list[AT_ADVD].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_ADVD].name)))
            	 {
		memset(dt->ble.advd,0,sizeof(dt->ble.advd));
        memcpy(dt->ble.advd,atctl_buf,len-1);	 
	 }
        else if (0 == strncasecmp(atctl_cmd_list[AT_SCAN_NAME].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_SCAN_NAME].name)))
           	 {
		memset(dt->ble.scan_name,0,sizeof(dt->ble.scan_name));
        memcpy(dt->ble.scan_name,atctl_buf,len-1);	 
	 }
        else if (0 == strncasecmp(atctl_cmd_list[AT_REGION].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_REGION].name)))
            	 {
		memset(dt->id.region,0,sizeof(dt->id.region));
        memcpy(dt->id.region,atctl_buf,len-1);	 
	 }
		 return ATCTL_RET_CMD_DELAY;
	}
	return ATCTL_RET_CMD_OK;
}
///**************************************************************
//* @brief  Parse the AT+DR command received data
//* @param  *buf: point to the data
//* @param  len: length of the data
//* @param  *dt: point to area used to store the parsed data
//* @retval LoRa return code
//**************************************************************/
//static atctl_ret_t atctl_dr(char *buf, int len, atctl_data_t *dt)
//{
//    int i;
//    int dr_value = 0;
//    uint8_t type_str[20] = {0};

//    i = sscanf((char *)buf, "%*s DR%d", (&dr_value));
//    if (i == 1)
//    {
//        dt->dr.dr_value = dr_value;
//        return ATCTL_RET_CMD_DR;
//    }

//    i = sscanf((char *)buf, "%*s %s", type_str);
//    if (i == 1)
//    {
//        /* need to do something about dr type */
//        return ATCTL_RET_CMD_DR;
//    }

//    i = sscanf((char *)buf, "%*s %*s %*s  %*s %s", type_str);
//    if (i == 1)
//    {
//        /* need to do something about dr type */
//        return ATCTL_RET_CMD_DR;
//    }

//    return ATCTL_RET_CMD_OK;
//}

///**************************************************************
//* @brief  Parse the AT+ID command received data
//* @param  *buf: point to the data
//* @param  len: length of the data
//* @param  *dt: point to area used to store the parsed data
//* @retval LoRa return code
//**************************************************************/
//static atctl_ret_t atctl_id(char *buf, int len, atctl_data_t *dt)
//{
//    int i;
//    int p[8];

//    i = sscanf((char *)buf, "%*s DevAddr, %02x:%02x:%02x:%02x", &p[0], &p[1], &p[2], &p[3]);
//    if (i == 4)
//    {
//        for (i = 0; i < 4; i ++)
//        {
//            dt->id.devaddr[i] = (uint8_t)p[i];
//        }
//        return ATCTL_RET_CMD_ID;
//    }

//    i = sscanf((char *)buf, "%*s DevEui, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", \
//               &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7]);
//    if (i == 8)
//    {
//        for (i = 0; i < 8; i ++)
//        {
//            dt->id.deveui[i] = (uint8_t)p[i];
//        }
//        return ATCTL_RET_CMD_ID;
//    }

//    i = sscanf((char *)buf, "%*s AppEui, %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", \
//               &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7]);
//    if (i == 8)
//    {
//        for (i = 0; i < 8; i ++)
//        {
//            dt->id.appeui[i] = (uint8_t)p[i];
//        }
//        return ATCTL_RET_CMD_ID;
//    }

//    return ATCTL_RET_CMD_OK;
//}

/**************************************************************
* @brief  Parse the AT+xxEUI command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_eui(char *buf, int len, atctl_data_t *dt)
{
    int i;
    int p[8];
	LOG_LUO("-------------atctl_eui \r\n ");
    i = sscanf((char *)buf, "%*s %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", \
               &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7]);

    if (0 == strncasecmp(atctl_cmd_list[AT_DEVEUI].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_DEVEUI].name)))
    {
        if (i == 8)
        {
            for (i = 0; i < 8; i ++)
            {
                dt->id.deveui[i] = (uint8_t)p[i];
            }
            return ATCTL_RET_CMD_ID;
        }
    }
    else if (0 == strncasecmp(atctl_cmd_list[AT_APPEUI].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_APPEUI].name)))
    {
        if (i == 8)
        {
            for (i = 0; i < 8; i ++)
            {
                dt->id.appeui[i] = (uint8_t)p[i];
            }
            return ATCTL_RET_CMD_ID;
        }
    }

    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT+xxADDR command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_addr(char *buf, int len, atctl_data_t *dt)
{
    int i;
    int p[8];

    i = sscanf((char *)buf, "%*s %02x:%02x:%02x:%02x", &p[0], &p[1], &p[2], &p[3]);

    if (0 == strncasecmp(atctl_cmd_list[AT_DADDR].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_DADDR].name)))
    {
        if (i == 4)
        {
            for (i = 0; i < 4; i ++)
            {
                dt->id.devaddr[i] = (uint8_t)p[i];
            }
            return ATCTL_RET_CMD_ID;
        }
    }

    else if (0 == strncasecmp(atctl_cmd_list[AT_MC_DADDR].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_MC_DADDR].name)))
    {
        if (i == 4)
        {
            for (i = 0; i < 4; i ++)
            {
                dt->mc.devaddr[i] = (uint8_t)p[i];
            }
            return ATCTL_RET_CMD_ID;
        }
    }

    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT+xxEUI command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_laddr(char *buf, int len, atctl_data_t *dt)
{
    int i;
    int p[8];

    i = sscanf((char *)buf, "%*s %02x:%02x:%02x:%02x:%02x:%02x", \
               &p[0], &p[1], &p[2], &p[3], &p[4], &p[5]);

    if (i == 6)
    {
        for (i = 0; i < 6; i ++)
        {
            dt->ble.laddr[i] = (uint8_t)p[i];
        }
        return ATCTL_RET_CMD_ID;
    }
    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT+xxKEY command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_key(char *buf, int len, atctl_data_t *dt)
{
    int i;
    int p[16];

    i = sscanf((char *)buf, "%*s %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", \
               &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7], &p[8], &p[9], &p[10], &p[11], &p[12], &p[13], &p[14], &p[15]);

    if (0 == strncasecmp(atctl_cmd_list[AT_APPKEY].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_APPKEY].name)))
    {
        if (i == 16)
        {
            for (i = 0; i < 16; i ++)
            {
                dt->id.appkey[i] = (uint8_t)p[i];
            }
            return ATCTL_RET_CMD_OK;
        }
    }

    else if (0 == strncasecmp(atctl_cmd_list[AT_NWKSKEY].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_NWKSKEY].name)))
    {
        if (i == 16)
        {
            for (i = 0; i < 16; i ++)
            {
                dt->id.nwkskey[i] = (uint8_t)p[i];
            }
            return ATCTL_RET_IDLE;
        }
    }
    else if (0 == strncasecmp(atctl_cmd_list[AT_APPSKEY].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_APPSKEY].name)))
    {
        if (i == 16)
        {
            for (i = 0; i < 16; i ++)
            {
                dt->id.appskey[i] = (uint8_t)p[i];
            }
            return ATCTL_RET_IDLE;
        }
    }
    else if (0 == strncasecmp(atctl_cmd_list[AT_MC_NSKEY].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_MC_NSKEY].name)))
    {
        if (i == 16)
        {
            for (i = 0; i < 16; i ++)
            {
                dt->mc.nwkskey[i] = (uint8_t)p[i];
            }
            return ATCTL_RET_IDLE;
        }
    }
    else if (0 == strncasecmp(atctl_cmd_list[AT_MC_ASKEY].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_MC_ASKEY].name)))
    {
        if (i == 16)
        {
            for (i = 0; i < 16; i ++)
            {
                dt->mc.appskey[i] = (uint8_t)p[i];
            }
            return ATCTL_RET_IDLE;
        }
    }

    return ATCTL_RET_CMD_OK;
}
/**************************************************************
* @brief  Parse the AT+XXSTD command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_statu(char *buf, int len, atctl_data_t *dt)
{
    int i;
    char str_mode[4] = {0};
    char on_mode[4] = "ON";
    char off_mode[4] = "OFF";
    int atctl_std = 0;

    i = sscanf((char *)buf, "%*s %s", str_mode);
    if (i == 1)
    {
        if (!memcmp(on_mode, str_mode, strlen(on_mode)))
        {
            atctl_std = 1;
        }
        else if (!memcmp(off_mode, str_mode, strlen(off_mode)))
        {
            atctl_std = 0;
        }
    if (0 == strncasecmp(atctl_cmd_list[AT_SCAN_STD].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_SCAN_STD].name)))
        dt->ble.scan_std = atctl_std;
    else if (0 == strncasecmp(atctl_cmd_list[AT_ATE].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_ATE].name)))
        dt->std.ate = atctl_std;
    else if (0 == strncasecmp(atctl_cmd_list[AT_CFM].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_CFM].name)))
        dt->std.cfm = atctl_std;
    else if (0 == strncasecmp(atctl_cmd_list[AT_ADR].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_ADR].name)))
        dt->std.adr = atctl_std;
    else if (0 == strncasecmp(atctl_cmd_list[AT_AUTO_JOIN].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_AUTO_JOIN].name)))
        dt->std.atuo_join = 0;
    else if (0 == strncasecmp(atctl_cmd_list[AT_MC].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_MC].name)))
        dt->mc.std = atctl_std;
    else if (0 == strncasecmp(atctl_cmd_list[AT_TEST].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_TEST].name)))
        dt->std.test = atctl_std;
    else if (0 == strncasecmp(atctl_cmd_list[AT_SLEEP].name, (char *)buf + 1, strlen(atctl_cmd_list[AT_SLEEP].name)))
        dt->std.sleep = atctl_std;
	        return ATCTL_RET_CMD_MODE;
    }
    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT+JOIN command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_join(char *buf, int len, atctl_data_t *dt)
{
	int i;
	char join_buf[4] = {0};
    char join_ok[8] = "JOINED";
	LOG_LUO("atctl_join :  %s  %d \r\n", buf, len);
	 i = sscanf((char *)buf, "%*s %s", join_buf);
    if (i == 1)
    {
        if (!memcmp(join_ok, join_buf, strlen(join_ok)))
        {
            dt->id.join_std = 1;
			return ATCTL_RET_CMD_JOIN;
        }
		else 
			dt->id.join_std = 0;	
	}
    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Parse the AT+MSG command received data
* @param  *buf: point to the data
* @param  len: length of the data
* @param  *dt: point to area used to store the parsed data
* @retval LoRa return code
**************************************************************/
static atctl_ret_t atctl_msg(char *buf, int len, atctl_data_t *dt)
{
    int i;
	char msg_ack[4] = "ACK";
    char recv_buf[256];
    int x0, x1, x2,x3;
    
	i = sscanf((char *)buf, "%*s %d:%d:%d:%d:%s", &x0,&x1,&x2,&x3,recv_buf);
    if (i == 5)
        {
			dt->dev_param.rssi = x0;
			dt->dev_param.snr = x1;
			dt->msg.port = x2;
			dt->msg.size = x3;
			if (!memcmp(msg_ack, recv_buf, strlen(msg_ack)))
				dt->msg.ack = true;
			else
				memcpy(dt->msg.buf,recv_buf,dt->msg.size);
            return ATCTL_RET_CMD_MSG;
        }

    return ATCTL_RET_CMD_OK;
}

/**************************************************************
* @brief  Reset the key variables of the atctl function
* @param  void
* @retval void
**************************************************************/
static void atctl_reset(void)
{
    atctl_sta = ATCTL_RX_HEAD;
    atctl_rx_wr_index = 0;
    atctl_rx_tmp_cnt = 0;
    atctl_rx_tmp_rd_index = atctl_rx_tmp_wr_index;
}

/**************************************************************
* @brief  Store the received data in some place base on the rule for easy parse
* @param  data: one byte received
* @retval void
**************************************************************/
static void atctl_rx_byte(uint8_t data)
{
    switch (atctl_sta)
    {
    case ATCTL_RX_HEAD:
        atctl_rx_buf[0] = data;
        if (atctl_rx_buf[0] == '+')
        {
            atctl_rx_wr_index = 1;
            atctl_sta = ATCTL_RX_CMD;
        }
        break;
    case ATCTL_RX_CMD:
        atctl_rx_buf[atctl_rx_wr_index++] = data;
        if (data == '\n')
        {
            atctl_sta = ATCTL_RX_DONE;
            atctl_rx_buf[atctl_rx_wr_index] = '\0';
        }
        else if ((data == '\t') || (data == '\r') || (data > 'a' && data <= 'z') || \
                 (data >= ' ' && data <= '~'))
        {

        }
        else
        {
            /** Unknow character */
            atctl_reset();
        }
        if ((atctl_rx_wr_index >= ATCTL_CMD_BUF_SIZE) && (atctl_sta != ATCTL_RX_DONE))
        {
            /** atmunication buffer overflow */
            atctl_reset();
        }
        break;
    case ATCTL_RX_DONE:
    case ATCTL_PARSE_DONE:
        /*save ongoing commands */
        if (atctl_rx_tmp_cnt < ATCTL_CMD_BUF_SIZE)
        {
            atctl_rx_tmp_buf[atctl_rx_tmp_wr_index++] = data;
            if (atctl_rx_tmp_wr_index == ATCTL_CMD_BUF_SIZE)
            {
                atctl_rx_tmp_wr_index = 0;
            }
            atctl_rx_tmp_cnt++;
        }
        break;
    }
}

///**************************************************************
//* @brief  Handle the input params
//* @retval length of buf
//**************************************************************/
//static int atctl_buf(char *buf, char *fmt, va_list ap)
//{
//    int i, d, ret, len;
//    char c, *s;
//    uint8_t *hbuf;
//    double f;

//    if (fmt == NULL)
//    {
//        return 0;
//    }
//    i = 0;
//    while (*fmt)
//    {
//        if (*fmt == '%')
//        {
//            fmt++;
//            switch (*fmt)
//            {
//            case 'd':
//                d = va_arg(ap, int);
//                ret = sprintf(buf + i, "%d", d);
//                i += ret;
//                break;
//            case 'x':
//            case 'X':
//                d = va_arg(ap, int);
//                ret = sprintf(buf + i, "%X", d);
//                i += ret;
//                break;
//            case 'h':
//                hbuf = va_arg(ap, uint8_t *);
//                len = va_arg(ap, int);
//                for (d = 0; d < len; d++)
//                {
//                    ret = sprintf(buf + i, "%02X", hbuf[d]);
//                    i += ret;
//                }
//                break;
//            case 's':
//                s = va_arg(ap, char *);
//                ret = sprintf(buf + i, "\"%s\"", s);
//                i += ret;
//                break;
//            case 'c':
//                c = (char)va_arg(ap, int);
//                ret = sprintf(buf + i, "%c", c);
//                i += ret;
//                break;
//            case 'f':
//                f = va_arg(ap, double);
//                ret = sprintf(buf + i, "%.3f", f);
//                i += ret;
//                break;
//            }
//            fmt++;
//        }
//        else
//        {
//            buf[i++] = *fmt++;
//        }
//    }

//    buf[i] = '\0';

//    return i;
//}

///**************************************************************
//* @brief  Send the command and receive
//* @param  *dt: atctl_data_t
//* @retval LoRa return code
//**************************************************************/
//atctl_ret_t atctl_tx(atctl_data_t *dt, ATCmd_t cmd, char *fmt, ...)
//{
//    char buf[256];
//    uint8_t wakeup_ch[4] = {0xFF, 0xFF, 0xFF, 0xFF};
//    va_list ap;
//    int i, len;
//    atctl_ret_t ret = ATCTL_RET_CMD_ERR;
//    HAL_StatusTypeDef HAL_Status;

//    for (i = 0; i < AT_MAX; i++)
//    {
//        if (cmd == atctl_cmd_list[i].cmd)
//        {
//            break;
//        }
//    }
//    if (i == AT_MAX)
//    {
//        return ATCTL_RET_ERR;
//    }

//    va_start(ap, fmt);
//    len = atctl_buf(buf, fmt, ap);
//    va_end(ap);

//    atctl_reset();

//    if (ATCTL_WAKEUP)
//    {
//        HAL_UART_Transmit(&huart1, wakeup_ch, strlen((char *)wakeup_ch), 5000);
//    }

//    memset(LoRa_AT_Cmd_Buff, 0x00, sizeof LoRa_AT_Cmd_Buff);

//    if ((len != 0) && (cmd != AT))
//    {
//        len = AT_VPRINTF("AT+%s=%s\r\n", atctl_cmd_list[i].name, buf);
//    }
//    else
//    {
//        if (cmd == AT)
//        {
//            len = AT_VPRINTF("AT\r\n");
//        }
//        else
//        {
//            len = AT_VPRINTF("AT+%s\r\n", atctl_cmd_list[i].name);
//        }
//    }

//    HAL_Status = at_cmd_send(len);
//    if (HAL_Status != HAL_OK)
//    {
//        return (ATCTL_RET_ERR); /*problem on UART transmission*/
//    }
//    if (cmd != AT_RESET)
//    {
//        ret = at_cmd_receive(cmd, dt);
//    }
//#if defined CMD_DEBUG
//    if (len)
//    {
//        at_printf_send((uint8_t *)LoRa_AT_Cmd_Buff, len);
//    }

//    if (response[0] != '\0')
//    {
//        at_printf_send((uint8_t *)response, strlen(response));
//    }
//#endif
//    return ret;
//}

/**************************************************************
* @brief  Parse the data stored in buffer
* @param  *dt: atctl_data_t
* @retval LoRa return code
**************************************************************/
atctl_ret_t atctl_rx(atctl_data_t *dt, int timeout)
{
    atctl_ret_t ret = ATCTL_RET_CMD_ERR;
    int curindex;

    if (timeout == 0)
    {
        if ((atctl_sta == ATCTL_RX_DONE) || ((atctl_sta == ATCTL_PARSE_DONE) && (atctl_rx_tmp_cnt > 0)))
        {
            timeout = ATCTL_RX_TIMEOUT;
        }
        else
        {
            return ATCTL_RET_IDLE;
        }
    }

    curindex = atctl_rx_wr_index;
    while (1)
    {
        if ((atctl_sta == ATCTL_PARSE_DONE) && (atctl_rx_tmp_cnt > 0))
        {
            __disable_irq();
            atctl_sta = ATCTL_RX_HEAD;
            while (atctl_rx_tmp_cnt > 0)
            {
                atctl_rx_byte(atctl_rx_tmp_buf[atctl_rx_tmp_rd_index++]);
                if (atctl_rx_tmp_rd_index == ATCTL_CMD_BUF_SIZE)
                {
                    atctl_rx_tmp_rd_index = 0;
                }
                atctl_rx_tmp_cnt--;
                if (atctl_sta == ATCTL_RX_DONE)
                {
                    break;
                }
            }
            __enable_irq();
        }
        else if (atctl_sta == ATCTL_RX_DONE)
        {
            atctl_sta = ATCTL_PARSE_DONE;
            ret = atctl_parse(atctl_rx_buf, atctl_rx_wr_index, dt);
            if (atctl_rx_tmp_cnt == 0)
            {
                atctl_sta = ATCTL_RX_HEAD;
            }
            return ret;
        }
        else if (atctl_sta == ATCTL_RX_HEAD)
        {
            return ret;
        }
        if (atctl_rx_wr_index > curindex)
        {
            curindex = atctl_rx_wr_index;
        }
    }
}

/******************************************************************************
* @brief  Configures modem UART interface.
* @param  None
* @retval AT_OK in case of success
* @retval AT_UART_LINK_ERROR in case of failure
*****************************************************************************/
//HAL_StatusTypeDef Modem_IO_Init(void)
//{
//  if (HW_UART_Modem_Init(BAUD_RATE) == HAL_OK)
//  {
//    return HAL_OK;
//  }
//  else
//  {
//    return HAL_ERROR;
//  }
//}


/******************************************************************************
* @brief  Deinitialise modem UART interface.
* @param  None
* @retval None
*****************************************************************************/
//void Modem_IO_DeInit(void)
//{
//  HAL_UART_MspDeInit(&huart2);
//  HAL_UART_MspDeInit(&huart1);
//}

/******************************************************************************
 * @brief  Handle the AT cmd following their Groupp type
 * @param  at_group AT group [control, set , get)
 *         Cmd AT command
 *         pdata pointer to the IN/OUT buffer
 * @retval module status
 *****************************************************************************/
ATEerror_t  Modem_AT_Cmd(ATGroup_t at_group, ATCmd_t Cmd, void *pdata,uint16_t timeout)
{
    ATEerror_t Status = ATCTL_RET_ERR;
    HAL_StatusTypeDef HAL_Status;
    uint16_t Len = 0;
//    uint8_t wakeup_ch[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    /*reset At_cmd buffer for each transmission*/
    memset(LoRa_AT_Cmd_Buff, 0x00, sizeof LoRa_AT_Cmd_Buff);

    atctl_reset();

//    if (ATCTL_WAKEUP)
//    {
//        HAL_UART_Transmit(&huart1, wakeup_ch, strlen((char *)wakeup_ch), 5000);
//    }
    
    switch (at_group)
    {
    case AT_CTRL:
    {
        Len = at_cmd_format(Cmd, NULL, CTRL_MARKER);
        HAL_Status = at_cmd_send(Len);
		delay_ms(200);
        if (HAL_Status != HAL_OK)
        {
            return (ATCTL_RET_ERR); /*problem on UART transmission*/
        }
        Status = at_cmd_receive(Cmd, &dt,timeout);
        break;
    }
    case AT_SET:
    {
        Len = at_cmd_format(Cmd, pdata, SET_MARKER);
        HAL_Status = at_cmd_send(Len);
		delay_ms(200);
        if (HAL_Status != HAL_OK)
        {
            return (ATCTL_RET_ERR); /*problem on UART transmission*/
        }
        Status = at_cmd_receive(Cmd, &dt,timeout);
        break;
    }
    case AT_GET:
    {
        Len = at_cmd_format(Cmd, pdata, GET_MARKER);
        HAL_Status = at_cmd_send(Len);
		delay_ms(200);
        if (HAL_Status != HAL_OK)
        {
            return (ATCTL_RET_ERR); /*problem on UART transmission*/
        }
        Status = at_cmd_receive(Cmd, &dt,timeout);
        break;
    }
    default:
        DBG_PRINTF("unknow group\n\r");
        break;

    } /*end switch(at_group)*/

//#if defined CMD_DEBUG
//    if (Len)
//    {
//        at_printf_send((uint8_t *)LoRa_AT_Cmd_Buff, Len);
//    }

//    if (response[0] != '\0')
//    {
//        at_printf_send((uint8_t *)response, strlen(response));
//    }
//#endif
    if(Status==ATCTL_RET_CMD_ERR)
	{
		LOG_LUO("------>timeout %d......\r\n", ATCTL_JOIN_TIMEOUT+ATCTL_TX_DELAY);
        delay_ms(ATCTL_JOIN_TIMEOUT+ATCTL_TX_DELAY);
	}
    else
        delay_ms(ATCTL_TX_DELAY);
    return Status;
}


/******************************************************************************
 * @brief  format the cmd in order to be send
 * @param  Cmd AT command
 *         ptr generic pointer to the IN/OUT buffer
 *         Marker to discriminate the Set from the Get
 * @retval length of the formated frame to be send
 *****************************************************************************/
static uint8_t at_cmd_format(ATCmd_t Cmd, void *ptr, Marker_t Marker)
{
    uint16_t len;      /*length of the formated command*/
    uint8_t *PtrValue; /*for IN/OUT buffer*/
    uint32_t value;    /*for 32_02X and 32_D*/
    uint8_t value_8;   /*for 8_D*/


    if(Marker == GET_MARKER) /*GET AT CMD*/
	{
		DBG_PRINTF("---GET_MARKER--->at_cmd_format : %s \n",CmdTab[Cmd]);
        len = AT_VPRINTF("%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_GET_MARKER, AT_TAIL);
	}
    else if(Marker == SET_MARKER)  /*SET AT CMD*/
    {
        DBG_PRINTF("---SET_MARKER--->at_cmd_format : %s \n",CmdTab[Cmd]);
        switch (Cmd)
        {
        case AT:              /*NO SET supported*/
        case AT_LADDR:        /*NO SET supported*/
        case AT_NAME:         /*supported*/
        case AT_ADVI:          /*supported*/
            /*Format = FORMAT_32_D_PARAM;*/
            if (Marker == SET_MARKER)
            {
                value =  *(uint32_t *)ptr;
                len = AT_VPRINTF("%s%s%s%u%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, value, AT_TAIL);
            }
            break;
        case AT_UUID:     			/*supported*/
        case AT_ADVD:          /*supported*/
        case AT_SCAN_STD:          /*supported*/
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_OFF, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_ON, AT_TAIL);
            }
            break;
        case AT_SCAN_NAME:          /*supported*/
        case AT_SCAN_RSSI:          /*supported*/
            /*Format = FORMAT_32_D_PARAM;*/
            if (Marker == SET_MARKER)
            {
                value =  *(uint32_t *)ptr;
                len = AT_VPRINTF("%s%s%s%u%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, value, AT_TAIL);
            }
            break;
        case AT_SLEEP:          /*supported*/
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_OFF, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_ON, AT_TAIL);
            }
            break;
        case AT_BAUD:
            /*Format = FORMAT_32_D_PARAM;*/
            if (Marker == SET_MARKER)
            {
                value =  *(uint32_t *)ptr;
                len = AT_VPRINTF("%s%s%s%u%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, value, AT_TAIL);
            }
            break;
        case AT_ATE:
            /*Format = FORMAT_8_D_PARAM;*/
			
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_OFF, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_ON, AT_TAIL);
            }
			LOG_LUO("SET ATE: %d \r\n",value_8 );
            break;
        case AT_DEVEUI:
        case AT_APPEUI:
            /*Format = FORMAT_8_02X_PARAM;*/
            PtrValue = (uint8_t *)ptr;
            len = AT_VPRINTF("%s%s%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s",
                             AT_HEADER, CmdTab[Cmd], AT_COMMA,
                             PtrValue[0], AT_COLON, PtrValue[1], AT_COLON, PtrValue[2], AT_COLON,
                             PtrValue[3], AT_COLON, PtrValue[4], AT_COLON, PtrValue[5], AT_COLON,
                             PtrValue[6], AT_COLON, PtrValue[7], AT_TAIL);
            break;
        case AT_APPKEY:
        case AT_NWKSKEY:
        case AT_APPSKEY:
            /*Format = FORMAT_16_02X_PARAM;*/
            PtrValue = (uint8_t *) ptr;
            len = AT_VPRINTF("%s%s%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s",
                             AT_HEADER, CmdTab[Cmd], AT_COMMA,
                             PtrValue[0], AT_COLON, PtrValue[1], AT_COLON,
                             PtrValue[2], AT_COLON, PtrValue[3], AT_COLON,
                             PtrValue[4], AT_COLON, PtrValue[5], AT_COLON,
                             PtrValue[6], AT_COLON, PtrValue[7], AT_COLON,
                             PtrValue[8], AT_COLON, PtrValue[9], AT_COLON,
                             PtrValue[10], AT_COLON, PtrValue[11], AT_COLON,
                             PtrValue[12], AT_COLON, PtrValue[13], AT_COLON,
                             PtrValue[14], AT_COLON, PtrValue[15], AT_TAIL);
            break;
        case AT_DADDR:
            /*Format = FORMAT_32_02X_PARAM;*/
            value =  *(uint32_t *)ptr;
            if (Marker == SET_MARKER)
            {
                len = AT_VPRINTF("%s%s%s%02x%02x%02x%02x%s", AT_HEADER, CmdTab[Cmd], AT_COMMA,
                                 (unsigned)((unsigned char *)(&value))[3],
                                 (unsigned)((unsigned char *)(&value))[2],
                                 (unsigned)((unsigned char *)(&value))[1],
                                 (unsigned)((unsigned char *)(&value))[0], AT_TAIL);
            }
            break;
        case AT_JOIN_MODE:
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_ABP, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_OTAA, AT_TAIL);
            }
            break;
        case AT_CLASS:
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_CLASS_A, AT_TAIL);
            }
            else if (value_8 == 2)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_CLASS_C, AT_TAIL);
            }
            break;
        case AT_JOINING:
        case AT_JOIN_STD:  /*NO SET*/
        case AT_AUTO_JOIN:
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_OFF, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_ON, AT_TAIL);
            }
            break;
        case AT_NWKID:
            /*Format = FORMAT_32_02X_PARAM;*/
            value =  *(uint32_t *)ptr;
            len = AT_VPRINTF("%s%s%s%02x%02x%02x%02x%s", AT_HEADER, CmdTab[Cmd], AT_COMMA,
                             (unsigned)((unsigned char *)(&value))[3],
                             (unsigned)((unsigned char *)(&value))[2],
                             (unsigned)((unsigned char *)(&value))[1],
                             (unsigned)((unsigned char *)(&value))[0], AT_TAIL);
            break;
        case AT_TXLEN:   /*NO SET*/
        case AT_SENDB:
			 if (Marker == SET_MARKER)
            {
			    Offset = AT_VPRINTF("%s%s%s%d%s%d%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER,((sSendDataBinary_t *)ptr)->Num,AT_COLON,((sSendDataBinary_t *)ptr)->Port,AT_COLON);
                unsigned i;
                for (i = 0; i < ((sSendDataBinary_t *)ptr)->DataSize; i++)
                {
                    Offset += AT_VPRINTF("%02x", ((sSendDataBinary_t *)ptr)->Buffer[i]);
                }
                Offset += AT_VPRINTF("%s", AT_TAIL);
                len = Offset;
                Offset = 0;
			}
			else {
                len = AT_VPRINTF("%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_GET_MARKER, AT_TAIL);
            }
            break;
        case AT_SEND:
        case AT_RECVB:
        {
            /*Format = FORMAT_BINARY_TEXT; */
            if (Marker == SET_MARKER)
            {
                Offset = AT_VPRINTF("%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER);
                unsigned i;
                for (i = 0; i < ((sSendDataBinary_t *)ptr)->DataSize; i++)
                {
                    Offset += AT_VPRINTF("%02x", ((sSendDataBinary_t *)ptr)->Buffer[i]);
                }
                Offset += AT_VPRINTF("%s", AT_TAIL);
                len = Offset;
                Offset = 0;
            }
            else
            {
                len = AT_VPRINTF("%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_GET_MARKER, AT_TAIL);
            }
            break;
        }
        case AT_RECV:
        {
            /*Format = FORMAT_PLAIN_TEXT;*/
            if (Marker == SET_MARKER)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, ((sSendDataBinary_t *)ptr)->Buffer, AT_TAIL);
            }
            else
            {
                len = AT_VPRINTF("%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_GET_MARKER, AT_TAIL);
            }
            break;
        }
        case AT_CFM:
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_OFF, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_ON, AT_TAIL);
            }
            break;
        case AT_UP_CNT:
            /*Format = FORMAT_32_D_PARAM;*/
            if (Marker == SET_MARKER)
            {
                value =  *(uint32_t *)ptr;
                len = AT_VPRINTF("%s%s%s%u%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, value, AT_TAIL);
            }
            break;
        case AT_DOWN_CNT:   /*NO SET*/
        case AT_ADR:
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_OFF, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_ON, AT_TAIL);
            }
            break;
        case AT_TXP:
            /*Format = FORMAT_32_D_PARAM;*/
            if (Marker == SET_MARKER)
            {
                value =  *(uint32_t *)ptr;
                len = AT_VPRINTF("%s%s%s%u%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, value, AT_TAIL);
            }
            break;
        case AT_DR:
            /*Format = FORMAT_32_D_PARAM;*/
            if (Marker == SET_MARKER)
            {
                value =  *(uint32_t *)ptr;
                len = AT_VPRINTF("%s%s%s%u%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, value, AT_TAIL);
            }
            break;
        case AT_CH:
        case AT_DUTY:
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_COMMA, AT_OFF, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_COMMA, AT_ON, AT_TAIL);
            }
            break;
        case AT_RX2FQ:
        case AT_RX2DR:
        case AT_RX1DL:
        case AT_RX2DL:
        case AT_JN1DL:
        case AT_JN2DL:
            /*Format = FORMAT_32_D_PARAM;*/
            if (Marker == SET_MARKER)
            {
                value =  *(uint32_t *)ptr;
                len = AT_VPRINTF("%s%s%s%u%s", AT_HEADER, CmdTab[Cmd], AT_COMMA, value, AT_TAIL);
            }
            break;
        case AT_REGION:
            /*Format = FORMAT_8_D_PARAM;*/
            if (Marker == SET_MARKER)
            {
                value_8 =  *(uint8_t *)ptr;
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, band_str[value_8].info, AT_TAIL);
            }
            break;
        case AT_VER:	/*NO SET supported*/
        case AT_SNR:	/*NO SET supported*/
        case AT_RSSI:  /*NO SET supported*/
        case AT_BAT:   /*NO SET supported*/
        case AT_MC:
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_OFF, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_ON, AT_TAIL);
            }
            break;
        case AT_MC_DADDR:
            /*Format = FORMAT_32_02X_PARAM;*/
            value =  *(uint32_t *)ptr;
            if (Marker == SET_MARKER)
            {
                len = AT_VPRINTF("%s%s%s%02x%02x%02x%02x%s", AT_HEADER, CmdTab[Cmd], AT_COMMA,
                                 (unsigned)((unsigned char *)(&value))[3],
                                 (unsigned)((unsigned char *)(&value))[2],
                                 (unsigned)((unsigned char *)(&value))[1],
                                 (unsigned)((unsigned char *)(&value))[0], AT_TAIL);
            }
            break;
        case AT_MC_NSKEY:
        case AT_MC_ASKEY:
            /*Format = FORMAT_16_02X_PARAM;*/
            PtrValue = (uint8_t *) ptr;
            len = AT_VPRINTF("%s%s%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s",
                             AT_HEADER, CmdTab[Cmd], AT_COMMA,
                             PtrValue[0], AT_COLON, PtrValue[1], AT_COLON,
                             PtrValue[2], AT_COLON, PtrValue[3], AT_COLON,
                             PtrValue[4], AT_COLON, PtrValue[5], AT_COLON,
                             PtrValue[6], AT_COLON, PtrValue[7], AT_COLON,
                             PtrValue[8], AT_COLON, PtrValue[9], AT_COLON,
                             PtrValue[10], AT_COLON, PtrValue[11], AT_COLON,
                             PtrValue[12], AT_COLON, PtrValue[13], AT_COLON,
                             PtrValue[14], AT_COLON, PtrValue[15], AT_TAIL);
            break;
        case AT_MC_CNT:
            /*Format = FORMAT_32_D_PARAM;*/
            if (Marker == SET_MARKER)
            {
                value =  *(uint32_t *)ptr;
                len = AT_VPRINTF("%s%s%s%u%s", AT_HEADER, CmdTab[Cmd], AT_COMMA, value, AT_TAIL);
            }
            break;
        case AT_RESET:  /*NO SET supported*/
        case AT_FACTORY:   /*NO SET supported*/
        case AT_TEST:
            /*Format = FORMAT_8_D_PARAM;*/
            value_8 =  *(uint8_t *)ptr;
            if (value_8 == 0)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_OFF, AT_TAIL);
            }
            else if (value_8 == 1)
            {
                len = AT_VPRINTF("%s%s%s%s%s", AT_HEADER, CmdTab[Cmd], AT_SET_MARKER, AT_ON, AT_TAIL);
            }
            break;
        case AT_TEST_TXLR:
        case AT_TEST_RXLR:
        case AT_TEST_CFG:
        case AT_TEST_SCAN:
        case AT_TEST_PIO0:
        case AT_TEST_PIO1:
        case AT_TEST_BLE:
            break;
        default:
            len = AT_VPRINTF("%s%s%s", AT_HEADER, CmdTab[Cmd], AT_TAIL);
            DBG_PRINTF("format not yet supported \n\r");
            break;
        } /*end switch(cmd)*/
    }
    else   /*GTR AT CMD*/
	{
		DBG_PRINTF("---CTRL_MARKER--->at_cmd_format : %s \n",CmdTab[Cmd]);
        len = AT_VPRINTF("%s%s%s", AT_HEADER, CmdTab[Cmd], AT_TAIL);
	}
    return len;
}

/******************************************************************************
* @brief This function sends an AT cmd to the slave device
* @param len: length of the AT cmd to be sent
* @retval HAL return code
******************************************************************************/
static HAL_StatusTypeDef at_cmd_send(uint16_t len)
{
    HAL_StatusTypeDef RetCode;

    /*transmit the command from master to slave*/
    RetCode = Usart2SendData_DMA((uint8_t *)LoRa_AT_Cmd_Buff, len);
	at_printf_send((uint8_t *)LoRa_AT_Cmd_Buff, len);
//  RetCode = HAL_UART_Transmit(&huart1, (uint8_t *)LoRa_AT_Cmd_Buff, len, 5000);
    return (RetCode);
}

/******************************************************************************
* @brief This function sends an AT cmd to debug in PC
* @param len: length of the AT cmd to be sent
* @retval HAL return code
******************************************************************************/
HAL_StatusTypeDef at_printf_send(uint8_t *buf, uint16_t len)
{
    HAL_StatusTypeDef RetCode;

    /*transmit the command from master to debug*/
//    RetCode = Usart1SendData_DMA((uint8_t *)buf, len);
//  RetCode = HAL_UART_Transmit(&huart2, (uint8_t *)buf, len, 5000);
    return (RetCode);
}

/******************************************************************************
* @brief This function receives response from the slave device
* @param viod
* @retval LoRa return code
******************************************************************************/
atctl_ret_t at_cmd_receive_evt(void)
{
//    uint8_t  ResponseComplete = 0;

    atctl_ret_t RetCode = ATCTL_RET_CMD_OK;
//    uint32_t time_ms = 0;
    /*cleanup the response buffer*/
    memset(response, 0x00, sizeof(response));

    record_num = 0;

    return RetCode;
}

/******************************************************************************
* @brief This function receives response from the slave device and parse
* @param Cmd: command type
* @param *dt: atctl_data_t type
* @retval LoRa return code
******************************************************************************/
atctl_ret_t at_cmd_receive(ATCmd_t Cmd, atctl_data_t *dt,uint16_t response_timeout)
{
    uint8_t  ResponseComplete = 0;
//    uint16_t i = 0;
    uint16_t time_cnt = 0;

    atctl_ret_t RetCode = ATCTL_RET_IDLE;
	atctl_sta = ATCTL_RX_HEAD;
	atctl_rx_wr_index = 0;
    /*cleanup the response buffer*/
    memset(atctl_rx_buf, 0x00, sizeof(atctl_rx_buf));
	memset(response, 0x00, sizeof(response));
//    LOG_LUO("----->at_cmd_receive :%d \r\n", response_timeout);
	while(!ResponseComplete)
	{
		delay_ms(10);
        time_cnt++;
		if(time_cnt>response_timeout)
		{
		   time_cnt=0;
		   ResponseComplete = 1;
	       break;
		}
//        LOG_LUO(":%d %d \r\n", time_cnt,UsartType2.receive_flag);
//		else time_cnt++;
		if(UsartType2.receive_flag)
		{
			UsartType2.receive_flag = 0;
			atctl_rx_wr_index = UsartType2.rx_len; 
			LOG_LUO("UsartType2.rx_len = %d \r\n",UsartType2.rx_len);
			memcpy(atctl_rx_buf, &UsartType2.usartDMA_rxBuf, atctl_rx_wr_index-2);
			UsartType2.rx_len=0;
			delay_ms(5);
//			LOG_LUO("111\r\n");
			memset(UsartType2.usartDMA_rxBuf, 0x00, sizeof(UsartType2.usartDMA_rxBuf));
//			LOG_LUO("222\r\n");
			Usart1SendData_DMA((uint8_t *)atctl_rx_buf, atctl_rx_wr_index);
			delay_ms(5);
			ResponseComplete = 1;
			atctl_sta = ATCTL_PARSE_DONE;
			 LOG_LUO("Usart2RecvData_DMA:%s  %d  \r\n", atctl_rx_buf, atctl_rx_wr_index);
			RetCode = atctl_parse(atctl_rx_buf, atctl_rx_wr_index, dt);
			break;
		}
	}
//    if (atctl_sta == ATCTL_PARSE_DONE)
//    {	
//		memcpy(response, atctl_rx_buf, atctl_rx_wr_index);		
//        Usart1SendData_DMA((uint8_t *)response, atctl_rx_wr_index);
//        
//        LOG_LUO("Usart2RecvData_DMA:  %s  %d  %d \r\n", response, atctl_rx_wr_index,RetCode);
//    }
	LOG_LUO("----->RetCode :%d \r\n", RetCode);
    return (RetCode);
}


/******************************************************************************
* @brief format the AT frame to be sent to the modem (slave)
* @param pointer to the format string
* @retval len of the string to be sent
******************************************************************************/
uint16_t at_cmd_vprintf(const char *format, ...)
{
    va_list args;
    uint16_t len;

    va_start(args, format);

    len = tiny_vsnprintf_like(LoRa_AT_Cmd_Buff + Offset, sizeof(LoRa_AT_Cmd_Buff) - Offset, format, args);

    va_end(args);

    return len;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
