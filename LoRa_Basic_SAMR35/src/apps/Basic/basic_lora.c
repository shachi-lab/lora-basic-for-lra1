/*!
 * @file    bacic_lora.c
 * @brief   LoRa extension for Basic Interpreter
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
#include "asf.h"
#include "board-mcu.h"
#include "bme280-board.h"
#include "sx1276-board.h"
#include "basic.h"
#include "basic_sub.h"
#include "bios.h"

DataFrame_t basic_LoRa_TxFrame;
DataFrame_t basic_LoRa_RxFrame;

uint8_t	basic_LoRa_stat = 0;

static bool basic_LoRa_wakeup_pin_old = false;
static uint8_t	basic_LoRa_Comm_Code = 0;

#define LORA_I2C_BUFF_SIZE			32
uint8_t basic_LoRa_I2c_buff[LORA_I2C_BUFF_SIZE];

static void basic_LoRa_Txd( void );
static uint8_t basic_LoRa_Comm_Hook( void );
static uint8_t basic_LoRa_Stat_Callback( uint8_t stat );
static void basic_LoRa_Txd_size( DataFrame_t *frame, uint8_t size );
static bool basic_LoRa_TRxd_size( DataFrame_t *frame, uint8_t size, var_t *val );
static var_t basic_LoRa_I2cD_Get( void );
static void basic_LoRa_I2cD_Set( void );
static var_t basic_LoRa_IsFree( void );
static var_t basic_LoRa_TxLen( void );
static var_t basic_LoRa_GetSens( void );

/*
 *
 */
void basic_LoRa_Init( void )
{
	static bool initialized = false;

#ifdef __UI_LORA_WAN__
	if ( basic_WanInit() )	return;
#endif
	Pvt_RadioInit();
	Pvt_SetCallback( basic_LoRa_Stat_Callback );
	if ( initialized ) return;
	Pvt_SetFrameHeader( &basic_LoRa_TxFrame, 0x100 );
	initialized = true;
}

/*
 *
 */
errCode_t basic_LoRa_Pvt_Check( void )
{
#ifdef __UI_LORA_WAN__
	if ( Ui_IsWanMode() )	errorCode = ERROR_MODE;
#endif
	return errorCode;
}

static errCode_t basic_LoRa_SetVerSubLevel2( var_t *val, var_t min, var_t max )
{
	if ( basic_CheckPassLevel_2() ) return errorCode;
	return basic_SetVarSub( val, min, max);
}

/*
 *
 */
uint8_t basic_LoRa_SetVar( uint8_t ch )
{
	var_t val;
	uint8_t minmax;

	switch ( ch )
	{
	case VAR_LORA_PWR :
		minmax = basic_IsPassLevel_2() ? LORA_POWER_MAX_ADMIN : LORA_POWER_MAX;
		if ( !basic_SetVarSub( &val, LORA_POWER_MIN, minmax ) )	Settings.Power = val;
		break;
	case VAR_LORA_SF :
		if ( !basic_SetVarSub( &val, LORA_SF_MIN, LORA_SF_MAX ) )
		{
			if ( Settings.Bw == LORA_BW_500k && val < 10 ) { errorCode = ERROR_PARA; break; }
			Settings.Sf = val;
		}
		break;
	case VAR_LORA_BW :
		minmax = basic_IsPassLevel_2() ? LORA_BW_MIN_ADMIN : LORA_BW_MIN;
		if ( !basic_SetVarSub( &val, minmax, LORA_BW_MAX ) )
		{
			if ( PvtUi_ChBwToFreq( Settings.Modem, Settings.Ch, val ) ) { errorCode = ERROR_PARA; break; }
			if ( val == LORA_BW_500k && Settings.Sf < 10 ) { errorCode = ERROR_PARA; break; }
			Settings.Bw = val;
		}
		break;
	case VAR_LORA_CR :
		if ( !basic_SetVarSub( &val, LORA_CR_MIN, LORA_CR_MAX ) )	Settings.Cr = val;
		break;
	case VAR_LORA_CH :
		if ( !basic_SetVarSub( &val, LORA_CH_MIN, LORA_CH_MAX ) )
		{
			if ( PvtUi_ChBwToFreq( Settings.Modem, val, Settings.Bw ) ) { errorCode = ERROR_PARA; break; }
			Settings.Ch = val;
		}
		break;
	case VAR_LORA_GID :
		if ( !basic_SetVarSub( &val, LORA_GID_MIN, LORA_GID_MAX ) ) {	Settings.Gid = basic_LoRa_TxFrame.GrpId = val; }
		break;
	case VAR_LORA_OWN :
		if ( !basic_SetVarSub( &val, LORA_OWN_MIN, LORA_OWN_MAX ) ) {	Settings.Own = basic_LoRa_TxFrame.OwnId = val; }
		break;
	case VAR_LORA_DST :
		if ( !basic_SetVarSub( &val, LORA_DST_MIN, LORA_DST_MAX ) ) {	Settings.Dst = basic_LoRa_TxFrame.DstId = val; }
		break;
	case VAR_LORA_MODEM :
		if ( !basic_SetVarSub( &val, LORA_MODEM_MIN, LORA_MODEM_MAX ) )
		{
			if ( PvtUi_ChBwToFreq( val, Settings.Ch, Settings.Bw ) ) { errorCode = ERROR_PARA; break; }
			Settings.Modem = val;
		}
		break;
	case VAR_LORA_PASS :
		if( *executionPointer == ST_PERIOD ) {
			executionPointer++;
			if ( !proc_LetSpecial( &val ) ){ Settings.PassKey = val; }
		} else
		if ( !proc_LetSpecial( &val ) ) { basic_SetPassLevel( val ); }
		break;
	case VAR_LORA_CTRL :
		proc_LetVariable( (var_t*)&CTRL_BITS );
		break;
	case ST_LORA :
	case VAR_LORA_TXD :
		basic_LoRa_Txd();
		break;
	case VAR_LORA_TXDW :
		basic_LoRa_Txd_size( &basic_LoRa_TxFrame, NUM_SIZE_WORD );
		break;
	case VAR_LORA_TXDL :
		basic_LoRa_Txd_size( &basic_LoRa_TxFrame, NUM_SIZE_LONG );
		break;
	case VAR_LORA_TXDM :
		basic_LoRa_Txd_size( &basic_LoRa_TxFrame, NUM_SIZE_MID );
		break;
	case VAR_LORA_STAT :
		if ( proc_LetSpecial( &val ) || val != 0 )	break;
		basic_LoRa_stat = 0;
		PvtUi_RxContinue();
		WanUi_RxContinue();
		break;
	case VAR_LORA_BRKCH :
		if ( !basic_SetVarSub( &val, -UINT8_MAX, UINT8_MAX) ) {
			bios_SetBreakCh( val );
			bios_UartSetBreakCh( val );
		}
		break;
	case VAR_LORA_I2CD :
		basic_LoRa_I2cD_Set();
		break;
	case VAR_LORA_FRQ :
		if ( !basic_LoRa_SetVerSubLevel2( &val, LORA_FREQ_MIN, LORA_FREQ_MAX ) )	Settings.Freq_Hz = FREQ_TO_HZ( val );
		break;
	case VAR_LORA_DEBUG :
		if ( !basic_LoRa_SetVerSubLevel2( &val, 0, UINT8_MAX) )		DEBUG_BITS = val;
		break;
	case VAR_LORA_PERIOD :
		if ( !basic_LoRa_SetVerSubLevel2( &val, 0, INT32_MAX) )		Settings.Period = val;
		break;
	case VAR_LORA_LEVEL :
		if ( !basic_LoRa_SetVerSubLevel2( &val, INT8_MIN, 0 ) )		Pvt_SetCarrierSensLevel( val );
		break;
	case VAR_LORA_SN :
		if ( !basic_LoRa_SetVerSubLevel2( &val, LORA_SN_MIN, LORA_SN_MAX ) )
		{
			Adjust.Sn = val;
			Ui_SetDevid_by_sn();
			Ui_Adjust_Save();
#ifdef	__UI_LORA_WAN__
			WanUi_DeveuiCopy();
			WanUi_Conf_Save();
#endif
		}
		break;
	case VAR_LORA_DRIFT :
		if ( !basic_LoRa_SetVerSubLevel2( &val, LORA_DRIFT_MIN, LORA_DRIFT_MAX ) ) { Adjust.Drift = val; Ui_Adjust_Save(); }
		break;
	case VAR_LORA_PROTID :
		if ( !basic_LoRa_SetVerSubLevel2( &val, 0, UINT16_MAX ) )	Settings.ProtId = val;
		break;
	case VAR_LORA_SYNCWORD :
		if ( !basic_LoRa_SetVerSubLevel2( &val, 0, UINT8_MAX ) )	Settings.SyncWord = val;
		break;
	case VAR_LORA_XTAL32K :
		if ( !proc_LetSpecial( &val ) ) { bios_SetXtal32k( val ); Ui_Adjust_Save(); }
		break;
#if __LORA_AES_ENABLE__
	case VAR_AESKEY :
//		if ( basic_LoRa_Pvt_Check() )	break;
		basic_SetKey( Settings.AesKey, AESKEY_LEN );
		Settings.AesEnable = basic_IsNotZero( Settings.AesKey, AESKEY_LEN );
		break;
#endif
#if __LORA_FWD_ENABLE__
	case VAR_HOP :
		if ( !basic_SetVarSub( &val, LORA_HOP_MIN, LORA_HOP_MAX ) )	Settings.FwdHop = val;
		break;
	case VAR_FWDCONF :
		if ( !proc_LetSpecial( &val ) )	Settings.FwdTxConf = Pvt_Forward_SetTxConf( val );
		break;
#endif
	case VAR_LORA_SNR :
	case VAR_LORA_RSSI :
	case VAR_LORA_MPUID :
	default :
		return 1;
	}
	return 0;
}

/*
 *
 */
bool basic_LoRa_GetVar( uint8_t ch, var_t *val )
{
	bool res = true;
	switch ( ch )
	{
	case VAR_LORA_PWR	:	*val = Settings.Power;		break;
	case VAR_LORA_SF	:	*val = Settings.Sf;			break;
	case VAR_LORA_BW	:	*val = Settings.Bw;			break;
	case VAR_LORA_CR	:	*val = Settings.Cr;			break;
	case VAR_LORA_CH	:	*val = Settings.Ch;			break;
	case VAR_LORA_GID	:	*val = Settings.Gid;		break;
	case VAR_LORA_OWN	:	*val = Settings.Own;		break;
	case VAR_LORA_DST	:	*val = Settings.Dst;		break;
	case VAR_LORA_MODEM	:	*val = Settings.Modem;		break;
	case VAR_LORA_FRQ	:	*val = Settings.Freq_Hz;	break;
	case VAR_LORA_DEVID :	*val = Ui_GetDevid_Sn();	break;
	case VAR_LORA_SNR	:	*val = StateInf.SnrValue;	break;
	case VAR_LORA_RSSI	:	*val = StateInf.RssiValue;	break;
	case VAR_LORA_STAT	:	*val = basic_LoRa_stat;		break;
	case VAR_LORA_PASS	:	*val = basic_GetPassLevel();break;
	case VAR_LORA_CTRL	:	*val = CTRL_BITS;			break;
	case VAR_LORA_DEBUG	:	*val = DEBUG_BITS;			break;
	case VAR_LORA_PERIOD:	*val = Settings.Period;		break;
	case VAR_LORA_TXD	:	res = basic_LoRa_TRxd_size( &basic_LoRa_TxFrame, NUM_SIZE_BYTE,val );	break;
	case ST_LORA        :
	case VAR_LORA_RXD	:	res = basic_LoRa_TRxd_size( &basic_LoRa_RxFrame, NUM_SIZE_BYTE,val );	break;
	case VAR_LORA_TXDW	:	res = basic_LoRa_TRxd_size( &basic_LoRa_TxFrame, NUM_SIZE_WORD,val );	break;
	case VAR_LORA_RXDW	:	res = basic_LoRa_TRxd_size( &basic_LoRa_RxFrame, NUM_SIZE_WORD,val );	break;
	case VAR_LORA_TXDL	:	res = basic_LoRa_TRxd_size( &basic_LoRa_TxFrame, NUM_SIZE_LONG,val );	break;
	case VAR_LORA_RXDL	:	res = basic_LoRa_TRxd_size( &basic_LoRa_RxFrame, NUM_SIZE_LONG,val );	break;
	case VAR_LORA_TXDM	:	res = basic_LoRa_TRxd_size( &basic_LoRa_TxFrame, NUM_SIZE_MID ,val );	break;
	case VAR_LORA_RXDM	:	res = basic_LoRa_TRxd_size( &basic_LoRa_RxFrame, NUM_SIZE_MID ,val );	break;
	case VAR_LORA_BRKCH :	*val = bios_GetBreakCh();			break;
	case VAR_LORA_LEVEL	:   *val = Pvt_GetCarrierSensLevel();	break;
	case VAR_LORA_I2CD	:   *val = basic_LoRa_I2cD_Get();		break;
	case VAR_LORA_SN	:	*val = Ui_GetSerialNumber();		break;
	case VAR_LORA_DRIFT :	*val = Adjust.Drift;				break;
	case VAR_LORA_PROTID:	*val = Settings.ProtId;				break;
	case VAR_LORA_SYNCWORD:	*val = Settings.SyncWord;			break;
	case VAR_LORA_MPUID	:	*val = bios_GetMpuId();				break;
	case VAR_LORA_XTAL32K:	*val = bios_GetXtal32kStat();		break;
	case FUNC_LORA_ISFREE:	*val = basic_LoRa_IsFree();			break;
	case VAR_LORA_TXLEN  :	*val = basic_LoRa_TxLen();			break;
	case ST_LORA_SENS	:	*val = basic_LoRa_GetSens();		break;
#if __LORA_AES_ENABLE__
	case VAR_AESKEY		:	*val = Settings.AesEnable;			break;
#endif
#if __LORA_FWD_ENABLE__
	case VAR_HOP		:	*val = Settings.FwdHop;				break;
	case VAR_FWDCONF	:	*val = Pvt_Forward_GetTxConf();		break;
#endif
	default : res = false;
	}
	return res;
}

/*
 * Default
 */
PROC( proc_LoRa_Default )
{
	var_t val = 0;
	
	if ( get_XargVal_def( &val, 1 ) )	return;
	if ( val == LORA_FORCE_INIT_NUMBER )
	{
		Ui_Settings_Init( true );
		return;
	}
	PvtUi_SettingsDefault();
#ifdef __UI_LORA_WAN__
	if( Ui_IsWanMode() )
	{
		WanUi_SettingsDefault();
	}
#endif
}

/*
 * #?
 */
PROC( proc_LoRa_Disp )
{
	if ( checkDelimiter() )	return;
	Ui_DispStat();
}

/*
 * SLoad
 */
PROC( proc_LoRa_Sload )
{
	if ( checkDelimiter() )	return;
	Ui_Settings_Load();
}

/*
 * SSave
 */
PROC( proc_LoRa_Ssave )
{
	if ( checkDelimiter() )	return;
	if ( Ui_Settings_Save() )
	{
		errorCode = ERROR_FLASH;
	}
}

/*
 * ALoad
 */
PROC( proc_LoRa_Aload )
{
	if ( checkDelimiter() )	return;
	memcpy( (uint8_t *)arrayVariables, (uint8_t*)FLASH_ARRAY_ADRS, sizeof(arrayVariables) );
}

/*
 * ASave
 */
PROC( proc_LoRa_Asave )
{
	if ( checkDelimiter() )	return;
	if ( bios_FlashWrite( FLASH_ARRAY_ADRS, (uint8_t *)arrayVariables, sizeof(arrayVariables) ) )
	{
		errorCode = ERROR_FLASH;
	}
}

/*
 * Txd=
 */
int16_t basic_LoRa_SetTxd( void )
{
	int16_t len = get_StringPara( STRING_PARA_FLAG_CRLF_DISABLE );
	if ( len >= 0 )
	{
		if ( len > LORA_FRAME_DATA_SIZE )	{ errorCode = ERROR_PARA; return -1; }
		memcpy( (void*)basic_LoRa_TxFrame.Data, (void*)bios_UartTxBuff, len );
		Pvt_SetFrameHeader( &basic_LoRa_TxFrame, (len | 0x100) );
	}
	return len;
}

/*
 *
 */
static void basic_LoRa_PrintStat( void )
{
	if ( basic_LoRa_stat ) Ui_PutRespStr( basic_LoRa_stat );
}

/*
 * Send
 */
PROC( proc_LoRa_Send )
{
	if ( basic_LoRa_Pvt_Check() )	return;

	int16_t len = 0;
	bool flag = false;
	do {
		if ( isExeDelimiter( ) ) break;		// ��������?
		if ( *executionPointer == ENCODE_IGNORE )			// IGNORE ?
		{
			flag = true;
			executionPointer++;
			if ( isExeDelimiter( ) ) break;
		}
		if ((len = basic_LoRa_SetTxd()) < 0 ) return;
	} while ( 0 );
	basic_LoRa_stat = PvtUi_Send( &basic_LoRa_TxFrame, len );
	if ( flag )	return;
	basic_LoRa_PrintStat();
}

/*
 * TxIf
 */
PROC( proc_LoRa_Txif )
{
	if ( basic_CheckPassLevel_2() ) return;
	if ( basic_LoRa_Pvt_Check() )	return;

	int16_t len = 0;
	if ( !isExeDelimiter( ) )	// ��������
	{
		if ((len = basic_LoRa_SetTxd()) < 0 ) return;
	}
	basic_LoRa_stat = PvtUi_Txif ( &basic_LoRa_TxFrame, len );
	basic_LoRa_PrintStat();
}

/*
 * TxWave
 */
PROC( proc_LoRa_TxWave )
{
	var_t val = 0;
	if ( basic_LoRa_Pvt_Check() )	return;
	if ( get_XargVal_def( &val, 1 ) ) return;
	if ( val <= 4 )
	{
		if ( !basic_CheckPassLevel_2() )
		{
			basic_LoRa_stat = 0;
			if ( val >= 0 )
			{
				PvtUi_Txcw( val );
			} else
			{
				PvtUi_Txcb( (char*)&basic_LoRa_TxFrame, LORA_FRAME_SIZE );
				memset( (void*)&basic_LoRa_TxFrame, 0, LORA_FRAME_SIZE );
			}
			return;
		}
	} else
	{
		if ( Settings.Modem != MODEM_LORA ) return;
		basic_LoRa_stat = PtvUi_TxPreamble( val );
		basic_LoRa_PrintStat();
		return;
	}
	errorCode = ERROR_PARA;
}

/*
 *
 */
bool basic_LoRa_GetCodecPara( uint8_t *code )
{
	if ( Ui_IsEncode( *executionPointer ) == false )	return false;
	uint8_t hidden = *code & ENCODE_HIDDEN;
	*code = *(executionPointer++);
	if ( *code == *executionPointer ) { *code |= hidden; executionPointer++; }	
	return true;
}

/*
 *
 */
errCode_t basic_LoRa_GetRecvPara( uint8_t *code, var_t *val )
{
	if ( isExeDelimiter( ) ) return errorCode;
	do {
		if ( basic_LoRa_GetCodecPara( code ) )	break;
		*val = expr();
		if ( !errorCode && (*executionPointer == ',') )
		{
			executionPointer++;
			basic_LoRa_GetCodecPara( code );
		}
	} while ( 0 );
	if ( !errorCode && !isExeDelimiter( ) )	errorCode = ERROR_PARA;
	return errorCode;
}

/*
 *
 */
errCode_t basic_LoRa_GetCommPara( uint8_t *code )
{
	*code = ENCODE_BINARY;
	basic_LoRa_GetCodecPara( code );
	if ( !isExeDelimiter( ) )	errorCode = ERROR_PARA;
	return errorCode;
}

/*
 * Recv
 */
PROC( proc_LoRa_Recv )
{
	if ( basic_LoRa_Pvt_Check() )	return;

	uint8_t code = ENCODE_BINARY+ENCODE_HIDDEN;
	var_t val = -1;

	if ( basic_LoRa_GetRecvPara( &code, &val ) )	return;

	basic_LoRa_stat = 0;
	PvtUi_Recv( &basic_LoRa_RxFrame, val, code );
}

/*
 *
 */
void basic_LoRa_Process( void )
{
	if ( Ui_IsWanMode() == false )	PvtUi_Process();
}

/*
 * RxStop
 */
PROC( proc_LoRa_RxStop )
{
	if ( basic_LoRa_Pvt_Check() )	return;

	if ( checkDelimiter() )	return;
	basic_LoRa_stat = 0;
	Pvt_RxStop();
}

/*
 * Comm
 */
PROC( proc_LoRa_Comm )
{
	if ( basic_LoRa_Pvt_Check() )	return;

	int16_t len;
	char *ptr;

	if ( basic_LoRa_GetCommPara( &basic_LoRa_Comm_Code ) )	return;
	bios_UartSetBreakCh( 0 );
	basic_LoRa_wakeup_pin_old = bios_Get_Wakeup_Pin();
	while ( 1 )
	{
		basic_LoRa_stat = 0;
		ptr = bios_UartRxBuff;
		len = GetUartString( basic_LoRa_Comm_Hook, ptr, BIOS_UART_RX_BUFF_SIZE, basic_LoRa_Comm_Code, false );
		if ( len == 0 )	continue;
		if ( len < 0 )	break;
		if ( len > LORA_FRAME_DATA_SIZE )	basic_LoRa_stat = UI_RES_INVALID_LENGTH;
		else
		{
			memcpy( basic_LoRa_TxFrame.Data, ptr, len );
			basic_LoRa_stat = PvtUi_Send( &basic_LoRa_TxFrame, (len | 0x100) );
		}
		Ui_PutRespStr( basic_LoRa_stat );
	}
	Pvt_RadioStandbySleep();
	bios_UartSetBreakCh( bios_GetBreakCh() );
}

/*
 *
 */
static uint8_t basic_LoRa_Comm_Hook( void )
{
	bool wakeup_pin_stat = bios_Get_Wakeup_Pin();
	bool wakeup_pin_temp = basic_LoRa_wakeup_pin_old;
	basic_LoRa_wakeup_pin_old = wakeup_pin_stat;
	if ((wakeup_pin_temp == true)&&(wakeup_pin_stat == false))		// Sleep request ?
	{
		if ( StateInf.RxEnable != false )	Pvt_RadioSleep();		// ��M��?
		bios_Sleep( false, 0, 0 );
		__NOP();
	}

	if ( IS_CTRL_TX_ONLY )	return 0;
	if ( StateInf.RxEnable == false )
	{
		StateInf.FramePtr = &basic_LoRa_RxFrame;
		StateInf.RxEnable = true;
		Pvt_RxStart( Settings.Modem, 0 );
	}
	if ( StateInf.State == RX_DONE )
	{
		PvtUi_PrintRxd( StateInf.FramePtr->OwnId, basic_LoRa_Comm_Code );
		StateInf.State = ACTIVE;
	}
	if ( StateInf.State == RX_ERROR )
	{
		Ui_PutRespStr( UI_RES_CRC_ERROR );
		StateInf.State = ACTIVE;
	}
	return 0;
}

/*
 * Sens
 */
PROC( proc_LoRa_Sens )
{
	var_t val[2] = { 0, 0 };

	if ( basic_CheckPassLevel_2() ) return;
	if ( basic_LoRa_Pvt_Check() )	return;

	if ( get_XargVal_def( val, 2 ) )	return;

	basic_LoRa_stat = 0;
	PvtUi_Sens( val[0], val[1] );
}

/*
 * Bme
 */
PROC( proc_LoRa_Bme )
{
	var_t *pvar[3] = { NULL, NULL, NULL };
	bme280_data_t data;

	for ( uint8_t i = 0; i < 3; i++ )
	{
		if ( isExeDelimiter( ) ) break;
		if ( *executionPointer == ',' ) executionPointer++;
		else
		{
			pvar[i] = getParameterPointer();
			if ( errorCode )	return;
			if ( isExeDelimiter( ) ) break;
			if ( chkST( ',' ) )	return;
		}
		if ( i == 2 ) { errorCode = ERROR_SYNTAX; return; }
	}
	if ( bme280_measurement() )
	{
		errorCode = ERROR_DEVICE;
		return;
	}
	bme280_to_data( &data );
	if ( pvar[0] || pvar[1] || pvar[2] )
	{
		if ( pvar[0] ) *pvar[0] = data.temperature;
		if ( pvar[1] ) *pvar[1] = data.humidity;
		if ( pvar[2] ) *pvar[2] = data.pressure;
	    return;
    }
    bios_UartPrintfLine( "%d %d %d", data.temperature, data.humidity, data.pressure );
}

/*
 *
 */
static bool basic_LoRa_I2c_Para( var_t *val )
{
	val[0] = val[1] = -1;
	val[2] = 1;

	if ( get_XargVal_def( val, 3 ) )	return errorCode;

	if ( val[0] < 0 )
	{
		bios_I2cDeInit();
		return true;
	}
	do {
		if ( val[0] > 0x7f )	break;
		if ( val[1] > 0xff )	break;
		if ( val[1] < 0 && val[2] == 0 )	break;
		if ( val[2] < 0 || val[2] > LORA_I2C_BUFF_SIZE )		break;

		bios_I2cInit();
		return false;
	} while (0);
	errorCode = ERROR_PARA;
	return true;
}

/*
 * I2cR
 */
PROC( proc_LoRa_I2cR )
{
	var_t val[3];

	if ( basic_LoRa_I2c_Para( val ) )	return;

	uint8_t rslt = bios_I2cReadMemory( val[0], val[1], basic_LoRa_I2c_buff, val[2] );
	if ( rslt )	errorCode = ERROR_DEVICE;
}

/*
 * I2cW
 */
PROC( proc_LoRa_I2cW )
{
	var_t val[3];

	if ( basic_LoRa_I2c_Para( val ) )	return;

	uint8_t rslt = bios_I2cWriteMemory( val[0], val[1], basic_LoRa_I2c_buff, val[2] );
	if ( rslt )	errorCode = ERROR_DEVICE;
}

/*
 * Get I2cD
 */
static var_t basic_LoRa_I2cD_Get( void )
{
	var_t val, index = getArrayIndex( -1, LORA_I2C_BUFF_SIZE - 1 );
	if ( errorCode )	return 0;
	val = (index < 0) ? bios_I2cGetBaudrate() : basic_LoRa_I2c_buff[index];
	return val;
}

/*
 * Set I2cD
 */
static void basic_LoRa_I2cD_Set( void )
{
	if ( *executionPointer == '=' )
	{
		executionPointer++;
		int16_t len = get_StringPara( STRING_PARA_FLAG_CRLF_DISABLE );
		if ( len >= 0 )
		{
			if ( len > LORA_I2C_BUFF_SIZE )	{ errorCode = ERROR_PARA; return; }
			memcpy( (void*)basic_LoRa_I2c_buff, (void*)bios_UartTxBuff, len );
		}
	} else
	{
		var_t index = getArrayIndex( -1, LORA_I2C_BUFF_SIZE - 1 );
		if ( errorCode )	return;
		var_t val;
		if ( proc_LetSpecial( &val ) ) return;
		if ( index < 0 )
		{
			bios_I2cSetBaudrate( val );
			return;
		}
		basic_LoRa_I2c_buff[index] = val;
	}
}

/*
 * Utrans
 */
PROC( proc_LoRa_Utrans )
{
	var_t val;

	if ( get_1argVal( &val ) ) return;
	bios_uart_trans( val );
}

/*
 * Pwm
 */
PROC( proc_LoRa_Pwm )
{
	var_t val[3] = { -1, -1, -1 };

	if ( get_XargVal_def( val, 3 ) )	return;

	if ( val[0] < 0 )		// Pin-Num
	{
		errorCode = ERROR_PARA;
		return;
	}
	if ( val[1] > 0 )		// Period >= 0 ?
	{
		if ( val[2] < 0 ) val[2] = (val[1]+1)/2;
	} else
	if ( val[2] >= 0 )		// Width >= 0 ?
	{
		errorCode = ERROR_PARA;
		return;
	}
	if ( bios_pwm_set( val[0], val[1], val[2] ) ) errorCode = ERROR_PARA;
}

/*
 * Ugps
 */
PROC( proc_LoRa_Ugps )
{
	var_t val[2] = { 0, 0 };

	if ( get_XargVal_def( val, 2 ) )	return;
	gps_GetRecv( val[0], val[1] );
}

/*
 * DateTime
 */
PROC( proc_LoRa_Datetime )
{
	var_t val[2];

	if ( get_2argVal( &val[0], &val[1] ) )	return;
	if ( val[1] < 0 || val[1] > ARRAY_INDEX_MAX-6 )
	{
		errorCode = ERROR_PARA;
		return;
	}
	gps_ConvDatetime( val[0], &arrayVariables[val[1]] );
}

/*
 * DateTime
 */
int32_t basic_LoRa_TimeVal( int32_t val )
{
	if ( val < 0 || val > ARRAY_INDEX_MAX-6 )
	{
		errorCode = ERROR_PARA;
		return 0;
	}
	val = gps_ConvTimeVal( &arrayVariables[val] );
	return val;
}

/*
 * size = 1(Byte)/2(Word)/3(Long)/4(Mid)
 */
static uint8_t *basic_LoRa_Buff_size( DataFrame_t *frame, uint8_t size, var_t *val )
{
	var_t index = getArrayIndexZero( LORA_FRAME_SIZE - get_sizeBytes( size ) );
	if ( errorCode )	return NULL;

	uint8_t *ptr = (uint8_t*)&(frame->GrpId);
	ptr += index;
	get_VarConv( val, ptr, size );
	return ptr;
}

/*
 * size = 1(Byte)/2(Word)/3(Long)/4(Mid)
 */
static bool basic_LoRa_TRxd_size( DataFrame_t *frame, uint8_t size, var_t *val )
{
	if ( !IS_OPEN_BRACKET(*executionPointer) && size == 0 ) return false;
	basic_LoRa_Buff_size( frame, size, val );
	return errorCode == ERROR_OK;
}

/*
 * size = 1(Byte)/2(Word)/3(Long)/4(Mid)
 */
static void basic_LoRa_Txd_size( DataFrame_t *frame, uint8_t size )
{
	var_t val;
	uint8_t *ptr = basic_LoRa_Buff_size( frame, size, &val );
	if ( errorCode )	return;
	if ( proc_LetVariable( &val ) ) return;
	set_VarConv( val, ptr, size );
}

/*
 *
 */
void basic_LoRa_Txd( void )
{
	if ( *executionPointer == '=' )
	{
		executionPointer++;
		basic_LoRa_SetTxd();
	} else
	{
		basic_LoRa_Txd_size( &basic_LoRa_TxFrame, NUM_SIZE_BYTE );
	}
}

/*
 *
 */
static char *basic_LoRa_Print_sub( char *buff, uint8_t *ptr, uint8_t len )
{
	if ( IS_OPEN_BRACKET(*executionPointer) ) return NULL;
	memcpy( buff, ptr, len );
	return buff + len;
}

/*
 *
 */
char *basic_LoRa_Print( uint8_t ch, char *buff )
{
	switch ( ch )
	{
	case ST_LORA		:
	case VAR_LORA_RXD	: return basic_LoRa_Print_sub( buff, basic_LoRa_RxFrame.Data, basic_LoRa_RxFrame.Length );
	case VAR_LORA_TXD	: return basic_LoRa_Print_sub( buff, basic_LoRa_TxFrame.Data, basic_LoRa_TxFrame.Length );
	case VAR_LORA_DEVID : return Ui_SetHexBuff( buff, Adjust.Device_Id, DEVICE_ID_LEN );
	case VAR_LORA_MPUID : return Ui_SetHexBuff( buff, (uint8_t*)BoardGetUniqueId(), UNIQUE_ID_LEN );
#if __LORA_AES_ENABLE__
	case VAR_AESKEY		:
//						  if ( basic_LoRa_Pvt_Check() )	break;
						  if ( Settings.AesEnable )  return Ui_SetHexBuff( buff, Settings.AesKey, AESKEY_LEN );
						  return buff;
#endif
	}
	return NULL;
}

/*
 *
 */
static uint8_t basic_LoRa_Stat_Callback( uint8_t stat )
{
	switch ( stat )
	{
	case RX_DONE	:
//		basic_LoRa_RxFrame.Ctrl = -StateInf.RssiValue;
		basic_LoRa_stat = UI_RES_RX;
		break;
	case TX_TIMEOUT :
	case RX_TIMEOUT :
		basic_LoRa_stat = UI_RES_TIMEOUT;
		break;
	case RX_ERROR	:
		if ( !IS_CTRL_CRCERR_ENABLE )	return ACTIVE;
		basic_LoRa_stat = UI_RES_CRC_ERROR;
		break;
	}
	return stat;
}

/*
 *
 */
static var_t basic_LoRa_IsFree( void )
{
	var_t val;
	if ( calcValueFunc_1( &val ) )	return 0;
	if ( val < 0 )
	{
		errorCode = ERROR_PARA;
		return 0;
	}
	return Pvt_CarrierSens( val );
}

/*
 *
 */
static var_t basic_LoRa_GetSens( void )
{
	if ( basic_LoRa_Pvt_Check() ) return 0;
	basic_LoRa_stat = 0;
	PvtUi_Sens( 0, -1 );
	return PvtUi_Sens_rssi_value;
}

/*
 *
 */
static var_t basic_LoRa_TxLen( void )
{
	uint8_t len;
	PtvUi_onair_time( &len );
	return len;
}

/*
 *
 */
int8_t SX1276GetOffsetTxPower( int8_t power )
{
	power += LORA_TXPWR_OFFSET;
	if ( power > LORA_POWER_MAX_ADMIN ) return LORA_POWER_MAX_ADMIN;
	return power;
}

/*
 *
 */
uint32_t SX1276GetDriftFreq( uint32_t freq )
{
	if ( Adjust.Drift < LORA_DRIFT_MIN )	return freq;
	if ( Adjust.Drift > LORA_DRIFT_MAX ) return freq;
	freq += Adjust.Drift * LORA_DRIFT_UNIT;
	return freq;
}

/*
 * Distance
 */
PROC( proc_LoRa_Distance )
{
	var_t index;

	if ( get_1argVal( &index ) )	return;
	if ( index > 255-6 ) { errorCode = ERROR_PARA; return; }
	
	gps_CalcDistance( &arrayVariables[index] );

}

/*
 * CAD (Channel Activity Detect)
 */
PROC( proc_LoRa_Cad		)
{
	var_t val[2] = { 0, 0 };
	if ( basic_LoRa_Pvt_Check() )	return;
	if ( get_XargVal_def( val, 2 ) )	return;
	if ( Settings.Modem != MODEM_LORA ) return;
	basic_LoRa_stat = PvtUi_Cad( val[0], val[1] );
	if ( val[0] > 0 ) basic_LoRa_PrintStat();
}
