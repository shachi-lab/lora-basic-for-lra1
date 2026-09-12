/*!
 * @file    wan_pro.c
 * @brief   Wrapper for LoRa-WAN mode LoRa protocol
 *
 * @details
 *
 * @code
 *   ____  _                _     _     _       _
 *  / ___)| |__   __ _  ___| |__ |_|   | | __ _| |__
 *  \___ \| '_ \ / _` |/ __) '_ \ _  _ | |/ _` | '_ \
 *   ___) | | | | (_| | (__| | | | ||_|| | (_| | |_) |
 *  (____/|_| |_|\__,_|\___)_| |_|_|   |_|\__,_|_.__/
 * @endcode
 *
 * @copyright
 *   (C) 2019-2026 Shachi-lab
 *
 * @author
 *   Shachi-lab
 *
 * @license
 *   MIT (Shachi-lab portions only; see root LICENSE.txt and THIRD_PARTY_NOTICES.md)
 */
#ifdef	__UI_LORA_WAN__

#include "asf.h"
#include <string.h>
#include <stdio.h>
#include "board-config.h"
#include "board-mcu.h"
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_timer.h"
#include "utilities.h"
#include "radio.h"
#include "basic.h"
#include "bios.h"
#include "Commissioning.h"

#define RX_WINDOW_DELAY_ADJUST		0

#ifndef ACTIVE_REGION
#if defined( REGION_AS923 )
#define ACTIVE_REGION   LORAMAC_REGION_AS923
#define PAYLOAD_SIZE	11
#else
#error "Please define a region parameter."
#endif
#endif

/*!
 * Defines the application data transmission duty cycle. 5s, value in [ms].
 */
#define APP_TX_DUTYCYCLE				5000

/*!
 * Defines a random delay for application data transmission duty cycle. 1s,
 * value in [ms].
 */
#define APP_TX_DUTYCYCLE_RND			1000

/*!
 * Default datarate
 */
#define LORAWAN_DEFAULT_DATARATE		DR_2

/*!
 * LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG_ON		false

/*!
 * LoRaWAN Adaptive Data Rate
 * \remark Please note that when ADR is enabled the end-device should be static
 */
#define LORAWAN_DEFAULT_ADR				false

/*!
 * LoRaWAN application port
 */
#define LORAWAN_DEFAULT_PORT			2

static volatile uint32_t TxDutyCycleTime;				// Defines the application data transmission duty cycle
static TimerEvent_t TxNextPacketTimer;			// Timer to handle the application data transmission duty cycle
static volatile bool Wan_ExitFlag = false;
static volatile UiRes_t Wan_Resp = UI_RES_OK;
static Wan_Info_t Wan_Info;
static Wan_TxPara_t	Wan_TxPara;
static Wan_RxPara_t	Wan_RxPara;
static MibRequestConfirm_t Wan_mibReq;
static LoRaMacStatus_t Wan_MacStat = LORAMAC_STATUS_OK;
static Wan_DeviceTimeReq_Callback_t	Wan_DeviceTimeReq_Callback;

#define LORA_MIB_GET( x )       {Wan_mibReq.Type=x;Wan_MacStat=LoRaMacMibGetRequestConfirm(&Wan_mibReq);}
#define LORA_MIB_SET( x )       {Wan_mibReq.Type=x;Wan_MacStat=LoRaMacMibSetRequestConfirm(&Wan_mibReq);}
#define LORA_MIB_GET_CHK( x )   {LORA_MIB_GET(x);if(Wan_MacStat!=LORAMAC_STATUS_OK)return Wan_Stat_Conv(Wan_MacStat);}
#define LORA_MIB_SET_CHK( x )   {LORA_MIB_SET(x);if(Wan_MacStat!=LORAMAC_STATUS_OK)return Wan_Stat_Conv(Wan_MacStat);}

static UiRes_t Wan_Stat_Conv( LoRaMacStatus_t stat );
static UiRes_t Wan_Event_Conv( LoRaMacEventInfoStatus_t stat );
static void Wan_StartDutyCycleTimer( void );
static UiRes_t Wan_SetJoinManual( Wan_JoinStat_t mode, Wan_Keys_t *key_p );
static LoRaMacStatus_t Wan_JoinRequest( void );

/*
 *
 *
 */
static LoRaMacStatus_t Wan_GetQueryTxPossible( LoRaMacTxInfo_t *txInfo, uint8_t size )
{
	if( Wan_SetAdr() != UI_RES_OK ) return Wan_MacStat;
	if( Wan_SetDatarate( Wan_TxPara.Datarate ) != UI_RES_OK )
	{
		if( Wan_SetDatarate( LORAWAN_DEFAULT_DATARATE ) != UI_RES_OK ) return Wan_MacStat;
	}
	if( Wan_SetPower( Wan_TxPara.Power ) != UI_RES_OK ) return Wan_MacStat;
	return LoRaMacQueryTxPossible( size, txInfo );
}

/*!
 * \brief   Prepares the payload of the frame
 *
 * \retval  [0: frame could be send, 1: error]
 */
static LoRaMacStatus_t Wan_TxFrame( void )
{
    LoRaMacTxInfo_t txInfo;
    McpsReq_t mcpsReq;
    LoRaMacStatus_t res;

	if( Wan_Info.NextTx == false )	return LORAMAC_STATUS_DUTYCYCLE_RESTRICTED;

	Wan_TxPara.Sending = false;

	res = Wan_GetQueryTxPossible( &txInfo , Wan_TxPara.Len );

    if( res != LORAMAC_STATUS_OK )
    {
#if 0
		// Send empty frame in order to flush MAC commands
		mcpsReq.Type = MCPS_UNCONFIRMED;
		mcpsReq.Req.Unconfirmed.fPort		= 0;
		mcpsReq.Req.Unconfirmed.fBuffer		= NULL;
		mcpsReq.Req.Unconfirmed.fBufferSize = 0;
		mcpsReq.Req.Unconfirmed.Datarate	= LORAWAN_DEFAULT_DATARATE;
#else
		return res;
#endif
    }else
    if( Wan_TxPara.Confirmed == false )
    {
		mcpsReq.Type = MCPS_UNCONFIRMED;
		mcpsReq.Req.Unconfirmed.fPort		= Wan_TxPara.Port;
		mcpsReq.Req.Unconfirmed.fBuffer		= Wan_TxPara.Data_ptr;
		mcpsReq.Req.Unconfirmed.fBufferSize = Wan_TxPara.Len;
		mcpsReq.Req.Unconfirmed.Datarate	= Wan_TxPara.Datarate;
	}
	else
	{
		mcpsReq.Type = MCPS_CONFIRMED;
		mcpsReq.Req.Confirmed.fPort			= Wan_TxPara.Port;
		mcpsReq.Req.Confirmed.fBuffer		= Wan_TxPara.Data_ptr;
		mcpsReq.Req.Confirmed.fBufferSize	= Wan_TxPara.Len;
		mcpsReq.Req.Confirmed.Datarate		= Wan_TxPara.Datarate;
		mcpsReq.Req.Confirmed.NbTrials		= Wan_TxPara.Trial;
	}
	LED_BUSY_ON();

	res = LoRaMacMcpsRequest( &mcpsReq );
	if( res == LORAMAC_STATUS_OK )
	{
		Wan_Info.NextTx = false;
        Wan_TxPara.Sending = true;
        Wan_StartDutyCycleTimer();
    }else
	{
		LED_BUSY_OFF();
	}
	return res;
}

/*
 *
 */
static LoRaMacStatus_t Wan_JoinRequest( void )
{
    MlmeReq_t mlmeReq;
    LoRaMacStatus_t res;

    mlmeReq.Type = MLME_JOIN;
    mlmeReq.Req.Join.DevEui = Wan_Info.JoinKeys_ptr->DevEui;
    mlmeReq.Req.Join.AppEui = Wan_Info.JoinKeys_ptr->JoinEui;
    mlmeReq.Req.Join.AppKey = Wan_Info.JoinKeys_ptr->AppKey;
    mlmeReq.Req.Join.Datarate = LORAWAN_DEFAULT_DATARATE;
    res = LoRaMacMlmeRequest( &mlmeReq );
//	if( res != LORAMAC_STATUS_OK )
//	{
//		Wan_StartDutyCycleTimer();
//	}
    return res;
}

/*!
 * \brief Function executed on TxNextPacket Timeout event
 */
static void OnTxNextPacketTimerEvent( void *context )
{
//	if( Wan_GetJoinStatus() != true )	return;
	Wan_Info.NextTx = true;
	LED_BUSY_OFF();
}

/*
 *
 */
static void Wan_StartDutyCycleTimer( void )
{
    // Schedule next packet transmission
    TxDutyCycleTime = APP_TX_DUTYCYCLE + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );

    // Schedule next packet transmission
    TimerSetValue( &TxNextPacketTimer, TxDutyCycleTime );
    TimerStart( &TxNextPacketTimer );
}

/*
 *
 */
uint32_t Wan_GetDutyCycleTime( void )
{
	uint32_t dutyCycleTime = 0;
	if( Wan_Info.NextTx == false )
	{
		dutyCycleTime = TimerGetRemainTime( &TxNextPacketTimer );
	}
	if( dutyCycleTime == 0 )
	{
		dutyCycleTime = LoRaMacGetDutyCycleTime();
	}
	return dutyCycleTime;
}

/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [IN] mcpsConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void McpsConfirm_callback( McpsConfirm_t *mcpsConfirm )
{
    if( mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
    {
        switch( mcpsConfirm->McpsRequest )
        {
            case MCPS_UNCONFIRMED:
            case MCPS_CONFIRMED:
            {
				Wan_Info.Sent.Channel = mcpsConfirm->Channel;
				Wan_Info.Sent.Datarate = mcpsConfirm->Datarate;
				Wan_Info.Sent.UpLinkCounter = mcpsConfirm->UpLinkCounter;
				Wan_Info.Sent.TxDoneTime = mcpsConfirm->TxDoneTime;
                // Check TxPower
                // Check AckReceived
                // Check NbTrials
                break;
            }
            case MCPS_PROPRIETARY:
            {
                break;
            }
            default:
                break;
        }
        // Switch LED 1 ON
//		LED_TX_ON();

        // Status is OK
        Wan_Resp = UI_RES_OK;
    }
    else
	{
		Wan_Resp = Wan_Event_Conv( mcpsConfirm->Status );
    }
	Wan_ExitFlag = true;
	Wan_TxPara.Sending = false;
//  NextTx = true;
}

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [IN] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 */
static void McpsIndication_callback( McpsIndication_t *mcpsIndication )
{
    if( mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        return;
    }
    switch( mcpsIndication->McpsIndication )
    {
        case MCPS_UNCONFIRMED:
        {
            break;
        }
        case MCPS_CONFIRMED:
        {
            break;
        }
        case MCPS_PROPRIETARY:
        {
            break;
        }
        case MCPS_MULTICAST:
        {
            break;
        }
        default:
            break;
    }

	Wan_RxPara.Pending	= mcpsIndication->FramePending;
	Wan_RxPara.Rssi     = mcpsIndication->Rssi;
	Wan_RxPara.Snr      = mcpsIndication->Snr;
	Wan_RxPara.Datarate = mcpsIndication->RxDatarate;
    if( mcpsIndication->RxData == true )
    {
		if( Wan_RxPara.Received == false && Wan_RxPara.Data_ptr )
		{
			Wan_Info.DownLinkCounter= mcpsIndication->DownLinkCounter;
			Wan_RxPara.Port	= mcpsIndication->Port;
			Wan_RxPara.Len	= mcpsIndication->BufferSize;
			memcpy( Wan_RxPara.Data_ptr, mcpsIndication->Buffer, Wan_RxPara.Len );
			Wan_RxPara.Received = true;
			Wan_ExitFlag = true;
			Wan_Resp = UI_RES_RX;
		}
	}
    // Switch LED 2 ON for each received downlink
    LED_RX_ON();
}

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [IN] mlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm_callback( MlmeConfirm_t *mlmeConfirm )
{
    switch( mlmeConfirm->MlmeRequest )
    {
        case MLME_JOIN:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Status is OK, node has joined the network
                Wan_Info.NextTx = true;
			}
            Wan_ExitFlag = true;
			Wan_Resp = Wan_Event_Conv( mlmeConfirm->Status );
            break;
        }
        case MLME_LINK_CHECK:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                Wan_Info.LinkChk.DemodMargin = mlmeConfirm->DemodMargin;
                Wan_Info.LinkChk.NbGateways = mlmeConfirm->NbGateways;
//				Wan_Info.LinkChk.Req = false;
            }
            break;
        }
        case MLME_DEVICE_TIME:
        {
	        if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
	        {
		        Wan_Info.DeviceTime.Seconds = mlmeConfirm->DeviceTime.seconds;			// GPS Epoch
		        Wan_Info.DeviceTime.Sub_seconds = mlmeConfirm->DeviceTime.sub_seconds;	// 1/256 sec
			
		 		uint32_t seconds = Wan_Info.DeviceTime.Seconds;
	 	 		uint32_t sub_sec = Wan_Info.DeviceTime.Sub_seconds;
	 	 		uint32_t delta_ms =  TimerGetCurrentTime() - Wan_Info.Sent.TxDoneTime;

				delta_ms = delta_ms + ((sub_sec * 4000) /1024);
				Wan_Info.DeviceTime.Seconds += delta_ms / 1000;
				delta_ms %= 1000;

				uint8_t result = Wan_DeviceTimeReq_Callback( seconds, delta_ms );
		        Wan_Info.DeviceTime.Req = 2 + result;
	        }
	        break;
        }
        default:
            break;
    }
//	NextTx = true;
}

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [IN] mlmeIndication - Pointer to the indication structure.
 */
static void MlmeIndication_callback( MlmeIndication_t *mlmeIndication )
{
    switch( mlmeIndication->MlmeIndication )
    {
        case MLME_SCHEDULE_UPLINK:
        {// The MAC signals that we shall provide an uplink as soon as possible
            OnTxNextPacketTimerEvent( NULL );
            break;
        }
        default:
            break;
    }
}

/*
 *
 */
UiRes_t Wan_wait( uint32_t waitTime )
{
    Wan_Resp = UI_RES_OK;
    Wan_ExitFlag = false;
	Wan_RxPara.Received = false;
	bios_setWaitTick( waitTime );
    while( Wan_ExitFlag == false )
	{
		Ui_LowpowerTimerProcess();
		if( bios_getWaitTick() == 0 ) { Wan_Resp = UI_RES_ERROR; break; }
		bios_ClearWDT();
	}
	if( Wan_Info.Class != CLASS_C )
	{
		Radio.Sleep( );
    }
	return Wan_Resp;
}

/*
 *
 */
bool Wan_RxCheck( void )
{
	if( Wan_Info.JoinStat == WAN_JOINSTAT_NONE ) return false;
	if( Wan_Info.Class != CLASS_C )	return false;
	return Wan_RxPara.Received;
}

/*
 *
 */
void Wan_RxClear( void )
{
	if( Wan_Info.JoinStat == WAN_JOINSTAT_NONE ) return;
	if( Wan_Info.Class != CLASS_C )	return;
	Wan_RxPara.Received = false;
	Wan_Info.IsPrinted = false;
}

/**
 * WAN-INIT (application entry point.)
 */
Wan_Info_t *Wan_init( Wan_JoinKeys_t *join_key_p )
{
	static LoRaMacPrimitives_t s_LoRaMacPrimitives;
	static LoRaMacCallback_t s_LoRaMacCallbacks;

	if( join_key_p == NULL )
	{
		LoRaMacInitialization( NULL, NULL, ACTIVE_REGION );			// Radio Reset only
	}else
	{
		s_LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm_callback;
		s_LoRaMacPrimitives.MacMcpsIndication = McpsIndication_callback;
		s_LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm_callback;
		s_LoRaMacPrimitives.MacMlmeIndication = MlmeIndication_callback;
		s_LoRaMacCallbacks.GetBatteryLevel = BoardGetBatteryLevel;
		LoRaMacInitialization( &s_LoRaMacPrimitives, &s_LoRaMacCallbacks, ACTIVE_REGION );

		TimerInit( &TxNextPacketTimer, OnTxNextPacketTimerEvent );

		Wan_mibReq.Param.EnablePublicNetwork = !IS_CTRL_WAN_PRIVATE_NETWORK;	// LORAWAN_PUBLIC_NETWORK;
		LORA_MIB_SET( MIB_PUBLIC_NETWORK );

		Wan_Info.JoinKeys_ptr = join_key_p;
		Wan_Info.TxPara_ptr	  = &Wan_TxPara;
		Wan_Info.RxPara_ptr   = &Wan_RxPara;
		Wan_Info.NextTx	      = true;
	}
	Wan_Info.Class = CLASS_A;
	Wan_SetClass( CLASS_A );

    return &Wan_Info;
}

/**
 * WAN-JOIN_OTAA (application entry point.)
 */
UiRes_t Wan_join_OTAA( void )
{
    if( Wan_Info.JoinKeys_ptr == NULL ) return UI_RES_ERROR;

    Wan_MacStat = Wan_JoinRequest();
	UiRes_t res =  Wan_Stat_Conv( Wan_MacStat );
	return res;
}

/**
 * WAN-JOIN_ABP (application entry point.)
 */
UiRes_t Wan_join_ABP( void )
{
    if( Wan_Info.JoinKeys_ptr == NULL ) return UI_RES_ERROR;

	// Choose a random device address if not already defined in Commissioning.h
	if( Wan_Info.JoinKeys_ptr->ABP_Keys.DevAddr == 0 )
	{
		// Random seed initialization
		srand1( Radio.Random( ) );

		// Choose a random device address
		Wan_Info.JoinKeys_ptr->ABP_Keys.DevAddr = randr( 0, 0x01FFFFFF );
	}
	UiRes_t res = Wan_SetJoinManual( WAN_JOINSTAT_ABP, &(Wan_Info.JoinKeys_ptr->ABP_Keys) );
	return res;
}

/**
 * WAN-JOIN-manual (application entry point.)
 */
static UiRes_t Wan_SetJoinManual( Wan_JoinStat_t mode, Wan_Keys_t *key_p )
{
    if( key_p == NULL ) return UI_RES_ERROR;

	Wan_mibReq.Param.NetID = key_p->NetID;
	LORA_MIB_SET( MIB_NET_ID );
	Wan_mibReq.Param.DevAddr = key_p->DevAddr;
	LORA_MIB_SET( MIB_DEV_ADDR );
	Wan_mibReq.Param.NwkSKey = key_p->NwkSkey;
	LORA_MIB_SET( MIB_NWK_SKEY );
	Wan_mibReq.Param.AppSKey = key_p->AppSkey;
	LORA_MIB_SET( MIB_APP_SKEY );

	Wan_mibReq.Param.IsNetworkJoined = mode;
	LORA_MIB_SET( MIB_NETWORK_JOINED );

	return UI_RES_OK;
}

/**
 * WAN-SEND (application entry point.)
 */
UiRes_t Wan_send( void )
{
	Wan_GetJoinStatus();
    if( !Wan_Info.JoinStat )  return UI_RES_NOT_JOINED;
	Wan_MacStat = Wan_TxFrame();
   	return Wan_Stat_Conv( Wan_MacStat );
}

/**
 * WAN-LINKCHK (application entry point.)
 */
UiRes_t Wan_linkchk( void )
{
	if( LoRaMacIsExistLinkChk() ) return UI_RES_OK;
    MlmeReq_t mlmeReq;
    mlmeReq.Type = MLME_LINK_CHECK;
    Wan_MacStat = LoRaMacMlmeRequest( &mlmeReq );
    return Wan_Stat_Conv( Wan_MacStat );
}

/**
 * WAN-SET_ADR (application entry point.)
 */
UiRes_t Wan_SetAdr( void )
{
    Wan_mibReq.Param.AdrEnable = Wan_Info.Adr;
    LORA_MIB_SET_CHK( MIB_ADR );
    return UI_RES_OK;
}

/**
 * WAN-GET_ADR (application entry point.)
 */
UiRes_t Wan_GetAdr( void )
{
    LORA_MIB_GET_CHK( MIB_ADR );
    Wan_Info.Adr = Wan_mibReq.Param.AdrEnable;
    return UI_RES_OK;
}

/**
 * WAN-GET_JOIN_STATUS (application entry point.)
 */
UiRes_t Wan_GetJoinStatus( void )
{
	LORA_MIB_GET( MIB_NETWORK_JOINED );
	Wan_Info.JoinStat = 0;
	if( Wan_MacStat == LORAMAC_STATUS_OK )
	{
		Wan_Info.JoinStat = Wan_mibReq.Param.IsNetworkJoined;
	}
    return Wan_MacStat;
}

/**
 * WAN-SET_JOIN_STATUS (application entry point.)
 */
UiRes_t Wan_SetJoinStatus( Wan_JoinStat_t stat )
{
    Wan_mibReq.Param.IsNetworkJoined = stat;
    LORA_MIB_SET_CHK( MIB_NETWORK_JOINED );
    return UI_RES_OK;
}

/**
 * WAN-GET_JOIN_KEYS (application entry point.)
 */
UiRes_t Wan_GetJoinKeys( void )
{
    if( Wan_Info.JoinKeys_ptr == NULL ) return UI_RES_ERROR;

    LORA_MIB_GET_CHK( MIB_NET_ID )
    Wan_Info.JoinKeys_ptr->OTAA_Keys.NetID = Wan_mibReq.Param.NetID;
    LORA_MIB_GET_CHK( MIB_DEV_ADDR );
    Wan_Info.JoinKeys_ptr->OTAA_Keys.DevAddr = Wan_mibReq.Param.DevAddr;
    LORA_MIB_GET_CHK( MIB_NWK_SKEY );
    memcpy( Wan_Info.JoinKeys_ptr->OTAA_Keys.NwkSkey, Wan_mibReq.Param.NwkSKey, NWKSKEY_LEN );
    LORA_MIB_GET_CHK( MIB_APP_SKEY );
    memcpy( Wan_Info.JoinKeys_ptr->OTAA_Keys.AppSkey, Wan_mibReq.Param.AppSKey, APPSKEY_LEN );
    return UI_RES_OK;
}

/**
 * WAN-GET_UPLINK_COUNTER (application entry point.)
 */
UiRes_t Wan_GetUplinkCounter( void )
{
    LORA_MIB_GET_CHK( MIB_UPLINK_COUNTER );
    Wan_Info.UpLinkCounter = Wan_mibReq.Param.UpLinkCounter;
    return UI_RES_OK;
}

/**
 * WAN-GET_DOWNLINK_COUNTER (application entry point.)
 */
UiRes_t Wan_GetDownlinkCounter( void )
{
    LORA_MIB_GET_CHK( MIB_DOWNLINK_COUNTER );
    Wan_Info.DownLinkCounter = Wan_mibReq.Param.DownLinkCounter;
    return UI_RES_OK;
}

/**
 * WAN-SET_UPLINK_COUNTER (application entry point.)
 */
UiRes_t Wan_SetUplinkCounter( void )
{
    Wan_mibReq.Param.UpLinkCounter = Wan_Info.UpLinkCounter;
    LORA_MIB_SET_CHK( MIB_UPLINK_COUNTER );
    return UI_RES_OK;
}

/**
 * WAN-SET_DOWNLINK_COUNTER (application entry point.)
 */
UiRes_t Wan_SetDownlinkCounter( void )
{
    Wan_mibReq.Param.DownLinkCounter = Wan_Info.DownLinkCounter;
    LORA_MIB_SET_CHK( MIB_DOWNLINK_COUNTER );
    return UI_RES_OK;
}

/**
 * WAN-GET_RX_DELAY (application entry point.)
 */
UiRes_t Wan_GetRxDelay( Wan_RxDelay_e rxDelay )
{
#if __WAN_RXDELAY_ENABLE__
	switch( rxDelay )
	{
	case WAN_RECV_DELAY_1 :
		LORA_MIB_GET( MIB_RECEIVE_DELAY_1 );
		Wan_Info.RxDelay_now.RxDelay[WAN_RECV_DELAY_1] = Wan_mibReq.Param.ReceiveDelay1;
		break;
	case WAN_RECV_DELAY_2 :
		LORA_MIB_GET( MIB_RECEIVE_DELAY_2 );
		Wan_Info.RxDelay_now.RxDelay[WAN_RECV_DELAY_2]  = Wan_mibReq.Param.ReceiveDelay2;
		break;
	case WAN_JOIN_DELAY_1 :
		LORA_MIB_GET( MIB_JOIN_ACCEPT_DELAY_1 );
		Wan_Info.RxDelay_now.RxDelay[WAN_JOIN_DELAY_1]  = Wan_mibReq.Param.JoinAcceptDelay1;
		break;
	case WAN_JOIN_DELAY_2 :
		LORA_MIB_GET( MIB_JOIN_ACCEPT_DELAY_2 );
		Wan_Info.RxDelay_now.RxDelay[WAN_JOIN_DELAY_2]  = Wan_mibReq.Param.JoinAcceptDelay2;
		break;
	}
#endif
    return UI_RES_OK;
}

/**
 * WAN-SET_RX_DELAY (application entry point.)
 */
UiRes_t Wan_SetRxDelay( Wan_RxDelay_e rxDelay )
{
#if __WAN_RXDELAY_ENABLE__
	if( Wan_Info.RxDelay_ptr == NULL )	return UI_RES_OK;
	switch( rxDelay )
	{
	case WAN_RECV_DELAY_1 :
	    Wan_mibReq.Param.ReceiveDelay1 = Wan_Info.RxDelay_ptr->RxDelay[WAN_RECV_DELAY_1];
	    LORA_MIB_SET( MIB_RECEIVE_DELAY_1 );
		break;
	case WAN_RECV_DELAY_2 :
		Wan_mibReq.Param.ReceiveDelay2 = Wan_Info.RxDelay_ptr->RxDelay[WAN_RECV_DELAY_2];
	    LORA_MIB_SET( MIB_RECEIVE_DELAY_2 );
		break;
	case WAN_JOIN_DELAY_1 :
		Wan_mibReq.Param.JoinAcceptDelay1 = Wan_Info.RxDelay_ptr->RxDelay[WAN_JOIN_DELAY_1];
	    LORA_MIB_SET( MIB_JOIN_ACCEPT_DELAY_1 );
		break;
	case WAN_JOIN_DELAY_2 :
		Wan_mibReq.Param.JoinAcceptDelay2 = Wan_Info.RxDelay_ptr->RxDelay[WAN_JOIN_DELAY_2];
		LORA_MIB_SET( MIB_JOIN_ACCEPT_DELAY_2 );
    	break;
	}
#endif
	return UI_RES_OK;
}

/**
 * WAN-GET_CLASS (application entry point.)
 */
UiRes_t Wan_GetClass( void )
{
	LORA_MIB_GET( MIB_DEVICE_CLASS );
	Wan_Info.Class = Wan_mibReq.Param.Class;
    return UI_RES_OK;
}

/**
 * WAN-SET_CLASS (application entry point.)
 */
UiRes_t Wan_SetClass( DeviceClass_t Class )
{
	Wan_mibReq.Param.Class = Wan_Info.Class = Class;
	LORA_MIB_SET( MIB_DEVICE_CLASS );
    return UI_RES_OK;
}

/**
 * WAN-SET_DATARATE (application entry point.)
 */
UiRes_t Wan_SetDatarate( uint8_t datarate )
{
	Wan_mibReq.Param.ChannelsDatarate = datarate;
	LORA_MIB_SET_CHK( MIB_CHANNELS_DATARATE );
    return UI_RES_OK;
}

/**
 * WAN-SET_POWER (application entry point.)
 */
UiRes_t Wan_SetPower( uint8_t power )
{
	Wan_mibReq.Param.ChannelsTxPower = power;
	LORA_MIB_SET_CHK( MIB_CHANNELS_TX_POWER );
    return UI_RES_OK;
}

/**
 * WAN-SET_CHMASK (application entry point.)
 */
UiRes_t Wan_SetChMask( void )
{
	static bool flag = false;
	if( Wan_Info.ChannelsMask || flag )
	{
		flag = true;
		uint16_t mask = ~Wan_Info.ChannelsMask;
		Wan_mibReq.Param.ChannelsMask = &mask;
		LORA_MIB_SET_CHK( MIB_CHANNELS_MASK );
    }
	return UI_RES_OK;
}

/**
 * WAN-GET_CH_MASK (application entry point.)
 */
UiRes_t Wan_GetChMask( uint16_t *ch_mask )
{
	LORA_MIB_GET_CHK( MIB_CHANNELS_MASK );
	*ch_mask = ~(*Wan_mibReq.Param.ChannelsMask);
	return UI_RES_OK;
}

/**
 * WAN-GET_CHANNEL (application entry point.)
 */
UiRes_t Wan_GetChannel( ChannelParams_t **ch_list )
{
	LORA_MIB_GET_CHK( MIB_CHANNELS );
	*ch_list = Wan_mibReq.Param.ChannelList;
	return UI_RES_OK;
}

/**
 * WAN-GET_PAYLOAD_LENGTH (application entry point.)
 */
UiRes_t Wan_GetPayloadLength( uint8_t *len )
{
    LoRaMacTxInfo_t txInfo;
	*len = 0;
	if( Wan_GetQueryTxPossible( &txInfo, 0 ) != LORAMAC_STATUS_OK ) return UI_RES_INVALID;
	*len = txInfo.MaxPossiblePayload;
	return UI_RES_OK;
}

/**
 * WAN-GET_DWELL_STAT (application entry point.)
 */
UiRes_t Wan_GetDwellStat( uint8_t *dwell_stat )
{
	*dwell_stat = (LoRaMacGetUpDwellTime() << 1) + LoRaMacGetDownDwellTime();
	return UI_RES_OK;
}

/**
 * WAN-GET_LAST_TX_CHANNEL (application entry point.)
 */
UiRes_t Wan_GetLastTxChannel( uint8_t *last_tx_ch )
{
	*last_tx_ch = LoRaMacGetLastTxChannel() + 1;
	return UI_RES_OK;
}

#if __WAN_CH_ADD_ENABLE__
/**
 * WAN-CHANNEL_ADD (application entry point.)
 */
UiRes_t Wan_ChannelAdd( uint8_t id, uint32_t freq, uint8_t dr_min, uint8_t dr_max )
{
	ChannelParams_t params;

	params.Band = 0;
	params.Frequency = freq;
	params.Rx1Frequency = freq;
	params.DrRange.Fields.Min = dr_min;
	params.DrRange.Fields.Max = dr_max;
	Wan_MacStat = LoRaMacChannelAdd( id, params );
	return Wan_Stat_Conv( Wan_MacStat );
}

/**
 * WAN-CHANNEL_REMOVE (application entry point.)
 */
UiRes_t Wan_ChannelRemove( uint8_t id )
{
	Wan_MacStat = LoRaMacChannelRemove( id );
	return Wan_Stat_Conv( Wan_MacStat );
}
#endif

/**
 * WAN-DEVICE_TIME_REQ (application entry point.)
 */
UiRes_t Wan_DeviceTimeReq( Wan_DeviceTimeReq_Callback_t callback_func )
{
	Wan_DeviceTimeReq_Callback = callback_func;
	if( LoRaMacIsExistTimeReq() ) return UI_RES_OK;
    MlmeReq_t mlmeReq;
    mlmeReq.Type = MLME_DEVICE_TIME;
    Wan_MacStat = LoRaMacMlmeRequest( &mlmeReq );
    return Wan_Stat_Conv( Wan_MacStat );
}

/*
 *
 */
uint8_t Wan_IsExistTimeReq( void )
{
	return LoRaMacIsExistTimeReq();
}

/*
 *
 */
uint8_t Wan_IsExistLinkChk( void )
{
	return LoRaMacIsExistLinkChk();
}

/*
 *
 */
uint8_t *Wan_GetMacCommand( uint8_t *len )
{
	return LoRaMacGetMacCommand( len );
}


volatile uint8_t Wan_error_log[2];

/*
 *
 */
static UiRes_t Wan_Stat_Conv( LoRaMacStatus_t stat )
{
	Wan_error_log[0] = stat;
    switch( stat )
    {
    case LORAMAC_STATUS_OK						: return UI_RES_OK;				// Service started successfully
    case LORAMAC_STATUS_BUSY					: return UI_RES_BUSY;			// Service not started - LoRaMAC is busy
    case LORAMAC_STATUS_DEVICE_OFF				: return UI_RES_ERROR;			// Service not started - the device is switched off
    case LORAMAC_STATUS_REGION_NOT_SUPPORTED	: return UI_RES_DENIED;			// Service not started - the specified region is not supported or not activated with preprocessor definitions.
    case LORAMAC_STATUS_PARAMETER_INVALID		: return UI_RES_INVALID;		// Service not started - invalid parameter
    case LORAMAC_STATUS_FREQUENCY_INVALID		: return UI_RES_INVALID;		// Service not started - invalid frequency
    case LORAMAC_STATUS_DATARATE_INVALID		: return UI_RES_INVALID;		// Service not started - invalid datarate
    case LORAMAC_STATUS_FREQ_AND_DR_INVALID		: return UI_RES_INVALID;		// Service not started - invalid frequency and datarate
    case LORAMAC_STATUS_NO_NETWORK_JOINED		: return UI_RES_NOT_JOINED;		// Service not started - the device is not in a LoRaWAN
    case LORAMAC_STATUS_LENGTH_ERROR			: return UI_RES_INVALID_LENGTH;	// Service not started - payload length error
    case LORAMAC_STATUS_NO_CHANNEL_FOUND		: return UI_RES_NO_FREE_CH;		//
    case LORAMAC_STATUS_NO_FREE_CHANNEL_FOUND	: return UI_RES_NO_FREE_CH;		//
    case LORAMAC_STATUS_DUTYCYCLE_RESTRICTED	: return UI_RES_DC_RESTRICTED;	//
    case LORAMAC_STATUS_SERVICE_UNKNOWN			: return UI_RES_ERROR;			// Service unknown
    }
    return UI_RES_ERROR;
}

/*
 *
 */
static UiRes_t Wan_Event_Conv( LoRaMacEventInfoStatus_t stat )
{
	Wan_error_log[1] = stat;
	switch( stat )
	{
	case LORAMAC_EVENT_INFO_STATUS_OK				: return UI_RES_OK;
	case LORAMAC_EVENT_INFO_STATUS_ERROR			: return UI_RES_ERROR;
	case LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT		: return UI_RES_TIMEOUT;
	case LORAMAC_EVENT_INFO_STATUS_RX1_TIMEOUT		: return UI_RES_TIMEOUT;
	case LORAMAC_EVENT_INFO_STATUS_RX2_TIMEOUT		: return UI_RES_TIMEOUT;
	case LORAMAC_EVENT_INFO_STATUS_RX1_ERROR		: return UI_RES_ERROR;
	case LORAMAC_EVENT_INFO_STATUS_RX2_ERROR		: return UI_RES_ERROR;
	case LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL		: return UI_RES_ERROR;
	case LORAMAC_EVENT_INFO_STATUS_DOWNLINK_REPEATED: return UI_RES_ERROR;
	case LORAMAC_EVENT_INFO_STATUS_TX_DR_PAYLOAD_SIZE_ERROR		 : return UI_RES_INVALID_LENGTH;
	case LORAMAC_EVENT_INFO_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS : return UI_RES_ERROR;
	case LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL		: return UI_RES_ERROR;
	case LORAMAC_EVENT_INFO_STATUS_MIC_FAIL			: return UI_RES_INVALID;
	}
	return UI_RES_ERROR;
}

#endif
