/*!
 * @file    pvt_pro.c
 * @brief   LRA1 LoRa P2P protocol (Radio layer)
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
#ifdef	__UI_LORA_PRIVATE__

#include "asf.h"
#include <stdio.h>
#include <string.h>
#include "board-config.h"
#include "board-mcu.h"
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_timer.h"
#include "radio.h"
#include "sx1276.h"
#include "basic.h"
#include "bios.h"

volatile StateInf_t StateInf;
static TimerEvent_t IntervalTimer;
static TimerEvent_t TxPauseTimer;
static TimerEvent_t RxTimer;
static uint8_t LoRa_payload_len;
static uint8_t (*Pvt_Callback_func)(uint8_t stat);

#define CRC_ENABLE_FLAG		true;

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;
static int8_t CarrierSensLevel;
int32_t Ptv_Freq_Drift_Hz;


#if __LORA_AES_ENABLE__
#include "LoRaMacCrypto.h"
uint8_t *Pvt_AesEncodeDecode( uint8_t *buff, uint16_t size );
#endif

static void Pvt_OnTxDone( void );
static void Pvt_OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
static void Pvt_OnTxTimeout( void );
static void Pvt_OnRxTimeout( void *context );
static void Pvt_OnRxError( void );
static void Pvt_OnTxPauseTimerEvent( void *context );
static void Pvt_OnIntervalTimerEvent( void *context );
static uint8_t Pvt_IsLowband( void );
static void Pvt_PayloadSizeConfig( uint8_t modem );
static void Pvt_SetPayloadSize( uint8_t modem );
static void Pvt_RadioReset( RadioEvents_t *event );
static void Pvt_CadDone( bool channelActivityDetected );

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
static void Pvt_OnTxDone( void )
{
	StateInf.State = Pvt_Callback( TX_DONE );
}

/*!
 * \brief Function to be executed on CAD Done event
 */
static void Pvt_CadDone( bool channelActivityDetected )
{
	StateInf.CadDoneFlag = true;
	StateInf.CadDetectFlag = channelActivityDetected;
}

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
static void Pvt_OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	if( StateInf.RxEnable == false )	return;
 
	uint8_t flags = Pvt_RecvCheck( payload, size );		// Mast be alignment !!!
	if( flags == RX_CHK_FLAGS_IGNORE )	return;

#if __LORA_FWD_ENABLE__
	// ## �]�� ? ##
	if( Pvt_Is_Forward_Enable() && (flags & RX_CHK_FLAGS_FORWARD) )
	{
		if( StateInf.Forward.Len == 0 )	// �]����?
		{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
			StateInf.Forward.FramePtr = (DataFrame_t *)payload;
#pragma GCC diagnostic pop
			StateInf.Forward.Len = size;
			StateInf.RxForwardFlag = true;
		}
	}
#endif
	if( StateInf.State != ACTIVE )	return;
	if( flags & RX_CHK_FLAGS_ACCEPT )
	{
		if( StateInf.BufferPtr != NULL )

		{
			memcpy( (uint8_t*)StateInf.BufferPtr, payload, size );
#if __LORA_AES_ENABLE__
			Pvt_AesEncodeDecode( (uint8_t*)StateInf.BufferPtr, size );
#endif
		}
        StateInf.BufferLen = size;
		StateInf.RssiValue = rssi;
		StateInf.SnrValue = snr;
       	StateInf.State = Pvt_Callback( RX_DONE );
	}
	LED_RX_ON();
}

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
static void Pvt_OnTxTimeout( void )
{
	StateInf.TimeOutFlags |= TIMEOUT_FLAG_TX;
//	StateInf.State = TX_TIMEOUT;
}

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
static void Pvt_OnRxTimeout( void *context )
{
	StateInf.TimeOutFlags |= TIMEOUT_FLAG_RX;
//	StateInf.State = RX_TIMEOUT;
}

/*!
 * \brief Function executed on Radio Rx Error event
 */
static void Pvt_OnRxError( void )
{
	StateInf.TimeOutFlags |= TIMEOUT_FLAG_ERROR;
//	StateInf.State = RX_ERROR;
}

/*!
 * \brief Function executed on Tx Interval Timeout event
 */
static void Pvt_OnIntervalTimerEvent( void *context )
{
    StateInf.IntervalFlag = true;
}

/*
 *
 */
void Pvt_SetCallback( uint8_t (*callback_func)(uint8_t stat) )
{
	Pvt_Callback_func = callback_func;
}

/*
 *
 */
States_t Pvt_Callback( States_t stat )
{
	if( Pvt_Callback_func == NULL )	return stat;
	return Pvt_Callback_func( stat );
}

/*
 *
 */
void Pvt_RadioInit( void )
{
 // Settings.Size = LORA_FRAME_DATA_SIZE;
 	StateInf.PayloadSize = LORA_FRAME_SIZE;
    StateInf.BufferLen = LORA_FRAME_SIZE;
    StateInf.RssiValue = 0;
    StateInf.SnrValue = 0;
	StateInf.BufferPtr = NULL;
    StateInf.State = LOWPOWER;
	StateInf.RxEnable = false;

	// Radio initialization
    RadioEvents.TxDone = Pvt_OnTxDone;
    RadioEvents.RxDone = Pvt_OnRxDone;
    RadioEvents.TxTimeout = Pvt_OnTxTimeout;
    RadioEvents.RxTimeout = NULL;
    RadioEvents.RxError = Pvt_OnRxError;
	RadioEvents.CadDone = Pvt_CadDone;

	Pvt_RadioReset( &RadioEvents );
//	Pvt_RadioSleep();

    TimerInit( &TxPauseTimer, Pvt_OnTxPauseTimerEvent );
    TimerInit( &IntervalTimer, Pvt_OnIntervalTimerEvent );
	TimerInit( &RxTimer, Pvt_OnRxTimeout );

//	TimerSetValue( &TxPauseTimer, TX_PAUSE_DURATION_VALUE );
    StateInf.TxPauseFlag = false;
	StateInf.SnrValue = Radio.Read( REG_LR_VERSION );	// SX1276 Chip Revision

	Pvt_RadioStandbySleep();
}

/*
 *
 */
static void Pvt_RadioReset( RadioEvents_t *event )
{
    Radio.Init( event );
	if ( IS_DEBUG_SPECIAL_SYNCWORD ) {
		SX1276SetSyncword( Settings.SyncWord );
	} else {
	    Radio.SetPublicNetwork( false );
	}
	LoRa_payload_len = LORA_PAYLOAD_LEN_DEFAULT;
}

/*
 *
 */
static void Pvt_PayloadSizeConfig( uint8_t modem )
{
	StateInf.CrcEnableFlag = false;
	StateInf.PayloadSize = LORA_FRAME_SIZE;
	StateInf.FixLen = LORA_FIX_LENGTH_PAYLOAD_ON;
	StateInf.BufferLen = 0;
	if( StateInf.FramePtr == NULL ) return;
	if( modem == MODEM_LORA || modem == MODEM_LORA_LONG )
	{
		if( Settings.Sf == 6 )
		{
			if((Settings.Bw <= LORA_BW_62k5) && (Pvt_IsLowband() == false) && (StateInf.PayloadSize > LORA_FRAME_SIZE_SF6BW62k5))
			{
				StateInf.PayloadSize = LORA_FRAME_SIZE_SF6BW62k5;
			}
			StateInf.BufferLen = StateInf.PayloadSize;
			StateInf.CrcEnableFlag = CRC_ENABLE_FLAG;
			return;
		}
	}
	StateInf.FixLen = LORA_FIX_LENGTH_PAYLOAD_OFF;
	StateInf.BufferLen = LORA_FRAME_HEAD_SIZE + StateInf.FramePtr->Length;
	StateInf.CrcEnableFlag = CRC_ENABLE_FLAG;
}

/*
 *
 */
static void Pvt_SetPayloadSize( uint8_t modem )
{
	if( modem == MODEM_LORA||modem == MODEM_LORA_LONG )
	{
		if( LoRa_payload_len != StateInf.PayloadSize ){
			LoRa_payload_len = StateInf.PayloadSize;
			Radio.Write( REG_LR_PAYLOADMAXLENGTH, LoRa_payload_len );
		}
	}
}

/*
 *
 */
void Pvt_TxConfig( uint8_t modem )
{
	bool iq_flag = IS_DEBUG_IQ_INVERT;

//	Pvt_RadioReset( NULL );
//	Radio.Standby();

	Pvt_PayloadSizeConfig( modem );
    Radio.SetChannel( Settings.Freq_Hz );
    if( modem == MODEM_LORA )
    {
		if( StateInf.FramePtr ) StateInf.PreambleLen = LORA_PREAMBLE_TX_LENGTH;
	    Radio.SetTxConfig( MODEM_LORA,
							Settings.Power, 0, Settings.Bw, Settings.Sf, Settings.Cr,
							StateInf.PreambleLen, StateInf.FixLen, StateInf.CrcEnableFlag,
							0, 0, iq_flag, TX_TIMEOUT_VALUE );
	}else
	if( modem == MODEM_LORA_LONG )
	{
	    Radio.SetTxConfig( MODEM_LORA,
							Settings.Power, 0, Settings.Bw, Settings.Sf, Settings.Cr,
							UINT16_MAX, StateInf.FixLen, StateInf.CrcEnableFlag,
							0, 0, iq_flag, UINT32_MAX );
	}else
	{	
		Radio.SetTxConfig( MODEM_FSK,
							Settings.Power, FSK_FDEV, 0, FSK_DATARATE, 0,
							FSK_PREAMBLE_BYTES, StateInf.FixLen, StateInf.CrcEnableFlag,
							0, 0, 0, TX_TIMEOUT_VALUE );
							
		uint8_t paramp = Radio.Read( REG_PARAMP ) & RF_PARAMP_MODULATIONSHAPING_MASK;
		if( modem == MODEM_GFSK )
		{
			paramp |= RF_PARAMP_MODULATIONSHAPING_10;	// Gaussian filter BT = 0.5
		}
		Radio.Write( REG_PARAMP, paramp );
	}
	Pvt_SetPayloadSize( modem );
}

static const uint32_t Bandwidth_kHz[] = { 7800, 10400, 15600, 20800, 31250, 41700, 62500, 125000, 250000, 500000 };
/*
 *
 */
void Pvt_RxConfig( uint8_t modem )
{
	bool iq_flag = IS_DEBUG_IQ_INVERT;

//	Pvt_RadioReset( NULL );
	Radio.Standby();

	Pvt_PayloadSizeConfig( modem );
    Radio.SetChannel( Settings.Freq_Hz );
	if( modem == MODEM_LORA )
	{
		if( StateInf.FramePtr ) StateInf.PreambleLen = LORA_PREAMBLE_RX_LENGTH;
	    Radio.SetRxConfig( MODEM_LORA, Settings.Bw, Settings.Sf, Settings.Cr, 0,
							StateInf.PreambleLen, LORA_SYMBOL_TIMEOUT, StateInf.FixLen,
							StateInf.PayloadSize, StateInf.CrcEnableFlag, 0, 0, iq_flag, LORA_RX_CONTINUOUS_ENABLE );
	}else
	if( modem == MODEM_FSK_SENS )
	{
		uint32_t bandwidth;
		if( Settings.Modem == MODEM_FSK || Settings.Modem == MODEM_GFSK )
		{
			bandwidth = FSK_RX_BANDWIDTH;
		}else{
			bandwidth = (Settings.Bw > 6) ? Bandwidth_kHz[Settings.Bw] / 2 : 50000;
		}
		Radio.SetRxConfig( MODEM_FSK, bandwidth, bandwidth, 0, bandwidth,
							FSK_PREAMBLE_BYTES, FSK_SYMBOL_TIMEOUT, StateInf.FixLen,
							0, StateInf.CrcEnableFlag, 0, 0, false, FSK_RX_CONTINUOUS_ENABLE );	
	}
	else
	{	//MODEM_FSK || modem == MODEM_GFSK
		Radio.SetRxConfig( MODEM_FSK, FSK_RX_BANDWIDTH, FSK_DATARATE, 0, FSK_AFC_BANDWIDTH,
							FSK_PREAMBLE_BYTES, FSK_SYMBOL_TIMEOUT, StateInf.FixLen,
							0, StateInf.CrcEnableFlag, 0, 0, false, FSK_RX_CONTINUOUS_ENABLE );
	}
	Pvt_SetPayloadSize( modem );
}

/*
 *
 */
void Pvt_RadioSleep( void )
{
    Radio.Sleep();
	StateInf.State = LOWPOWER;
	StateInf.RxEnable = false;
}

/*
 *
 */
void Pvt_RadioStandby( void )
{
    Radio.Standby();
	StateInf.State = STANDBY;
	StateInf.RxEnable = false;
}

/*
 *
 */
void Pvt_RadioStandbySleep( void )
{
	if( IS_DEBUG_ALWAYS_RF_STANDBY )
	{
		Pvt_RadioStandby();
	}else{
		Pvt_RadioSleep();
	}
}

/*
 *
 */
int32_t Pvt_GetRssi( uint8_t modem )
{
	return Radio.Rssi( modem );	
}

/*
 *
 */
void Pvt_SetCarrierSensLevel( int8_t level )
{
	CarrierSensLevel = (level < 0) ? level : CARRIER_SENS_RSSI_LEVEL;
}

/*
 *
 */
int8_t Pvt_GetCarrierSensLevel( void )
{
	Pvt_SetCarrierSensLevel( CarrierSensLevel );
	return CarrierSensLevel;
}

/*
 * retval isFree [true: Free, false: In-use]
 */
bool Pvt_CarrierSens( uint32_t waitTime )
{
	uint32_t sensTime, baseTime;
	bool isfree = false;

	sensTime = Pvt_IsLowband() ? CARRIER_SENS_L_TIME : CARRIER_SENS_H_TIME;

	Pvt_GetCarrierSensLevel();
	Pvt_RxConfig( MODEM_FSK_SENS );
    baseTime = TimerGetCurrentTime( );
	do{
		isfree = Radio.IsChannelFree( MODEM_FSK, Settings.Freq_Hz, CarrierSensLevel, sensTime );
		if( isfree )	break;
		Ui_LowpowerTimerProcess();
	}while( TimerGetElapsedTime( baseTime ) < waitTime );
#if 0
	if( Settings.Modem == MODEM_LORA )
	{
		Pvt_RxConfig( MODEM_LORA );
		Radio.IsChannelFree( MODEM_LORA, Settings.Freq_Hz, CarrierSensLevel, CARRIER_SENS_L_TIME );
	}
#endif
	return isfree;
}

/*
 *
 */
uint32_t Pvt_GetOnairTime( uint8_t modem, uint8_t len )
{
	return Radio.TimeOnAir( modem, len );
}

/*
 *
 */
static uint32_t Pvt_GetSendMaxTime( void )
{
	return Pvt_IsLowband() ? LORA_SEND_TIME_MAX_L : LORA_SEND_TIME_MAX_H;	
}

/*
 *
 */
UiRes_t Pvt_CheckOnAirTime( uint8_t len )
{
	uint8_t modem = (Settings.Modem == MODEM_LORA) ? MODEM_LORA : MODEM_FSK;
	StateInf.OnairTime = Radio.TimeOnAir( modem, len );
	if( Pvt_GetSendMaxTime() <= StateInf.OnairTime ) return UI_RES_INVALID_LENGTH;
	return UI_RES_OK;	
}

/*
 *
 */
void Pvt_Send( uint8_t *buff, uint8_t len )
{
	StateInf.RxEnable = false;
	Radio.Send( buff, len );
	StateInf.TimeOutFlags &= ~TIMEOUT_FLAG_TX;
    StateInf.State = ACTIVE;
}

/*
 *
 */
void Pvt_SetFrameHeader( DataFrame_t *frame, int16_t len )
{
	if( frame == NULL )	return;	
	frame->Signature = Settings.ProtId;
	frame->Ctrl = 0;
	frame->Seq = StateInf.SeqNum;
	if( len )
	{
		frame->GrpId = Settings.Gid;
		frame->OwnId = Settings.Own;
		frame->DstId = Settings.Dst;
		frame->Length = (uint8_t)len;
	}
}

#if __LORA_FWD_ENABLE__
/*
 *
 */
static void Pvt_RegistFwdList( DataFrame_t *frame )
{
	if( (frame->DstId == frame->OwnId)
	  ||(frame->DstId == LORA_BROADCAST_ID)
	  ||(frame->OwnId == LORA_BROADCAST_ID) )
	{
		Pvt_Forward_Check( frame );
	}
}
#endif

/*
 *
 */
static void Pvt_TxPauseTimerStart( uint32_t timeout, bool pause_flag )
{
	StateInf.TxPauseFlag = pause_flag;
	TimerSetValue( &TxPauseTimer, timeout );
	TimerStart( &TxPauseTimer );
}

/*
 *
 */
uint32_t Pvt_GetTxPauseTimerRemain( void )
{
	if( StateInf.TxPauseFlag ) return TimerGetRemainTime( &TxPauseTimer );
	return 0;
}

/*
 *
 */
UiRes_t Ptv_SetPreambleLen( int32_t ms )
{
	if( Pvt_GetSendMaxTime() <= ms ) return UI_RES_INVALID_LENGTH;
#if 0
	int32_t tt = (1 << Settings.Sf) * 1000;
	double bw = SX1276GetBandwidth( Settings.Bw );
	double pl = (((double)ms * bw) / (double)tt) - (4.25 + 8);
	int16_t len = (int16_t)pl;
#else
	int32_t tt = (1 << Settings.Sf);
	int32_t bw = SX1276GetBandwidth( Settings.Bw );
	int32_t pl = ((ms * bw) / tt) - (4250 + 8000);
	int16_t len = pl / 1000;
#endif
	StateInf.PreambleLen = max( len, 6 );
	return UI_RES_OK;
}

/*
 *
 */
UiRes_t Pvt_SendFrame( DataFrame_t *frame, int16_t len, bool forward )
{
    uint8_t res = UI_RES_OK;

	StateInf.FramePtr = frame;
	if( !forward )
	{
		Pvt_SetFrameHeader( frame, len );
		Pvt_RxStop();
	}
    Pvt_TxConfig( Settings.Modem );

	if( StateInf.BufferLen > StateInf.PayloadSize )	return UI_RES_INVALID_LENGTH;
	
	if( (res = Pvt_CheckOnAirTime( StateInf.BufferLen )) != UI_RES_OK ) return UI_RES_INVALID_LENGTH;

	while( StateInf.TxPauseFlag )	Ui_LowpowerTimerProcess();	// Interval time wait

#if __LORA_AES_ENABLE__
	if( !forward )
	{
		Pvt_AesEncodeDecode( StateInf.BufferPtr, StateInf.BufferLen );
	}
#endif
	LED_BUSY_ON();
    do{
		bool isfree = Pvt_CarrierSens( CARRIER_SENS_WAIT_TIME );
		if( isfree == false ){ res = UI_RES_NO_FREE_CH; break; }

	    Pvt_TxConfig( Settings.Modem );
	    Pvt_Send( StateInf.BufferPtr, StateInf.BufferLen );

		uint32_t tx_guard_time = StateInf.OnairTime + 10;

		Pvt_TxPauseTimerStart( tx_guard_time, false );	// Tx Guard timer
	    while( StateInf.State == ACTIVE )				// Wait for Send
		{
			Ui_LowpowerTimerProcess();
			if( StateInf.TimeOutFlags & TIMEOUT_FLAG_TX ) StateInf.State = TX_TIMEOUT;
		}
		TimerStop( &TxPauseTimer );
		if( StateInf.State == TX_TIMEOUT ) { Pvt_RadioInit(); }
		if( StateInf.State != TX_DONE    ) { res = UI_RES_ERROR; break; }

		StateInf.TxPauseTime = Pvt_IsLowband() ? TX_PAUSE_DURATION_VALUE : (StateInf.OnairTime * 10) + 2;
		Pvt_TxPauseTimerStart( StateInf.TxPauseTime, true );
		if( !forward && frame != NULL )
		{
			StateInf.SeqNum++;
#if __LORA_FWD_ENABLE__
			Pvt_RegistFwdList( frame );
#endif
		}
	}while(0);
	Pvt_RadioStandbySleep();

	if( res != UI_RES_OK )	LED_BUSY_OFF();
    return res;
}

/*
 *
 */
static uint8_t Pvt_IsLowband( void )
{
	uint32_t border = LORA_BORDER_FREQ;
	if( Settings.Modem == MODEM_LORA )
	{
		if( Settings.Bw == LORA_BW_250k )	border -= 100000;
		if( Settings.Bw == LORA_BW_500k )	border -= 200000;
	}
	return	(Settings.Freq_Hz < border );
}

/*
 *
 */
static void Pvt_RxTimetStart( int32_t timeout )
{
	if( timeout < 0 ) return;
	StateInf.TimeOutFlags = 0;
	if( timeout > 0 )
	{
		TimerSetValue( &RxTimer, timeout );
		TimerStart( &RxTimer );
	}
}

/*
 *
 */
static void Pvt_RxTimetStop( void )
{
	if( RxTimer.IsStarted )
	{
		TimerStop( &RxTimer );
	}
	StateInf.TimeOutFlags = 0;
}

/*
 *
 */
void Pvt_RxStart( uint8_t modem, int32_t timeout )
{
    Pvt_RxConfig( modem );
	Radio.Rx( 0 );
	StateInf.State = ACTIVE;
	Pvt_RxTimetStart( timeout );
}

/*
 *
 */
void Pvt_RxStop( void )
{
	Pvt_RxTimetStop();
	if( Radio.GetStatus() != RF_IDLE )
	{
		Pvt_RadioStandbySleep();
	}
}

/*
 *
 */
inline bool Pvt_ProId_Check( DataFrame_t *fp )
{
#if __LORA_FWD_ENABLE__
	if( Pvt_Is_Forward_Enable() )
	{
		uint16_t fwd_protid = ~Settings.ProtId;
		if((fp->Signature == fwd_protid) && (fp->Hop > 0))	return true;
		if( Pvt_Is_Forward_FwdRxOnly() ) return false;
	}
#endif
	if(	fp->Signature == Settings.ProtId )	return true;
	return false;
}

/*
 *
 */
uint8_t Pvt_RecvCheck( uint8_t *payload, uint16_t size )
{
	volatile uint8_t	res = RX_CHK_FLAGS_IGNORE;

	if( size > LORA_FRAME_SIZE )		return	res;
	if( size < LORA_FRAME_HEAD_SIZE )	return	res;
	if( IS_DEBUG_RECV_ALL )			return	RX_CHK_FLAGS_ACCEPT;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
	DataFrame_t *frame_p = (DataFrame_t *)payload;
#pragma GCC diagnostic pop

#if LORA_RECV_CHECK < 3
	if( Pvt_ProId_Check( frame_p ) == false )	return res;
#if LORA_RECV_CHECK < 2
	if( frame_p->GrpId != Settings.Gid )		return res;
#if LORA_RECV_CHECK < 1
	res = RX_CHK_FLAGS_FORWARD;
	__NOP();
	if( Settings.Own   == frame_p->DstId    )	res = RX_CHK_FLAGS_ACCEPT;
	if( Settings.Own   == LORA_BROADCAST_ID )	res = RX_CHK_FLAGS_ACCEPT | RX_CHK_FLAGS_FORWARD;
	if( frame_p->DstId == LORA_BROADCAST_ID )	res = RX_CHK_FLAGS_ACCEPT | RX_CHK_FLAGS_FORWARD;
#endif
#endif
#endif
#if __LORA_FWD_ENABLE__
	if( Pvt_Is_Forward_Enable() )
	{
		if( frame_p->Hop >= Settings.FwdHop )	res &= ~RX_CHK_FLAGS_FORWARD;
		if( res == RX_CHK_FLAGS_IGNORE )	return res;
		if( Pvt_Forward_Check( frame_p ) )	res = RX_CHK_FLAGS_IGNORE;	// ��M�ς�
		return res;
	}
#endif
	res &= ~RX_CHK_FLAGS_FORWARD;
	return res;
}

/*
 *
 */
void Pvt_TxContinuousWave( void )
{
	LED_TX_ON();
	Radio.SetTxContinuousWave( Settings.Freq_Hz, Settings.Power, UINT16_MAX );
}

/*
 *
 */
void Pvt_IntervalTimetStart( uint32_t timeout )
{
	if( timeout == 0 )
	{
		StateInf.IntervalFlag = true;
	}else
	{
		StateInf.IntervalFlag = false;
		TimerSetValue( &IntervalTimer, timeout );
		TimerStart( &IntervalTimer );
	}
}

/*
 *
 */
void Pvt_IntervalTimetStop( void )
{
	TimerStop( &IntervalTimer );
}

/*!
 * \brief Function executed on Tx Pause Timeout event
 */
static void Pvt_OnTxPauseTimerEvent( void *context )
{
	if( StateInf.TxPauseFlag )
	{
	    StateInf.TxPauseFlag = false;
		LED_BUSY_OFF();
	}else
	if( StateInf.State == ACTIVE )
	{
		Pvt_OnTxTimeout();
	}
}

#if __LORA_AES_ENABLE__

#include "crypto_hw_aes.h"

uint8_t *Pvt_AesEncodeDecode( uint8_t *buff, uint16_t size )
{
	if( buff == NULL ) return buff;
	if( Settings.AesEnable == false )	return buff;
	if( size <= LORA_FRAME_HEAD_SIZE )	return buff;
	uint32_t add = buff[0] + (buff[1]<<8) + (buff[2]<<16) + (buff[3]<<24);	// Sig,Gid
	uint32_t seq = buff[4] + (buff[5]<<8) + (buff[6]<<16) + (buff[7]<<24);	// Own,Dst
	uint8_t dir = buff[9];	// Length
	size -= LORA_FRAME_HEAD_SIZE;
	LoRaMacPayloadEncrypt( buff+LORA_FRAME_HEAD_SIZE, size, Settings.AesKey, add, dir, seq, buff+LORA_FRAME_HEAD_SIZE );
	return buff;
}
#endif

/*
 *
 */
void Pvt_CadStart( int32_t timeout )
{
	Pvt_RxTimetStart( timeout );
	StateInf.CadDoneFlag = false;
	Radio.StartCad();
	while( !StateInf.CadDoneFlag );
}

#endif