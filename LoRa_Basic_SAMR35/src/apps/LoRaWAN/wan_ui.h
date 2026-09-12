/*!
 * @file    wan_ui.h
 * @brief   HeaderFile of User interface for LoRa-WAN mode LoRa protocol
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
#ifndef __WAN_UI_H__
#define __WAN_UI_H__

#include "RegionAS923.h"

// Settings.Wan

typedef enum
{
	WAN_ADR_OFF = 0,
	WAN_ADR_ON	= 1,
} Wan_Adr_t;

typedef enum
{
	WAN_CNF_OFF = 0,
	WAN_CNF_ON	= 1,
} Wan_Cnf_t;

#define WAN_DR_MIN					0
#define WAN_DR_MAX					6
#define WAN_DR_DEFAULT				2

#define WAN_FPORT_MIN				1
#define WAN_FPORT_MAX				223
#define WAN_FPORT_DEFAULT			WAN_FPORT_MIN

#define WAN_TRIAL_MIN				1
#define WAN_TRIAL_MAX				8
#define WAN_TRIAL_DEFAULT			WAN_TRIAL_MIN

#define WAN_TXPOWER_MIN				0
#define WAN_TXPOWER_MAX				7
#define WAN_TXPOWER_DEFAULT			WAN_TXPOWER_MIN		// MaxEIRP

#define WAN_ADR_MIN                 WAN_ADR_OFF
#define WAN_ADR_MAX                 WAN_ADR_ON
#define WAN_ADR_DEFAULT             WAN_ADR_OFF

#define WAN_CNF_MIN                 WAN_CNF_OFF
#define WAN_CNF_MAX                 WAN_CNF_ON
#define WAN_CNF_DEFAULT             WAN_CNF_OFF

#define WAN_CHMASK_DEFAULT			0x00

// WanConf

typedef enum
{
	WAN_JOINMODE_NONE	= 0,
	WAN_JOINMODE_OTAA	= 1,
	WAN_JOINMODE_ABP	= 2,
} Wan_JoinMode_t;

#define WAN_JOINMODE_MIN			WAN_JOINMODE_OTAA
#define WAN_JOINMODE_MAX			WAN_JOINMODE_ABP
#define WAN_JOINMODE_DEFAULT		WAN_JOINMODE_OTAA

typedef enum
{
	WAN_CLASS_A = 0,
	WAN_CLASS_B	= 1,
	WAN_CLASS_C	= 2,
} Wan_Class_t;

#define WAN_CLASS_DEFAULT			WAN_CLASS_A

#define WAN_RX_DELAY_MIN			500
#define WAN_RX_DELAY_MAX			9999
#define WAN_RECV_DELAY1_DEFAULT		AS923_RECEIVE_DELAY1
#define WAN_RECV_DELAY2_DEFAULT		AS923_RECEIVE_DELAY2
#define WAN_JOIN_DELAY1_DEFAULT		AS923_JOIN_ACCEPT_DELAY1
#define WAN_JOIN_DELAY2_DEFAULT		AS923_JOIN_ACCEPT_DELAY2

#define LORA_BATLEVEL_MIN			0
#define LORA_BATLEVEL_MAX			255
#define LORA_BATLEVEL_DEFAULT		0

#define WAN_SETTINGS_INITIALIZED	0x1234

#define WAN_CONF_INITIALIZED		0x1238

typedef struct
{
	uint16_t		Initialized;	// Identifier
	Wan_JoinKeys_t	JoinKeys;
	Wan_RxDelay_t	RxDelay;
	Wan_JoinMode_t	JoinMode;
	Wan_Class_t		Class;
} WanConf_t;

extern	WanConf_t	WanConf;

/*!
 * \brief Function to UI Command Main
 */
void WanUi_Init( void );
void WanUi_Stat( void );
UiRes_t WanUi_Join_OTAA( void );
UiRes_t WanUi_Join_ABP( void );
UiRes_t WanUi_Unjoin( void );
Wan_JoinStat_t WanUi_GetJoinStatus( void );
UiRes_t WanUi_SetLinkchk( void );
uint8_t WanUi_GetLinkChk( void );
char *WanUi_GetLinkCheckStr( char *buff );
UiRes_t WanUi_Tx( WanDataFrame_t *txframe, WanDataFrame_t *rxframe, uint8_t code );
UiRes_t WanUi_Rx( WanDataFrame_t *rxframe, int32_t timeout, uint8_t code );
UiRes_t	WanUi_RxStop( void );
UiRes_t WanUi_ConfJoinMode( Wan_JoinMode_t mode );
uint16_t WanUi_GetChMask( void );
void WanUi_SetChMask( uint16_t ChannelMask );
uint32_t WanUi_GetUpLinkCount( void );
uint32_t WanUi_GetDonwLinkCount( void );
void WanUi_SetUpLinkCount( uint32_t linkCount );
void WanUi_SetDownLinkCount( uint32_t linkCount );
void WanUi_SettingsDefault( void );
void WanUi_Conf_Init( bool force );
uint8_t WanUi_Conf_Save( void );
void WanUi_DeveuiInit( void );
void WanUi_DeveuiCopy( void );
uint8_t WanUi_GetLength( void );
uint8_t WanUi_GetDwellStat( void );
uint8_t WanUi_GetLastTxCh( void );
int8_t WanUi_GetTxPower( uint8_t pwr );
void WanUi_SetRxDelay( Wan_RxDelay_e rxDelay );
uint16_t WanUi_GetRxDelay( Wan_RxDelay_e rxDelay );
UiRes_t WanUi_ConfClass( uint8_t class );
UiRes_t WanUi_SetClass( uint8_t class );
void WanUi_ChList( void );
uint8_t WanUi_GetPayloadLen( WanDataFrame_t *txframe );
uint8_t WanUi_GetPending( void );
bool WanUi_RxProcess( WanDataFrame_t *rxframe );
void WanUi_RxContinue( void );
#if __WAN_CH_ADD_ENABLE__
UiRes_t WanUi_ChannelAdd( uint8_t id, uint32_t freq, uint8_t dr_min, uint8_t dr_max );
UiRes_t WanUi_ChannelRemove( uint8_t id );
#endif
UiRes_t WanUi_SetTimeReq( int32_t val );
uint8_t WanUi_GetTimeReq( void );
uint32_t WanUi_GetDutyTime( void );
uint8_t *WanUi_GetMacCommand( uint8_t *len );
void WanUi_SetBatLevel( uint8_t batlevel );
uint8_t WanUi_GetBatLevel( void );

inline bool Ui_IsWanMode( void ){ return Settings.Lora_Mode == LORA_MODE_WAN; }
inline bool WanUi_IsWanJoinABP ( void ){ return WanConf.JoinMode == WAN_JOINMODE_ABP ; }
inline bool WanUi_IsWanJoinOTAA( void ){ return WanConf.JoinMode == WAN_JOINMODE_OTAA; }
inline bool WanUi_IsWanAdr( void ){ return Settings.Wan.Adr != WAN_ADR_OFF; }
inline bool WanUi_IsWanCnf( void ){ return Settings.Wan.Cnf != WAN_CNF_OFF; }

#endif
