/*!
 * @file    ui_common.c
 * @brief   Common function for user interface
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
#include <ctype.h>
#include "board-config.h"
#include "board-mcu.h"
#include "sys_gpio.h"
#include "sys_timer.h"
#include "basic.h"
#include "bios.h"
#include "hw_timer.h"

Settings_t	Settings;
Adjust_t	Adjust;

PassLevel_t basic_PassLevel;

#define CONTEXT_LED1		((void*)0x01)
#define CONTEXT_LED2		((void*)0x02)
#define CONTEXT_LED12		((void*)0x03)

#define SERIAL_NUMBER_WORDS	((uint32_t*)0x0080a00c)

static void Ui_ResPrint( const char *str );
static void Ui_Led_On( void *context, uint16_t timer );
static void Ui_Led_Off( void *context );

/*!
 * Timer to handle the state of LED1, LED2
 */
static TimerEvent_t Led1Timer;
static TimerEvent_t Led2Timer;

/*!
 * LED GPIO pins objects
 */
extern Gpio_t Led1;
extern Gpio_t Led2;

static Gpio_t *Ui_led1_obj;
static Gpio_t *Ui_led2_obj;

/*
 *
 */
void Ui_Settings_Init( bool force )
{
	bios_FlashInit();
	Ui_Settings_Load();
	if ( Settings.Initialized != SETTINGS_INITIALIZED ) force = true;
	if ( force )
	{
		initializeVariables( true );
		bios_FlashProgSave( NULL, PROGRAM_AREA_SIZE );
		memset( &Settings, 0, sizeof(Settings) );
		PvtUi_SettingsDefault();
#ifdef	__UI_LORA_WAN__
	}
	if ( Settings.Wan.WanInit != WAN_SETTINGS_INITIALIZED ) force = true;
	if ( force )
	{
		WanUi_Conf_Init( true );
		MAC_BACKUP_INITIALIZE();
		WanUi_SettingsDefault();
#endif
		Ui_Settings_Save();
	}
	basic_SetPassLevel( PassLevel_0 );
	HwTimer_SetXtal32k( &Settings.X32k_flag );
}

/*
 *
 */
uint8_t Ui_Settings_Save( void )
{
	return bios_FlashWrite( FLASH_SETTINGS_ADRS, (uint8_t *)&Settings, sizeof(Settings_t) );
}

/*
 *
 */
void Ui_SetSerialNumber( uint32_t sn )
{
	Adjust.Sn = sn;
}

/*
 *
 */
uint32_t Ui_GetSerialNumber( void )
{
	uint32_t sn = Adjust.Sn;
	if ( sn < LORA_SN_MIN || sn > LORA_SN_MAX ) sn = 0;
	return sn;
}

/*
 *
 */
 uint32_t Ui_GetDevid_Sn( void )
{
	return DEVID_SN( Ui_GetSerialNumber() );
}

/*
 *
 */
void Ui_Settings_Load( void )
{
	memcpy( (uint8_t *)&Settings, (uint8_t*)FLASH_SETTINGS_ADRS, sizeof(Settings_t) );
}

/*
 * DevID : 70-B3-D5-59-E0-1x-xx-xx
 */
void Ui_SetDevid_by_sn( void )
{
	uint32_t tmp = Ui_GetDevid_Sn();
	Adjust.Device_Id[0] = DEVID_OUI36_0;
	Adjust.Device_Id[1] = DEVID_OUI36_1;
	Adjust.Device_Id[2] = DEVID_OUI36_2;
	Adjust.Device_Id[3] = DEVID_OUI36_3;
	Adjust.Device_Id[4] = DEVID_OUI36_4 | (DEVID_PRODUCT >> 4);
	Adjust.Device_Id[5] = (DEVID_PRODUCT << 4) | (tmp >> 16);
	Adjust.Device_Id[6] = tmp >> 8;
	Adjust.Device_Id[7] = tmp;
}

/*
 *
 */
void Ui_Adjust_Init( void )
{
	memcpy( (uint8_t *)&Adjust, (uint8_t*)FLASH_ADJUST_ADRS, sizeof(Adjust_t) );
	if( Adjust.Initialized != ADJUST_INITIALIZED )
	{
		Adjust.Initialized = ADJUST_INITIALIZED;
		Adjust.Sn = LORA_SN_DEFAULT;
		Adjust.Drift = LORA_DRIFT_DEFAULT;
		Ui_Adjust_Save();
	}

	Ui_SetDevid_by_sn();
}

/*
 *
 */
uint8_t Ui_Adjust_Save( void )
{
	return bios_FlashWrite( FLASH_ADJUST_ADRS, (uint8_t *)&Adjust, sizeof(Adjust_t) );
}

/*
 *
 */
void Ui_Led_Init( void )
{
	if( IS_CTRL_LED_DISABLE )	return;
	GpioInit( &Led1, LED_1, PIN_OUTPUT, PIN_NO_PULL, LED_OFF );
	GpioInit( &Led2, LED_2, PIN_OUTPUT, PIN_NO_PULL, LED_OFF );
	TimerInit( &Led1Timer, Ui_Led_Off );	Led1Timer.Context = CONTEXT_LED1;
	TimerInit( &Led2Timer, Ui_Led_Off );	Led2Timer.Context = CONTEXT_LED2;
	Ui_led1_obj = &Led1;
	Ui_led2_obj = &Led2;
	if( IS_CTRL_LED_SWAP ){ Ui_led1_obj = &Led2; Ui_led2_obj = &Led1; }
}

/*
 *
 */
static void Ui_Led_On( void *context, uint16_t timer )
{
	if( IS_CTRL_LED_DISABLE )	return;
	if( (uint32_t)context & 0x1 )
	{
		GpioWrite( Ui_led1_obj, LED_ON );
		if( timer ){
			TimerSetValue( &Led1Timer, timer );
			TimerStart( &Led1Timer );
		}
	}
	if( (uint32_t)context & 0x2 )
	{
	    GpioWrite( Ui_led2_obj, LED_ON );
	    if( timer ){
			TimerSetValue( &Led2Timer, timer );
			TimerStart( &Led2Timer );
		}
	}
}


/*!
 * \brief Function executed on Led 1 Timeout event
 */
static void Ui_Led_Off( void *context )
{
	if( IS_CTRL_LED_DISABLE )	return;
	if( (uint32_t)context & 0x01 ){
		TimerStop( &Led1Timer );
		GpioWrite( Ui_led1_obj, LED_OFF );
	}
	if( (uint32_t)context & 0x2 ){
		TimerStop( &Led2Timer );
		GpioWrite( Ui_led2_obj, LED_OFF );
	}
}

/*
 *
 */
void Ui_Led_Tx_On( void )
{
	if (!IS_CTRL_LED_IS_BUSY) { Ui_Led_On( CONTEXT_LED1, 0 );}
}

/*
 *
 */
void Ui_Led_Tx_Off( void )
{
	if (!IS_CTRL_LED_IS_BUSY) { Ui_Led_Off( CONTEXT_LED1 );}
}

/*
 *
 */
void Ui_Led_Busy_On( void )
{
	if( IS_CTRL_LED_IS_BUSY ) { Ui_Led_On( CONTEXT_LED1, 0 );}
}

/*
 *
 */
void Ui_Led_Busy_Off( void )
{
	if( IS_CTRL_LED_IS_BUSY ){ Ui_Led_Off( CONTEXT_LED1 );}
}

/*
 *
 */
void Ui_Led_Rx_On( void )
{
	Ui_Led_On( CONTEXT_LED2, LED_TIMER_VALUE );
}

/*
 *
 */
void Ui_Led_Rx_Off( void )
{
	Ui_Led_Off( CONTEXT_LED2 );
}

/*
 *
 */
void Ui_Led_TRx_On( void )
{
	Ui_Led_On( CONTEXT_LED12, 0 );
}

/*
 *
 */
void Ui_Led_TRx_Off( void )
{
	Ui_Led_Off( CONTEXT_LED12 );
}

/*
 *
 */
static void Ui_PutXtal32k( void )
 {
 	bios_UartPuts( "Xtal32k=" );
	int32_t stat = bios_GetXtal32kStat();
	if( stat == 0 )
	{
		bios_UartPuts( "0[int" );
	}else
	{
		bios_UartPuts( "1[ext" );
		if( stat > 1 )	bios_UartPuts( "-output" );
		if( stat < 0 )	bios_UartPuts( "-fail" );
	}
	bios_UartPutLine( "]" );
 }

/*
 *
 */
void Ui_DispStat( void )
{
    bios_UartPrintfLine( "Sn=%lu"	, Ui_GetSerialNumber() );
    Ui_StatPrintHex    ( "DevId"	, Adjust.Device_Id , DEVICE_ID_LEN );
#ifdef __UI_LORA_WAN__
	bios_UartPuts( "Wan_mode=" );
	if( Ui_IsWanMode() )
	{
		bios_UartPutLine( "1[Wan]" );
		WanUi_Stat();
	}else
	{
		bios_UartPutLine( "0[P2P]" );
		PvtUi_Stat();
	}
#else
	PvtUi_Stat();
#endif
	bios_UartPrintfLine( "Ctrl=$%08lx"		, CTRL_BITS );
    bios_UartPrintfLine( "Echo=%u[%s]"		, Settings.Echo, Ui_OnOffStr(Settings.Echo) );
	Ui_PutXtal32k();
    bios_UartPrintfLine( "Auto=\x22%s\x22"	, Settings.AutoRun );
	if( basic_IsPassLevel_2() )
	{
		bios_UartPrintfLine( "Level=%d(dBm)"	, Pvt_GetCarrierSensLevel() );
		bios_UartPrintfLine( "Drift=%d(100Hz)"	, Adjust.Drift );
		bios_UartPrintfLine( "Debug=$%02x"		, DEBUG_BITS );
	}
}

/*
 *
 */
void Ui_StatPrintHex( const char *str, uint8_t *ptr, uint8_t len )
{
	bios_UartPuts( str );
	bios_UartPuts( "=$\x22" );
	Ui_Hexdec_Encode( bios_UartPutChar, ptr, len );
	bios_UartPutLine( "\x22" );
}

/*
 *
 */
static void Ui_ResPrint( const char *str )
{
    bios_UartPutChar( '*' );
	bios_UartPutLine( str );
}

static const char* const resp_str[] =
{
	"ok"                  ,	// 0 : UI_RES_OK				(OK)
	"incompatible_class"  ,	// 1 : UI_RES_INCOMPATIBLE_CLASS(�ΏۊOClass)
	"invalid_parameter"   ,	// 2 : UI_RES_INVALID			(�s���p�����[�^�[)
	"denied"              ,	// 3 : UI_RES_DENIED			(����)
	"no_free_ch"          ,	// 4 : UI_RES_NO_FREE_CH		(�L�����A�Z���X�G���[)
	"invalid_data_length" ,	// 5 : UI_RES_INVALID_LENGTH	(���M�T�C�Y�I�[�o�[)
	"not_joined"          ,	// 6 : UI_RES_NOT_JOINED		(Join �G���[)
	"dutycycle_restricted",	// 7 : UI_RES_DC_RESTRICTED		(Dutycycle ������)
	"timeout"             ,	// 8 : UI_RES_TIMEOUT			(�^�C���A�E�g)
	"crc_error"           ,	// 9 : UI_RES_CRC_ERROR			(CRC �G���[)
	NULL                  ,	// 10 : UI_RES_RX				(�f�[�^�[��M (Dummy))
	"busy"                ,	// 11 : UI_RES_BUSY				(������)
	"error"               , // 12 : UI_RES_ERROR			(���̑��G���[)
	NULL                  , // 13 : UI_RES_STR				(������o��)
	NULL                  , // 14 : UI_RES_NOTHING			(�\�����Ȃ�)
};

/*
 *
 */
UiRes_t Ui_PutRespStr( UiRes_t res )
{
	if( res == UI_RES_STR )
	{
		Ui_ResPrint( (const char*)bios_UartTxBuff );
		return res;
	}
	if( res > UI_RES_NOTHING ) res = UI_RES_ERROR;
	if( resp_str[res] != NULL )	Ui_ResPrint( resp_str[res] );
	return res;
}

/*
 * LP + Timer
 */
void Ui_LowpowerTimerProcess( void )
{
	bios_Lowpower();
	bios_TimerProcess();
}

/*
 * LP + Break + Timer + WDT
 */
uint8_t Ui_IsBreak_LP( void )
{
	bios_Lowpower();
	return basic_checkBreak();
}

/*
 * LoRa + Break + Timer + WDT
 */
uint8_t Ui_IsBreak_BK( void )
{
//	bios_Lowpower();
	basic_LoRa_Process();
	basic_Wan_Process();
	return basic_checkBreak();
}

/*
 *
 */
PassLevel_t basic_SetPassLevel( int32_t val )
{
	switch ( val )
	{
	case PASSWORD_NUMBER_PROTECT: Settings.Protect = 1; break;
	case PASSWORD_NUMBER_LV1	: basic_PassLevel = PassLevel_1;	break;
	case PASSWORD_NUMBER_LV2	: basic_PassLevel = PassLevel_2;	break;
	default : basic_PassLevel = PassLevel_0; break;
	}
	if ( Settings.Protect ) basic_PassLevel = PassLevel_Protect;
	return basic_PassLevel;
}

/*
 *
 */
errCode_t basic_CheckPassLevel( PassLevel_t level )
{
	if( basic_PassLevel < level )	errorCode = ERROR_PERMISSION;
	return errorCode;
}

