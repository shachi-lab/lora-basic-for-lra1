/*!
 * @file    pvt_fwd.c
 * @brief   LRA1 LoRa P2P protocol (Forwarding Extension)
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
#include "sys_delay.h"
#include "sys_timer.h"
#include "rtc-board.h"
#include "radio.h"
#include "basic.h"
#include "bios.h"
#include "utilities.h"
#include "pvt_fwd.h"

#if __LORA_FWD_ENABLE__

typedef struct
{
	uint32_t	Timestamp;
	uint32_t	CheckSum;
} Forward_list_t;

static Forward_list_t Forward_list[FORWARD_LIST_SIZE] = {0};
static uint8_t fwd_list_top = 0;
static uint8_t fwd_list_cnt = 0;
Forward_list_t fwd_buff;
static DataFrame_t fwd_TxBuff;

/*
 * ��M�t���[����Checksum(CRC32)���v�Z
 * (Hop,Signature��0�Ƃ��Čv�Z����j
 */
static uint32_t Pvt_Forward_GetChecsum( DataFrame_t *frame_p )
{
	uint8_t ctrl_save = frame_p->Ctrl;
	uint16_t sig_save = frame_p->Signature;
	frame_p->Hop = 0;
	frame_p->Signature = 0;
	uint32_t sum = CalculateCRC32( frame_p, frame_p->Length + LORA_FRAME_HEAD_SIZE, CRCBEATSIZE_BYTE );
	frame_p->Ctrl = ctrl_save;
	frame_p->Signature = sig_save;
	return sum;
}

/*
 *	��M�ς݂̃`�F�b�N
 *  true:��M�� (List�ɑ���) / false:����M (List�ɒǉ�)
 */
bool Pvt_Forward_Check( DataFrame_t *frame_p )
{
	Forward_list_t *p;

	if( Pvt_Is_Forward_FwdRxBoth() ) return false;

	fwd_buff.Timestamp = TimerGetCurrentTime();
	fwd_buff.CheckSum = Pvt_Forward_GetChecsum( frame_p );

	uint8_t next = fwd_list_top;
	uint8_t	cnt = fwd_list_cnt;
	while( 1 )
	{
		p = &Forward_list[next];
		if( cnt == 0 )	break;
		cnt--;
		if( ++next >= FORWARD_LIST_SIZE ) next = 0;

		if( (fwd_buff.Timestamp - p->Timestamp ) > FORWARD_KEEP_TIME )
		{
			fwd_list_cnt = cnt;
			fwd_list_top = next;
		}else
		if( p->CheckSum == fwd_buff.CheckSum )	return true;
	}

	p->Timestamp = fwd_buff.Timestamp;
	p->CheckSum = fwd_buff.CheckSum;

	if( ++fwd_list_cnt > FORWARD_LIST_SIZE )
	{
		fwd_list_cnt = FORWARD_LIST_SIZE;
		if( ++fwd_list_top >= FORWARD_LIST_SIZE ) fwd_list_top = 0;
	}
	return false;
}

static enum
{
	Fwd_IDLE	= 0,
	Fwd_WAIT	= 1,
	Fwd_TIMEOUT = 2,
} Pvt_Forword_stat = Fwd_IDLE;
//static uint32_t Pvt_Forward_time;
static uint32_t Pvt_Forword_delay;
static uint32_t Pvt_Forword_limit;

/*
 * �]�����M���ԑ҂�
 * stop_flag = 0:�J�n&�`�F�b�N / 1:���g���C / -1:����
 * Return = true:�҂��� / false:�҂�����
 */
static bool Pvt_Forword_IsWaiting( int8_t stop_flag )
{
	uint32_t now = RtcGetTimerValue();
	if( stop_flag == 0 )						// 1st & Check
	{
		if( Pvt_Forword_stat == Fwd_IDLE )
		{
			Pvt_Forword_stat = Fwd_WAIT;
//			Pvt_Forward_time = now;
			Pvt_Forword_delay = now + (Radio.Random() & FORWARD_DELAY_MASK) + FORWARD_DELAY_MIN;
			Pvt_Forword_limit = now + FORWARD_TIME_MAX;
			return true;
		}
		if( Pvt_Forword_stat == Fwd_WAIT )
		{
			int32_t tt = Pvt_Forword_delay - now;
			if( tt > 0 ) return true;
			Pvt_Forword_stat = Fwd_TIMEOUT;
		}
		return false;
	}

	if( stop_flag > 0 )							// Retry
	{
		int32_t tt = Pvt_Forword_limit - now;
		if( tt > 0 )
		{
			Pvt_Forword_stat = Fwd_WAIT;
			Pvt_Forword_delay += FORWARD_TIME_RETRY;		
			return true;
		}
	}
	Pvt_Forword_stat = Fwd_IDLE;
	return false;
}

/*
 * �]���p�����[�^���`�F�b�N
 */
static fwdTxConf_t sv_txconf;
static uint32_t sv_txfreq;
static uint32_t Pvt_Forward_CheckTxConf( void )
{
	fwdTxConf_t p;
	p.parm = Pvt_Forward_GetTxConf();
	if(( p.Sf > LORA_SF_MAX || p.Sf < LORA_SF_MIN )
	 ||( p.Bw > LORA_BW_MAX || p.Bw < LORA_BW_MIN )
	 ||( p.Cr > LORA_CR_MAX || p.Cr < LORA_CR_MIN )
	 ||( p.Ch > LORA_CH_MAX || p.Ch < LORA_CH_MIN ))
	{
		return 0;
	}
	return p.parm;
}

/*
 * �ݒ�p�����[�^���擾
 */
uint32_t Pvt_Forward_SetTxConf( int32_t val )
{
	if( val != 0 ) return val;
	fwdTxConf_t p;
	p.Sf = Settings.Sf;
	p.Bw = Settings.Bw;
	p.Cr = Settings.Cr;
	p.Ch = Settings.Ch;
	return p.parm;
}

/*
 * �]���p�����[�^���擾
 */
uint32_t Pvt_Forward_GetTxConf( void )
{
	return Settings.FwdTxConf ? Settings.FwdTxConf : UINT32_MAX;
}

/*
 * �ݒ�p�����[�^��]���p�ɐݒ�
 */
static void Fwd_LoadTxConf( void )
{
	fwdTxConf_t p;
	p.parm = Pvt_Forward_CheckTxConf();
	if( p.parm == 0 ) return;
	sv_txconf.parm = Pvt_Forward_SetTxConf( 0 );
	sv_txfreq = Settings.Freq_Hz;
	Settings.Sf = p.Sf;
	Settings.Bw = p.Bw;
	Settings.Cr = p.Cr;
	Settings.Ch = p.Ch;
	PvtUi_ChBwToFreq( Settings.Modem, Settings.Ch, Settings.Bw );
}

/*
 * �ݒ�p�����[�^�����ɖ߂�
 */
static void Fwd_RestTxConf( void )
{
	if( sv_txconf.Sf == 0 )	return;
	Settings.Sf = sv_txconf.Sf;
	Settings.Bw = sv_txconf.Bw;
	Settings.Cr = sv_txconf.Cr;
	Settings.Ch = sv_txconf.Ch;
	Settings.Freq_Hz = sv_txfreq;
}

/*
 * �]�����M
 */
UiRes_t Pvt_Forward_Send( void )
{
	UiRes_t res = UI_RES_NOTHING;
	do{
		if( Settings.FwdHop == 0 ) break;
		if( StateInf.RxEnable == false ) break;	//��M���ȊO�͓]�����Ȃ�
		if( StateInf.Forward.Len < LORA_FRAME_HEAD_SIZE ) break;
		if( StateInf.Forward.FramePtr != NULL )
		{
			if( StateInf.Forward.FramePtr->Hop >= LORA_HOP_MAX ) break;
			memcpy( &fwd_TxBuff, StateInf.Forward.FramePtr, StateInf.Forward.Len );
			StateInf.Forward.FramePtr = NULL;
			fwd_TxBuff.Signature = ~Settings.ProtId;
			fwd_TxBuff.Hop++;
		}
		if( Pvt_Forword_IsWaiting( 0 ) ) return UI_RES_DC_RESTRICTED;

		CRITICAL_SECTION_BEGIN();
		States_t ss = StateInf.State;
		Pvt_RadioStandbySleep();
		CRITICAL_SECTION_END();

		uint8_t *ptr_save = StateInf.BufferPtr;
		uint16_t len_save = StateInf.BufferLen;
		Fwd_LoadTxConf();
		res = Pvt_SendFrame( &fwd_TxBuff, StateInf.Forward.Len, true );
		Fwd_RestTxConf();
		StateInf.BufferPtr = ptr_save;
		StateInf.BufferLen = len_save;

		Pvt_RxStart( Settings.Modem, -1 );
		StateInf.RxEnable = true;
		StateInf.State = ss;
		if( res == UI_RES_NO_FREE_CH )
		{
			if( Pvt_Forword_IsWaiting( 1 ) ) return UI_RES_DC_RESTRICTED;
		}
		res = UI_RES_OK;
	}while( 0 );
	StateInf.Forward.Len = 0;
	StateInf.Forward.FramePtr = NULL;
	Pvt_Forword_IsWaiting( -1 );
	return res;
}

#endif

