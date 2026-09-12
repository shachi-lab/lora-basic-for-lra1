/*!
 * @file    bios.c
 * @brief   BIOS for Basic Interpreter
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
#include <stdarg.h>
//#include "version.h"
#include "board-config.h"
#include "board-mcu.h"
#include "uart-board.h"
#include "rtc-board.h"
#include "utilities.h"
#include "hw_timer.h"
#include "sys_i2c.h"
#include "sys_delay.h"
#include "basic.h"
#include "bios.h"
#include "hw_timer.h"

char __attribute__ ((aligned(4))) bios_UartRxBuff[BIOS_UART_RX_BUFF_SIZE];
char __attribute__ ((aligned(4))) bios_UartTxBuff[BIOS_UART_TX_BUFF_SIZE];

//*************************************************
//    UART
//*************************************************
//uint32_t uart2_baudrate = BIOS_UART2_BAUDRATE_DAFULT;

//*************************************************
void bios_UartInit( void )
{
	bios_UartSetBreakCh( bios_GetBreakCh() );
	while ( bios_UartSetBaudrate( Settings.Baudrate ) )
	{
		Settings.Baudrate = 0;
	}
    UartInit( &Uart1, UART_1 );
}

//*************************************************
void bios_UartDeinit( void )
{
	UartMcuDeInit( &Uart1 );
}

//*************************************************
static uint8_t bios_UartCheckBaudrate( uint32_t baudrate )
{
	uint32_t val_x2 = UART_BAUDRATE_MIN;
	uint32_t val_x3 = val_x2;
	uint32_t opt = baudrate % 100;
	if ( opt > UART_BAUD_OPT_BITS ) return 1;
	baudrate -= opt;
	while ( 1 )
	{
		if ( val_x2 == baudrate )	return 0;
		if ( val_x3 == baudrate )	return 0;
		if ( val_x3 >= UART_BAUDRATE_MAX )	return 1;
		val_x3 = val_x2 * 3;
		val_x2 = val_x2 * 2;
	}
}

//*************************************************
uint8_t bios_UartSetBaudrate( uint32_t baudrate )
{
	if ( (int32_t)baudrate < 0 )
	{
		bios_UartDeinit();
		return 0;
	}
	if ( baudrate == 0 )	baudrate = DEFAULT_BAUDRATE;
	if ( bios_UartCheckBaudrate( baudrate ) )	return 1;
	Settings.Baudrate = baudrate;

	if ( IS_CTRL_UART_PULLDOWN )
	{
		uint32_t opt = baudrate % 100;
		if ( !(opt & UART_BAUD_OPT_RX_PULL_DOWN) ) baudrate += UART_BAUD_OPT_RX_PULL_DOWN;
	}
	UartSetBaudrate( &Uart1, baudrate );
	return 0;
}

#if BIOS_WRIPPER_IS_FUNCTION
//*************************************************
uint32_t bios_UartGetBaudrate( void )
{
	return Settings.Baudrate;
}
#endif

//*************************************************
void bios_UartPutChar( char ch )
{
	while ( UartMcuPutChar( &Uart1, (uint8_t)ch ) );
}

//*************************************************
int bios_UartGetChar( void )
{
	uint8_t ch;

	if ( UartMcuGetChar( &Uart1, &ch ) )
	{
		return -1;
	}
	return ch;
}

//*************************************************
void bios_UartWrite( uint8_t *str, uint8_t len )
{
	UartMcuPutBuffer( &Uart1, str, len );
}

//*************************************************
void bios_UartPuts( const char *str )
{
	if ( str == NULL )	return;
	while ( *str )
	{
		bios_UartPutChar( *str++ );
	}
}

//*************************************************
void bios_UartPutCrLf( void )
{
	bios_UartPuts( "\r\n" );
}

//*************************************************
void bios_UartPutLine( const char *str )
{
    bios_UartPuts( str );
	bios_UartPutCrLf();
}

//*************************************************
void bios_UartPrintf( const char *format, ... )
{
	va_list arg_ptr;

	va_start( arg_ptr, format );
	vsprintf( bios_UartTxBuff, format, arg_ptr );
	bios_UartPuts( bios_UartTxBuff );
	va_end(arg_ptr);
}

//*************************************************
void bios_UartPrintfLine( const char *format, ... )
{
	va_list arg_ptr;

	va_start( arg_ptr, format );
	vsprintf( bios_UartTxBuff, format, arg_ptr );
	bios_UartPutLine( bios_UartTxBuff );
	va_end(arg_ptr);
}

//*************************************************
uint8_t bios_UartGetBreak( void )
{
	return UartMcuIsBreak( &Uart1 );
}

//*************************************************
void bios_SetBreakCh( int32_t ch )
{
	Settings.BrkDisable = (ch < 0);
	if ( Settings.BrkDisable ) ch = -ch;
	Settings.BreakChar = (uint8_t)ch;
}

//*************************************************
int32_t bios_GetBreakCh( void )
{
	int32_t ch = Settings.BreakChar;
	if ( Settings.BrkDisable ) ch = -ch;
	return ch;
}

//*************************************************
void bios_UartSetBreakCh( int32_t ch )
{
	Uart1.BrkDisable = (ch < 0);
	if ( Uart1.BrkDisable ) ch = -ch;
	Uart1.BreakChar = (uint8_t)ch;
}

//*************************************************
void bios_UartWaitTxEmpty( void )
{
	UartMcuWaitTxEmpty( &Uart1 );
}

//*************************************************
void bios_UartFlush( void )
{
	UartMcuFlush( &Uart1 );
}

//*************************************************
uint16_t bios_UartTxBuffLen( void )
{
	return UartMcuTxBuffLen( &Uart1 );
}

//*************************************************
uint16_t bios_UartRxBuffLen( void )
{
	return UartMcuRxBuffLen( &Uart1 );
}

#if BIOS_WRIPPER_IS_FUNCTION
//*************************************************
void bios_UartSetEcho( uint8_t val )
{
	Settings.Echo = val ? 1 : 0;
}

//*************************************************
uint8_t bios_UartGetEcho( void )
{
	return Settings.Echo;
}
#endif

//*************************************************
void bios_UartPutCharEcho( char ch )
{
	if ( Settings.Echo )	bios_UartPutChar( ch );
}

//*************************************************
void bios_UartPutStrEcho( const char *str )
{
	if ( Settings.Echo )	bios_UartPuts( str );
}

//*************************************************
void bios_Uart2Init( void )
{
	bios_Uart2GetBaudrate();
	UartInit( &Uart2, UART_2 );
}

//*************************************************
uint8_t bios_Uart2SetBaudrate( uint32_t baudrate )
{
	if ( (int32_t)baudrate < 0 )
	{
		bios_Uart2Deinit();
		return 0;
	}
	if ( baudrate == 0 )	baudrate = BIOS_UART2_BAUDRATE_DAFULT;
	if ( bios_UartCheckBaudrate( baudrate ) )	return 1;
	UartSetBaudrate( &Uart2, baudrate );
	return 0;
}

//*************************************************
uint32_t bios_Uart2GetBaudrate( void )
{
	bios_Uart2SetBaudrate( Uart2.Baudrate );
	return Uart2.Baudrate;
}

//*************************************************
void bios_Uart2Deinit( void )
{
    UartDeInit( &Uart2 );
}

//*************************************************
void bios_Uart2PutChar( char ch )
{
	while ( UartMcuPutChar( &Uart2, (uint8_t)ch ) );
}

//*************************************************
int bios_Uart2GetChar( void )
{
	uint8_t ch;

	if ( UartMcuGetChar( &Uart2, &ch ) )
	{
		return -1;
	}
	return ch;
}

//*************************************************
void bios_Uart2Write( uint8_t *str, uint8_t len )
{
	UartMcuPutBuffer( &Uart2, str, len );
}

//*************************************************
uint16_t bios_Uart2TxBuffLen( void )
{
	return UartMcuTxBuffLen( &Uart2 );
}

//*************************************************
uint16_t bios_Uart2RxBuffLen( void )
{
	return UartMcuRxBuffLen( &Uart2 );
}

//*************************************************
bool bios_IsUartEnable( void )
{
	return	Uart1.IsInitialized;
}

//*************************************************
bool bios_IsUart2Enable( void )
{
	return	Uart2.IsInitialized;
}

//*************************************************
static void bios_Uart2GpioCheck( int32_t pin )
{
	if ( pin == UART2_TX ) Uart2.IsGpioTxInit = false;
	if ( pin == UART2_RX ) Uart2.IsGpioRxInit = false;		
}
	
//*************************************************
void bios_Uart2Begin( int32_t pin )
{
	if ( Uart2.IsInitialized ) {
		if ( pin == UART2_TX ) {
			if( Uart2.IsGpioTxInit ) return; 
		} else
		if ( pin == UART2_RX ) {
			if( Uart2.IsGpioRxInit ) return;
		} else {
			return;
		}
		Uart2.IsInitialized = false;
	}
	bios_Uart2Init();
}

//*************************************************
void bios_PutVersion( void )
{
	bios_UartPutCrLf();
	bios_UartPutLine( _Product_.Name );
	bios_UartPuts( _Product_.Version );
	if ( Ui_GetSerialNumber() == 0 ) bios_UartPutChar( '*' );
	bios_UartPutCrLf();
}

//*************************************************
uint8_t bios_SetAutorun( char *str )
{
	if ( strlen( str ) >= AUTORUN_LEN-1 )	return 1;
	memcpy( Settings.AutoRun, str, AUTORUN_LEN );
	return 0;
}

#if BIOS_WRIPPER_IS_FUNCTION
//*************************************************
char *bios_GetAutorun( void )
{
	return Settings.AutoRun;
}
#endif

//*************************************************
void bios_ClearWDT( void )
{
	BoardClearWDT();
}

//*************************************************
//	Timer
//*************************************************
static uint32_t bios_systemTick = 0;
static uint32_t bios_waitTick = 0;
static uint16_t bios_miliseconds = 0;

//*************************************************
void bios_TimerInit( void )
{
	bios_systemTick = RtcGetTimerValue();
}

//*************************************************
void bios_setWaitTick( int32_t tick )
{
	bios_waitTick = RtcGetTimerValue();
	if ( tick > 0 ) bios_waitTick += (uint32_t)tick;
}

//*************************************************
int32_t bios_getWaitTick( void )
{
	int32_t wait = (int32_t)bios_waitTick;
	int32_t tick = (int32_t)RtcGetTimerValue();
	wait -= tick;
	if ( wait > 0 )	return wait;
	return 0;
}

//*************************************************
uint32_t bios_getSystemTick( void )
{
	return RtcGetTimerValue() - bios_systemTick;
}

//*************************************************
void bios_setSystemTick( uint32_t tick )
{
	bios_systemTick = RtcGetTimerValue() - tick;
}

//*************************************************
uint32_t bios_getSystemClock( void )
{
	return RtcGetCalendarTime( &bios_miliseconds );
}

//*************************************************
uint32_t bios_getSystemMilisec( void )
{
	return (uint32_t)bios_miliseconds;
}

//*************************************************
void bios_setSystemClock( uint32_t seconds )
{
	RtcSetCalendarTime( seconds, 0 );
}

//*************************************************
int32_t *bios_getBackupRegPtr( uint32_t n )
{
	return (int32_t*)&RTC_BACKUP_REG( n );
}

//*************************************************
//
//*************************************************
static uint32_t bios_random_seed = 0;
//*************************************************
void bios_Randomize( uint32_t val )
{
	if ( !val )
	{
		uint16_t miliseconds;
		HwTimer_RTC_GetSec( &miliseconds );
		val = miliseconds;
	}
	bios_random_seed = val;
	bios_random_seed ^= BoardGetRandomSeed();
	val = (bios_random_seed & 0x0f) + 1;
	while ( val-- )
	{
		bios_GetRandom();
	}
}

//*************************************************
static void bios_DeinitTRNG( void )
{
#if RANDOM_BY_TRNG
	TRNG->CTRLA.reg = 0;		// SAM L21 errata 1.15.1
	TRNG->CTRLA.reg = 0;
	MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_TRNG;
#endif
}

//*************************************************
uint32_t bios_GetTRNG( void )
{
#if RANDOM_BY_TRNG
	if ( !(TRNG->CTRLA.reg & TRNG_CTRLA_ENABLE) )
	{
		MCLK->APBCMASK.reg |= MCLK_APBCMASK_TRNG;
		TRNG->CTRLA.reg = TRNG_CTRLA_ENABLE;
	}
	TRNG->INTFLAG.reg = TRNG_INTFLAG_DATARDY;
	while ( !(TRNG->INTFLAG.reg & TRNG_INTFLAG_DATARDY) );
	bios_random_seed = TRNG->DATA.reg;
	bios_random_seed &= 0x7ffffff;
	bios_DeinitTRNG();
	return bios_random_seed;
#else
	return bios_GetRandom();
#endif
}

//*************************************************
uint32_t bios_GetRandom( void )
{
#if RANDOM_BY_TRNG
	return bios_GetTRNG();
#else
	do {
		bios_random_seed *= 1103515245UL;
		bios_random_seed += 12345UL;
	} while ( !bios_random_seed );
	bios_random_seed &= 0x7ffffff;
	return bios_random_seed;
#endif
}

#if 0		// inline
//*************************************************
uint32_t bios_GetMpuId( void )
{
	return DSU->DID.reg;
}
#endif

//--------------------------------
// FLASH������
//--------------------------------
void bios_FlashInit( void )
{
	struct nvm_fusebits fusebits;

	nvm_get_fuses( &fusebits );
	if ( fusebits.eeprom_size != NVM_EEPROM_EMULATOR_SIZE_4096 )
	{
		fusebits.eeprom_size = NVM_EEPROM_EMULATOR_SIZE_4096;
		nvm_set_fuses( &fusebits );
	}

	struct nvm_config config;
	nvm_get_config_defaults(&config);
	config.wait_states = 2;
	config.manual_page_write = false;
	nvm_set_config(&config);
}

#define FLASH_RETRY_MAX	1000
//--------------------------------
// FLASH��������
//--------------------------------
uint8_t bios_FlashWrite( uint32_t adrs, uint8_t *buff, uint16_t len )
{
	enum status_code status = STATUS_OK;
	uint32_t page_address = adrs;

	if ( ((adrs < FLASH_BEGIN_ADDRESS)||(adrs > FLASH_END_ADDRESS))&&
		((adrs < FLASH_PROGRAM_ADRS_1)||(adrs >(FLASH_PROGRAM_ADRS_1+FLASH_PROGRAM_SIZE_1-1))) ) return 1;

	if ( len & 1 ) len++;
	while ( len )
	{
		uint16_t size = (len > NVMCTRL_PAGE_SIZE) ? NVMCTRL_PAGE_SIZE : len;
		CRITICAL_SECTION_BEGIN();
		if ( (page_address & ((NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES)-1)) == 0 )
		{
			for ( uint32_t i = 0; i < FLASH_RETRY_MAX; i++ )
			{
				if ( (status = nvm_erase_row(page_address)) != STATUS_BUSY ) break;
			}
		}
		if ( status == STATUS_OK && buff )
		{
			for ( uint32_t i = 0; i < FLASH_RETRY_MAX; i++ )
			{
				if ( (status = nvm_write_buffer(page_address, buff, size)) != STATUS_BUSY ) break;
			}
			buff += size;
		}
		CRITICAL_SECTION_END();
		if ( status != STATUS_OK ) return status;
		len -= size;
		page_address += size;

	}
	return STATUS_OK;
}

//*************************************************
uint8_t bios_FlashProgSave( uint8_t *buff, uint16_t len )
{
	int size = (len > FLASH_PROGRAM_SIZE_1) ? FLASH_PROGRAM_SIZE_1 : len;
	uint8_t res = bios_FlashWrite( FLASH_PROGRAM_ADRS_1, buff, size );
	size = len - size;
	if ( !res && size ) {
		if ( size > FLASH_PROGRAM_SIZE_2 ) size = FLASH_PROGRAM_SIZE_2;
		buff += FLASH_PROGRAM_SIZE_1;
		res = bios_FlashWrite( FLASH_PROGRAM_ADRS_2, buff, size );
	}
	return res;
}

//*************************************************
uint8_t bios_FlashProgLoad( uint8_t *buff, uint16_t len )
{
	if ( *((uint32_t*)FLASH_PROGRAM_ADRS_1) == UINT32_MAX ) return 1;

	int size = (len > FLASH_PROGRAM_SIZE_1) ? FLASH_PROGRAM_SIZE_1 : len;
	memcpy( buff, (uint8_t*)FLASH_PROGRAM_ADRS_1, size );	
	size = len - size;
	if ( size ) {
		buff += FLASH_PROGRAM_SIZE_1;
		memcpy( buff, (uint8_t*)FLASH_PROGRAM_ADRS_2, size );
	}
	return 0;
}

//*************************************************
static bool bios_IsGpioValid( uint8_t pin )
{
	for( uint8_t const *p = GpioMcu_PinList; *p <= PIN_MAX; p++ )
	{
		if( *p == pin ) return true;
	}
	return false;
}

//**************************************************
static uint8_t bios_GetGpio( int32_t pin )
{
	return port_pin_get_input_level( pin );
}
	
//*************************************************
uint8_t bios_GpioInput( int32_t pin, uint8_t *stat )
{
	int32_t type = BIOS_GPIO_OUT_STAT_IN_HIZ;
	if ( pin & BIOS_GPIO_IN_PULL_DOWN )	type = BIOS_GPIO_OUT_STAT_IN_DOWN;
	if ( pin & BIOS_GPIO_IN_PULL_UP   )	type = BIOS_GPIO_OUT_STAT_IN_UP;
	pin &= 0x3f;
	if ( bios_GpioOutput( pin, type ) ) return 1;
	*stat = bios_GetGpio( pin );
	return 0;
}

//*************************************************
uint8_t bios_GpioOutput( int32_t pin, int32_t stat )
{
	if ( pin & BIOS_GPIO_OUT_PIN_PACK )
	{
		for( uint8_t const *p = GpioMcu_PinList; *p <= PIN_MAX; p++ ) {
			if( pin & 1 ) {
				bios_GpioOutput( *p, stat );			
			}
			pin >>= 1;
		}
		return 0;		
	}

	if ( !bios_IsGpioValid( pin ) ) return 1;

	PinModes mode = PIN_OUTPUT;
	PinTypes type = PIN_NO_PULL;

	if ( stat < BIOS_GPIO_OUT_STAT_LOW )
	{
		mode = PIN_INPUT;
		if ( stat == BIOS_GPIO_OUT_STAT_FLOAT   ) mode = PIN_ANALOGIC;
		if ( stat == BIOS_GPIO_OUT_STAT_IN_DOWN ) type = PIN_PULL_DOWN;	
		if ( stat == BIOS_GPIO_OUT_STAT_IN_UP   ) type = PIN_PULL_UP;
	}

	GpioInit( NULL, pin, mode, type, stat );
	bios_Uart2GpioCheck( pin );
	return 0;
}

/*
 *
 */
int32_t bios_GetAdc( uint32_t pin )
{
	struct adc_module adc_instance;
	struct adc_config config_adc;
	uint16_t result = UINT16_MAX;

	adc_get_config_defaults(&config_adc);
	switch ( pin )
	{
		case ADC_POSITIVE_INPUT_PIN6  :
		case ADC_POSITIVE_INPUT_PIN7  :
		case ADC_POSITIVE_INPUT_PIN10 :
		case ADC_POSITIVE_INPUT_PIN11 :
		case ADC_POSITIVE_INPUT_PIN16 :
		case ADC_POSITIVE_INPUT_PIN17 :
			config_adc.reference = ADC_REFERENCE_INTVCC2;
			SUPC->VREF.reg = 0;
			break;
		case ADC_POSITIVE_INPUT_TEMP :
			config_adc.reference = ADC_REFERENCE_INTVCC2;
			SUPC->VREF.reg = SUPC_VREF_TSEN;
			break;
		case ADC_POSITIVE_INPUT_BANDGAP :
			config_adc.reference = ADC_REFERENCE_INTVCC2;
			SUPC->VREF.reg = SUPC_VREF_VREFOE | SUPC_VREF_SEL_1V0;
			break;
		case ADC_POSITIVE_INPUT_SCALEDCOREVCC :
		case ADC_POSITIVE_INPUT_SCALEDIOVCC :
			config_adc.reference = ADC_REFERENCE_INTREF;
			SUPC->VREF.reg = SUPC_VREF_VREFOE | SUPC_VREF_SEL_1V0;
			break;
		case BIOS_ADC_INPUT_TEMP_CONV :
			return bios_GetTemp();
		default :
			return INT16_MAX;
	}
	config_adc.positive_input = pin;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV128;
	config_adc.sample_length = 31;

	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
	adc_start_conversion(&adc_instance);
	while (adc_read(&adc_instance, &result) == STATUS_BUSY);
	adc_disable(&adc_instance);
	SUPC->VREF.reg = 0;
	if ( pin >= ADC_POSITIVE_INPUT_SCALEDCOREVCC ) result = (result * 4000) / 4095;

	return result;
}

/*
 * NVM Temperature Log Row
 */
#define TEMP_LOG_ADDR		_UL_(0x00806030)    /**< TEMP_LOG base address (type: fuses)*/
#define NVM_TEMPERATURE_LOG_ROW_BASE		(*(uint32_t*)TEMP_LOG_ADDR)
#define NVM_TEMPERATURE_LOG_ROW_HIG_BASE    (*(uint32_t*)(TEMP_LOG_ADDR + 4))
// room temperature
#define ROOM_TEMP_VAL_INT	(NVM_TEMPERATURE_LOG_ROW_BASE & 0xFF)
#define ROOM_TEMP_VAL_DEC   ((NVM_TEMPERATURE_LOG_ROW_BASE>>8) & 0x0F)
// hot temperature
#define HOT_TEMP_VAL_INT    ((NVM_TEMPERATURE_LOG_ROW_BASE>>12) & 0xFF)
#define HOT_TEMP_VAL_DEC    ((NVM_TEMPERATURE_LOG_ROW_BASE>>20) & 0x0F)
// 2's complement of the internal 1V reference drift at room/hot temperature
#define ROOM_INT1V_VAL		((NVM_TEMPERATURE_LOG_ROW_BASE>>24) & 0xFF)
#define HOT_INT1V_VAL		((NVM_TEMPERATURE_LOG_ROW_HIG_BASE) & 0xFF)
// Temperature sensor 12bit ADC conversion at room/hot temperature
#define ROOM_ADC_VAL		((NVM_TEMPERATURE_LOG_ROW_HIG_BASE>>8) & 0xFFF)
#define HOT_ADC_VAL			((NVM_TEMPERATURE_LOG_ROW_HIG_BASE>>20) & 0xFFF)

typedef	struct {
	int16_t	temp_r;
	int16_t	temp_h;
	int8_t	int1_r;
	int8_t	int1_h;
	int32_t	adc_r;
	int32_t	adc_h;
} nvm_Temp_LogRow_t;

/*
 *
 */
int32_t bios_GetTemp( void )
{
	volatile nvm_Temp_LogRow_t TempLog;
	TempLog.temp_r = (ROOM_TEMP_VAL_INT * 10) + ROOM_TEMP_VAL_DEC;
	TempLog.temp_h = (HOT_TEMP_VAL_INT * 10) + HOT_TEMP_VAL_DEC;
	TempLog.int1_r = ROOM_INT1V_VAL;
	TempLog.int1_h = HOT_INT1V_VAL;
	TempLog.adc_r  = ROOM_ADC_VAL;
	TempLog.adc_h  = HOT_ADC_VAL;

	int32_t adc_vdd = bios_GetAdc( ADC_POSITIVE_INPUT_SCALEDIOVCC );
	int32_t adc_tmp = bios_GetAdc( ADC_POSITIVE_INPUT_TEMP );

	volatile int32_t temp_c = adc_tmp;
	temp_c *= adc_vdd;	// REF=INTVCC2�Ȃ̂�VCC�{����1.0V���Z�̒l�ɂ���
	temp_c /= 1000;
	temp_c -= TempLog.adc_r;
	temp_c *= (TempLog.temp_h - TempLog.temp_r);
	temp_c /= (TempLog.adc_h - TempLog.adc_r);
    temp_c += TempLog.temp_r;
	return temp_c;
}

static void extwakeup_callback( void );
static void bios_Wakeup_Enable( bool deep, uint8_t mode );
static void bios_Wakeup_Disable( void );

#define INTERRUPT_FLAG_EXIWAKEUP		0x01
#define INTERRUPT_FLAG_RTC				0x02
#define BIOS_RTC_CLOCK_DIV_VAL			2
#define BIOS_RTC_CLOCK_DIV_REG			RTC_COUNT_PRESCALER_DIV_512

static bool sleep_initialized = false;
static volatile uint8_t sleep_exit_flag = 0;
static void (*sleep_callback)(void) = NULL;
static TimerEvent_t bios_sleepTimer;

//*************************************************
void bios_SystemReset( void )
{
    __disable_irq();
    system_reset();
}

//*************************************************
uint32_t bios_GetResetCause( void )
{
	return (sleep_exit_flag << 16) + (RSTC->BKUPEXIT.reg << 8) + RSTC->RCAUSE.reg;
}

#if BIOS_WRIPPER_IS_FUNCTION
/*
 * Enter to Sleep mode
 */
void bios_Lowpower( void )
{
	system_sleep();
}

/*
 *
 */
bool bios_TimerProcess( void )
{
	return TimerProcess( );	// sys_timer.c -> rtc_board.c (once)
}
#endif

/*
 *
 */
void bios_SetActive( bool act )
{
	if ( IS_CTRL_ACTIVE_DISABLE )	return;
	if ( act )
	{
		GpioInit( NULL, PIN_PA14, PIN_OUTPUT, PIN_NO_PULL, IS_CTRL_ACTIVE_INV ? 1 : 0 );
	} else
	{
		GpioInit( NULL, PIN_PA14, PIN_ANALOGIC, PIN_PULL_DOWN, 0 );	
	}
}

/*
 *
 */
static void bios_sleepTimerEvent( void *context )
{
	sleep_exit_flag |= INTERRUPT_FLAG_RTC;
}

/*
 *
 */
static void extwakeup_callback( void )
{
	sleep_exit_flag |= INTERRUPT_FLAG_EXIWAKEUP;
	bios_Wakeup_Disable();
}

/*
 *
 */
void bios_set_sleep_callback( void (*callback_func)(void) )
{
	sleep_callback = callback_func;
}


TimerEvent_t *GetTimerHeadObj(void);

/*
 *
 */
void bios_Sleep( bool deep, int32_t time, uint8_t mode )
{
#if _DEBUG_LOG_ENABLE_
 DEBUG_LOG_INC(0);
 DEBUG_LOG_SET(1, time);
 DEBUG_LOG_SET(2, (mode << 4) + deep );
 DEBUG_LOG_SET(3, bios_getSystemClock());
#endif

	bios_UartWaitTxEmpty();
	Pvt_RadioSleep();
	BoardDeInitMcu();
	bios_DeinitTRNG();
//	bios_GpioInitSetting();
	bios_Lcd_Disable();
	bios_Uart2Deinit();
	Ui_Led_TRx_Off();
	bios_Sleep_Init( deep ? SYSTEM_SLEEPMODE_BACKUP : SYSTEM_SLEEPMODE_STANDBY );

	if ( deep )	{ TimerClearAll(); }		// Deep�̂Ƃ��͑STimer���N���A����
	HwTimer_TC2_to_RTC();

	CRITICAL_SECTION_BEGIN();
	sleep_exit_flag = 0;
	if ( time <= 0 || mode )
	{
		time = abs(time);
		bios_Wakeup_Enable( deep, mode );
	}
	CRITICAL_SECTION_END();

	if ( time > 0 )
	{
		TimerSetValue( &bios_sleepTimer, time * 1000 );
		TimerStart( &bios_sleepTimer );
	}

	uint32_t s = SysTick->CTRL;
	SysTick->CTRL = 0;
	bios_SetActive( false );
	if ( HwTimer_Get_X32k_Stat() == X32K_STAT_INT )
	{
		if( deep || !IS_CTRL_SLEEP_INT32K )
		{
			HwTimer_UL32k_Select( true );	// UL32k
		}
	}

#if _DEBUG_LOG_ENABLE_
  DEBUG_LOG_SET(2, DEBUG_LOG_GET(2) + 0x100 );
  DEBUG_LOG_SET(4, RTC->MODE0.COUNT.reg);
  DEBUG_LOG_SET(5, RTC->MODE0.COMP[0].reg);
#endif

	CRITICAL_SECTION_BEGIN();
	if ( HwTimerIsRtcTimerTooShort() )		// RTC�^�C�}���Z������Ƃ���Polling����
	{
		RtcSetTimeoutPendingPolling();
	}
	CRITICAL_SECTION_END();

#if _DEBUG_LOG_ENABLE_
  DEBUG_LOG_SET(11, RtcIsTimeoutPendingPolling());
  TimerEvent_t *tp = TimerGetHeadObj();
  DEBUG_LOG_SET(12, (var_t)tp);
  DEBUG_LOG_SET(13, tp->Timestamp);
  DEBUG_LOG_SET(14, tp->ReloadValue);
  RTC->MODE0.GP[0].reg = RTC->MODE0.GP[1].reg = 0;
#endif

	while ( 1 )
	{
		while ( TimerProcess() );
		if ( sleep_exit_flag && !deep )	break;

		BoardMainClockSelect( GCLK_SOURCE_OSCULP32K );	// UL32k
		system_sleep();

#if _DEBUG_LOG_ENABLE_
  DEBUG_LOG_SET(6, DEBUG_LOG_GET(0));
#endif
		BoardMainClockSelect( GCLK_SOURCE_OSC16M );		// 16M
		if ( sleep_callback ) { sleep_callback(); }
	}

	if ( time > 0 )
	{
		TimerStop( &bios_sleepTimer );
	}
	if ( HwTimer_Get_X32k_Stat() == X32K_STAT_INT )
	{
		HwTimer_UL32k_Select( false );					// OSC32k
	}
	bios_Sleep_Init( SYSTEM_SLEEPMODE_IDLE );
	SysTick->CTRL = s;

	BoardInitMcu();
	HwTimer_RTC_to_TC2();
	bios_UartInit();
	basic_LoRa_Init();
	bios_SetActive( true );
}

/*
 *
 */
static void bios_Wakeup_Enable( bool deep, uint8_t mode )
{
	struct extint_chan_conf config_extint_chan;

	if ( !mode || (mode > EXTINT_DETECT_LOW) || (deep && mode < EXTINT_DETECT_HIGH))
	{
		mode = IS_CTRL_WAKEUP_INV ? EXTINT_DETECT_FALLING : EXTINT_DETECT_RISING;
	}

	switch ( mode )
	{
		case EXTINT_DETECT_FALLING :
		case EXTINT_DETECT_LOW :
			RSTC->WKPOL.reg &= ~CONF_EXT_WAKEUP_PIN_MASK;		// Active low
			config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
			break;
		case EXTINT_DETECT_RISING :
		case EXTINT_DETECT_HIGH :
			RSTC->WKPOL.reg |= CONF_EXT_WAKEUP_PIN_MASK;		// Active high
			config_extint_chan.gpio_pin_pull = EXTINT_PULL_DOWN;
			break;
		case EXTINT_DETECT_BOTH :
			config_extint_chan.gpio_pin_pull = EXTINT_PULL_NONE;
			break;
	}
	config_extint_chan.detection_criteria = mode;
	config_extint_chan.enable_async_edge_detection = false;
	config_extint_chan.filter_input_signal = false;
	config_extint_chan.gpio_pin = CONF_EXT_WAKEUP_PIN;
	config_extint_chan.gpio_pin_mux = CONF_EXT_WAKEUP_PINMUX;
	extint_chan_set_config( CONF_EXT_WAKEUP_INTNUM, &config_extint_chan );

	if ( deep )
	{
		RSTC->WKDBCONF.reg = RSTC_WKDBCONF_WKDBCNT_OFF;
		RSTC->WKEN.reg = CONF_EXT_WAKEUP_PIN_MASK;
	} else
	{
		extint_register_callback( extwakeup_callback, CONF_EXT_WAKEUP_INTNUM, EXTINT_CALLBACK_TYPE_DETECT );
		BoardEnableExtint( CONF_EXT_WAKEUP_INTNUM );
//		extint_chan_enable_callback(CONF_EXT_WAKEUP_INTNUM, EXTINT_CALLBACK_TYPE_DETECT);
	}
}

/*
 *
 */
static void bios_Wakeup_Disable( void )
{
	RSTC->WKEN.reg &= ~CONF_EXT_WAKEUP_PIN_MASK;
	BoardDisableExtint( CONF_EXT_WAKEUP_INTNUM );
//	extint_chan_disable_callback(CONF_EXT_WAKEUP_INTNUM, EXTINT_CALLBACK_TYPE_DETECT);
}

/*
 *
 */
bool bios_Get_Wakeup_Pin( void )
{
	return port_pin_get_input_level( CONF_EXT_WAKEUP_PIN );
}

/*
 * mode = SYSTEM_SLEEPMODE_IDLE, SYSTEM_SLEEPMODE_STANDBY, SYSTEM_SLEEPMODE_BACKUP
 */
void bios_Sleep_Init( enum system_sleepmode mode )
{
	if ( sleep_initialized == false )
	{
		sleep_initialized = true;
		SUPC->BOD33.reg &= ~SUPC_BOD33_ENABLE;/* BOD33 disabled */
		system_switch_performance_level( SYSTEM_PERFORMANCE_LEVEL_0 );
//		system_performance_level_disable();
		TimerInit( &bios_sleepTimer, bios_sleepTimerEvent );
	}

	system_set_sleepmode( mode );
	if ( mode == SYSTEM_SLEEPMODE_STANDBY )
	{
		PM->STDBYCFG.reg = PM_STDBYCFG_PDCFG_DEFAULT
/*						 | PM_STDBYCFG_DPGPD0			*/
/*						 | PM_STDBYCFG_DPGPD1			*/
						 | PM_STDBYCFG_VREGSMOD_LP
						 | PM_STDBYCFG_LINKPD_DEFAULT
/*						 | PM_STDBYCFG_BBIASHS(SYSTEM_RAM_BACK_BIAS_RETENTION)		*/
/*						 | PM_STDBYCFG_BBIASLP(SYSTEM_RAM_BACK_BIAS_RETENTION);		*/
						 | PM_STDBYCFG_BBIASHS(SYSTEM_RAM_BACK_BIAS_STANDBY_OFF)
						 | PM_STDBYCFG_BBIASLP(SYSTEM_RAM_BACK_BIAS_STANDBY_OFF);
	}
	bios_Wakeup_Disable();
}

/*
 *
 */
void bios_Initialize( void )
{
	bios_UartInit();
	bios_TimerInit();
	bios_Sleep_Init( SYSTEM_SLEEPMODE_IDLE );
//	bios_Wakeup_Init();
	bios_SetActive( true );
	bios_Randomize( 0 );
}

/*
 *
 */
void bios_set_extint( uint32_t pinMux, uint32_t irqLine, enum extint_pull pull, enum extint_detect irqMode, extint_callback_t callback )
{
	struct extint_chan_conf config_extint_chan;

	if ( callback == NULL )	return;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.detection_criteria = irqMode;
	config_extint_chan.gpio_pin_pull = pull;
	config_extint_chan.gpio_pin = pinMux >> 16;
	config_extint_chan.gpio_pin_mux = pinMux;
	extint_chan_set_config(irqLine, &config_extint_chan);
	extint_register_callback(callback, irqLine, EXTINT_CALLBACK_TYPE_DETECT);
	BoardEnableExtint(irqLine);
//	extint_chan_enable_callback(irqLine, EXTINT_CALLBACK_TYPE_DETECT);
}

/*
 *
 */
void bios_del_extint( uint32_t irqLine )
{
	BoardDisableExtint(irqLine);
//	extint_chan_disable_callback(irqLine, EXTINT_CALLBACK_TYPE_DETECT);
}

typedef const struct
{
	uint8_t	tcc;
	uint8_t	wo;
	uint8_t pchctrl;
	uint8_t	flag;
	uint8_t	tcc_mask;
	uint8_t	pch_mask;
	uint8_t gpio_pin;
	uint8_t mux_position;
} bios_pwm_config_t;
static bios_pwm_config_t const bios_pwm_config[8] =
{
	{ 0, 0, TCC0_GCLK_ID, 0x01, 0x0f, 0x3f, PIN_PA08, MUX_PA08E_TCC0_WO0 },
	{ 0, 1, TCC0_GCLK_ID, 0x02, 0x0f, 0x3f,	PIN_PA09, MUX_PA09E_TCC0_WO1 },
	{ 0, 2, TCC0_GCLK_ID, 0x04, 0x0f, 0x3f, PIN_PA18, MUX_PA18F_TCC0_WO2 },
	{ 0, 3, TCC0_GCLK_ID, 0x08, 0x0f, 0x3f,	PIN_PA19, MUX_PA19F_TCC0_WO3 },
	{ 1, 0, TCC1_GCLK_ID, 0x10, 0x30, 0x3f, PIN_PA24, MUX_PA24F_TCC1_WO2 },
	{ 1, 1, TCC1_GCLK_ID, 0x20, 0x30, 0x3f, PIN_PA25, MUX_PA25F_TCC1_WO3 },
	{ 2, 0, TCC2_GCLK_ID, 0x40, 0xc0, 0xc0, PIN_PA00, MUX_PA00E_TCC2_WO0 },
	{ 2, 1, TCC2_GCLK_ID, 0x80, 0xc0, 0xc0, PIN_PA01, MUX_PA01E_TCC2_WO1 },
};
static Tcc* const bios_Tcc[3] = { (Tcc*)TCC0, (Tcc*)TCC1, (Tcc*)TCC2 };
static uint8_t bios_pwm_flag = 0;
/*
 *
 */
bool bios_pwm_set( uint8_t pin, int32_t per_val, int32_t cc_val )
{
	bios_pwm_config_t *pwm_cfg = bios_pwm_config;

	while ( pwm_cfg->gpio_pin != pin )
	{
		if ( pwm_cfg->flag & 0x80 ) return true;
		pwm_cfg++;
	}
	uint8_t nTcc = pwm_cfg->tcc;
	uint8_t wo = pwm_cfg->wo;
	uint8_t	pchctrl = pwm_cfg->pchctrl;
	uint32_t cmask = MCLK_APBCMASK_TCC0 << nTcc;
	Tcc *hw = bios_Tcc[nTcc];

//	if ( cc_val < 0 )	cc_val = 0;
	if ( per_val > 0 )
	{
		if ( bios_pwm_flag & pwm_cfg->tcc_mask )
		{
			hw->PERBUF.reg = per_val;
			hw->CCBUF[wo].reg = cc_val;
		}
		else
		{
			MCLK->APBCMASK.reg |= cmask;
			GCLK->PCHCTRL[pchctrl].reg = GCLK_PCHCTRL_CHEN + GCLK_PCHCTRL_GEN_GCLK0;
			hw->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV8;
			hw->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
			hw->PER.reg = per_val;
			hw->CC[wo].reg = cc_val;
			while ( hw->SYNCBUSY.reg );
			hw->CTRLA.reg |= TCC_CTRLA_ENABLE;
		}
	}

 	struct system_pinmux_config pin_conf;
 	pin_conf.powersave    = false;
	pin_conf.input_pull   = SYSTEM_PINMUX_PIN_PULL_NONE;
 	pin_conf.direction    = SYSTEM_PINMUX_PIN_DIR_OUTPUT;

	if ( per_val > 0 )
	{
		if ( (bios_pwm_flag & pwm_cfg->flag) == 0 )
		{
			bios_pwm_flag |= pwm_cfg->flag;
			pin_conf.mux_position = pwm_cfg->mux_position;
			system_pinmux_pin_set_config( pin, &pin_conf );
			system_pinmux_pin_set_output_strength( pin, SYSTEM_PINMUX_PIN_STRENGTH_HIGH );
		}
	} else
	{
		if ( (bios_pwm_flag & pwm_cfg->flag) != 0 )
		{
	 		bios_pwm_flag &= ~pwm_cfg->flag;
	 		pin_conf.mux_position = SYSTEM_PINMUX_GPIO;
	 		system_pinmux_pin_set_config( pin, &pin_conf );
		}
	}

	if ( (bios_pwm_flag & pwm_cfg->tcc_mask) == 0 )
	{
		if ( hw->CTRLA.reg & TCC_CTRLA_ENABLE )
		{
			while ( hw->SYNCBUSY.reg );			/* Wait for sync */
			hw->CTRLA.reg &= ~TC_CTRLA_ENABLE;	/* Disable the TCC module */
			while ( hw->SYNCBUSY.reg );
			MCLK->APBCMASK.reg &= ~cmask;
			if ( (bios_pwm_flag & pwm_cfg->pch_mask) == 0 )
			{
				GCLK->PCHCTRL[pchctrl].reg = 0;	// GCLK_TCCx
			}
		}
	}
	return false;
}

/*
 *
 */
void bios_pwm_init( int32_t val_1, int32_t val_2 )
{
	bios_pwm_set( PIN_PA08, val_1, val_2 );
}

/*
 *
 */
void bios_pwm_deinit( void )
{
	bios_pwm_set( PIN_PA08 , -1, 0 );
}

/*
 *
 */
void bios_uart_trans( int32_t timeout )
{
	uint8_t cx;
	int32_t tt = timeout;

	bios_Uart2Init();
	DelayMs(10);
	while ( !UartMcuGetChar( &Uart1, &cx ));
	while ( !UartMcuGetChar( &Uart2, &cx ));

	while ( !Ui_IsBreak_LP() )
	{
		if ( tt ) bios_setWaitTick( tt );
		tt = 0;
		while ( !UartMcuGetChar( &Uart1, &cx ) ) { UartMcuPutChar( &Uart2, cx ); tt = timeout; }
		while ( !UartMcuGetChar( &Uart2, &cx ) ) { UartMcuPutChar( &Uart1, cx ); tt = timeout; }
		if ( timeout && !bios_getWaitTick() ) break;
	}
    bios_Uart2Deinit();
}

static int32_t bios_Xtal32kStat = X32K_STAT_INT;
/*
 *      |     Setting     |
 *  Stat|  0  |  1  |32768|
 *      | Int | Ext | Out |
 * -----+-----+-----+-----+
 *  Int |  0  | -1  | <-- |
 * -----+-----+-----+-----+
 *  Ext |  x  |  1  |32768|
 * -----+-----+-----+-----+
 */
void bios_SetXtal32k( int32_t val )
{
	switch ( val )
	{
	case X32K_STAT_INT    :
	case X32K_STAT_EXT    :
		if ( Settings.X32k_flag != val || bios_Xtal32kStat == X32K_STAT_FAIL )
		{
			Settings.X32k_flag = val;
			HwTimer_X32k_Reset( val );
			bios_CheckXtal32k();
		}
		break;
	case X32K_STAT_OUTPUT :
		if ( bios_Xtal32kStat == X32K_STAT_EXT )
		{
			HwTimer_X32k_Output();
			bios_CheckXtal32k();
		}
		break;
	}
}

/*
 *
 */
int32_t bios_CheckXtal32k( void )
{
	bios_Xtal32kStat = HwTimer_Get_X32k_Stat();
	if ( bios_Xtal32kStat == X32K_STAT_INT && Settings.X32k_flag != X32K_INT )
	{
		bios_Xtal32kStat = X32K_STAT_FAIL;
		bios_UartPutLine( "*ext32k failure" );
	}
	return bios_Xtal32kStat;
}

/*
 *
 */
int32_t bios_GetXtal32kStat( void )
{
	return bios_Xtal32kStat;
}

/* Include the following functions in "bios.h" as inline functions */
#if 0
/*
 *
 */
bool bios_I2cIsEnable( void )
{
	return I2cIsEnable( );
}

/*
 *
 */
void bios_I2cInit( void )
{
	I2cInit( );
}

/*
 *
 */
void bios_I2cDeInit( void )
{
	I2cDeInit( );
}

/*
 *
 */
void bios_I2cSetBaudrate( uint32_t val )
{
	I2cSetBaudrate( (uint32_t)val );
}

/*
 *
 */
uint32_t bios_I2cGetBaudrate( void )
{
	return I2cGetBaudrate( );
}

/*
 *
 */
uint8_t bios_I2cWriteMemory( uint8_t dev_adrs, int16_t reg_addr, uint8_t *reg_data, uint16_t len )
{
	return I2CWriteMemory( dev_adrs, reg_addr, reg_data, len );
}

/*
 *
 */
uint8_t bios_I2cReadMemory( uint8_t dev_adrs, int16_t reg_addr, uint8_t *reg_data, uint16_t len )
{
	return I2CReadMemory( dev_adrs, reg_addr, reg_data, len );
}
#endif

Spi_t bios_spi;
/*
 *
 */
bool bios_SpiInit( int32_t baud )
{
	if ( baud == 0 )
	{
		if ( bios_spi.baudrate ) SpiDeInit( &bios_spi );
		return false;
	}
	if ( baud < 1000 || baud > (SPI_BAUD_MAX + 7) ) return true;
	if ( bios_spi.baudrate ) SpiDisable( &bios_spi );
	SpiInit( &bios_spi, SPI_2, BIOS_MOSI, BIOS_MISO, BIOS_SCLK, BIOS_NSS, baud );
	SpiEnable( &bios_spi );
	bios_spi.enable = true;
	return false;
}

/*
 *
 */
uint32_t bios_SpiStat( void )
{
	return bios_spi.baudrate;
}

/*
 *
 */
bool bios_SpiTRx( uint8_t *val_out, uint8_t *val_in, int32_t len )
{
	if ( bios_spi.baudrate == 0 ) return true;
	if ( len < 1 ) len = 1;
	if ( len > 4 ) len = 4;
	while ( len-- )
	{
		*val_out++ = SpiInOut( &bios_spi, *val_in++ );
	}
	return false;
}
