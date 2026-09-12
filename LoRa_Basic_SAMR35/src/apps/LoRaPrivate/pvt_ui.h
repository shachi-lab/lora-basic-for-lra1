/*!
 * @file    pvt_ui.h
 * @brief   HeaderFile of "LRA1 LoRa P2P protocol (UI layer)"
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
#ifndef __PVT_UI_H__
#define __PVT_UI_H__

#define LORA_MODEM_MIN						0			// FSK
#define LORA_MODEM_MAX						1			// LoRa
#define LORA_MODEM_FSK
#define LORA_MODEM_LORA						1
#define LORA_MODEM_DEFAULT					1

#define LORA_POWER_OFFSET					0			// 0dBm
#define	LORA_POWER_MIN						-4
#define	LORA_POWER_MAX_ADMIN				15
#define	LORA_POWER_MAX						13
#define LORA_POWER_DEFAULT                  13			// dBm

#define	LORA_SF_MIN							6
#define	LORA_SF_MAX							12
#define LORA_SF_DEFAULT                     10			// [SF6..SF12]

#define	LORA_BW_7k8							0			// 0: 7.8 kHz
#define	LORA_BW_10k4						1			// 1: 10.4 kHz
#define	LORA_BW_15k6						2			// 2: 15.6 kHz
#define	LORA_BW_20k8						3			// 3: 20.8 kHz
#define	LORA_BW_31k25						4			// 4: 31.25 kHz
#define	LORA_BW_41k7						5			// 5: 41.7 kHz
#define	LORA_BW_62k5						6			// 6: 62.5 kHz
#define	LORA_BW_125k						7			// 7: 125 kHz
#define	LORA_BW_250k						8			// 8: 250 kHz
#define	LORA_BW_500k						9			// 9: 500 kHz
#define	LORA_BW_MIN_ADMIN					LORA_BW_7k8
#define	LORA_BW_MIN							LORA_BW_62k5
#define	LORA_BW_MAX							LORA_BW_500k
#define LORA_BW_DEFAULT                     LORA_BW_125k

#define	LORA_CR_4_5							1			// 4/5
#define	LORA_CR_4_6							2			// 4/6
#define	LORA_CR_4_7							3			// 4/7
#define	LORA_CR_4_8							4			// 4/8
#define	LORA_CR_MIN							LORA_CR_4_5
#define	LORA_CR_MAX							LORA_CR_4_8
#define LORA_CR_DEFAULT                     LORA_CR_4_5	// [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]

#define	LORA_GID_MIN						0
#define	LORA_GID_MAX						UINT16_MAX
#define LORA_GID_DEFAULT					0

#define	LORA_OWN_MIN						0
#define	LORA_OWN_MAX						UINT16_MAX
#define LORA_OWN_DEFAULT					1

#define	LORA_DST_MIN						0
#define	LORA_DST_MAX						UINT16_MAX
#define LORA_DST_DEFAULT					0

#define LORA_SEND_TIME_MAX_L				4000
#define LORA_SEND_TIME_MAX_H				400

#define RSSI_INT_TIME_VALUE                 200         // 200m sec

#define LORA_FRAME_SIGNATURE_P2P            0x4932

#define LORA_SYNCWORD						0x12		// LORA_MAC_PRIVATE_SYNCWORD

#define TX_TIMEOUT_VALUE                    5000        // msec
#define RX_TIMEOUT_VALUE                    10000       // msec
#define TX_PAUSE_DURATION_VALUE             50          // msec

#define LORA_CH_MIN                         24
#define LORA_CH_MAX                         61
#define LORA_CH_DEFAULT                     36

#define FREQ_UNIT_MODE						1			// 0:100kHz / 1:1kHz

#if FREQ_UNIT_MODE == 0
#define FREQ_DIV                            100000
#define FREQ_UNIT_STR						"(x100kHz)"
#else
#define FREQ_DIV                            1000
#define FREQ_UNIT_STR						"(kHz)"
#endif
#define HZ_TO_FREQ(hz)						(hz / FREQ_DIV)
#define FREQ_TO_HZ(frq)						(frq * FREQ_DIV)

#define	LORA_BORDER_FREQ					923500000ul
#define LORA_CH0_HZ							915800000ul
#define LORA_CH_STEP_HZ						200000ul

#define LORA_CH_TO_HZ(ch)					(LORA_CH0_HZ + (ch * LORA_CH_STEP_HZ))
#define LORA_HZ_DEFAULT						LORA_CH_TO_HZ( LORA_CH_DEFAULT )
#define	LORA_HZ_MIN							LORA_CH_TO_HZ( LORA_CH_MIN )
#define	LORA_HZ_MAX							LORA_CH_TO_HZ( LORA_CH_MAX )
#define LORA_FREQ_MIN                       HZ_TO_FREQ( LORA_HZ_MIN )
#define LORA_FREQ_MAX                       HZ_TO_FREQ( LORA_HZ_MAX )

#define LORA_PERIOD_DEFAULT					1000        // 1sec

#define LORA_RECV_CHECK                     0           // 0:Sig+Gid+Dst, 1:Sig+Gid, 2:Sig, 3:None, 4:None(hex)

#if __LORA_AES_ENABLE__
#define LORA_AES_DEFAULT					0
#endif

#if __LORA_FWD_ENABLE__
#define LORA_HOP_MIN					0
#define LORA_HOP_MAX					7
#define LORA_HOP_DEFAULT				LORA_HOP_MIN
#define LORA_FWDCONF_DEFAULT			-1
#endif

extern const char* const PvtUi_CrStr[];
extern const char* const PvtUi_BwStr[];
extern const char* const PvtUi_ModemStr[];

extern int16_t PvtUi_Sens_rssi_value;

/*!
 * \brief Function to UI Command Main
 */
void PvtUi_SettingsDefault( void );
void PvtUi_Stat( void );
UiRes_t PvtUi_Recv( DataFrame_t *frame, int32_t timeout, uint8_t code );
UiRes_t PvtUi_Send( DataFrame_t *frame, uint8_t len );
UiRes_t PvtUi_Comm( void );
UiRes_t PvtUi_Sens( int32_t sens_mode, int32_t times );
UiRes_t PtvUi_TxPreamble( int32_t ms );
UiRes_t PvtUi_Txcw( uint32_t mode );
UiRes_t PvtUi_Txcb( char *buff, uint8_t len );
UiRes_t PvtUi_Txif( DataFrame_t *frame, uint8_t len );
UiRes_t PvtUi_Cad( int32_t interval, int32_t timeout );
UiRes_t PvtUi_Default( char *str );
uint8_t PvtUi_ChBwToFreq( uint8_t modem, uint16_t ch, uint16_t bw );
void PvtUi_SetIsBreak( bool (*isBreak_func)(void) );
void PvtUi_RxContinue( void );
void PvtUi_PrintRxd( uint16_t adrs, uint8_t code );
void PvtUi_Process( void );
uint32_t PtvUi_onair_time( uint8_t *siz );
UiRes_t Ptv_SetPreambleLen( int32_t ms );

#endif
