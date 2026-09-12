 /*!
 * @file    wan_ui.c
 * @brief   User interface for LoRa-WAN mode LoRa protocol
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
#include "rtc-board.h"
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_timer.h"
#include "radio.h"
#include "Commissioning.h"
#include "basic.h"
#include "bios.h"

const WanConf_t WanConf_Default =
{
	WAN_CONF_INITIALIZED	,
	{
	  LORAWAN_DEVICE_EUI		,
	  LORAWAN_APPLICATION_EUI	,
	  LORAWAN_APPLICATION_KEY	,
	  {
	    LORAWAN_NETWORK_ID		,
	    LORAWAN_DEVICE_ADDRESS	,
	    LORAWAN_NWKSKEY			,
	    LORAWAN_APPSKEY			,
	  },
	  {
	    LORAWAN_NETWORK_ID		,
	    LORAWAN_DEVICE_ADDRESS	,
	    LORAWAN_NWKSKEY			,
	    LORAWAN_APPSKEY			,
	  },
	},
	{
		{
			WAN_RECV_DELAY1_DEFAULT	,
			WAN_RECV_DELAY2_DEFAULT	,
			WAN_JOIN_DELAY1_DEFAULT	,
			WAN_JOIN_DELAY2_DEFAULT	,
		},
	},
	WAN_JOINMODE_DEFAULT	,
	WAN_CLASS_DEFAULT	,
};

#define WAN_WAIT_ONE_TIME			8000		// 8sec

WanConf_t	WanConf;

const char* const Wan_JoinStr[] = { "OTAA", "ABP" };
const uint8_t WanUi_DrTable[]  = {0,0,11,53,125,242,242};
static Wan_Info_t *Wan_ip = NULL;
//static UiRes_t Wan_error = UI_RES_OK;

//static void WanUi_InitRxDelay( Wan_RxDelay_t *ptr );
static void WanUi_JoinReset( void );
static UiRes_t WanUi_IsClassC( void );
static UiRes_t WanUi_IsJoined( void );
static void WanUi_RestoreRxDelay( void );
static void WanUi_GetRxFrame( WanDataFrame_t *rxframe, bool flag );
#if __WAN_CH_ADD_ENABLE__
static UiRes_t WanUi_RespParaChk( UiRes_t res );
#endif

#define WAN_IP_VALID_CHECK()	do{ if(Wan_ip == NULL) return Ui_PutRespStr(UI_RES_ERROR); }while(0)

/*
 *
 */
void WanUi_Init( void )
{
	static bool initialized = false;

	WanUi_DeveuiInit();
    if( initialized )
	{
		Wan_ip = Wan_init( NULL );
		return;
	}
	initialized = true;
	Wan_ip = Wan_init( &WanConf.JoinKeys );
	if( Wan_ip == NULL ) return;

	Wan_GetJoinStatus();
	if( Wan_ip->JoinStat == WAN_JOINSTAT_OTAA )	Wan_GetJoinKeys();
	WanUi_RestoreRxDelay();
}

/*
 *
 */
void WanUi_Stat( void )
{
	Wan_Keys_t *key_p;
    bios_UartPrintfLine( "Class=%d[%c]", WanConf.Class, 'A'+WanConf.Class );
    bios_UartPuts( "JoinMode=" );
	if( WanUi_IsWanJoinOTAA() )
	{
		key_p = &WanConf.JoinKeys.OTAA_Keys;
	    bios_UartPuts( "1[OTAA]"  );
	}else
	{
		key_p = &WanConf.JoinKeys.ABP_Keys;
	    bios_UartPuts( "2[ABP]"  );
	}
	if( WanUi_GetJoinStatus() ) bios_UartPutChar( '*' );
	bios_UartPutCrLf();
	bios_UartPrintfLine( "TxPwr=%d[%ddBm]"	, Settings.Wan.TxPwr, WanUi_GetTxPower(Settings.Wan.TxPwr) );
	bios_UartPrintfLine( "Dr=%d"			, Settings.Wan.Datarate );
	bios_UartPrintfLine( "FPort=%d"			, Settings.Wan.FPort );
	bios_UartPrintfLine( "Cnf=%d"			, Settings.Wan.Cnf );
	bios_UartPrintfLine( "Adr=%d"			, Settings.Wan.Adr );
	bios_UartPrintfLine( "Trial=%d"			, Settings.Wan.Trial );

	Ui_StatPrintHex ( "DevEui"	, WanConf.JoinKeys.DevEui , DEVEUI_LEN  );
	if( WanUi_IsWanJoinOTAA() )
	{
		Ui_StatPrintHex ( "JoinEui"	, WanConf.JoinKeys.JoinEui , JOINEUI_LEN  );
		if( basic_IsPassLevel_1() )
		{
			Ui_StatPrintHex ( "AppKey"	, WanConf.JoinKeys.AppKey , APPKEY_LEN  );
		}
	}
	bios_UartPrintfLine( "DevAddr=$%08lx", key_p->DevAddr );
	if( basic_IsPassLevel_1() )
	{
		bios_UartPrintfLine( "NetID=$%06lx"	, key_p->NetID );
		Ui_StatPrintHex ( "AppSkey"		, key_p->AppSkey, APPSKEY_LEN );
		Ui_StatPrintHex ( "NwkSkey"		, key_p->NwkSkey, NWKSKEY_LEN );
	}
}

/*
 *
 */
static void WanUi_JoinReset( void )
{
	if( Wan_ip == NULL ) return;
	if( Wan_ip->JoinStat == WAN_JOINSTAT_OTAA )
	{
		WanConf.JoinKeys.OTAA_Keys.DevAddr = 0;
		WanConf.JoinKeys.OTAA_Keys.NetID = 0;
		memset( WanConf.JoinKeys.OTAA_Keys.AppSkey, 0, APPSKEY_LEN );
		memset( WanConf.JoinKeys.OTAA_Keys.NwkSkey, 0, NWKSKEY_LEN );
	}
	WanUi_SetClass( WAN_CLASS_A );
	Wan_ip->JoinStat = WAN_JOINSTAT_NONE;
	Wan_SetJoinStatus( WAN_JOINSTAT_NONE );
	WanUi_RestoreRxDelay();
}

/*
 *
 */
static void WanUi_RestoreRxDelay( void )
{
	if( Wan_ip->JoinStat == WAN_JOINSTAT_NONE )
	{
		Wan_SetRxDelay( WAN_JOIN_DELAY_1 );
		Wan_SetRxDelay( WAN_JOIN_DELAY_2 );
		Wan_SetRxDelay( WAN_RECV_DELAY_1 );
		Wan_SetRxDelay( WAN_RECV_DELAY_2 );
	}
}

/*
 *
 */
static bool WanUi_JoinCheckAndReset( void )
{
	if( WanUi_GetJoinStatus() )
	{
		bios_UartPutLine( "already_joined" );
		return true;
	}
	WanUi_JoinReset();
	return false;
}

/*
 * Join on OTAA�iOver-The-Air Activation)
 */
UiRes_t WanUi_Join_OTAA( void )
{
	WAN_IP_VALID_CHECK();
	if( WanUi_JoinCheckAndReset() )	return UI_RES_OK;

//	WanUi_SetClass( CLASS_A );
	UiRes_t res;
	res = Wan_join_OTAA();
	if( res == UI_RES_OK )
	{
		res = Wan_wait( WAN_WAIT_ONE_TIME );
	}
	if( res != UI_RES_OK )
	{
		MAC_BACKUP_JOIN_SAVE_FLASH();
		return Ui_PutRespStr( res );
	}
	Wan_GetJoinKeys();
	WanUi_SetChMask( Settings.Wan.ChMask );
//	WanUi_SetClass( Settings.Wan.Class );
	WanUi_GetJoinStatus();
	MAC_BACKUP_JOIN_SAVE_FLASH();
	MAC_BACKUP_COUNT_SAVE_FLASH();
	bios_UartPutLine( "join_accepted" );
	return res;
}

/*
 * Join on ABP�iActivation By Personalization�j
 */
UiRes_t WanUi_Join_ABP( void )
{
	WAN_IP_VALID_CHECK();
	if( WanUi_JoinCheckAndReset() )	return UI_RES_OK;

//	WanUi_SetClass( CLASS_A );

	UiRes_t res = Wan_join_ABP();
	if( res != UI_RES_OK )
	{
		MAC_BACKUP_JOIN_SAVE_FLASH();
		return Ui_PutRespStr( res );
	}
	WanUi_SetChMask( Settings.Wan.ChMask );
//	WanUi_SetClass( Settings.Wan.Class );
	WanUi_GetJoinStatus();
	MAC_BACKUP_JOIN_SAVE_FLASH();
	MAC_BACKUP_COUNT_SAVE_FLASH();
	bios_UartPutLine( "joined" );
    return res;
}

/*
 *
 */
Wan_JoinStat_t WanUi_GetJoinStatus( void )
{
	if( Wan_ip == NULL )	return WAN_JOINSTAT_NONE;
	Wan_GetJoinStatus();
	return Wan_ip->JoinStat;
}

/*
 *
 */
UiRes_t WanUi_Unjoin( void )
{
	UiRes_t res = WanUi_IsJoined();

	WanUi_JoinReset();
	MAC_BACKUP_JOIN_SAVE_FLASH();
	if( res != UI_RES_OK )	return res;
	bios_UartPutLine( "Unjoined" );
	return res;
}

/*
 *
 */
static void WanUi_JoinModeNormalization( Wan_JoinMode_t *mode )
{
	if( *mode < WAN_JOINMODE_MIN || *mode > WAN_JOINMODE_MAX ) *mode = WAN_JOINMODE_DEFAULT;
}

/*
 *
 */
UiRes_t WanUi_ConfJoinMode( Wan_JoinMode_t mode )
{
	WAN_IP_VALID_CHECK();
	if( WanUi_GetJoinStatus() != WAN_JOINSTAT_NONE ) return Ui_PutRespStr( UI_RES_DENIED );
	WanUi_JoinModeNormalization( &mode );
	if( mode != WanConf.JoinMode )
	{
		WanConf.JoinMode = mode;
		WanUi_Conf_Save();
	}
	return UI_RES_OK;
}

/*
 *
 */
char *WanUi_GetLinkCheckStr( char *buff )
{
	buff[0] = 0;
	if( Wan_ip->LinkChk.NbGateways )
	{
		sprintf( buff, "#%d,%d", Wan_ip->LinkChk.DemodMargin, Wan_ip->LinkChk.NbGateways );
		buff += strlen( buff );
	}
	return buff;
}

/*
 *
 */
static void WanUi_PrintLinkChk( void )
{
	char buff[10];
	WanUi_GetLinkCheckStr( buff );
	if( buff[0] ) bios_UartPutLine( buff );
}

/*
 *
 */
UiRes_t WanUi_SetLinkchk( void )
{
	WAN_IP_VALID_CHECK();
 	UiRes_t res = UI_RES_OK;
	if( (res = WanUi_IsJoined()) != UI_RES_OK ) return res;
	if( (res = Wan_linkchk()) != UI_RES_OK ) return res;
	MAC_BACKUP_REQUEST_SAVE();
	Wan_ip->LinkChk.Req = true;
	Wan_ip->LinkChk.NbGateways = 0;
	Wan_ip->LinkChk.DemodMargin = 0;
	return res;
}

/*
 *
 */
uint8_t WanUi_GetLinkChk( void )
{
	Wan_ip->LinkChk.Req = Wan_IsExistLinkChk();
	return Wan_ip->LinkChk.Req;
}

/*
 *
 */
static uint8_t WanUi_TimeReq_Callback( uint32_t seconds, uint32_t milli_seconds )
{
	if( seconds < GPS_EPOCH_TO_20JAN01 ) return 1;	// NG
	seconds -= GPS_EPOCH_TO_20JAN01;
	RtcSetCalendarTime( seconds, milli_seconds );
	return 0;
}

/*
 *
 */
UiRes_t WanUi_SetTimeReq( int32_t val )
{
	WAN_IP_VALID_CHECK();
	if( val == 0 )
	{
		if( Wan_ip->DeviceTime.Req == 1 ) return UI_RES_BUSY;
		Wan_ip->DeviceTime.Req = 0;
		return UI_RES_OK;
	}
	if( val != 1 ) return UI_RES_INVALID;
	UiRes_t res;
	if( (res = WanUi_IsJoined()) != UI_RES_OK ) return res;
	if( (res = Wan_DeviceTimeReq( WanUi_TimeReq_Callback )) != UI_RES_OK ) return res;
	MAC_BACKUP_REQUEST_SAVE();
	Wan_ip->DeviceTime.Req = 1;
	return res;
}

uint8_t WanUi_GetTimeReq( void )
{
	if( Wan_ip->DeviceTime.Req < 2 )	Wan_ip->DeviceTime.Req = Wan_IsExistTimeReq();
	return Wan_ip->DeviceTime.Req;
}

/*
 *
 */
uint32_t WanUi_GetDutyTime( void )
{
	return Wan_GetDutyCycleTime();
}

/*
 *
 */
static void WanUi_ClassNormalization( Wan_Class_t *class )
{
	if( *class != WAN_CLASS_C ) *class = WAN_CLASS_A;
}

/*
 *
 */
UiRes_t WanUi_ConfClass( uint8_t class )
{
	WAN_IP_VALID_CHECK();
	if( WanUi_GetJoinStatus() != WAN_JOINSTAT_NONE ) return Ui_PutRespStr( UI_RES_DENIED );
	WanUi_ClassNormalization( &class );
	if( class != WanConf.Class )
	{
		WanConf.Class = class;
		WanUi_Conf_Save();
	}
	return UI_RES_OK;
}

/*
 *
 */
UiRes_t WanUi_SetClass( uint8_t class )
{
	if( WanUi_GetJoinStatus() == WAN_JOINSTAT_NONE ) return UI_RES_OK;
	WanUi_ClassNormalization( &class );
	Wan_GetClass();
	if( class == Wan_ip->Class )	return UI_RES_OK;
	return	Wan_SetClass( class );
}

/*
 *
 */
bool WanUi_RxProcess( WanDataFrame_t *rxframe )
{
	if( !Wan_RxCheck() ) return false;
	WanUi_GetRxFrame( rxframe, 0 );
	MAC_BACKUP_COUNT_SAVE_FLASH();
	return true;
}

/*
 *
 */
static void WanUi_GetRxFrame( WanDataFrame_t *rxframe, bool flag )
{
	Wan_RxPara_t *rxp = Wan_ip->RxPara_ptr;

	StateInf.SnrValue	= rxp->Snr;
	StateInf.RssiValue	= rxp->Rssi;
	if( flag )	return;
	rxframe->LinkCounter[0]	= (uint16_t)(Wan_ip->DownLinkCounter);
	rxframe->LinkCounter[1]	= (uint16_t)(Wan_ip->DownLinkCounter >> 16);
	rxframe->Dr			= rxp->Datarate;
	rxframe->Port		= rxp->Port;
	rxframe->Length		= rxp->Len;
	rxframe->Rssi		= rxp->Rssi;
	StateInf.BufferPtr	= (uint8_t*)rxframe;
}

/*
 *
 */
static bool WanUi_PrintRx( WanDataFrame_t *rxframe, uint8_t code, uint8_t stat )
{
	switch( stat )
	{
	case UI_RES_RX :
		if( Wan_ip->IsPrinted == false )
		{
			WanUi_GetRxFrame( rxframe, 0 );	// Frame
			PvtUi_PrintRxd( rxframe->Port, code );
			Wan_ip->IsPrinted = true;
		}
		return false;
	case UI_RES_OK :
		WanUi_GetRxFrame( rxframe, 1 );		// Snr, Rssi
		return false;
	default :
		Ui_PutRespStr( stat );
		return true;
	}
}

/*
 *
 */
static void WanUi_SetTxPara( void )
{
	Wan_TxPara_t *txp = Wan_ip->TxPara_ptr;

	Wan_ip->Adr		= Settings.Wan.Adr;
	txp->Confirmed	= Settings.Wan.Cnf;
	txp->Datarate	= Settings.Wan.Datarate;
	txp->Power		= Settings.Wan.TxPwr;
	txp->Port		= Settings.Wan.FPort;
	txp->Trial		= (Settings.Wan.Cnf && Settings.Wan.Trial) ? Settings.Wan.Trial : WAN_TRIAL_DEFAULT;
}

/*
 *
 */
static UiRes_t WanUi_IsClassC( void )
{
	WAN_IP_VALID_CHECK();
	if( WanConf.Class == WAN_CLASS_C )	return UI_RES_OK;
	return Ui_PutRespStr( UI_RES_INCOMPATIBLE_CLASS );
}

/*
 *
 */
static UiRes_t WanUi_IsJoined( void )
{
	WAN_IP_VALID_CHECK();
	Wan_GetJoinStatus();
	if( Wan_ip->JoinStat != WAN_JOINSTAT_NONE )	return UI_RES_OK;
	return Ui_PutRespStr( UI_RES_NOT_JOINED );
}

/*
 *
 */
UiRes_t WanUi_Tx( WanDataFrame_t *txframe, WanDataFrame_t *rxframe, uint8_t code )
{
	UiRes_t res;
	if( (res = WanUi_IsJoined()) != UI_RES_OK )	return res;

	WanUi_SetTxPara();
	Wan_ip->TxPara_ptr->Len      = txframe->Length;
	Wan_ip->TxPara_ptr->Data_ptr = txframe->Data;
	Wan_ip->RxPara_ptr->Data_ptr = rxframe->Data;

	WanUi_GetLinkChk();
	res = Wan_send();
	if( res == UI_RES_OK )
	{	
		uint32_t tout = Wan_ip->TxPara_ptr->Trial * WAN_WAIT_ONE_TIME;
		res = Wan_wait( tout );
		if( Wan_ip->LinkChk.Req )
		{
			if( Wan_ip->LinkChk.NbGateways ) Wan_ip->LinkChk.Req = false;
			if( code != ENCODE_IGNORE )	WanUi_PrintLinkChk();
		}
		txframe->LinkCounter[0]	= (uint16_t)(Wan_ip->Sent.UpLinkCounter);
		txframe->LinkCounter[1]	= (uint16_t)(Wan_ip->Sent.UpLinkCounter >> 16);
		txframe->Ch = Wan_ip->Sent.Channel;
		txframe->Dr = Wan_ip->Sent.Datarate;
		txframe->Port = Wan_ip->TxPara_ptr->Port;
		Wan_ip->IsPrinted = false;
	}
	MAC_BACKUP_REQUEST_SAVE();
	MAC_BACKUP_COUNT_SAVE_FLASH();
	WanUi_PrintRx( rxframe, code, res );
	return res;
}

/*
 *
 */
UiRes_t WanUi_Rx( WanDataFrame_t *rxframe, int32_t timeout, uint8_t code )
{
	UiRes_t res;
	if( (res = WanUi_IsJoined()) != UI_RES_OK )	return res;
	if( (res = WanUi_IsClassC()) != UI_RES_OK )	return res;

	WanUi_SetClass( WAN_CLASS_C );

	if( timeout > 0 )	
	{
		bios_setWaitTick( timeout );
	}

	Wan_ip->RxPara_ptr->Data_ptr = rxframe->Data;
	while( 1 )
	{
		if( Wan_RxCheck() )
		{
			res = UI_RES_RX;
			MAC_BACKUP_REQUEST_SAVE();
			MAC_BACKUP_COUNT_SAVE_FLASH();
			WanUi_PrintRx( rxframe, code, res );
			Wan_RxClear();
			if( timeout >= -1 ) break;
		}
		if( timeout == 0 ) break;
		if( timeout > 0 && bios_getWaitTick() == 0 )
		{
			res = UI_RES_TIMEOUT;
			break;
		}
		if( Ui_IsBreak_LP() )
		{
			WanUi_SetClass( WAN_CLASS_A );
			break;
		}
	}
	Ui_PutRespStr( res );
//	MAC_BACKUP_COUNT_SAVE_FLASH();
	return res;
}

/*
 *
 */
UiRes_t	WanUi_RxStop( void )
{
	UiRes_t res;
	if( (res = WanUi_IsJoined()) != UI_RES_OK )	return res;
	if( (res = WanUi_IsClassC()) != UI_RES_OK )	return res;

	WanUi_SetClass( WAN_CLASS_A );
	return res;
}

/*
 * ��M�p�� (Stat=0 �ŌĂ΂��j
 */
void WanUi_RxContinue( void )
{
	if( Ui_IsWanMode() == false ) return;
	Wan_RxClear();
}

/*
 *
 */
uint16_t WanUi_GetChMask( void )
{
	if( Wan_ip == NULL )	return 0;
	return Wan_ip->ChannelsMask;
}

/*
 *
 */
void WanUi_SetChMask( uint16_t ChannelsMask )
{
	if( Wan_ip == NULL )	return;
	Wan_ip->ChannelsMask = ChannelsMask;
	Wan_SetChMask();
}

/*
 *
 */
uint32_t WanUi_GetUpLinkCount( void )
{
	if( Wan_ip == NULL )	return 0;
	Wan_GetUplinkCounter( );
	return Wan_ip->UpLinkCounter;
}

/*
 *
 */
uint32_t WanUi_GetDonwLinkCount( void )
{
	if( Wan_ip == NULL )	return 0;
    Wan_GetDownlinkCounter();
	return Wan_ip->DownLinkCounter;
}

/*
 *
 */
void WanUi_SetUpLinkCount( uint32_t linkCount )
{
	if( Wan_ip == NULL )	return;
	Wan_ip->UpLinkCounter = linkCount;
	Wan_SetUplinkCounter();
}

/*
 *
 */
void WanUi_SetDownLinkCount( uint32_t linkCount )
{
	if( Wan_ip == NULL )	return;
	Wan_ip->DownLinkCounter = linkCount;
    Wan_SetDownlinkCounter();
}

#if 0
/*
 *
 */
static void WanUi_InitRxDelay( Wan_RxDelay_t *ptr )
{
	if( Wan_ip == NULL )	return;
	Wan_ip->RxDelay_ptr = ptr;
	if( ptr->Rx1 < WAN_RX_DELAY_MIN || ptr->Rx1 >= WAN_RX_DELAY_MAX )
	{
		Wan_GetRxDelay();
		return;
	}
	Wan_SetRxDelay();
}
#endif

/*
 *
 */
void WanUi_SettingsDefault( void )
{
	Settings.Wan.WanInit  = WAN_SETTINGS_INITIALIZED;
	Settings.Wan.TxPwr    = WAN_TXPOWER_DEFAULT;
    Settings.Wan.Datarate = WAN_DR_DEFAULT;
	Settings.Wan.FPort    = WAN_FPORT_DEFAULT;
	Settings.Wan.Cnf      = WAN_CNF_DEFAULT;
    Settings.Wan.Adr      = WAN_ADR_DEFAULT;
    Settings.Wan.Trial    = WAN_TRIAL_DEFAULT;
    Settings.Wan.ChMask   = WAN_CHMASK_DEFAULT;

	memcpy((uint8_t*)&WanConf.RxDelay, (uint8_t*)&WanConf_Default.RxDelay, sizeof(Wan_RxDelay_t));
}

/*
 *
 */
void WanUi_Conf_Init( bool force )
{
	memcpy( &WanConf, (void*)FLASH_WANCONF_ADRS, sizeof(WanConf_t) );
	if( force || WanConf.Initialized != WAN_CONF_INITIALIZED )
	{
		memcpy( &WanConf, &WanConf_Default, sizeof(WanConf_t) );
		WanUi_DeveuiInit();
		WanUi_Conf_Save();
	}
	WanUi_JoinModeNormalization( &WanConf.JoinMode );
	WanUi_ClassNormalization( &WanConf.Class );
}

/*
 *
 */
uint8_t WanUi_Conf_Save( void )
{
	return bios_FlashWrite( FLASH_WANCONF_ADRS, (uint8_t *)&WanConf, sizeof(WanConf_t) );
}

void WanUi_DeveuiCopy( void )
{
	memcpy( WanConf.JoinKeys.DevEui, Adjust.Device_Id, DEVEUI_LEN );
}

/*
 *
 */
void WanUi_DeveuiInit( void )
{
	for( uint8_t i = 0; i < DEVEUI_LEN; i++ ) if( WanConf.JoinKeys.DevEui[i] )	return;
	WanUi_DeveuiCopy();
}

/*
 *
 */
uint8_t WanUi_GetLength( void )
{
	uint8_t len;

	WanUi_SetTxPara();
	Wan_GetPayloadLength( &len );
	return len;
}

/*
 *
 */
uint8_t WanUi_GetDwellStat( void )
{
	uint8_t dwell_stat;
	Wan_GetDwellStat( &dwell_stat );
	return dwell_stat;
}

/*
 *
 */
uint8_t WanUi_GetLastTxCh( void )
{
	uint8_t ch;
	Wan_GetLastTxChannel( &ch );
	return ch;
}

/*
 *
 */
int8_t WanUi_GetTxPower( uint8_t pwr )
{
	static const int8_t pwr_dbm[] = {13, 11, 9, 7, 5, 3, 1, -1 };
	if( pwr > 7 ) return pwr = 0;
	return pwr_dbm[pwr];
}

/*
 *
 */
void WanUi_ChList( void )
{
	ChannelParams_t *ch_list;
	uint16_t ch_mask = Wan_ip->ChannelsMask;
	
//	Wan_GetChMask( &ch_mask );
	Wan_GetChannel( &ch_list );
	for( uint8_t i = 1; i <= 16; i++ )
	{
		if( ch_list->Frequency )
		{
			bios_UartPrintfLine( "%u:%lu%c,DR%d-%d", i
				, ch_list->Frequency
				, ((ch_mask & 1)?'*':' ')
				, ch_list->DrRange.Fields.Min
				, ch_list->DrRange.Fields.Max );
		}
		ch_mask >>= 1;
		ch_list++;
	}
}

/*
 *
 */
uint8_t WanUi_GetPending( void )
{
	if( Wan_ip == NULL )	return 0;
	return Wan_ip->RxPara_ptr->Pending;
}

/*
 *
 */
uint8_t *WanUi_GetMacCommand( uint8_t *len )
{
	return Wan_GetMacCommand( len );
}

#if __WAN_RXDELAY_ENABLE__
/*
 *
 */
void WanUi_SetRxDelay( Wan_RxDelay_e rxDelay )
{
	if( Wan_ip == NULL )	return;
	Wan_GetJoinStatus();
	if( Wan_ip->JoinStat == WAN_JOINSTAT_NONE )	return;
	Wan_SetRxDelay( rxDelay );
}

/*
 *
 */
uint16_t WanUi_GetRxDelay( Wan_RxDelay_e rxDelay )
{
	if( Wan_ip == NULL )	return 0;
	Wan_GetRxDelay( rxDelay );
	return Wan_ip->RxDelay_now.RxDelay[rxDelay];
}
#endif

#if __WAN_CH_ADD_ENABLE__
/*
 *
 */
static UiRes_t WanUi_RespParaChk( UiRes_t res )
{
	if( res == UI_RES_INVALID )	return res;
	if( res != UI_RES_OK )	Ui_PutRespStr( res );
	return UI_RES_OK;
}

/*
 *
 */
UiRes_t WanUi_ChannelAdd( uint8_t id, uint32_t freq, uint8_t dr_min, uint8_t dr_max )
{
	UiRes_t res = Wan_ChannelAdd( id-1, freq, dr_min, dr_max );
	return WanUi_RespParaChk( res );
}

/*
 *
 */
UiRes_t WanUi_ChannelRemove( uint8_t id )
{
	UiRes_t res = Wan_ChannelRemove( id-1 );
	return WanUi_RespParaChk( res );
}
#endif
#endif

/*
 *
 */
void WanUi_SetBatLevel( uint8_t level )
{
	BoardSetBatteryLevel( level );
}

/*
 *
 */
uint8_t WanUi_GetBatLevel( void )
{
	return BoardGetBatteryLevel();
	
}

