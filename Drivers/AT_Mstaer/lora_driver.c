/**
  ******************************************************************************
  * @file    Lora_driver.c
  * @author  MCD Application Team
  * @brief   LoRa module API
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "hw.h"
#include "lora_driver.h"
#include "atcmd_modem.h"
#include "Driver_IIC_SHT30.h"
/* External variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

char Test_Send_buf[20]={0x32,0x3a,0x0c,0x3a,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};
int16_t Sensor_temp;
uint16_t Sensor_hum;

Protocol_data User_Protocol_data;
/* Private variable ----------------------------------------------------------*/
//#ifdef USE_LRWAN_NS1
#define FW_VER      "VERSION: v0.1.8 20th November,2017\r\n"
atctl_data_t dt;
extern uint32_t record_num;
extern char response[DATA_RX_MAX_BUFF_SIZE];
//sSendDataString_t SendDataBinary;
//#endif
//#if USE_MDM32L07X01 | USE_LRWAN_NS1
//static TimerEvent_t JoinStatusDelayTimer; /*timer to handle the join status delay*/
//#endif
//static __IO uint8_t JoinTimeOutFlag = 0;  /*to indicate if timeout raised on Join procedure*/


///* Object definition for data to be sent to loRa application server*/
static char DataBinaryBuff[64]={0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};

static sSendDataBinary_t SendDataBinary = { DataBinaryBuff,1,12,0};


//static LoRaDriverCallback_t *LoraDriverCallbacks;
static LoRaDriverParam_t LoraDriverParam;

static uint8_t PtrValueFromDevice[32]  ;  /*to get back the device address in */
/*11:22:33:44 format before*/
/*to be translated into uint32_t type*/

static uint8_t PtrTempValueFromDevice[16]  ;  /*to get back the device address in */
/*11:22:33;44:55:66:77 format before*/
/*to be translated into and array of */
/*8 uint8_t*/

static uint8_t PtrTempValueFromDeviceKey[47]  ;  /* in relation with the response size*/

//#ifdef USE_I_NUCLEO_LRWAN1
//static uint8_t PtrDataFromNetwork[64]  ;      /* Payload size max returned by USI modem*/
//#endif

//static TimerEvent_t NextSensorMeasureTimer; /*timer to handle next sensor measure*/

//#ifdef USE_I_NUCLEO_LRWAN1
//static void Lora_OnJoinProtectTimerEvt(void *context);
//static TimerEvent_t JoinProtectTimer;  /* timer to protect the  USI Join request*/
//#endif

static DeviceState_t DeviceState = DEVICE_INIT ;

static DeviceState_t DeviceSubState = DEVICE_INIT ; /* to manage the join transition*/

//static TimerEvent_t DemoLedTimer;                  /*timer to handle Demo Led*/

//__IO uint8_t SensorTimerWakeup = 0;

/* Private define ------------------------------------------------------------*/

/******************************************************************************/
/*                    To put USI modem in sleep mode                          */
/*  From USI FW V2.6, modem sleep mode is only supported on ABP Join mode     */
/*  From USI FW V3.0, modem sleep mode is supported for ABP and OTAA Join mode*/
/******************************************************************************/

/*put in comment MODEM_IN_SLEEP_MODE defined in "hw_conf.h" */
/*if we do not want to have the USI MCU modem in sleep mode*/

//#if defined (USE_I_NUCLEO_LRWAN1) && defined (MODEM_IN_SLEEP_MODE)
//static sPowerCtrlSet_t   PowerCtrlSettings = {0, 1, 0};
//#endif

//#if defined (USE_I_NUCLEO_LRWAN1)
//#define USI_FW_V26               26
//#define USI_FW_V30               30
//#define STRING_USI_FW_V26       "2.6"
//#define STRING_USI_FW_V30       "3.0"
//uint8_t FWVersion = USI_FW_V30;             /*we fixe the FWVersion to 3.0 value*/
//#endif                                      /*User has to remove this fixe value*/
/*if he want to support Version < 3.0*/
/*Artifice has been decide with the*/
/*introduction of LoRaWAN PAck*/
/*From since, we expect that USI modem*/
/*will be manufactured with Version > 4.0*/

void Lora_Ctx_Init(void);


static void print02x(uint8_t *pt, uint8_t ptlen);
/* Private functions ---------------------------------------------------------*/

//#if USE_MDM32L07X01 | USE_LRWAN_NS1
//static void Lora_OnJoinStatusDelayTimerEvt(void *context);
//#endif

static ATEerror_t Lora_Send(void);

//static void Lora_OnNextSensorMeasureTimerEvt(void *context);

//static void Lora_OnLedTimerEvent(void *context);         /*timer function to handle Demo Led*/


/**************************************************************
 * @brief  Check if the LoRa module is working
 * @param  void
 * @retval status of the module (ready or not ready)
**************************************************************/
RetCode_t Lora_Init(void)
{
    ATEerror_t Status;

    /*check if the module is working*/
    Status = Modem_AT_Cmd(AT_CTRL, AT, NULL, ATCTL_RX_TIMEOUT);
    if (Status == ATCTL_RET_CMD_OK)
        /* received Ok from module*/
        return (MODULE_READY);
    else
    {
        return (MODULE_NO_READY);
    }
}


/**************************************************************
 * @brief  reset of the LoRa module
 * @param  void
 * @retval void
**************************************************************/
void Lora_Reset(void)
{
    /*reset the lora module*/
    Modem_AT_Cmd(AT_CTRL, AT_RESET, NULL, ATCTL_RX_TIMEOUT);

}

/**************************************************************
 * @brief  Do a request to establish a LoRa Connection with the gateway
 * @param  Mode: by OTAA or by ABP
 * @retval LoRA return code
 * @Nota param is relevant for USI WM_SG_SM_XX modem/LRWAN_NS1 - Not relevant for MDM32L07X01 modem
**************************************************************/
ATEerror_t Lora_Join(uint8_t Mode)
{
    ATEerror_t Status = ATCTL_RET_IDLE;
//    uint32_t current_st = 0;
    switch (Mode)
    {
    case ABP_JOIN_MODE:
        return ATCTL_RET_CMD_JOIN;

    case OTAA_JOIN_MODE:
        /*request a join connection*/
        Status = Modem_AT_Cmd(AT_CTRL, AT_JOINING, NULL, ATCTL_JOIN_TIMEOUT);
        break;
    default:
        break;
    }
    return (Status);
}

/**************************************************************
* @brief  Close ATE.
* @param void
* @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetATE(uint8_t onoff)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_ATE, &onoff, ATCTL_RX_TIMEOUT);

    return (Status);
}
/**************************************************************
* @brief  Do a request to reset LoRaWAN AT modem to factory default configuration.
* @param void
* @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDefault(void)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_CTRL, AT_FACTORY, NULL, ATCTL_RX_TIMEOUT);

    return (Status);
}
/**************************************************************
 * @brief  Do a request to set the Network join Mode
 * @param  Mode : OTAA, ABP
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetJoinMode(uint8_t Mode)
{
    ATEerror_t Status;

    /*Set the nwk Join mode */
    Status = Modem_AT_Cmd(AT_SET, AT_JOIN_MODE, &Mode, ATCTL_RX_TIMEOUT);

    return (Status);
}


/**************************************************************
 * @brief  Do a request to get the Network join Mode
 * @param  pointer to the Join mode out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetJoinMode(uint8_t *Mode)
{
    ATEerror_t Status;

    /*Get the nwk Join mode */
    Status = Modem_AT_Cmd(AT_GET, AT_JOIN_MODE, Mode, ATCTL_RX_TIMEOUT);

    return (Status);
}

/********* MiB MananagementLora **************/


/**************************************************************
 * @brief  key configuration
 * @param  KeyType : APPKEY, NWKSKE, APPSKEY
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetKey(ATCmd_t KeyType, uint8_t *PtrKey)
{
    ATEerror_t Status;

    /*Set a key type to the LoRa device*/
    Status = Modem_AT_Cmd(AT_SET, KeyType, PtrKey, ATCTL_RX_TIMEOUT);
    return (Status);
}

/**************************************************************
 * @brief  Request the key type configuration
 * @param  KeyType : APPKEY, NWKSKE, APPSKEY
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetKey(ATCmd_t KeyType, uint8_t *PtrKey)
{
    ATEerror_t Status;

    /*get the key type from the LoRa device*/
    Status = Modem_AT_Cmd(AT_GET, KeyType, PtrTempValueFromDeviceKey, ATCTL_RX_TIMEOUT);
    if (Status == 0)
    {
    AT_VSSCANF((char *)PtrTempValueFromDeviceKey + AT_FRAME_KEY_OFFSET, AT_FRAME_KEY,
               &PtrKey[0], &PtrKey[1], &PtrKey[2], &PtrKey[3],
               &PtrKey[4], &PtrKey[5], &PtrKey[6], &PtrKey[7],
               &PtrKey[8], &PtrKey[9], &PtrKey[10], &PtrKey[11],
               &PtrKey[12], &PtrKey[13], &PtrKey[14], &PtrKey[15]);
        return (Status);
    }
    else
    {
        return (Status);
    }
}

/**************************************************************
 * @brief  Set the Application Identifier
 * @param  pointer to the APPEUI in value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetAppID(uint8_t *PtrAppID)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_APPEUI, PtrAppID, ATCTL_RX_TIMEOUT);

    return (Status);
}

/**************************************************************
 * @brief  Request the Application Identifier
 * @param  pointer to the APPEUI out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetAppID(uint8_t *AppEui)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_APPEUI, PtrTempValueFromDevice, ATCTL_RX_TIMEOUT);
    uint8_t i = 0;
    if (Status == ATCTL_RET_CMD_ID)
    {
        for (i = 0; i < 8; i ++)
        {
            AppEui[i] = dt.id.appeui[i];
        }
        return (Status);
    }
    else
    {
        return (Status);
    }
}

/**************************************************************
 * @brief  Set the device extended universal indentifier
 * @param  pointer to the DEUI in value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetDeviceID(uint8_t *PtrDeviceID)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_DEVEUI, PtrDeviceID, ATCTL_RX_TIMEOUT);

    return (Status);
}
/**************************************************************
 * @brief  Request the device extended universal indentifier
 * @param  pointer to the DEUI out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetDeviceID(uint8_t *PtrDeviceID)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_DEVEUI, PtrTempValueFromDevice, ATCTL_RX_TIMEOUT);

    uint8_t i = 0;
    if (Status == ATCTL_RET_CMD_ID)
    {
        for (i = 0; i < 8; i ++)
        {
            PtrDeviceID[i] = dt.id.deveui[i];
        }
        return (Status);
    }
    else
    {
        return (Status);
    }
}


/**************************************************************
 * @brief  Set the device address
 * @param  pointer to the DADDR in value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetDeviceAddress(uint32_t DeviceAddr)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_DADDR, &DeviceAddr, ATCTL_RX_TIMEOUT);

    return (Status);
}
/**************************************************************
 * @brief  Request the device address
 * @param  pointer to the DADDR out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetDeviceAddress(uint8_t *Value)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_DADDR, PtrValueFromDevice, ATCTL_RX_TIMEOUT);
    uint8_t i = 0;
    if (Status == ATCTL_RET_CMD_ID)
    {
        for (i = 0; i < 4; i ++)
        {
            Value[i] = dt.id.devaddr[i];
        }
        return (Status);
    }
    else
    {
        return (Status);
    }
}

/**************************************************************
 * @brief  Set the NetWork ID
 * @param  NWKID in value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetNetworkID(uint32_t NetworkID)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_NWKID, &NetworkID, ATCTL_RX_TIMEOUT);
    return (Status);
}


/**************************************************************
 * @brief  Request the network ID
 * @param  pointer to the NWKID out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetNetworkID(uint32_t *Value)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_NWKID, PtrValueFromDevice, ATCTL_RX_TIMEOUT);
    if (Status == 0)
    {
//    AT_VSSCANF((char *)PtrValueFromDevice, "%hhx:%hhx:%hhx:%hhx",
//               &((unsigned char *)(Value))[0],
//               &((unsigned char *)(Value))[1],
//               &((unsigned char *)(Value))[2],
//               &((unsigned char *)(Value))[3]);
        return (Status);
    }
    else
    {
        return (Status);
    }
}

/*************   Network Management *****************/

/**************************************************************
 * @brief  Do a request to set the adaptive data rate
 * @param  ADR in value 0(off) / 1(on)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetAdaptiveDataRate(uint8_t Rate)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_ADR, &Rate, ATCTL_RX_TIMEOUT);

    return (Status);
}

/**************************************************************
 * @brief  Do a request to get the adaptive data rate
 * @param  pointer to ADR out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetAdaptiveDataRate(uint8_t *Rate)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_ADR, Rate, ATCTL_RX_TIMEOUT);
    return (Status);;
}

/**************************************************************
 * @brief  Do a request to set the LoRa Class
 * @param  CLASS in value [0,1,2]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetClass(uint8_t Class)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_CLASS, &Class, ATCTL_RX_TIMEOUT);

    return (Status);
}

/**************************************************************
 * @brief  Do a request to get the LoRa class
 * @param  pointer to CLASS out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetClass(uint8_t *Class)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_CLASS, Class, ATCTL_RX_TIMEOUT);
    return (Status);
}

/**************************************************************
 * @brief  Do a request to set the duty cycle
 * @brief  only used in test mode
 * @param  DCS in value 0(disable) / 1(enable)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDutyCycle(uint8_t DutyCycle)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_DUTY, &DutyCycle, ATCTL_RX_TIMEOUT);

    return (Status);
}

/**************************************************************
 * @brief  Do a request to get the duty cycle
 * @brief  only used in test mode
 * @param  pointer to DCS out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetDutyCycle(uint8_t *DutyCycle)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_DUTY, DutyCycle, ATCTL_RX_TIMEOUT);
    return (Status);
}

/**************************************************************
 * @brief  Do a request to set the data Rate
 * @param  DR in value [0,1,2,3,4,5,6,7]
 *         If use USE_LRWAN_NS1, DR range in 0~15
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDataRate(uint8_t DataRate)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_DR, &DataRate, ATCTL_RX_TIMEOUT);

    return (Status);
}

/**************************************************************
 * @brief  Do a request to get the data Rate
 * @param  pointer to DR out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetDataRate(uint8_t *DataRate)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_DR, DataRate, ATCTL_RX_TIMEOUT);

    return (Status);
}
/**************************************************************
 * @brief  Do a request to set the frame counter
 * @param  FrameCounterType : FCD, FCU
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetFrameCounter(ATCmd_t FrameCounterType, uint32_t FrameCounternumber)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, FrameCounterType, &FrameCounternumber, ATCTL_RX_TIMEOUT);
    return (Status);

}

/**************************************************************
 * @brief  Request the frame counter number
 * @param  frameCounterType : FCD, FCU
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetFrameCounter(ATCmd_t FrameCounterType, uint32_t *FrameCounternumber)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, FrameCounterType, PtrValueFromDevice, ATCTL_RX_TIMEOUT);
    if (Status == 0)
    {
//    AT_VSSCANF((char *)PtrValueFromDevice, "%lu", FrameCounternumber);
        return (Status);
    }
    else
    {
        return (Status);
    }
}





/**************************************************************
 * @brief  Do a request to set the join accept delay between
 * @brief  the end of the Tx and the join Rx#n window
 * @param  RxWindowType : JN1DL, JN2DL
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetJoinDelayRxWind(ATCmd_t RxWindowType, uint32_t JoinDelayInMs)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, RxWindowType, &JoinDelayInMs, ATCTL_RX_TIMEOUT);
    return (Status);

}

/**************************************************************
 * @brief  Do a request to get the join accept delay between
 * @brief  the end of the Tx and the join Rx#n window
 * @param  RxWindowType : JN1DL, JN2DL
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetJoinDelayRxWind(ATCmd_t RxWindowType, uint32_t *JoinDelayInMs)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, RxWindowType, PtrValueFromDevice, ATCTL_RX_TIMEOUT);
    if (Status == 0)
    {
        AT_VSSCANF((char *)PtrValueFromDevice, "%lu", JoinDelayInMs);
        return (Status);
    }
    else
    {
        return (Status);
    }
}

/**************************************************************
 * @brief  Do a request to set the Public Network mode
 * @param  PNM in value 0(off) / 1(on)
 * @retval LoRa return code
**************************************************************/
//ATEerror_t Lora_SetPublicNetworkMode(uint8_t NetworkMode)
//{
//  ATEerror_t Status;

//  Status = Modem_AT_Cmd(AT_SET, AT_PNM, &NetworkMode);

//  return (Status);
//}
///**************************************************************
// * @brief  Do a request to get the Public Network mode
// * @param  pointer to PNM out value
// * @retval LoRa return code
//**************************************************************/
//ATEerror_t Lora_GetPublicNetworkMode(uint8_t *NetworkMode)
//{
//  ATEerror_t Status;

//  Status = Modem_AT_Cmd(AT_GET, AT_PNM, NetworkMode);
//  return (Status);
//}

/**************************************************************
 * @brief  Do a request to set the delay between the end of the Tx
 * @brief  the end of the Tx and the join Rx#n window
 * @param  RxWindowType : RX1DL, RX2DL
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_SetDelayRxWind(ATCmd_t RxWindowType, uint32_t RxDelayInMs)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, RxWindowType, &RxDelayInMs, ATCTL_RX_TIMEOUT);
    return (Status);

}

/**************************************************************
 * @brief  Do a request to get the delay between the end of the Tx
 * @brief  the end of the Tx and the join Rx#n window
 * @param  RxWindowType : RX1DL, RX2DL
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetDelayRxWind(ATCmd_t RxWindowType, uint32_t *RxDelayInMs)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, RxWindowType, PtrValueFromDevice, ATCTL_RX_TIMEOUT);
    if (Status == 0)
    {
        AT_VSSCANF((char *)PtrValueFromDevice, "%lu", RxDelayInMs);
        return (Status);
    }
    else
    {
        return (Status);
    }
}




/**************************************************************
 * @brief  Set the frequency of the Rx2 window
 * @param  pointer to the RX2FQ in value
 * @retval LoRa return code
************************************************************
**/
ATEerror_t LoRa_SetFreqRxWind2(uint32_t Rx2WindFrequency)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_RX2FQ, &Rx2WindFrequency, ATCTL_RX_TIMEOUT);
    return (Status);
}


/**************************************************************
 * @brief  Request the frequency of the Rx2 window
 * @param  pointer to the RX2FQ out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t LoRa_GetFreqRxWind2(uint32_t *Rx2WindFrequency)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_RX2FQ, PtrValueFromDevice, ATCTL_RX_TIMEOUT);
    if (Status == 0)
    {
        AT_VSSCANF((char *)PtrValueFromDevice, "%lu", Rx2WindFrequency);
        return (Status);
    }
    else
    {
        return (Status);
    }
}

/**************************************************************
 * @brief  Do a request to set the transmit Tx power
 * @param  TXP in value [0,1,2,3,4,5]
 *         If defined USE_LRWAN_NS1, TXP range in TX_POWER_6_DBM ~ TX_POWER_20_DBM
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetTxPower(uint8_t TransmitTxPower)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_TXP, &TransmitTxPower, ATCTL_RX_TIMEOUT);

    return (Status);
}
/**************************************************************
 * @brief  Do a request to get the transmit Tx Power
 * @param  pointer to TXP out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetTxPower(uint8_t *TransmitTxPower)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_TXP, TransmitTxPower, ATCTL_RX_TIMEOUT);
    return (Status);
}
/**************************************************************
 * @brief  Do a request to set the data Rate of Rx2 window
 * @param  RX2DR in value [0,1,2,3,4,5,6,7]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDataRateRxWind2(uint8_t DataRateRxWind2)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_RX2DR, &DataRateRxWind2, ATCTL_RX_TIMEOUT);
    return (Status);
}


/**************************************************************
 * @brief  Do a request to get the data Rate of Rx2 window
 * @param  pointer to RX2DR out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetDataRateRxWind2(uint8_t *DataRateRxWind2)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_RX2DR, DataRateRxWind2, ATCTL_RX_TIMEOUT);
    return (Status);
}

///**************************************************************
// * @brief  Do a request to set the band plan
// * @param  BandPlan
// * @retval LoRa return code
//**************************************************************/
//ATEerror_t Lora_SetDeviceBand(uint8_t DeviceBand)
//{
//  ATEerror_t Status;

//  Status = Modem_AT_Cmd(AT_SET, AT_REGION, &DeviceBand,ATCTL_RX_TIMEOUT);
//  return (Status);
//}

/**************************************************************
 * @brief  Do a request to set the data Rate of Rx2 window
 * @param  Freq: Frequency
 * @param  DRoSF: Spread factor value or date rate value
 * @Param  BW: Bandwidth
 * @note   When param BW is 0, the param DRoSF is date rate.
 * @note   When param BW is not 0, the param DRoSF is spread factor
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetRxWind2(float Freq)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_RX2FQ, &Freq, ATCTL_RX_TIMEOUT);

    return (Status);
}

///**************************************************************
//* @brief  Do a request to set port
//* @param  port
//* @retval LoRa return code
//**************************************************************/
//ATEerror_t Lora_SetPort(uint8_t port)
//{
//  ATEerror_t Status;
//  Status = Modem_AT_Cmd(AT_SET, AT_PORT, &port,ATCTL_RX_TIMEOUT);

//  return (Status);
//}

/**************************************************************
* @brief  Do a request to get the maximum payload length which is supported to send according to current data rate
* @param len point to the length out value
* @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetLwLEN(uint8_t *len)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_TXLEN, len, ATCTL_RX_TIMEOUT);

    if (Status == ATCTL_RET_CMD_LW)
    {
        *len = dt.dev_param.txlen;
    }

    return (Status);
}

/************ Data Path Management ***************/
/**************************************************************
 * @brief  Do a request to get the battery level of the modem (slave)
 * @param  BAT in value
 *              0:    battery connected to external power supply
 *       [1..254]:    1 being at minimum and 254 being at maximum
 *             255:    not able to measure the battery level
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetBatLevel(uint32_t *BatLevel)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_BAT, BatLevel, ATCTL_RX_TIMEOUT);
    if (Status == ATCTL_RET_CMD_VDD)
    {
        if (dt.vdd > 3.3)
        {
            *BatLevel = 254;
        }
        else if (dt.vdd < 1.8)
        {
            *BatLevel = 1;
        }
        else
        {
            *BatLevel = (uint32_t)((dt.vdd - 1.8) / 1.5 * 254);
        }
        return (Status);
    }
    else
    {
        return (Status);
    }
}

/**************************************************************
 * @brief  Send text data to a giving prot number
 * @note   RisingHF module need to set port number or use the default port number 8
 * @param  SEND in value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SendData(sSendDataString_t *PtrStructData)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_SEND, PtrStructData, ATCTL_RX_TIMEOUT);

    Status = Lora_Send();

    return (Status);
}

/**************************************************************
 * @brief  Send binary data to a giving port number
 * @param  SENDB in value ( USE_MDM32L07X01) SEND in value ( USE_I_NUCLEO_LRWAN1)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SendDataBin(sSendDataBinary_t *PtrStructData)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_SENDB, PtrStructData, ATCTL_RX_TIMEOUT);
    Status = Lora_Send();

    return (Status);
}

/**************************************************************
* @brief  Handler after the Send_msg command executed
* @param  void
* @retval LoRA return code
**************************************************************/
static ATEerror_t Lora_Send(void)
{
//    uint32_t current_st = 0;
//    record_num = 0;
    ATEerror_t Status = ATCTL_RET_IDLE;
//    atctl_data_t dttmp;

    return Status;
}

/**************************************************************
 * @brief  Do a request to get rx data
 * @brief  received by the Slave
 * @note   Used in CLASS C, and the rx data is stored in "dt.msg"
 * @param  void
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_ReceiveData(void)
{
    ATEerror_t Status = ATCTL_RET_CMD_ERR;
//    atctl_data_t dttmp;

    return Status;
}

/**************************************************************
 * @brief  Do a request to get the last data (in binary format)
 * @brief  received by the Slave
 * @param  pointer to RECVB out value
 * @retval LoRa return code
**************************************************************/
//ATEerror_t Lora_ReceivedDataBin(sReceivedDataBinary_t *PtrStructData)
//{
//  ATEerror_t Status;
//  uint8_t sizebuf;
//  uint8_t i;
//  char TempBuf[3] = {0};


//  Status = Modem_AT_Cmd(AT_GET, AT_RECVB, PtrValueFromDevice);
//  if (Status == 0)
//  {
//    AT_VSSCANF((char *)PtrValueFromDevice, "%d", &(PtrStructData->Port));

//    if ((sizebuf = strlen((char *)&PtrValueFromDevice[3])) > DATA_RX_MAX_BUFF_SIZE)   /*shrink the Rx buffer to MAX size*/
//    {
//      sizebuf = DATA_RX_MAX_BUFF_SIZE;
//    }

//    for (i = 0; i <= ((sizebuf / 2) - 1); i++)
//    {
//      TempBuf[0] = PtrValueFromDevice[3 + (i * 2)];
//      TempBuf[1] = PtrValueFromDevice[3 + (i * 2) + 1];
//      AT_VSSCANF(TempBuf, "%hhx",  &PtrStructData->Buffer[i]);
//    }
//    PtrStructData->DataSize = i;
//    return (Status);
//  }
//  else
//  {
//    return (Status);
//  }
//}


/**************************************************************
 * @brief  Do a request to set the confirmation mode
 * @param  CFM in value 0(unconfirm) / 1(confirm)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetSendMsgConfirm(uint8_t ConfirmMode)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_CFM, &ConfirmMode, ATCTL_RX_TIMEOUT);
    return (Status);
}


/**************************************************************
 * @brief  Do a request to get the confirmation mode
 * @param  pointer to CFM out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetSendMsgConfirm(uint8_t *ConfirmMode)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_CFM, ConfirmMode, ATCTL_RX_TIMEOUT);
    return (Status);
}


/**************************************************************
 * @brief  Do a request to get the msg status of the last send cmd
 * @param  CFS in value 0(unconfirm) / 1(confirm)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetSendMsgStatus(uint8_t *MsgStatus)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_CFM, MsgStatus, ATCTL_RX_TIMEOUT);
    return (Status);
}

/**************************************************************
 * @brief  Do a request to get the RSSI of the last received packet
 * @param  RSSI in value [in dbm]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetRSSI(int32_t *SigStrengthInd)
{
    ATEerror_t Status;


    Status = Modem_AT_Cmd(AT_GET, AT_RSSI, PtrValueFromDevice, ATCTL_RX_TIMEOUT);
    if (Status == 0)
    {
        AT_VSSCANF((char *)PtrValueFromDevice, "%ld", SigStrengthInd);
        return (Status);
    }
    else
    {
        return (Status);
    }
}


/**************************************************************
 * @brief  Do a request to get the SNR of the last received packet
 * @param  SNR in value [in db]
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetSNR(uint32_t *SigToNoice)
{
    ATEerror_t Status;


    Status = Modem_AT_Cmd(AT_GET, AT_SNR, PtrValueFromDevice, ATCTL_RX_TIMEOUT);
    if (Status == 0)
    {
        AT_VSSCANF((char *)PtrValueFromDevice, "%ld", SigToNoice);
        return (Status);
    }
    else
    {
        return (Status);
    }
}

/**************************************************************
 * @brief  Do a request to get the LoRa stack version of the modem (slave)
 * @param  pointer to VER out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetVersion(uint8_t *PtrVersion)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_VER, PtrValueFromDevice, ATCTL_RX_TIMEOUT);
    if (Status == 0)
    {

    }
    else
    {
        return (Status);
    }
	return (Status);
}

/**************************************************************
 * @brief  Do a request to set the country band code for LoRaWAN
 * @brief  Need to write to DCT and Reset module to enable this setting
 * @param  BAND in value 0(EU-868 Band) / 1(US-Band)
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SetDeviceBand(uint8_t DeviceBand)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_REGION, &DeviceBand, ATCTL_RX_TIMEOUT);
    return (Status);
}


/**************************************************************
 * @brief  Do a request to get the country band code for LoRaWAN
 * @brief  only used in test mode
 * @param  pointer to BAND out value
 * @retval LoRa return code
**************************************************************/
ATEerror_t Lora_GetDeviceBand(uint8_t *DeviceBand)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_GET, AT_REGION, DeviceBand, ATCTL_RX_TIMEOUT);
    return (Status);
}


/**************************************************************
* @brief  Do a request to make module enter sleep mode with ultra-low power consumption
* @param IsEnterSleepMode: The param can be "ATUOON"/"AUTOOFF"
*               AUTOON: Enter extremely low power mode
*               AUTOOFF: Exit extremely low power mode
* @retval LoRa return code
**************************************************************/
ATEerror_t Lora_SleepMode(uint8_t IsEnterSleepMode)
{
    ATEerror_t Status;

    Status = Modem_AT_Cmd(AT_SET, AT_SLEEP, &IsEnterSleepMode, ATCTL_RX_TIMEOUT);

    return (Status);
}

///**************************************************************
//* @brief  Do a request to turn on/off internal watchdog of the modlue
//* @note The watchdog is on by default, this will enhance the module stability
//* @note After WDT is turned on, the sleep current will be inreased by around 0.7uA
//* @param onoff: The param can be "ON"/"OFF"
//* @retval LoRa return code
//**************************************************************/
//ATEerror_t Lora_SetWDT(uint8_t onoff)
//{
//  ATEerror_t Status;

//  Status = Modem_AT_Cmd(AT_SET, AT_WDG, &onoff);

//  return (Status);
//}

/*********** Finate State Machine for LoRa Modem mangement *************/
#ifdef USE_LRWAN_NS1
/* For test purpose -- node's attributs declared at gateway level*/
/* The default "DevEui" can be used at gateway level */
/* AppEui will be user defined*/

/*static uint8_t DevEui[] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 }; */
/*uncomment this line if you want to define your own AppEui -- cf line 1785*/
/*static uint8_t AppEui[] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 }; */
#endif

/******************************************************************************
  * @Brief lora Modem state machine
  * @param void
  * @retval None
******************************************************************************/
uint8_t Check_cnt = 0;
uint8_t Check_cnt_ack = 0;

void Lora_fsm(void)
{
    RetCode_t LoraModuleRetCode;
    ATEerror_t LoraCmdRetCode;
//    uint8_t devEui[8];
//    uint8_t appEui[8];
    uint8_t devAddr[4];
//	uint8_t appkey[16]={0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c}; 

    switch (DeviceState)
    {
    case DEVICE_INIT:
    {
        /* Check if the LoRa Modem is ready to work*/

        LoraModuleRetCode = Lora_Init();

        if (LoraModuleRetCode == MODULE_READY)
        {
            /* Set ATE OFF */
            Lora_SetATE(0);
            DBG_PRINTF("Lora_SetATE OFF \n");
            /* get the embedded DevEUI of the modem*/
            LoRa_GetDeviceID(LoraDriverParam.deveui);
            DBG_PRINTF("LoRa_GetdevEui :");
            print02x(LoraDriverParam.deveui, 8);
            /* get the embedded AppEUI */
            LoRa_GetAppID(LoraDriverParam.appeui);
            DBG_PRINTF("LoRa_GetappEui :");
            print02x(LoraDriverParam.appeui, 8);
            /*Set Class MODE */
			Lora_SetClass(LoraDriverParam.Class);
            DBG_PRINTF("Set Class:%d \r\n",LoraDriverParam.Class);
            /*Set CONFIMR/UNCONFIRM */
            Lora_SetSendMsgConfirm(LoraDriverParam.Confirm);
            DBG_PRINTF("Set Confirm:%d \r\n",LoraDriverParam.Confirm);
            /*Set ADR */
            if(LoraDriverParam.Adr_En)
            {   
            LoraCmdRetCode = Lora_SetAdaptiveDataRate(LoraDriverParam.Adr_En);
            DBG_PRINTF("Set ADR ON \r\n");
            }
            else
            {
                LoraCmdRetCode = Lora_SetDataRate(LoraDriverParam.Dr);
                DBG_PRINTF("Set DR %d \r\n",LoraDriverParam.Dr);
            }
            if(LoraDriverParam.JoinMode)
            {
                DBG_PRINTF("Device OTAA JOINING...");
                /*Set OTAA APPKEY */
                LoRa_GetKey(AT_APPKEY,LoraDriverParam.appkey);
                DBG_PRINTF("LoRa_Get APPKey : ");
    			print02x(LoraDriverParam.appkey, 16);
                /*Get OTAA APPKEY */
//    			LoRa_SetKey(AT_APPKEY,appkey);
//                DBG_PRINTF("LoRa_Set APPKey : ");
//    			print02x(LoraDriverParam.appkey, 16);
            }
            else
            {
                DBG_PRINTF("Device ABP...");
                LoRa_GetDeviceAddress(devAddr);
                DBG_PRINTF("LoRa_GetDeviceAddress : ");
                print02x(devAddr, 4);
                LoRa_SetKey(AT_APPSKEY,LoraDriverParam.appskey);
                DBG_PRINTF("LoRa_SetAppSKey : ");
    			print02x(LoraDriverParam.appskey, 16);
                LoRa_SetKey(AT_NWKSKEY,LoraDriverParam.nwkskey);
                DBG_PRINTF("LoRa_SetNwkSKey : ");
    			print02x(LoraDriverParam.nwkskey, 16);
            }
            /*Set JOIN MODE */
            Lora_SetJoinMode(LoraDriverParam.JoinMode);
			DBG_PRINTF("OTAA_JOIN_MODE");
            DeviceState = DEVICE_READY;
        }
        else
        {
            DBG_PRINTF("Lora module not ready\n");   /*we stay in Init state and redo Lora_Init*/
        }
        break;
    }
    case DEVICE_READY:
    {
        /* Do a join request to the LoRaWAN network - can be ABP or OTAA  Join mode*/
        /* Nota : Join Mode parameter relevant for USI modem - For Murata not releavant cf.User manual*/
        DBG_PRINTF("Lora module DEVICE_READY\n");
        LoraCmdRetCode = Lora_Join(OTAA_JOIN_MODE);
		HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);
		delay_ms(200);
		HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_SET);
        switch (LoraCmdRetCode)
        {
        case ATCTL_RET_CMD_JOIN:
        {
            DeviceState = DEVICE_JOINED;

            DeviceSubState = DEVICE_INIT;
            DBG_PRINTF("Nwk Joined\n");
//			HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_SET);
            break;
        }
        case ATCTL_RET_IDLE:
        {
            DeviceState = DEVICE_SLEEP;
            DeviceSubState = DEVICE_JOIN_ON_GOING;
            break;
        }
        default:
            DBG_PRINTF("Nwk Not Joined\n");   /* we stay in ready state and redo LoRa_Join*/
            break;
        }
        break;
    }
    case DEVICE_JOINED:
    {
        DBG_PRINTF("Nwk Joined - waiting\n");
        /*Schedule the first mesaure */
        DeviceState = DEVICE_SEND;
        break;
    }
    case DEVICE_SLEEP:
    {
		delay_ms(1100);
		DeviceState = DEVICE_CHECK;
		Check_cnt = 0;
        /* Wake up through RTC events or asynchronous event coming from modem*/
        if (DeviceSubState == DEVICE_JOIN_ON_GOING)
        {
            DeviceState = DEVICE_READY;
        }
        break;
    }
    case DEVICE_SEND:
    {
        /*Sensor reading on slave device*/

        /*Send data to Slave device  */
		
		Read_tempHum_valu();
		User_Protocol_data.Temp=Get_sht30TemValu();
		User_Protocol_data.Hnum = Get_sht30HumValu();
		HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
		delay_ms(100);
		HAL_GPIO_TogglePin(GPIOA, G_LED_Pin);
//		DBG_PRINTF("------>Read_tempHum_valu...Temp = [%2d] Hum = [%2d]...\r\n",User_Protocol_data.Temp,User_Protocol_data.Hnum);	
		SendDataBinary.DataSize = 7;//strlen(Test_Send_buf);
		memcpy(SendDataBinary.Buffer,&User_Protocol_data,SendDataBinary.DataSize);
        LoraCmdRetCode = Lora_SendDataBin(&SendDataBinary);

        /*Schedule the next measure */          
        DeviceState = DEVICE_SLEEP;
        break;
    }
	case DEVICE_CHECK:
	{
	    LoraModuleRetCode = Lora_Init();	
		Check_cnt = Check_cnt+1;
		if(LoraModuleRetCode==MODULE_NO_READY)
		{
		  if(Check_cnt_ack==2)
		  {
				Check_cnt_ack = 0;
				Check_cnt = 0;
			  	HAL_GPIO_WritePin(GPIOA, B_LED_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, G_LED_Pin, GPIO_PIN_RESET);
				DeviceState = DEVICE_INIT;
		  }
		  else 
		  Check_cnt_ack = Check_cnt_ack+1; 
		}
		else
		{
			Check_cnt_ack=0;
			delay_ms(500);
			if(Check_cnt==6)
			{
				Check_cnt = 0;
				DeviceState = DEVICE_SEND;
			}
		}
	}
	break;
    default:
    {
        DeviceState = DEVICE_INIT;
        break;
    }
    }
}
/******************************************************************************
  * @Brief get the current device finate state
  * @param void
  * @retval deviceState
******************************************************************************/
DeviceState_t lora_getDeviceState(void)
{
    return DeviceState;
}

void Lora_Ctx_Init(void)
{
    
    LoraDriverParam.Adr_En = ADAPT_DATA_RATE_ENABLE;
    LoraDriverParam.BandPlan = 1;
    LoraDriverParam.Class = LORA_CLASS_A;
    LoraDriverParam.Confirm = LORA_UNCONFIRM_MODE;
    LoraDriverParam.JoinMode = OTAA_JOIN_MODE;
    LoraDriverParam.Port = 12;
    LoraDriverParam.Repet_Cnt = 1;
    LoraDriverParam.Dr = DR_1;
    LoraDriverParam.SensorCycleMeasure = 10000;
    LoraDriverParam.Channel[0] = 0;
    LoraDriverParam.Channel[1] = 7;
	DeviceState = DEVICE_INIT ;
}


static void print02x(uint8_t *pt, uint8_t ptlen)
{
    uint8_t i;
    for(i = 0; i < ptlen; i++)
    {
        LOG_LUO("%02X:", pt[i]);
    }
    LOG_LUO("\r\n");
}

/*********** private (static) Lora functions *************/



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
