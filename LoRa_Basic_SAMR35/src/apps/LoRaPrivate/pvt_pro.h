/*!
 * @file    pvt_pro.h
 * @brief   HeaderFile of "LRA1 LoRa P2P protocol (Radio layer)"
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
#ifndef __PVT_PRO_H__
#define __PVT_PRO_H__

#define CARRIER_SENS_WAIT_TIME			500
#define CARRIER_SENS_L_TIME             7           // 7 msec
#define CARRIER_SENS_H_TIME             0           // 125 usec
#define CARRIER_SENS_RSSI_LEVEL         -75         // -75 dBm

#define LORA_PREAMBLE_RX_LENGTH			8           // Symbols
#define LORA_PREAMBLE_TX_LENGTH			8           // Symbols

#define LORA_SYMBOL_TIMEOUT				0           // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_OFF		false
#define LORA_FIX_LENGTH_PAYLOAD_ON		true		// SF=6 only
#define LORA_CRC_ENABLE					true
#define LORA_IQ_INVERSION_OFF			false
#define LORA_RX_CONTINUOUS_ENABLE		true

#define FSK_FDEV						25000       // 25kHz
#define FSK_DATARATE					50000       // 50kbps
#define FSK_RX_BANDWIDTH				50000		// 50kHz
#define FSK_AFC_BANDWIDTH				83333       // 83.3kHz
#define FSK_PREAMBLE_BYTES				5           // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_OFF		false
#define FSK_CRC_ENABLE					LORA_CRC_ENABLE
#define FSK_SYMBOL_TIMEOUT				-1			// bytes
#define FSK_RX_CONTINUOUS_ENABLE		true

#define LORA_FRAME_DATA_SIZE			242
#define LORA_FRAME_DATA_SIZE_SF6BW62k5	120
#define LORA_FRAME_HEAD_SIZE			10
#define	LORA_FRAME_SIZE					(LORA_FRAME_HEAD_SIZE + LORA_FRAME_DATA_SIZE)
#define LORA_FRAME_SIZE_SF6BW62k5		(LORA_FRAME_HEAD_SIZE + LORA_FRAME_DATA_SIZE_SF6BW62k5)

#define LORA_FRAME_SEQ_INIT				0
#define LORA_FRAME_SEQ_MIN				1
#define LORA_FRAME_SEQ_MAX				7

#define LORA_PAYLOAD_LEN_DEFAULT		0x40

#define LORA_BROADCAST_ID				UINT16_MAX

#define IS_SPECIAL_SYNCWORD()			((Settings.Ctrl & LORA_CTRL_SPECIAL_SYNCWORD) != 0)

/*
#define	MODEM_FSK						0
#define	MODEM_LORA						1
*/
#define	MODEM_GFSK						2
#define MODEM_FSK_SENS					3
#define MODEM_LORA_LONG					4

typedef enum
{
    LOWPOWER,
	STANDBY,
    ACTIVE,
    RX_DONE,
    RX_TIMEOUT,
    RX_ERROR,
    TX_DONE,
    TX_TIMEOUT,
}States_t;

#define TIMEOUT_FLAG_NONE			0x00
#define TIMEOUT_FLAG_RX				0x01
#define TIMEOUT_FLAG_TX				0x02
#define TIMEOUT_FLAG_ERROR			0x04

#define RX_CHK_FLAGS_IGNORE			0x00
#define RX_CHK_FLAGS_ACCEPT			0x01
#define RX_CHK_FLAGS_FORWARD		0x02

//typedef struct __attribute__ ((packed))
typedef struct
{
	uint16_t	Signature;		// P2P SIG
    uint16_t	GrpId;			// Network ID (PANID)
	uint16_t	OwnId;			// Own ID
    uint16_t	DstId;			// Dst ID
	union{
		uint8_t	Ctrl;			// Control
		struct {
			uint8_t Seq : 3;	// Sequence No
			uint8_t Hop : 3;	// Hop Count
			uint8_t	Cmd : 2;	// Command
		};
	};
	uint8_t		Length;			// Length of Data
	uint8_t		Data[LORA_FRAME_DATA_SIZE];
}DataFrame_t;

typedef struct
{
    States_t	State;
	uint8_t		TimeOutFlags;
    int16_t		RssiValue;
    int8_t		SnrValue;

	bool 		RxEnable;
	bool		FixLen;
	bool		TxPauseFlag;
	bool		IntervalFlag;
	bool		RxForwardFlag;
	bool		CadDoneFlag;
	bool		CadDetectFlag;
	bool		CrcEnableFlag;
	uint8_t		SeqNum;

	uint16_t	PayloadSize;
    uint16_t	BufferLen;
	uint16_t	PreambleLen;
	uint32_t	OnairTime;
	uint32_t	TxPauseTime;
    union
	{
        uint8_t *BufferPtr;
        DataFrame_t *FramePtr;
    };
	struct{
		DataFrame_t	*FramePtr;
	    uint16_t	Len;
	}Forward;
}StateInf_t;

extern volatile StateInf_t  StateInf;

void Pvt_RadioInit( void );
void Pvt_TxConfig( uint8_t modem );
void Pvt_RxConfig( uint8_t modem );
void Pvt_RxStart( uint8_t modem, int32_t timeout );
void Pvt_RxStop( void );
void Pvt_RadioStandby( void );
void Pvt_RadioSleep( void );
void Pvt_RadioStandbySleep( void );
int32_t Pvt_GetRssi( uint8_t modem );
void Pvt_SetCarrierSensLevel( int8_t level );
int8_t Pvt_GetCarrierSensLevel( void );
bool Pvt_CarrierSens( uint32_t waitTime );
void Pvt_Send( uint8_t *buff, uint8_t len );
UiRes_t Pvt_SendFrame( DataFrame_t *frame, int16_t len, bool forward );
void Pvt_SetFrameHeader( DataFrame_t *frame, int16_t len );
uint8_t Pvt_RecvCheck( uint8_t *payload, uint16_t size );
void Pvt_LedOn( uint8_t led, uint16_t time );
void Pvt_LedOff( uint8_t led );
void Pvt_IntervalTimetStart( uint32_t timeout );
void Pvt_IntervalTimetStop( void );
uint32_t Pvt_GetOnairTime( uint8_t modem, uint8_t len );
UiRes_t Pvt_CheckOnAirTime( uint8_t len );
void Pvt_TxContinuousWave( void );
void Pvt_SetCallback( uint8_t (*callback_func)(uint8_t stat) );
uint32_t Ptv_onair_time( uint8_t *siz );
States_t Pvt_Callback( States_t stat );
void Pvt_CadStart( int32_t timeout );
inline void Pvt_CadStop( void ){ Pvt_RxStop(); }
uint32_t Pvt_GetTxPauseTimerRemain( void );

#endif
