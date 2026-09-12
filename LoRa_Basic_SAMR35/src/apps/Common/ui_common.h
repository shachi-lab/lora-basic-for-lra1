/*!
 * @file    ui_common.h
 * @brief   HeaderFile of Common function for user interface
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
#ifndef __UI_COMMON_H__
#define __UI_COMMON_H__

#define _BUTTON_					0

#define LED_TIMER_VALUE             15          // msec

#define Ui_OnOffStr( flag )			( flag == 0 ? "Off" : "On" )

typedef enum
{
    UI_RES_OK				= 0	,   // OK
	UI_RES_INCOMPATIBLE_CLASS	,	// �ΏۊOClass
    UI_RES_INVALID				,   // �s���p�����[�^�[
    UI_RES_DENIED				,   // ����
    UI_RES_NO_FREE_CH			,   // �L�����A�Z���X�G���[
    UI_RES_INVALID_LENGTH		,	// ���M�T�C�Y�I�[�o�[
    UI_RES_NOT_JOINED			,   // Join �G���[
    UI_RES_DC_RESTRICTED		,   // Dutycycle ������
	UI_RES_TIMEOUT				,	// �^�C���A�E�g
	UI_RES_CRC_ERROR			,	// CRC �G���[
    UI_RES_RX					,   // �f�[�^�[��M
	UI_RES_BUSY					,	// ������
    UI_RES_ERROR				,   // ���̑��G���[
    UI_RES_STR					,   // ������o��
    UI_RES_NOTHING				,	// �\�����Ȃ�
}UiRes_t;

typedef enum
{
	LORA_MODE_P2P = 0,
	LORA_MODE_WAN = 1,
}Lora_mode_t;

#define SETTINGS_INITIALIZED		0x123f
#define AUTORUN_LEN					64
#define AESKEY_LEN					16		// 128bits

#define DEFAULT_BAUDRATE            115200
#define DEFAULT_ECHO                1
#define DEFAULT_BREAK_CHAR			0x03

#define X32K_INT					0
#define X32K_EXT					1
#define DEFAULT_X32K				X32K_INT

typedef struct
{
	uint16_t	Initialized;
	uint8_t		Lora_Mode;
	uint8_t		Modem;
	uint8_t		Reserved_1;	// Size (delete)
	int8_t      Power;
	uint8_t     Sf;
	uint8_t     Bw;
	uint8_t     Cr;
	uint8_t     Ch;
	uint16_t    Gid;
	uint16_t    Own;
	uint16_t    Dst;
	uint32_t    Freq_Hz;
	uint32_t    Period;
	uint32_t    Ctrl_bits;
	uint32_t    Baudrate;
	uint16_t	ProtId;
	uint8_t		SyncWord;
	uint8_t		BreakChar;
	struct {
	uint8_t		Echo	  : 1;
	uint8_t		AesEnable : 1;
	uint8_t		BrkDisable: 1;
	uint8_t		Protect   : 1;
	};
	uint8_t		X32k_flag;
	uint16_t	Debug_bits;
	struct {
	uint16_t	WanInit;
	uint8_t		Reserved_2;	// -> WanConf.Class
	uint8_t		TxPwr;
	uint8_t     Datarate;
	uint8_t		FPort;
	uint8_t		Cnf;
	uint8_t     Adr;
	uint8_t     Trial;
	uint8_t		Reserved_3[1];
	uint16_t	ChMask;
	uint8_t		Reserved_4[4];
	} Wan;
	uint8_t		Reserved_5[4];
	int32_t		PassKey;
	uint32_t	FwdTxConf;
	uint8_t		FwdHop;
	uint8_t		Reserved_6[1];
	char        AutoRun[AUTORUN_LEN];
	uint8_t		AesKey[AESKEY_LEN];
} Settings_t;

extern	Settings_t	Settings;
#define FLASH_SETTINGS	((Settings_t *)FLASH_SETTINGS_ADRS)

#define LORA_CTRL_RSSI_DISABLE			0x00000001
#define LORA_CTRL_ADRS_DISABLE			0x00000002
#define LORA_CTRL_AT_DISABLE			0x00000004
#define LORA_CTRL_CRCERR_ENABLE			0x00000008
#define LORA_CTRL_TITLE_DISABLE			0x00000010
#define LORA_CTRL_LED_DISABLE			0x00000020
#define LORA_CTRL_SLEEP_INT32K			0x00000040
#define LORA_CTRL_RESET_ON_ERROR		0x00000080
#define LORA_CTRL_TX_ONLY				0x00000100
#define LORA_CTRL_WAKEUP_INV			0x00000200
#define LORA_CTRL_ACTIVE_INV			0x00000400
#define LORA_CTRL_UART_PULLDOWN			0x00000800
#define LORA_CTRL_ACTIVE_DISABLE		0x00001000
#define LORA_CTRL_LED_SWAP				0x00002000
#define LORA_CTRL_LED_IS_BUSY			0x00004000
#define LORA_CTRL_FORCE_INT_TIMER		0x00008000
#define LORA_CTRL_CLKOUT_RF1M			0x00010000
#define LORA_CTRL_CLKOUT_TC1K			0x00020000
#define LORA_CTRL_FORWARD_1				0x00040000
#define LORA_CTRL_FORWARD_2				0x00080000
#define LORA_CTRL_WAN_BACKUP_0			0x00100000
#define LORA_CTRL_WAN_BACKUP_1			0x00200000
#define LORA_CTRL_WAN_BACKUP_2			0x00400000
#define LORA_CTRL_WAN_UPCOUNT_FIX		0x00800000
#define	LORA_CTRL_WAN_DWELL				0x01000000
#define	LORA_CTRL_WAN_PRIVATE_NETWORK	0x02000000
#define LORA_CTRL_FAST_START_ON_DEEP	0x04000000
#define LORA_CTRL_0x08000000_			0x08000000
#define LORA_CTRL_0x10000000_			0x10000000
#define LORA_CTRL_0x20000000_			0x20000000
#define LORA_CTRL_0x40000000_			0x40000000
#define LORA_CTRL_0x80000000_			0x80000000

#define LORA_DEBUG_WDT_DISABLE			0x0001
#define LORA_DEBUG_RECV_ALL				0x0002
#define LORA_DEBUG_IQ_INVERT			0x0004
#define LORA_DEBUG_NO_CARRIER_SENS		0x0008
#define LORA_DEBUG_0x0010_				0x0010
#define LORA_DEBUG_SPECIAL_SYNCWORD		0x0020
#define LORA_DEBUG_SAVE_LINE_NUMBER		0x0040
#define LORA_DEBUG_ALWAYS_RF_STANDBY	0x0080

#define CTRL_BITS					Settings.Ctrl_bits
#define DEBUG_BITS					Settings.Debug_bits

#define IS_CTRL_RSSI_DISABLE		((CTRL_BITS & LORA_CTRL_RSSI_DISABLE       )!=0)
#define IS_CTRL_ADRS_DISABLE		((CTRL_BITS & LORA_CTRL_ADRS_DISABLE       )!=0)
#define IS_CTRL_AT_DISABLE			((CTRL_BITS & LORA_CTRL_AT_DISABLE         )!=0)
#define IS_CTRL_CRCERR_ENABLE		((CTRL_BITS & LORA_CTRL_CRCERR_ENABLE      )!=0)
#define IS_CTRL_TITLE_DISABLE		((CTRL_BITS & LORA_CTRL_TITLE_DISABLE      )!=0)
#define IS_CTRL_LED_DISABLE			((CTRL_BITS & LORA_CTRL_LED_DISABLE        )!=0)
#define	IS_CTRL_SLEEP_INT32K		((CTRL_BITS & LORA_CTRL_SLEEP_INT32K       )!=0)
#define IS_CTRL_RESET_ON_ERROR		((CTRL_BITS & LORA_CTRL_RESET_ON_ERROR     )!=0)
#define IS_CTRL_TX_ONLY				((CTRL_BITS & LORA_CTRL_TX_ONLY            )!=0)
#define IS_CTRL_WAKEUP_INV			((CTRL_BITS & LORA_CTRL_WAKEUP_INV         )!=0)
#define IS_CTRL_ACTIVE_INV			((CTRL_BITS & LORA_CTRL_ACTIVE_INV         )!=0)
#define IS_CTRL_UART_PULLDOWN		((CTRL_BITS & LORA_CTRL_UART_PULLDOWN      )!=0)
#define IS_CTRL_ACTIVE_DISABLE		((CTRL_BITS & LORA_CTRL_ACTIVE_DISABLE     )!=0)
#define IS_CTRL_LED_SWAP			((CTRL_BITS & LORA_CTRL_LED_SWAP           )!=0)
#define IS_CTRL_LED_IS_BUSY			((CTRL_BITS & LORA_CTRL_LED_IS_BUSY        )!=0)
#define IS_CTRL_FORCE_INT_TIMER		((CTRL_BITS & LORA_CTRL_FORCE_INT_TIMER    )!=0)
#define IS_CTRL_CLKOUT_RF1M			((CTRL_BITS & LORA_CTRL_CLKOUT_RF1M        )!=0)
#define IS_CTRL_CLKOUT_TC1K			((CTRL_BITS & LORA_CTRL_CLKOUT_TC1K        )!=0)
#define IS_CTRL_FORWARD_1			((CTRL_BITS & LORA_CTRL_FORWARD_1          )!=0)
#define IS_CTRL_FORWARD_2			((CTRL_BITS & LORA_CTRL_FORWARD_2          )!=0)
#define IS_CTRL_WAN_BACKUP_0		((CTRL_BITS & LORA_CTRL_WAN_BACKUP_0       )!=0)
#define IS_CTRL_WAN_BACKUP_1		((CTRL_BITS & LORA_CTRL_WAN_BACKUP_1       )!=0)
#define IS_CTRL_WAN_BACKUP_2		((CTRL_BITS & LORA_CTRL_WAN_BACKUP_2       )!=0)
#define IS_CTRL_WAN_UPCOUNT_FIX		((CTRL_BITS & LORA_CTRL_WAN_UPCOUNT_FIX    )!=0)
#define IS_CTRL_WAN_DWELL			((CTRL_BITS & LORA_CTRL_WAN_DWELL          )!=0)
#define IS_CTRL_WAN_PRIVATE_NETWORK	((CTRL_BITS & LORA_CTRL_WAN_PRIVATE_NETWORK)!=0)
#define	GET_CTRL_FORWARD()			((CTRL_BITS >> 18) & 3)
#define	GET_CTRL_WAN_BACKUP()		((CTRL_BITS >> 21) & 3)
#define IS_CTRL_FAST_START_ON_DEEP	((CTRL_BITS & LORA_CTRL_FAST_START_ON_DEEP )!=0)

#define IS_DEBUG_WDT_DISABLE		((DEBUG_BITS & LORA_DEBUG_WDT_DISABLE      )!=0)
#define IS_DEBUG_RECV_ALL			((DEBUG_BITS & LORA_DEBUG_RECV_ALL         )!=0)
#define IS_DEBUG_IQ_INVERT			((DEBUG_BITS & LORA_DEBUG_IQ_INVERT        )!=0)
#define IS_DEBUG_NO_CARRIER_SENS	((DEBUG_BITS & LORA_DEBUG_NO_CARRIER_SENS  )!=0)
#define IS_DEBUG_SPECIAL_SYNCWORD	((DEBUG_BITS & LORA_DEBUG_SPECIAL_SYNCWORD )!=0)
#define IS_DEBUG_SAVE_LINE_NUMBER	((DEBUG_BITS & LORA_DEBUG_SAVE_LINE_NUMBER )!=0)
#define	IS_DEBUG_ALWAYS_RF_STANDBY	((DEBUG_BITS & LORA_DEBUG_ALWAYS_RF_STANDBY)!=0)

#ifndef UINT24_MAX
#define UINT24_MAX					0x00ffffff
#endif

#define ADJUST_INITIALIZED			0x1234

#define LORA_SN_DEFAULT				0
#define LORA_SN_MIN					100000
#define	LORA_SN_MAX					999999

#define LORA_DRIFT_MIN				-250		// -25KHz
#define LORA_DRIFT_MAX				250			// 25kHz
#define LORA_DRIFT_UNIT				100			// 100Hz
#define LORA_DRIFT_DEFAULT			9999

#define DEVICE_ID_LEN				8

typedef struct
{
	uint16_t	Initialized;				//
	uint32_t	Sn;							// Serial number
	int16_t		Drift;						// Drift
	uint8_t		Device_Id[DEVICE_ID_LEN];	// Device-ID
	uint8_t		Reserved[8];
} Adjust_t;

extern	Adjust_t	Adjust;

void Ui_Settings_Init( bool force );
uint8_t Ui_Settings_Save( void );
void Ui_Settings_Load( void );

void Ui_Adjust_Init( void );
uint8_t Ui_Adjust_Save( void );
void Ui_DispStat( void );
void Ui_SetDevid_by_sn( void );
void Ui_SetSerialNumber( uint32_t sn );
uint32_t Ui_GetSerialNumber( void );
uint32_t Ui_GetDevid_Sn( void );
 
void Ui_Led_Init( void );
void Ui_Led_Tx_On( void );
void Ui_Led_Tx_Off( void );
void Ui_Led_Busy_On( void );
void Ui_Led_Busy_Off( void );
void Ui_Led_Rx_On( void );
void Ui_Led_Rx_Off( void );
void Ui_Led_TRx_On( void );
void Ui_Led_TRx_Off( void );

void Ui_StatPrintHex( const char *str, uint8_t *ptr, uint8_t len );
UiRes_t Ui_PutRespStr( UiRes_t res );
void Ui_LowpowerTimerProcess( void );
uint8_t Ui_IsBreak_LP( void );
uint8_t Ui_IsBreak_BK( void );

#define	LED_TX_ON()		Ui_Led_Tx_On()
#define	LED_TX_OFF()	Ui_Led_Tx_Off()
#define	LED_BUSY_ON()	Ui_Led_Busy_On()
#define	LED_BUSY_OFF()	Ui_Led_Busy_Off()
#define	LED_RX_ON()		Ui_Led_Rx_On()
#define	LED_RX_OFF()	Ui_Led_Rx_Off()
#define LED_TRX_ON()	Ui_Led_TRx_On()
#define LED_TRX_OFF()	Ui_Led_TRx_Off()

#define PASSWORD_NUMBER_PROTECT	-1
#define PASSWORD_NUMBER_LV1		999999
#define PASSWORD_NUMBER_LV2		123456

#ifdef __BASIC_CONF_H__

typedef enum
{
	PassLevel_0 = 0,
	PassLevel_1 = 1,
	PassLevel_2 = 2,
	PassLevel_Protect = -1,
} PassLevel_t;

extern PassLevel_t basic_PassLevel;

PassLevel_t basic_SetPassLevel( int32_t val );
errCode_t basic_CheckPassLevel( PassLevel_t level );
inline errCode_t basic_CheckPassLevel_1( void ){ return basic_CheckPassLevel( PassLevel_1 ); }
inline errCode_t basic_CheckPassLevel_2( void ){ return basic_CheckPassLevel( PassLevel_2 ); }

inline PassLevel_t basic_GetPassLevel( void ){ return basic_PassLevel; }
inline bool basic_IsPassLevel_1( void ){ return basic_PassLevel >= PassLevel_1; }
inline bool basic_IsPassLevel_2( void ){ return basic_PassLevel >= PassLevel_2; }

#endif

#endif
