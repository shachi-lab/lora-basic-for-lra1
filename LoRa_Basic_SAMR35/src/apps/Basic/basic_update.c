 /*!
 * @file    basic_update.c
 * @brief   BASIC App Update
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
 #include "radio.h"
 #include "basic.h"
 #include "bios.h"
 #include "utilities.h"
 #include "basic_update.h"

static int update_send_cmd( uint8_t *ptr, uint32_t len ,uint8_t cmd, uint8_t seq );
static uint8_t update_send_res(void);
static uint8_t update_send_frame( DataFrame_t *frame, uint32_t len );
typedef struct {
	uint16_t pro;
	uint8_t sf;
	uint8_t bw;
	uint8_t cr;
	uint8_t ch;
	uint8_t hop;
} update_conf_save_t;
static void update_config( update_conf_save_t *save, bool restore );

#define UPDATE_CMD_STRAT	0
#define UPDATE_CMD_CODE		1
#define UPDATE_CMD_END		2

#define UPDATE_POS_CMD		0
#define UPDATE_POS_SEQ		1
#define UPDATE_POS_DATA		2
#define UPDATE_HDR_LEN		2
#define UPDATE_RES_FLAG		0x80

static uint16_t update_size;
static uint16_t update_total;

//*************************************************
bool basic_psave_update( uint8_t *top, uint32_t size )
{
	uint32_t len;
	uint8_t seq = 0;
	update_conf_save_t save;

	if( basic_LoRa_Pvt_Check() ) return false;
	bios_UartPutLine( "Psave on the air" );
#if 0
	if( top == NULL ) {
		top = PROGRAM_AREA_TOP;
		size = progLength;
	}
	if ( !size ) return true;
#endif
	uint8_t *ptr = top;
	update_total = size;
	update_size = 0;
	update_config( &save, false );
	int res = update_send_cmd( (uint8_t*)&size, sizeof(size), UPDATE_CMD_STRAT, seq++ );
	uint16_t sum = basic_sum( ptr, size );
	while ( !res )
	{
		if ( size )
		{
			len = size > UPDATE_BLOCK_SIZE ? UPDATE_BLOCK_SIZE : size;
			update_size += len;
			res = update_send_cmd( ptr, len, UPDATE_CMD_CODE, seq++ );
			ptr += len;
			size -= len;
		} else {
			res = update_send_cmd( (uint8_t*)&sum, sizeof(sum), UPDATE_CMD_END, seq );
			break;
		}
	}
	update_config( &save, true );
	if( !res ) bios_UartPutCrLf();
	return false;
}

static void update_put_status( void )
{
	bios_UartPrintf("\x1b[G%4d/%d  ", update_size, update_total );
}

//*************************************************
static int update_send_cmd( uint8_t *ptr, uint32_t len ,uint8_t cmd, uint8_t seq )
{
	basic_LoRa_TxFrame.Data[UPDATE_POS_CMD] = cmd;
	basic_LoRa_TxFrame.Data[UPDATE_POS_SEQ] = seq;

	if ( ptr ) {
		memcpy( &basic_LoRa_TxFrame.Data[UPDATE_POS_DATA], ptr, len );
	}
//	bios_UartPuts("-\b");
	update_put_status();

	for (int retry = UPDATE_SEND_RETRY; retry; retry--)
	{
		update_send_frame( &basic_LoRa_TxFrame, len + UPDATE_HDR_LEN );
		PvtUi_Recv( &basic_LoRa_RxFrame, 0, 0 );
	 	bios_setWaitTick( UPDATE_RES_TIMEOUT );
	 	while (1)
		{
			if ( !bios_getWaitTick() ) break;
	 		if ( Ui_IsBreak_BK() ) goto STOP;

			if (StateInf.State != RX_DONE) continue;
			if (basic_LoRa_RxFrame.Length == UPDATE_HDR_LEN &&
				basic_LoRa_RxFrame.Data[UPDATE_POS_CMD] == (cmd | UPDATE_RES_FLAG) &&
				basic_LoRa_RxFrame.Data[UPDATE_POS_SEQ] == seq )
			{
				bios_UartPuts("\b ");
				basic_LoRa_stat = UI_RES_OK;
				goto STOP;
			}
			StateInf.State = ACTIVE;
		}
		bios_UartPuts("\b?");
	}
	basic_LoRa_stat = UI_RES_TIMEOUT;
STOP:
	Pvt_RxStop();
	if ( errorCode ) return -1;
	if ( !basic_LoRa_stat ) return 0;
	bios_UartPutCrLf();
	return Ui_PutRespStr(basic_LoRa_stat);
}

//*************************************************
static bool chk_rcv_cmd( uint8_t cmd, uint8_t seq )
{
	if ( basic_LoRa_RxFrame.Data[UPDATE_POS_CMD] != cmd ) return false;
	if ( seq == 0 ) return true;
	if( basic_LoRa_RxFrame.Data[UPDATE_POS_SEQ] == seq ) return true;
	if( basic_LoRa_RxFrame.Data[UPDATE_POS_SEQ] == seq - 1 ) {
		update_send_res();
	}
	return false;
}

//*************************************************
static uint8_t update_send_frame( DataFrame_t *frame, uint32_t len )
{
	uint8_t res;
	for ( int i = 0; i < UPDATE_SEND_RETRY; i++ ) {
		DelayMs( UPDATE_SEND_DELAY );
		res = Pvt_SendFrame( frame, len, false );
		if( !res ) return res;
	}
	return res;
}

//*************************************************
bool basic_pload_update( bool append, int32_t timeout )
{
	uint8_t *top = PROGRAM_AREA_TOP;
//	uint32_t size = 0;
//	uint32_t total = 0;
	uint32_t len;
	uint8_t seq = 0;
	update_conf_save_t save;
	bool destroyed = false;

	if( basic_LoRa_Pvt_Check() ) return false;
	bios_UartPuts( "Pload on the air " );
	if ( append ) {
		if( progLength ) {
			top = &top[progLength] - 1;
			bios_UartPutChar( '+' );
		} else {
			bios_UartPutChar( '-' );
		}
	}
	bios_UartPutCrLf();
	uint8_t *ptr = top;

	if( timeout ) bios_setWaitTick( timeout );
	update_config( &save, false );
	while ( 1 )
	{
		if ( StateInf.RxEnable == false ) {
			PvtUi_Recv( &basic_LoRa_RxFrame, 0, 0 );
		}
		if ( timeout && !bios_getWaitTick() ) break;
	 	if ( Ui_IsBreak_BK() ) goto STOP;
		if (StateInf.State != RX_DONE) continue;
		if (basic_LoRa_RxFrame.Length > UPDATE_HDR_LEN) {
			len = basic_LoRa_RxFrame.Length - UPDATE_HDR_LEN;
			if ( chk_rcv_cmd( UPDATE_CMD_STRAT, 0 ) && len >= 2 ) {
				update_total = basic_LoRa_RxFrame.Data[UPDATE_POS_DATA+1];
				update_total = (update_total << 8) + basic_LoRa_RxFrame.Data[UPDATE_POS_DATA+0];
				update_size = 0;
				seq = 1;
				ptr = top;
				update_send_res();
			} else
			if ( chk_rcv_cmd( UPDATE_CMD_CODE, seq ) ) {
				if ( len <= UPDATE_BLOCK_SIZE ) {
					if( (ptr + len) > (PROGRAM_AREA_TOP + PROGRAM_AREA_SIZE) ) {
						errorCode = ERROR_OVERFLOW;
						goto STOP;
					}
					memcpy( ptr, &basic_LoRa_RxFrame.Data[UPDATE_POS_DATA], len );
					destroyed = true;
					ptr += len;
					update_size += len;
					seq++;
					update_send_res();
				}
			} else
			if ( chk_rcv_cmd( UPDATE_CMD_END, seq ) && len >= 2 ) {
				uint16_t sum = basic_LoRa_RxFrame.Data[UPDATE_POS_DATA+1];
				sum = (sum << 8) + basic_LoRa_RxFrame.Data[UPDATE_POS_DATA+0];
				if ( sum == basic_sum( top, update_size ) ) {
					if( ptr < (PROGRAM_AREA_TOP + PROGRAM_AREA_SIZE) ) 	*ptr = '\0';
					update_send_res();
					update_send_res();
					basic_LoRa_stat = UI_RES_OK;
					goto STOP;
				}
			}
		}
		if (StateInf.RxEnable) StateInf.State = ACTIVE;
	}
	basic_LoRa_stat = UI_RES_TIMEOUT;
STOP:
	Pvt_RxStop();
	bios_UartPutCrLf();
	update_config( &save, true );
	if ( errorCode || basic_LoRa_stat ) {
		if ( destroyed ) *top = ST_EOL;
		if (!errorCode ) Ui_PutRespStr(basic_LoRa_stat);
	} else
	if( (top == ptr) && seq ) return true;
	return false;
}

//*************************************************
static uint8_t update_send_res(void)
{
	if (StateInf.RxEnable) {
		Pvt_RxStop();
		Settings.Dst = basic_LoRa_RxFrame.OwnId;
		basic_LoRa_RxFrame.Data[UPDATE_POS_CMD] |= UPDATE_RES_FLAG;
		update_put_status();
//		bios_UartPutChar('o');
	}
	return update_send_frame( &basic_LoRa_RxFrame, UPDATE_HDR_LEN );
}

//*************************************************
static void update_config( update_conf_save_t *save, bool restore )
{
	if( restore )
	{
		Settings.Sf = save->sf;
		Settings.Bw = save->bw;
		Settings.Cr = save->cr;
		Settings.Ch = save->ch;
		Settings.ProtId = save->pro;
		Settings.FwdHop = save->hop;
	} else
	{
		save->sf = Settings.Sf;
		save->bw = Settings.Bw;
		save->cr = Settings.Cr;
		save->ch = Settings.Ch;
		save->pro = Settings.ProtId;
		save->hop = Settings.FwdHop;
#if UPDATE_CONF_SF
		Settings.Sf = UPDATE_CONF_SF;
#endif
#if UPDATE_CONF_BW
		Settings.Bw = UPDATE_CONF_BW;
#endif
#if UPDATE_CONF_CR
		Settings.Cr = UPDATE_CONF_CR;
#endif
#if UPDATE_CONF_CH
		Settings.Ch = UPDATE_CONF_CH;
#endif
#if UPDATE_CONF_PROID
		Settings.ProtId = UPDATE_CONF_PROID;
#endif
		Settings.FwdHop = 0;
	}
	PvtUi_ChBwToFreq( Settings.Modem, Settings.Ch, Settings.Bw );
}

//*************************************************
uint16_t basic_sum( uint8_t *ptr, uint32_t len )
{
	uint16_t sum = 0;
	while ( len-- )	sum += *ptr++;
	return sum;
}

#ifdef ST_UPDATE
/*
 * Update
 */
PROC( proc_Update )
{
	update_conf_save_t save;
	update_config( &save, false );

	if ( *executionPointer == '!' )
    {
		executionPointer++;
		basic_psave_update();
	} else
	if( *executionPointer == '&' )
	{
		executionPointer++;
		basic_pload_update(true);
	} else {
		basic_pload_update(false);
	}
	update_config( &save, true );
}
#endif
