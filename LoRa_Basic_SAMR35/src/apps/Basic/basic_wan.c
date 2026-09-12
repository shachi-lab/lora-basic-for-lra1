/*!
 * @file    bacic_wan.c
 * @brief   LoRa-Wan extension for Basic Interpreter
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
#include "board-mcu.h"
#include "bme280-board.h"
#include "basic.h"
#include "bios.h"

#define WAN_MODE_LEVEL( mode, level )		((mode << 4) + level)
#define WAN_JOINMODE_BOTH	(WAN_JOINMODE_OTAA|WAN_JOINMODE_ABP)
typedef enum
{
	WAN_OTAA_L0	=	WAN_MODE_LEVEL( WAN_JOINMODE_OTAA, PassLevel_0 ),
	WAN_OTAA_L1	=	WAN_MODE_LEVEL( WAN_JOINMODE_OTAA, PassLevel_1 ),
	WAN_OTAA_L2	=	WAN_MODE_LEVEL( WAN_JOINMODE_OTAA, PassLevel_2 ),
	WAN_ABP_L0	=	WAN_MODE_LEVEL( WAN_JOINMODE_ABP , PassLevel_0 ),
	WAN_ABP_L1	=	WAN_MODE_LEVEL( WAN_JOINMODE_ABP , PassLevel_1 ),
	WAN_ABP_L2	=	WAN_MODE_LEVEL( WAN_JOINMODE_ABP , PassLevel_2 ),
	WAN_NONE_L0	=	WAN_MODE_LEVEL( WAN_JOINMODE_BOTH, PassLevel_0 ),
	WAN_NONE_L1	=	WAN_MODE_LEVEL( WAN_JOINMODE_BOTH, PassLevel_1 ),
	WAN_NONE_L2	=	WAN_MODE_LEVEL( WAN_JOINMODE_BOTH, PassLevel_2 ),
}Wan_ModeLevel_t;

#if __WAN_RXDELAY_ENABLE__
static errCode_t wan_SetDelayRxVal( Wan_RxDelay_e rxDelay );
#endif
static errCode_t wan_CheckModeLevel( Wan_ModeLevel_t modelevel );

/*
 *
 */
static errCode_t proc_wan_mode_check( uint8_t ch )
{
	if ( Ui_IsWanMode() ) return errorCode;
	if ( ch == VAR_WAN_MODE ) return errorCode;
	return errorCode = ERROR_MODE;
}

/*
 *
 */
uint8_t proc_wan_exec( uint8_t ch )
{
	if ( ch != ST_CODE_WAN )	return 1;
	ch = *executionPointer++;
	if ( proc_wan_mode_check( ch ) )	return 0;
	if ( (ch >= ST_WAN_CODE_BEGIN) && (ch <= ST_WAN_CODE_END) )
	{
		uint8_t index = ch - ST_WAN_CODE_BEGIN;
		if ( procWanCodeList[index] )
		{
			(*procWanCodeList[index])(ch);
		}
		return 0;	
	}
	basic_WanSetVar( ch );
	return 0;
}

/*
 *
 */
bool basic_WanInit( void )
{
	if ( Ui_IsWanMode() == 0 )	return false;
	WanUi_Init();
	return true;
}

/*
 * Wan_Join
 */
PROC( proc_wan_join )
{
	if ( checkDelimiter() ) return;
	if ( WanUi_IsWanJoinOTAA() ) { basic_LoRa_stat = WanUi_Join_OTAA(); return; }
	if ( WanUi_IsWanJoinABP () ) { basic_LoRa_stat = WanUi_Join_ABP (); return; }
}

/*
 * Wan_UnJoin
 */
PROC( proc_wan_unjoin )
{
	if ( checkDelimiter() ) return;
	basic_LoRa_stat = WanUi_Unjoin();
}

/* Wan_LinkChk
 *
 */
PROC( proc_wan_linkchk )
{
	if ( checkDelimiter() ) return;
	basic_LoRa_stat = WanUi_SetLinkchk();
}

/*
 * Wan_Tx
 */
PROC( proc_wan_tx )
{
	uint8_t code = ENCODE_BINARY;
	int16_t len = 0;

	if ( basic_LoRa_GetCodecPara( &code ) && *executionPointer == ST_STRING )
	{
		code = ENCODE_BINARY;
		executionPointer--;
	}
	if ( !isExeDelimiter( ) )	// ��������
	{
		if ((len = basic_LoRa_SetTxd()) < 0 ) return;
	}
	basic_LoRa_stat = WanUi_Tx( (WanDataFrame_t*)&basic_LoRa_TxFrame, (WanDataFrame_t*)&basic_LoRa_RxFrame, code );
}

/*
 * Wan_Rx
 */
PROC( proc_wan_rx )
{
	uint8_t code = ENCODE_BINARY;
	var_t val = -1;

	if ( basic_LoRa_GetRecvPara( &code, &val ) )	return;
	basic_LoRa_stat = WanUi_Rx( (WanDataFrame_t*)&basic_LoRa_RxFrame, val, code );
}

/*
 * Wan_Save
 */
PROC( proc_wan_save )
{
	if ( checkDelimiter() ) return;
	if ( WanUi_Conf_Save() )
	{
		errorCode = ERROR_FLASH;
	}
	MAC_BACKUP_JOIN_SAVE_FLASH();
	MAC_BACKUP_COUNT_SAVE_FLASH();
}

/*
 * Wan_ChList
 */
PROC( proc_wan_chlist )
{
	if ( checkDelimiter() ) return;
	WanUi_ChList();
}

/*
 * Wan_RxStop
 */
PROC( proc_wan_rxstop )
{
	if ( checkDelimiter() ) return;
	WanUi_RxStop();
}

#if __WAN_CH_ADD_ENABLE__
/*
 * Wan_ChAdd
 */
PROC( proc_wan_chadd )
{
	var_t val[4] = {-1};

	if ( wan_CheckModeLevel( WAN_ABP_L1 )) return;
	get_XargVal_def( val, 4 );
	for ( uint8_t i = 0; i < 4; i++ )
	{
		if ( val[i] < 0 ) errorCode = ERROR_PARA; 
	}
	if ( errorCode == ERROR_OK )
	{
		UiRes_t res = WanUi_ChannelAdd( val[0], val[1], val[2], val[3] );
		if ( res == UI_RES_INVALID )	 errorCode = ERROR_PARA;
	}
}

/*
 * Wan_ChRemove
 */
PROC( proc_wan_chremove )
{
	var_t val;

	if ( wan_CheckModeLevel( WAN_ABP_L1 )) return;
	errorCode = get_1argVal( &val );
	if ( errorCode == ERROR_OK )
	{
		UiRes_t res = WanUi_ChannelRemove( val );
		if ( res == UI_RES_INVALID )	 errorCode = ERROR_PARA;
	}
}
#endif

/*
 *
 */
void basic_Wan_Process( void )
{
	if ( Ui_IsWanMode() == false )	return;
	if( basic_LoRa_stat == UI_RES_RX ) return;
	if( !WanUi_RxProcess( (WanDataFrame_t*)&basic_LoRa_RxFrame ) ) return;
	basic_LoRa_stat = UI_RES_RX;
}

/*
 *
 */
static errCode_t wan_CheckModeLevel( Wan_ModeLevel_t modelevel )
{
	if ( basic_CheckPassLevel( modelevel & 0x03 ) )	return errorCode;
	if ( WanConf.JoinMode & (modelevel >> 4) ) return errorCode;
	Ui_PutRespStr( UI_RES_DENIED );
//	errorCode = ERROR_JOINMODE;
	return errorCode;
}

/*
 *
 */
static errCode_t wan_SetConfKey( Wan_ModeLevel_t modelevel, void *buff, uint8_t len )
{
	if ( wan_CheckModeLevel( modelevel ) )	return errorCode;
	if ( WanUi_GetJoinStatus() )
	{
		Ui_PutRespStr( UI_RES_DENIED );
		return errorCode;
	}
	if ( basic_SetKey( buff, len ) == 0 )
	{
		WanUi_Conf_Save();
	}
	return errorCode;
}

/*
 *
 */
static char *wan_SetHexBuff( Wan_ModeLevel_t modelevel, char *buff, uint8_t *ptr, uint8_t len )
{
	if ( wan_CheckModeLevel( modelevel ) )	return NULL;
	return Ui_SetHexBuff( buff, ptr, len );
}

/*
 *
 */
static errCode_t wan_SetConfClass( uint8_t class )
{
#if _WAN_CLASS_C_ == false
	if ( class != WAN_CLASS_A )	errorCode = ERROR_PARA;
#else
	if ( class != WAN_CLASS_A && class != WAN_CLASS_C )	return errorCode = ERROR_PARA;
	WanUi_ConfClass( class );
#endif
	return errorCode;
}

/*
 *
 */
static Wan_Keys_t *Wan_Key_ptr( void )
{
	return WanUi_IsWanJoinOTAA() ? &WanConf.JoinKeys.OTAA_Keys : &WanConf.JoinKeys.ABP_Keys;
}

/*
 *
 */
errCode_t basic_WanSetVar( uint8_t ch )
{ 
	var_t val;

	switch ( ch )
	{
	case VAR_WAN_MODE     : if ( !basic_SetVarSub( &val, LORA_MODE_P2P   , LORA_MODE_WAN    ) ) { Settings.Lora_Mode    = val; } break;
	case VAR_WAN_CLASS	  : if ( !proc_LetSpecial( &val ) ) { return wan_SetConfClass( val ); } break;
	case VAR_WAN_TXPWR	  : if ( !basic_SetVarSub( &val, WAN_TXPOWER_MIN , WAN_TXPOWER_MAX  ) ) { Settings.Wan.TxPwr    = val; } break;
	case VAR_WAN_FPORT	  : if ( !basic_SetVarSub( &val, WAN_FPORT_MIN   , WAN_FPORT_MAX    ) ) { Settings.Wan.FPort    = val; } break;
	case VAR_WAN_CNF	  : if ( !basic_SetVarSub( &val, WAN_CNF_MIN     , WAN_CNF_MAX      ) ) { Settings.Wan.Cnf      = val; } break;
	case VAR_WAN_ADR	  :	if ( !basic_SetVarSub( &val, WAN_ADR_MIN     , WAN_ADR_MAX      ) ) { Settings.Wan.Adr      = val; } break;
	case VAR_WAN_DR		  : if ( !basic_SetVarSub( &val, WAN_DR_MIN      , WAN_DR_MAX       ) ) { Settings.Wan.Datarate = val; } break;
	case VAR_WAN_TRIAL	  : if ( !basic_SetVarSub( &val, WAN_TRIAL_MIN   , WAN_TRIAL_MAX    ) ) { Settings.Wan.Trial    = val; } break;
	case VAR_WAN_JOINMODE : if ( !basic_SetVarSub( &val, WAN_JOINMODE_MIN, WAN_JOINMODE_MAX ) ) { WanUi_ConfJoinMode( val );    } break;
	case VAR_WAN_DEVEUI	  :	return wan_SetConfKey( WAN_NONE_L1, WanConf.JoinKeys.DevEui	, DEVEUI_LEN );
	case VAR_WAN_JOINEUI  :	return wan_SetConfKey( WAN_OTAA_L1, WanConf.JoinKeys.JoinEui , JOINEUI_LEN );
	case VAR_WAN_APPKEY	  :	return wan_SetConfKey( WAN_OTAA_L1, WanConf.JoinKeys.AppKey , APPKEY_LEN );
	case VAR_WAN_DEVADDR  :	return wan_SetConfKey( WAN_ABP_L1 , &WanConf.JoinKeys.ABP_Keys.DevAddr, sizeof(uint32_t) );
	case VAR_WAN_NETID    :	return wan_SetConfKey( WAN_ABP_L1 , &WanConf.JoinKeys.ABP_Keys.NetID  , sizeof(uint32_t) );
	case VAR_WAN_APPSKEY  :	return wan_SetConfKey( WAN_ABP_L1 , WanConf.JoinKeys.ABP_Keys.AppSkey , APPSKEY_LEN );
	case VAR_WAN_NWKSKEY  :	return wan_SetConfKey( WAN_ABP_L1 , WanConf.JoinKeys.ABP_Keys.NwkSkey , NWKSKEY_LEN );
	case VAR_WAN_UPCNT	  :	if ( !proc_LetSpecial( &val ) ) { WanUi_SetUpLinkCount  ( val ); } break;
	case VAR_WAN_DOWNCNT  :	if ( !proc_LetSpecial( &val ) ) { WanUi_SetDownLinkCount( val ); } break;
	case VAR_WAN_CHMASK	  : if ( !proc_LetSpecial( &val ) ) { WanUi_SetChMask( Settings.Wan.ChMask = val ); } break;
#if __WAN_RXDELAY_ENABLE__
	case VAR_WAN_RECVDELAY1 :
	case VAR_WAN_RECVDELAY2 :
	case VAR_WAN_JOINDELAY1 :
	case VAR_WAN_JOINDELAY2 : return wan_SetDelayRxVal( WAN_RECV_DELAY_1 + (ch - VAR_WAN_RECVDELAY1) );
#endif
	case VAR_WAN_TIMEREQ  : if ( !basic_SetVarSub( &val, 0, 1 ) ) { WanUi_SetTimeReq( val ); } break;
	case VAR_WAN_BATLEVEL : if ( !basic_SetVarSub( &val, LORA_BATLEVEL_MIN, LORA_BATLEVEL_MAX) ) { WanUi_SetBatLevel( val ); } break;
	case VAR_WAN_JOINSTAT :
	case VAR_WAN_LENGTH   :
	case VAR_WAN_DUTYTIME : 
	case VAR_WAN_DWELLSTAT:
	case VAR_WAN_LASTTXCH :
	default				  :	errorCode = ERROR_SYNTAX;
	}
	return errorCode;
}

/*
 *
 */
void basic_WanGetVar( uint8_t ch, var_t *val )
{ 
	if ( ch == VAR_WAN_MODE ) { *val = Settings.Lora_Mode; return; }
	if ( proc_wan_mode_check( ch ) ) return;

	switch ( ch )
	{
	case VAR_WAN_CLASS	  :	*val = WanConf.Class;			return;
	case VAR_WAN_TXPWR    : *val = Settings.Wan.TxPwr;		return;
	case VAR_WAN_FPORT	  :	*val = Settings.Wan.FPort;		return;
	case VAR_WAN_CNF	  :	*val = Settings.Wan.Cnf;		return;
	case VAR_WAN_ADR	  :	*val = Settings.Wan.Adr;		return;
	case VAR_WAN_DR		  :	*val = Settings.Wan.Datarate;	return;
	case VAR_WAN_TRIAL	  :	*val = Settings.Wan.Trial;		return;
	case VAR_WAN_JOINMODE : *val = WanConf.JoinMode;		return;
	case VAR_WAN_DEVEUI	  :	*val = 0; return;
	case VAR_WAN_JOINEUI  :	*val = 0; return;
	case VAR_WAN_APPKEY	  :	*val = 0; return;
	case VAR_WAN_DEVADDR  :	*val = Wan_Key_ptr()->DevAddr;	return;
	case VAR_WAN_NETID	  :	*val = basic_IsPassLevel_1() ? Wan_Key_ptr()->NetID   : 0; return;
	case VAR_WAN_APPSKEY  :	*val = 0; return;
	case VAR_WAN_NWKSKEY  :	*val = 0; return;
	case VAR_WAN_JOINSTAT :	*val = WanUi_GetJoinStatus();	return;
	case VAR_WAN_UPCNT	  :	*val = WanUi_GetUpLinkCount();	return;
	case VAR_WAN_DOWNCNT  :	*val = WanUi_GetDonwLinkCount();return;
	case VAR_WAN_CHMASK	  :	*val = Settings.Wan.ChMask;		return;
	case VAR_WAN_PENDING  : *val = WanUi_GetPending();return;
#if __WAN_RXDELAY_ENABLE__
	case VAR_WAN_RECVDELAY1 :
	case VAR_WAN_RECVDELAY2 :
	case VAR_WAN_JOINDELAY1 :
	case VAR_WAN_JOINDELAY2 : *val = WanUi_GetRxDelay( WAN_RECV_DELAY_1 + (ch - VAR_WAN_RECVDELAY1) ); return;
#endif
	case ST_WAN_LINKCHK   : *val = WanUi_GetLinkChk(); return;
	case VAR_WAN_LENGTH   :	*val = WanUi_GetLength(); return;
	case VAR_WAN_TIMEREQ  :	*val = WanUi_GetTimeReq(); return;
	case VAR_WAN_DUTYTIME : *val = WanUi_GetDutyTime(); return;
	case VAR_WAN_DWELLSTAT: *val = WanUi_GetDwellStat(); return;
	case VAR_WAN_LASTTXCH : *val = WanUi_GetLastTxCh(); return;
	case VAR_WAN_BATLEVEL : *val = WanUi_GetBatLevel(); return;
	}
	errorCode = ERROR_SYNTAX;
}

/*
 *
 */
char *basic_Wan_Print( uint8_t ch, char *buff )
{
	if ( proc_wan_mode_check( ch ) )	return NULL;
	switch ( ch )
	{
	case VAR_WAN_DEVEUI	:	return Ui_SetHexBuff( buff, WanConf.JoinKeys.DevEui , DEVEUI_LEN );
	case VAR_WAN_JOINEUI:	return wan_SetHexBuff( WAN_OTAA_L0, buff, WanConf.JoinKeys.JoinEui, JOINEUI_LEN );
	case VAR_WAN_APPKEY	:	return wan_SetHexBuff( WAN_OTAA_L1, buff, WanConf.JoinKeys.AppKey, APPKEY_LEN );
	case VAR_WAN_APPSKEY:	return wan_SetHexBuff( WAN_NONE_L1, buff, Wan_Key_ptr()->AppSkey, APPSKEY_LEN );
	case VAR_WAN_NWKSKEY:	return wan_SetHexBuff( WAN_NONE_L1, buff, Wan_Key_ptr()->NwkSkey, NWKSKEY_LEN );
	case ST_WAN_LINKCHK :	return WanUi_GetLinkCheckStr( buff );
	case VAR_WAN_MACCMD :	{ uint8_t len, *ptr = WanUi_GetMacCommand( &len );
							return wan_SetHexBuff( WAN_NONE_L2, buff, ptr, len ); }
	}
	return NULL;
}

#if __WAN_RXDELAY_ENABLE__
/*
 *
 */
static errCode_t wan_SetDelayRxVal( Wan_RxDelay_e rxDelay )
{
	var_t val;
	if ( basic_CheckPassLevel( PassLevel_1 ) ) return errorCode;
	if ( basic_SetVarSub( &val, WAN_RX_DELAY_MIN, WAN_RX_DELAY_MAX ) )	return errorCode;
	WanConf.RxDelay.RxDelay[rxDelay] = val;
	WanUi_Conf_Save();
	WanUi_SetRxDelay( rxDelay );
	return errorCode;
}
#endif
#endif
