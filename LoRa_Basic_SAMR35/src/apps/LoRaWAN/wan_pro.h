/*!
 * @file    wan_pro.h
 * @brief   HeaderFile of Wrapper for LoRa-WAN mode LoRa protocol
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
#ifndef __WAN_PRO_H__
#define __WAN_PRO_H__

#include "LoRaMac.h" 

/*!
 * Device states
 */
typedef enum
{
    DEVICE_STATE_NULL = 0,
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOINED,
    DEVICE_STATE_SEND,
    DEVICE_STATE_CYCLE,
    DEVICE_STATE_SLEEP
}Wan_devstat_t;

typedef enum
{
	WAN_JOINSTAT_NONE	= 0,
	WAN_JOINSTAT_OTAA	= 1,
	WAN_JOINSTAT_ABP	= 2,
} Wan_JoinStat_t;


#define DEVEUI_LEN			8
#define JOINEUI_LEN			8
#define APPKEY_LEN			16
#define NWKSKEY_LEN			16
#define APPSKEY_LEN			16
#define NETID_LEN			4
#define DEVADDR_LEN			4

typedef struct
{
	uint16_t	Signature;		// P2P SIG
	uint16_t	LinkCounter[2];	// Up/Down link counter
	union {
	uint8_t		Rssi;			// Rssi
	uint8_t		Ch;				// Channel-id
	};
	uint8_t		Dr;				// Datarate
	uint8_t		Port;			// Port
	uint8_t		Length;			// Length of Data
	uint8_t		Data[LORA_FRAME_DATA_SIZE];
}WanDataFrame_t;

typedef struct
{
    bool		Sending;
    bool		Confirmed;
    uint8_t		Len;
    uint8_t		Port;
    uint8_t		Datarate;
    uint8_t		Trial;
    uint8_t		Power;
	uint8_t		reserve;
	uint8_t		*Data_ptr;
} Wan_TxPara_t;

typedef struct
{
	bool		Received;
	bool		Pending;
	uint8_t		Len;
	uint8_t		Port;
	uint8_t		Datarate;
	int8_t		Snr;
	int16_t		Rssi;
	uint8_t		*Data_ptr;
} Wan_RxPara_t;

typedef struct
{
	uint32_t	NetID;
	uint32_t	DevAddr;
	uint8_t		NwkSkey[NWKSKEY_LEN];
	uint8_t		AppSkey[APPSKEY_LEN];
} Wan_Keys_t;

typedef struct
{
	uint8_t		DevEui[DEVEUI_LEN];
	uint8_t		JoinEui[JOINEUI_LEN];
	uint8_t		AppKey[APPKEY_LEN];
	Wan_Keys_t	ABP_Keys;
	Wan_Keys_t	OTAA_Keys;
} Wan_JoinKeys_t;

typedef struct
{
	uint16_t	RxDelay[4];
} Wan_RxDelay_t;

typedef enum {
	WAN_RECV_DELAY_1	= 0,
	WAN_RECV_DELAY_2	= 1,
	WAN_JOIN_DELAY_1	= 2,
	WAN_JOIN_DELAY_2	= 3,
} Wan_RxDelay_e;

typedef struct
{
	Wan_JoinStat_t	JoinStat;
	uint8_t			Class;		// Class
	bool			NextTx;		// Indicates if a new packet can be sent
	bool			Adr;
	bool			IsPrinted;
	struct {
		uint8_t		Channel;
		uint8_t		Datarate;
		uint32_t	UpLinkCounter;
		uint32_t	TxDoneTime;
	} Sent;
	struct {
		bool		Req;
	    uint8_t		DemodMargin;
		uint8_t		NbGateways;
	} LinkChk;
	uint16_t		ChannelsMask;
	uint32_t		DownLinkCounter;
	uint32_t		UpLinkCounter;
	Wan_RxDelay_t	RxDelay_now;
	Wan_RxDelay_t	*RxDelay_ptr;
	Wan_RxPara_t	*RxPara_ptr;
	Wan_TxPara_t	*TxPara_ptr;
	Wan_JoinKeys_t	*JoinKeys_ptr;
	struct {
		uint8_t		Req;
		uint8_t		Sub_seconds;
		uint32_t	Seconds;
	}DeviceTime;
} Wan_Info_t;

Wan_Info_t *Wan_init( Wan_JoinKeys_t *join_key_p );
UiRes_t Wan_join_OTAA( void );
UiRes_t Wan_join_ABP( void );
UiRes_t Wan_linkchk( void );
UiRes_t Wan_send( void );
UiRes_t Wan_wait( uint32_t waitTime );
bool Wan_RxCheck( void );
void Wan_RxClear( void );
UiRes_t Wan_SetAdr( void );
UiRes_t Wan_GetAdr( void );
UiRes_t Wan_GetUplinkCounter( void );
UiRes_t Wan_GetDownlinkCounter( void );
UiRes_t Wan_SetUplinkCounter( void );
UiRes_t Wan_SetDownlinkCounter( void );
UiRes_t Wan_GetRxDelay( Wan_RxDelay_e rxDelay );
UiRes_t Wan_SetRxDelay( Wan_RxDelay_e rxDelay );
UiRes_t Wan_GetJoinKeys( void );
UiRes_t Wan_GetJoinStatus( void );
UiRes_t Wan_SetJoinStatus( Wan_JoinStat_t stat );
UiRes_t Wan_GetClass( void );
UiRes_t Wan_SetClass( DeviceClass_t Class );
UiRes_t Wan_SetDatarate( uint8_t datarate );
UiRes_t Wan_SetPower( uint8_t txpower );
UiRes_t Wan_SetChMask( void );
UiRes_t Wan_GetChannel( ChannelParams_t **ch_list );
UiRes_t Wan_GetChMask( uint16_t *ch_mask );
UiRes_t Wan_GetPayloadLength( uint8_t *len );
UiRes_t Wan_GetDwellStat( uint8_t *dwell_stat );
UiRes_t Wan_GetLastTxChannel( uint8_t *last_tx_ch );
#if __WAN_CH_ADD_ENABLE__
UiRes_t Wan_ChannelAdd( uint8_t id, uint32_t freq, uint8_t dr_min, uint8_t dr_max );
UiRes_t Wan_ChannelRemove( uint8_t id );
#endif
uint32_t Wan_GetDutyCycleTime( void );
typedef uint8_t (*Wan_DeviceTimeReq_Callback_t)( uint32_t seconds, uint32_t milli_seconds );
UiRes_t Wan_DeviceTimeReq( Wan_DeviceTimeReq_Callback_t callback_func );
uint8_t Wan_IsExistTimeReq( void );
uint8_t Wan_IsExistLinkChk( void );
uint8_t *Wan_GetMacCommand( uint8_t *len );
#endif
