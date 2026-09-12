/*!
 * @file    pvt_ui.c
 * @brief   LRA1 LoRa P2P protocol (UI layer)
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
#include <stdio.h>
#include <string.h>
#include "board-config.h"
#include "board-mcu.h"
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_timer.h"
#include "radio.h"
#include "bme280-board.h"
#include "basic.h"
#include "bios.h"
#include "utilities.h"

#define ONAIR_TIME_LIST_ENABLE			0			// OnAir-time list display at DEVID
#define ESC_MESSAGE_ENABLE				0			// ESC Message enable

const char* const PvtUi_CrStr[]   = { "4/5", "4/6", "4/7", "4/8" };
const char* const PvtUi_BwStr[]   = { "7.8", "10.4", "15.6", "20.8", "31.25", "41.7", "62.5", "125", "250", "500" };
const char* const PvtUi_ModemStr[]= { "Fsk", "LoRa", "Gfsk" };

void PvtUi_TxPutStr( const char *str );
uint8_t Ui_GetTransStr( char *buff );
void PvtUi_TxCwCp( bool flag );
void PvtUi_DispOnairList( void );
static void PvtUi_Pn9SetBuff( uint8_t *buff, uint8_t len );

int16_t PvtUi_Sens_rssi_value;

/*
 *
 */
void PvtUi_SettingsDefault(void)
{
	Settings.Initialized = SETTINGS_INITIALIZED;
    CTRL_BITS = 0;
    DEBUG_BITS = 0;
    Settings.Baudrate	= DEFAULT_BAUDRATE;
    Settings.Echo		= DEFAULT_ECHO;
	Settings.BrkDisable = 0;
	Settings.BreakChar	= DEFAULT_BREAK_CHAR;
    Settings.AutoRun[0] = 0;
	Pvt_SetCarrierSensLevel( 0 );
	bios_UartSetBreakCh( DEFAULT_BREAK_CHAR );
#ifdef	__UI_LORA_WAN__
	if( Ui_IsWanMode() )	return;
#endif
	Settings.Modem	= LORA_MODEM_DEFAULT;
	Settings.Power	= LORA_POWER_DEFAULT;
    Settings.Sf		= LORA_SF_DEFAULT;
    Settings.Bw		= LORA_BW_DEFAULT;
    Settings.Cr		= LORA_CR_DEFAULT;
    Settings.Ch		= LORA_CH_DEFAULT;
    Settings.Freq_Hz= LORA_HZ_DEFAULT;
    Settings.Gid	= LORA_GID_DEFAULT;
    Settings.Own	= LORA_OWN_DEFAULT;
    Settings.Dst	= LORA_DST_DEFAULT;
//  Settings.Size	= LORA_FRAME_DATA_SIZE;
    Settings.Period	= LORA_PERIOD_DEFAULT;
	Settings.SyncWord = LORA_SYNCWORD;
	Settings.ProtId	= LORA_FRAME_SIGNATURE_P2P;
#if __LORA_AES_ENABLE__
	Settings.AesEnable = LORA_AES_DEFAULT;
	memset( Settings.AesKey, 0, AESKEY_LEN );
#endif
#if __LORA_FWD_ENABLE__
	Settings.FwdHop = LORA_HOP_DEFAULT;
	Settings.FwdTxConf = LORA_FWDCONF_DEFAULT;
#endif
}

/*
 *
 */
void PvtUi_Stat( void )
{
	bios_UartPrintfLine( "Modem=%u[%s]"	, Settings.Modem, PvtUi_ModemStr[Settings.Modem] );
	bios_UartPrintfLine( "Pwr=%d(dBm)"	, Settings.Power );
	bios_UartPrintfLine( "Ch=%u"		, Settings.Ch );
	bios_UartPrintfLine( "Sf=%u"		, Settings.Sf );
	bios_UartPrintfLine( "Bw=%u[%skHz]"	, Settings.Bw, PvtUi_BwStr[Settings.Bw] );
	bios_UartPrintfLine( "Cr=%u[%s]"	, Settings.Cr, PvtUi_CrStr[Settings.Cr-1] );
	bios_UartPrintfLine( "Gid=%u"		, Settings.Gid );
	bios_UartPrintfLine( "Own=%u"		, Settings.Own );
	bios_UartPrintfLine( "Dst=%u"		, Settings.Dst );
#if __LORA_FWD_ENABLE__
	if( Pvt_Is_Forward_Enable() )
	{
		bios_UartPrintfLine( "Hop=%u"	, Settings.FwdHop );
	}
#endif
#if __LORA_AES_ENABLE__
	bios_UartPuts( "AesKey=\x22" );
	if( Settings.AesEnable )
	{
#if 0
		Ui_Hexdec_Encode( bios_UartPutChar, Settings.AesKey, AESKEY_LEN );
#else
		Ui_Hexdec_Encode( bios_UartPutChar, Settings.AesKey, AESKEY_LEN-4 );
		bios_UartPuts( "xxxxxxxx" );
#endif
	}
	bios_UartPutLine( "\x22" );
#endif
	if( basic_IsPassLevel_2() )
	{
		bios_UartPrintfLine( "SyncWord=$%02x"	, Settings.SyncWord );
		bios_UartPrintfLine( "ProtId=$%04x"		, Settings.ProtId );
		bios_UartPrintfLine( "Frq=%lu" FREQ_UNIT_STR, HZ_TO_FREQ(Settings.Freq_Hz) );
//		bios_UartPrintfLine( "Size=%u"			, Settings.Size );
		bios_UartPrintfLine( "Period=%lu(msec)"	, Settings.Period );
	}
}

/*
 *
 */
void PvtUi_PrintRxd( uint16_t adrs, uint8_t code )
{
	if( code == ENCODE_IGNORE )	return;

	uint8_t *buff = StateInf.FramePtr->Data;
	uint8_t len = StateInf.FramePtr->Length;

	if( IS_DEBUG_RECV_ALL )
	{
		buff = StateInf.BufferPtr;
		len  = StateInf.BufferLen;
		adrs = StateInf.SnrValue;
	}
	if( !IS_CTRL_AT_DISABLE   )	bios_UartPutChar( '@' );
	if( !IS_CTRL_RSSI_DISABLE )	bios_UartPrintf( "%d,", StateInf.RssiValue );
	if( !IS_CTRL_ADRS_DISABLE )	bios_UartPrintf( "%u,", adrs );
	Ui_Encode_Print( bios_UartPutChar, buff, len, code );
	bios_UartPutCrLf();
}

/*
 * RECV
 */
UiRes_t PvtUi_Recv( DataFrame_t *frame, int32_t timeout, uint8_t code )
{
	uint8_t res = UI_RES_OK;
	StateInf.FramePtr = frame;
	bool errVissible = true;

	int32_t timeout_abs = abs( timeout );
	if( timeout == -1 )	timeout_abs = 0;

	Pvt_RxStart( Settings.Modem, timeout_abs );
	StateInf.RxEnable = true;
	if( timeout == 0 )	return res;
	if( timeout > 0 ) code = ENCODE_IGNORE + ENCODE_HIDDEN;
	if( code & ENCODE_HIDDEN ) errVissible = false;
	code &= ~ENCODE_HIDDEN;

	StateInf.RxForwardFlag = false;
	States_t sta = ACTIVE;
    while( Ui_IsBreak_LP() == 0 )
	{
		CRITICAL_SECTION_BEGIN();
		sta = StateInf.State;
		if( sta != ACTIVE )	StateInf.State = ACTIVE;
		CRITICAL_SECTION_END();

		switch( sta )
		{
		case RX_DONE :
			PvtUi_PrintRxd( StateInf.FramePtr->OwnId, code );
			break;
		case RX_TIMEOUT :
			if( errVissible ) Ui_PutRespStr( UI_RES_TIMEOUT );
			break;
		case RX_ERROR :
			if( errVissible ) Ui_PutRespStr( UI_RES_CRC_ERROR );
			break;
		default :
			sta = ACTIVE;
			break;
		}
		if( sta != ACTIVE && timeout != -1 ) break;
		PvtUi_Process();
    }

    Pvt_RxStop();
	Pvt_Callback( sta );
    return res;
}

/*
 * �]������
 */
static void PvtUi_Forward( void )
{
#if __LORA_FWD_ENABLE__
	if( Pvt_Is_Forward_Enable() == false )	return;
	if( StateInf.RxForwardFlag == false ) return;
	// ## �]���J�n ##
	UiRes_t res = Pvt_Forward_Send();
	if( res == UI_RES_DC_RESTRICTED ) return;
	StateInf.RxForwardFlag = false;
#endif
}

/*
 * �o�b�N�O���E���h����
 */
void PvtUi_Process( void )
{
	PvtUi_Forward();
	if( StateInf.State != ACTIVE )	return;
	if( StateInf.TimeOutFlags & TIMEOUT_FLAG_RX )
	{
		StateInf.TimeOutFlags &= ~TIMEOUT_FLAG_RX;
		StateInf.State = RX_TIMEOUT;
	}else
	if( StateInf.TimeOutFlags & TIMEOUT_FLAG_ERROR )
	{
		StateInf.TimeOutFlags &= ~TIMEOUT_FLAG_ERROR;
		StateInf.State = RX_ERROR;
	}else
	if( StateInf.TimeOutFlags & TIMEOUT_FLAG_TX )
	{
		StateInf.TimeOutFlags &= ~TIMEOUT_FLAG_TX;
		StateInf.State = TX_TIMEOUT;
	}
	StateInf.State = Pvt_Callback( StateInf.State );

#if __LORA_FWD_ENABLE__
	if( StateInf.RxForwardFlag == false ) return;
	if( Pvt_Is_Forward_Enable() )
	{
		// ## �]���J�n ##
		UiRes_t res = Pvt_Forward_Send();
		if( res == UI_RES_DC_RESTRICTED ) return;
	}
	StateInf.RxForwardFlag = false;
#endif
}

/*
 * ��M�p�� (Stat=0 �ŌĂ΂��j
 */
void PvtUi_RxContinue( void )
{
	if( Ui_IsWanMode() != false ) return;
	if( StateInf.RxEnable == false ) return;
	StateInf.TimeOutFlags &= ~(TIMEOUT_FLAG_RX | TIMEOUT_FLAG_ERROR);
	StateInf.State = ACTIVE;
}

/*
 * Sens RSSI
 */
UiRes_t PvtUi_Sens( int32_t sens_mode, int32_t times )
{
    int16_t rssi_now, rssi_max = -175, count = 0;
	int32_t	rssi_sum = 0;
	uint8_t modem = (Settings.Modem != MODEM_LORA) ? MODEM_FSK_SENS : MODEM_LORA;
	uint8_t res = UI_RES_OK;

	if( times >= 0 )
	{	
		bios_UartPuts( PvtUi_ModemStr[Settings.Modem] );
		bios_UartPutLine( sens_mode == 0 ? " (Average)" : " (Peak)" );
	}
	Pvt_IntervalTimetStart( RSSI_INT_TIME_VALUE );
	StateInf.State = LOWPOWER;

    while( Ui_IsBreak_LP() == 0 )
    {
        if( StateInf.State == ACTIVE )
        {
            rssi_now = Pvt_GetRssi( Settings.Modem );
			if( sens_mode )
			{
				count++;
				rssi_sum += rssi_now;
			}else
			{
				if( rssi_now < 0 && rssi_now > rssi_max )	rssi_max = rssi_now;
			}
        }else
        {
	        Pvt_RxStart( modem, 0 );
	        DelayMs( 1 );
        }

        if( StateInf.IntervalFlag )
        {
			if( sens_mode )
			{
				rssi_max = rssi_sum / count;
				rssi_sum = count = 0;
			}
			if( times < 0 ) break;			
            bios_UartPrintfLine( "%d", rssi_max );
			if( times && --times == 0 ) break;
            rssi_max = -175;
			Pvt_IntervalTimetStart( RSSI_INT_TIME_VALUE );
        }
    }
	PvtUi_Sens_rssi_value = rssi_max;
	Pvt_IntervalTimetStop();
    Pvt_RxStop();
    return res;
}

/*
 *
 */
UiRes_t PvtUi_Send( DataFrame_t *frame, uint8_t len )
{
	return Pvt_SendFrame( frame, len, false );
}

/*
 * TXPREAMBLE
 */
UiRes_t PtvUi_TxPreamble( int32_t ms )
{
	UiRes_t res = Ptv_SetPreambleLen( ms );

	if( res != UI_RES_OK ) return res;
	res = PvtUi_Send( NULL, 0 );
    return res;
}

/*
 * TXCW
 */
UiRes_t PvtUi_Txcw( uint32_t mode )
{
	uint32_t t1, t2;

	switch(mode)
	{
	case 1 : t1 = 4000; t2 = 98040; break;	//    4s : 100ms
	case 2 : t1 = 4000; t2 =  3100; break;	//    4s : 5ms
	case 3 : t1 =  400; t2 =  2250; break;	// 400ms : 4m
	case 4 : t1 =  400; t2 =     1; break;	// 400ms : 125us
	default: t1 =    0; t2 =     0; break;
	}

#if ESC_MESSAGE_ENABLE
    PvtUi_TxPutStr( "continuous wave" );
#endif
    StateInf.State = LOWPOWER;
    while( Ui_IsBreak_LP() == 0 )
    {
        if( StateInf.State != ACTIVE )
        {
			Pvt_TxContinuousWave();
            StateInf.State = ACTIVE;		
			bios_setWaitTick( t1 );
        }
		if( t1 && bios_getWaitTick() == 0 )
		{
			Pvt_RadioStandby();
			DelayUs( t2 );
//			StateInf.State = TX_DONE;
		}
		
	}
    Pvt_RadioStandbySleep();
    return UI_RES_OK;
}

/*
 * TXCB
 */
UiRes_t PvtUi_Txcb( char *buff, uint8_t len )
{
	uint8_t res = UI_RES_RX;
#if ESC_MESSAGE_ENABLE
    PvtUi_TxPutStr( "continuous random burst" );
#endif
	Pvt_RxStop();
	Pvt_TxConfig( ( Settings.Modem == MODEM_LORA ) ? MODEM_LORA_LONG : Settings.Modem );

	StateInf.State = LOWPOWER;
    while( Ui_IsBreak_LP() == 0 )
    {
        if( StateInf.State != ACTIVE )
        {
			DelayMs( 10 );
            PvtUi_Pn9SetBuff( (uint8_t*)buff, len );
            Pvt_Send( (uint8_t*)buff, len );
        }
    }
    Pvt_RadioStandbySleep();
    return res;
}

/*
 * TXIF
 */
UiRes_t PvtUi_Txif( DataFrame_t *frame, uint8_t len )
{
    static uint16_t	count = 0;
    uint8_t res = UI_RES_OK;

    if( frame == NULL )   return UI_RES_INVALID;
	if( frame->Data[0] == '#' && frame->Data[1] == 0 )
	{
		count = 1;
		frame->Length = 5;
	}else
	if( len )
	{
		frame->Length = len;
	}
	if( frame->Length > LORA_FRAME_DATA_SIZE ) frame->Length = LORA_FRAME_DATA_SIZE;
	StateInf.FramePtr = frame;
	Pvt_RxStop();
	Pvt_TxConfig( Settings.Modem );
	
	if( StateInf.BufferLen > StateInf.PayloadSize )	StateInf.BufferLen = StateInf.PayloadSize;
	while( Pvt_CheckOnAirTime( StateInf.BufferLen ) != UI_RES_OK )
	{
		if( --StateInf.BufferLen == LORA_FRAME_HEAD_SIZE )	return UI_RES_INVALID_LENGTH;
	}
	frame->Length = StateInf.BufferLen - LORA_FRAME_HEAD_SIZE;
	if( len )	len = frame->Length;
#if ESC_MESSAGE_ENABLE
	bios_UartPrintfLine( "%lu msec", StateInf.OnairTime );
    PvtUi_TxPutStr( "interval data frame" );
#endif
	StateInf.IntervalFlag = true;
    while( Ui_IsBreak_LP() == 0 )
    {
        if( StateInf.IntervalFlag == true )
        {
			if( count ){
                if( count++ > 9999 ) count = 1;
                sprintf( (char*)frame->Data, "#%04d", count );
            }
            res = Pvt_SendFrame( frame, len, false );
			if( res == UI_RES_INVALID )	break;
			if( res == UI_RES_INVALID_LENGTH )	break;
			Pvt_IntervalTimetStart( Settings.Period );
        }
    }
    Pvt_IntervalTimetStop();
    Pvt_RadioStandbySleep();
    return res;
}

/*
 * CAD
 */
UiRes_t PvtUi_Cad( int32_t interval, int32_t timeout )
{
    uint8_t res = UI_RES_OK;
	int32_t timeout_abs = abs( timeout );
	if( interval < 0 ) interval = 0;

	Pvt_RadioStandbySleep();
	Pvt_RxConfig( MODEM_LORA );

	StateInf.IntervalFlag = true;
	StateInf.CadDetectFlag = false;
	Pvt_CadStart( timeout_abs );

    while( Ui_IsBreak_LP() == 0 )
    {
		if( StateInf.CadDetectFlag ) break;
		if( StateInf.IntervalFlag )
		{
			Pvt_CadStart( -1 );
//			if( interval )
//			{
				Pvt_IntervalTimetStart( interval );
//				Pvt_RadioStandbySleep();
//			}
		}
		if( StateInf.TimeOutFlags & TIMEOUT_FLAG_RX )
		{
			res = UI_RES_TIMEOUT;
			break;
		}
	}
	Pvt_IntervalTimetStop();
	Pvt_CadStop();
    return res;
}

/*
 *
 */
uint32_t PtvUi_onair_time( uint8_t *siz )
{
	Pvt_TxConfig( Settings.Modem );
	uint8_t cnt, len;
	cnt = len = LORA_FRAME_HEAD_SIZE;
	while( cnt <= StateInf.PayloadSize && Pvt_CheckOnAirTime( cnt ) == UI_RES_OK )	len = cnt++;
	*siz = ( len < LORA_FRAME_HEAD_SIZE ) ? 0 : len - LORA_FRAME_HEAD_SIZE;
	uint32_t tt = *siz ? Pvt_GetOnairTime( Settings.Modem, len ) : 0;
	return tt;
}

#if ONAIR_TIME_LIST_ENABLE
/*
 *
 */
static void Pvt_onair_len( uint8_t ch )
{
	uint8_t siz;
	uint32_t tt;

	Settings.Modem = MODEM_LORA;	
	bios_UartPrintfLine( "MODEM = LORA / CH = %u", ch );
	for( Settings.Bw = LORA_BW_MIN; Settings.Bw <= LORA_BW_MAX; Settings.Bw++ ){
		bios_UartPrintfLine( "BW = %u", Settings.Bw );
		for( Settings.Cr = LORA_CR_MIN; Settings.Cr <= LORA_CR_MAX; Settings.Cr++ ){
			bios_UartPrintf( "CR %u : ", Settings.Cr );
			for( Settings.Sf = LORA_SF_MIN; Settings.Sf <= LORA_SF_MAX; Settings.Sf++ ){
				PvtUi_ChBwToFreq( ch, Settings.Bw );
				tt = PtvUi_onair_time( &siz );
				bios_UartPrintf( " ,%u (%lu)", siz, tt );
			}
			bios_UartPutCrLf();
		}
	}
	Settings.Modem = MODEM_FSK;
	tt = Ptv_onair_time( &siz );
	bios_UartPrintfLine( "MODEM = FSK / CH = %u\r\n%u (%lu)", ch, siz, tt );
}

/*
 *
 */
void PvtUi_DispOnairList( void )
{
	Ptv_onair_len( 30 );
	Ptv_onair_len( 50 );
}
#endif

/*
 *
 */
static void PvtUi_Pn9SetBuff( uint8_t *buff, uint8_t len )
{
    static uint16_t reg = 0x1ff;

    while( len )
    {
        for( uint8_t i = 0; i < 8; i++ )
        {
		    uint8_t out = ((reg >> 8) ^ (reg >> 4)) & 1;
			reg = (reg << 1) | out;
            *buff <<= 1;
            *buff |= out;
        }
        buff++;
        len--;
    }
}

/*
 *
 */
UiRes_t PvtUi_ChBwToFreq( uint8_t modem, uint16_t ch, uint16_t bw )
{
	if( ch == 0 )	return UI_RES_OK;
	if( modem == MODEM_FSK )	bw = LORA_BW_62k5;
#ifdef ARIB_T108_COMPLIANT
	if((bw >= LORA_BW_250k) && ((ch == 32)||(ch >= 61)))	return UI_RES_INVALID;
	if((bw == LORA_BW_500k) && ((ch <= 24)||(ch == 33)))	return UI_RES_INVALID;
#else
	if((bw >= LORA_BW_250k) && (ch >= 61))	return UI_RES_INVALID;
	if((bw == LORA_BW_500k) && (ch <= 24))	return UI_RES_INVALID;
#endif
	Settings.Freq_Hz = LORA_CH_TO_HZ( ch );
	if( bw == LORA_BW_250k )
	{
		Settings.Freq_Hz += LORA_CH_STEP_HZ / 2;
	}
	return UI_RES_OK;
}

#if ESC_MESSAGE_ENABLE
/*
 *
 */
void PvtUi_TxPutStr( const char *str )
{
    bios_UartPuts( str );
    bios_UartPutLine( " transmission.\r\nESC-key to stop." );
}
#endif
