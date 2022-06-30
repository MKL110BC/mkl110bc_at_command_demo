/**
  ******************************************************************************
  * @file    atcmd_modem.h
  * @author  MCD Application Team
  * @brief   Header for AT commands definition
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

#ifndef __ATCMD_MODEM_H__
#define __ATCMD_MODEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CMD_INDEX
#define AT_CMD_STRING
#define AT_ERROR_INDEX
#define AT_CMD_MARKER

#ifdef AT_CMD_INDEX
/*
 * AT Command Index . In direct relationship with "CmdTab" static array
 * in atcmd.c file
 */
typedef enum
{
    AT,	              /*OK*/
	AT_LADDR,
	AT_NAME,
	AT_ADVI,
	AT_UUID,
	AT_ADVD,
	AT_SCAN_STD,
	AT_SCAN_NAME,
	AT_SCAN_RSSI,	
	AT_SLEEP,	
	AT_BAUD,
    AT_ATE,	
	AT_DEVEUI,        /*AT+DEVEUI,*/
	AT_APPEUI,
	AT_APPKEY,
	AT_NWKSKEY,
    AT_APPSKEY,
	AT_DADDR,       /*AT+DEVADDR,*/
	AT_JOIN_MODE,
	AT_CLASS,
	AT_JOINING,
	AT_JOIN_STD,
	AT_AUTO_JOIN,
    AT_NWKID,       /*KO*/
	AT_TXLEN,
	AT_SENDB,
	AT_SEND,
    AT_RECVB,       /*KO*/
    AT_RECV,        /*KO*/
	AT_CFM,         /*include in the send command*/
	AT_UP_CNT,
	AT_DOWN_CNT,	
    AT_ADR,
    AT_TXP,         /*TX power*/
	AT_DR,
	AT_CH,
	AT_DUTY,
	AT_RX2FQ,       /*KO, include in the RX2 command*/
	AT_RX2DR,       /*KO, include in the RX2 command*/
	AT_RX1DL,
    AT_RX2DL,
	AT_JN1DL,
    AT_JN2DL,
	AT_REGION,	
	AT_VER,
	AT_SNR,         /*KO, include in the received data frame*/
	AT_RSSI,        /*KO, include in the received data frame*/
	AT_BAT,
	AT_MC,
	AT_MC_DADDR,
	AT_MC_NSKEY,
	AT_MC_ASKEY,
	AT_MC_CNT,
    AT_RESET,
    AT_FACTORY,
	AT_TEST,
	AT_TEST_TXLR,
	AT_TEST_RXLR,
	AT_TEST_CFG,
	AT_TEST_SCAN,
	AT_TEST_PIO0,
	AT_TEST_PIO1,
	AT_TEST_BLE,
//	
//    AT_NJS,         /*KO*/
//    AT_DCS,         /*DutyCycle settings*/
//    AT_PNM,         /*Public network mode*/
//    AT_NJM,
//    AT_FCU,         /*KO*/
//    AT_FCD,         /*KO*/
//    AT_CFS,         /*KO*/
//    AT_WDCT,        /*KO*/
//    AT_DEFMODE,      /*KO*/
//    AT_WDG,
//    AT_PS,          /*KO*/
//    AT_RX2,         /*Combine AT_RX2FQ with AT_RX2DR*/
//    AT_FDEFAULT,
//    AT_CMSG,
//    AT_CMSGHEX,
//    AT_PORT,
//    AT_VDD,
    AT_MAX,         /* Last command for searching, not real command */
} ATCmd_t;

#endif

#ifdef AT_CMD_STRING
/*list of AT string cmd supported by the RisingHF LoRa modem*/
static char *CmdTab[]
#ifdef __GNUC__
__attribute__((unused))
#endif
=
{
    "",
    "+LADDR",
    "+NAME",              /* +BAND country band */
    "+ADVI",            /* +JOIN*/
    "+UUID",             /* KO */
    "+ADVD",       /* +ID device ID*/
    "+SCAN_STD",      /* +ID device Address*/
    "+SCAN_NAME",      /* +KEY application key*/
    "+SCAN_RSSI",      /* +KEY Network session Key*/
    "+SLEEP",     /* +KEY application Session key*/
    "+BAUD",       /* +ID application Identifier*/
    "+ATE",             /* +ADR adaptive data rate*/
    "+DEVEUI",           /* +POWER transmit Tx power*/
    "+APPEUI",              /* +DR data rate*/
    "+APPKEY",           /* +LW duty cycle settings*/
    "+NWKSKEY",          /* +LW public network or private network*/
    "+APPSKEY",           /* KO */
    "+DEVADDR",           /* KO */
    "+JOIN_MODE",       /* +DELAY Delay of the Rx1 window*/
    "+CLASS",       /* +DELAY delay of the Rx2 window*/
    "+JOINING",      /* +DELAY Join delay on Rx Wind 1*/
    "+JOIN_STD",      /* +DELAY Join delay on Rx Wind 2*/
    "+AUTO_JOIN",            /* +MODE Nwk Join Mode*/
    "+NWKID",           /* KO */
    "+TX_LEN",             /* KO */
    "+SENDB",             /* KO */
    "+SEND",           /* +CLASS LoRa class*/
    "+RECVB",          /* +MSGHEX send data binary format*/
    "+RECV",             /* +MSG send data in raw format*/
    "+CONFIRM",             /* KO, include in the data frame*/
    "+UP_CNT",             /* KO */
    "+DOWN_CNT",          /* +LW  battery level*/
    "+ADR",            /* KO */
    "+TX_POWER",             /* KO */
    "+DR",             /* Firmware version */
    "+CH",            /* KO */
    "+DUTYCYCLE",         /* KO */
    "+RX2FQ",
    "+RX2DR",                /* KO */
    "+RX1DL",        /* for enter immediatly in sleep mode (slave) following the power control setting*/
    "+RX2DL",              /* KO*/
    "+JN_RX1DL",          /* +RXWIN2, RX2 windouw */
    "+JN_RX2DL",        /* Set the module default status */
    "+REGION",            /* Send the confirmed data frame */
    "+VER",         /* Send the confirmed HEX data frame  */
    "+SNR",            /* Set the port in sending data */
    "+LW=RSSI",
    "+BAT",              /* Get the vdd */	
	"+MC",	
	"+MC_DEVADDR",	
	"+MC_NWKSKEY",	
	"+MC_APPSKEY",	
	"+MC_CNT",	
	"+RESET"	
	"+FACTORY",	
	"+TEST",	
	"+TEST_TXLORA",	
	"+TEST_RXLORA",	
	"+TEST_CONF",	
	"+TEST_SCAN",	
	"TEST_PIO0",	
	"TEST_PIO1"	
	"+TEST_BLE_CON"	
};

#endif

#ifdef AT_ERROR_INDEX
/*
 * AT Command Index errors. In direct relationship with ATE_RetCode static array
 * in atcmd.c file
 */
typedef enum
{
    ATCTL_RET_TIMEOUT = -3,         /* RX data timeout */
    ATCTL_RET_ERR = -2,             /* Unknown command */
    ATCTL_RET_CMD_ERR = -1,         /* Get command +CMD: ERROR(x) */
    ATCTL_RET_IDLE = 0,
    ATCTL_RET_CMD_OK,               /* Command is OK, but can't parse */
    ATCTL_RET_CMD_AT,
    ATCTL_RET_CMD_ID,
    ATCTL_RET_CMD_VER,
    ATCTL_RET_CMD_RTC,
    ATCTL_RET_CMD_VDD,
    ATCTL_RET_CMD_MSG,
    ATCTL_RET_CMD_JOIN,
    ATCTL_RET_CMD_EEPROM,
    ATCTL_RET_CMD_DR,
    ATCTL_RET_CMD_MODE,
    ATCTL_RET_CMD_LW,
    ATCTL_RET_CMD_DELAY,
} atctl_ret_t, ATEerror_t;

#endif

#ifdef AT_CMD_MARKER
/* Marker to design the AT command string*/
#define AT_HEADER       "AT"
#define AT_SET_MARKER   "="
#define AT_GET_MARKER   "=?"
#define AT_NULL_MARKER  ""
#define AT_COLON        ":"
#define AT_COMMA        "="
#define AT_TAIL         "\r\n"
#define AT_SEPARATOR    ""
#define AT_FRAME_KEY  "%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx"
#define AT_FRAME_KEY_OFFSET  2
#define AT_SEND_MARKER  ": "

#define AT_ON           "ON"
#define AT_OFF          "OFF"

#define AT_LPAUTOOFF    "AUTOOFF"
#define AT_LPAUTOON     "AUTOON"

#define AT_ABP          "ABP"
#define AT_OTAA         "OTAA"

#define AT_CLASS_A      "A"
#define AT_CLASS_C      "C"

#endif




#ifdef __cplusplus
}
#endif

#endif /*__ATCMD_MODEM_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
