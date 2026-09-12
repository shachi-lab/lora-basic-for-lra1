/*!
 * @file    bios.c
 * @brief   HeaderFile of BIOS for Basic Interpreter
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
#ifndef __BIOS_H
#define __BIOS_H

#include "version.h"

#define	RANDOM_BY_TRNG				1

#define	FLASH_BEGIN_ADDRESS			0x0001e400			// 0x01f400�`0x01ffff : 7168 bytes
#define	FLASH_END_ADDRESS			0x0001ffff

#define FLASH_LORAMAC_COUNT_ADRS	0x0001f400			// 0x01f400�`0x01f4ff :  256 bytes
#define FLASH_LORAMAC_PARM_ADRS		0x0001f500			// 0x01f500�`0x01f7ff :  768 bytes (3*256)
#define FLASH_ARRAY_ADRS			0x0001f800			// 0x01f800�`0x01fbff : 1024 bytes (4*256)
														// 0x01fc00�`0x01fcff :  256 bytes
#define FLASH_ADJUST_ADRS			0x0001fd00			// 0x01fd00�`0x01fdff :  256 bytes
#define FLASH_SETTINGS_ADRS			0x0001fe00			// 0x01fe00�`0x01feff :  256 bytes
#define FLASH_WANCONF_ADRS			0x0001ff00			// 0x01ff00�`0x01ffff :  256 bytes

#define FLASH_LORAMAC_SIZE			0x00000300			// 768 bytes

#define	FLASH_PROGRAM_ADRS_1		0x00400000			// 0x400000�`0x400fff : 4096 byte (RWW)
#define	FLASH_PROGRAM_SIZE_1		0x00001000
#define	FLASH_PROGRAM_ADRS_2		0x0001e400			// 0x01e400�`0x01f3ff : 4096 bytes
#define	FLASH_PROGRAM_SIZE_2		0x00001000

#define PROGRAM_AREA_SIZE			0x00002000			// 8192 bytes

#define CONF_EXT_WAKEUP_PIN			PIN_PA06A_RSTC_EXTWAKE6
#define CONF_EXT_WAKEUP_MUX			MUX_PA06A_RSTC_EXTWAKE6
#define CONF_EXT_WAKEUP_PINMUX		PINMUX_PA06A_RSTC_EXTWAKE6
#define CONF_EXT_WAKEUP_INTNUM		PIN_PA06A_EIC_EXTINT_NUM
#define CONF_EXT_WAKEUP_PIN_MASK	(1 << CONF_EXT_WAKEUP_PIN)
#define CONF_EXT_WAKEUP_INT_MASK	(1 << PIN_PA06A_EIC_EXTINT_NUM)

#ifndef LPRAM
#define LPRAM	__attribute__ ((section(".lpram"),aligned(8)))
#endif
#ifndef VARBSS
#define VARBSS	__attribute__ ((section(".var_bss"),aligned(4)))
#endif
extern	uint8_t _svar_bss, _evar_bss;

#ifndef ARYBSS
#if _DEBUG_LOG_ENABLE_
#define ARYBSS	__attribute__ ((section(".debug_bss"),aligned(4)))
#else
#define ARYBSS  VARBSS
#endif
#endif

#define BIOS_UART_TX_BUFF_SIZE		256
#define BIOS_UART_RX_BUFF_SIZE		256

extern char bios_UartRxBuff[BIOS_UART_TX_BUFF_SIZE];
extern char bios_UartTxBuff[BIOS_UART_RX_BUFF_SIZE];

#define BIOS_UART2_BAUDRATE_DAFULT	9600

#define BIOS_ADC_VAL_MAX			4095			// 12bit
#define BIOS_ADC_INPUT_TEMP_CONV	28

#define BIOS_GPIO_IN_PULL_DOWN		0x100
#define BIOS_GPIO_IN_PULL_UP		0x200

#define	BIOS_GPIO_OUT_PIN_PACK		0x80000000

#define	BIOS_GPIO_OUT_STAT_HIGH		1
#define	BIOS_GPIO_OUT_STAT_LOW		0
#define	BIOS_GPIO_OUT_STAT_FLOAT	-1
#define	BIOS_GPIO_OUT_STAT_IN_UP	-2
#define	BIOS_GPIO_OUT_STAT_IN_DOWN	-3
#define	BIOS_GPIO_OUT_STAT_IN_HIZ	-4

#define BIOS_WRIPPER_IS_FUNCTION	0

void bios_UartInit( void );
void bios_UartDeinit( void );
uint8_t bios_UartSetBaudrate( uint32_t baudrate );
//int32_t bios_UartGetBaudrate( void );
void bios_UartPutChar( char ch );
int bios_UartGetChar( void );
void bios_UartWrite( uint8_t *str, uint8_t len );
void bios_UartPuts( const char *str );
void bios_UartPutCrLf( void );
void bios_UartPutLine( const char *str );
void bios_UartPrintf( const char *format, ... )  __attribute__((format(printf, 1, 2)));
void bios_UartPrintfLine( const char *format, ... )  __attribute__((format(printf, 1, 2)));
uint8_t bios_UartGetBreak( void );
void bios_UartWaitTxEmpty( void );
void bios_UartFlush( void );
void bios_UartSetBreakCh( int32_t ch );
void bios_SetBreakCh( int32_t ch );
int32_t bios_GetBreakCh( void);
uint16_t bios_UartTxBuffLen( void );
uint16_t bios_UartRxBuffLen( void );

uint8_t bios_Uart2SetBaudrate( uint32_t baudrate );
uint32_t bios_Uart2GetBaudrate( void );
void bios_Uart2Init( void );
void bios_Uart2Deinit( void );
void bios_Uart2PutChar( char ch );
int bios_Uart2GetChar( void );
void bios_Uart2Write( uint8_t *str, uint8_t len );
uint16_t bios_Uart2TxBuffLen( void );
uint16_t bios_Uart2RxBuffLen( void );
void bios_Uart2Begin( int32_t pin );

#if BIOS_WRIPPER_IS_FUNCTION
uint32_t bios_UartGetBaudrate( void );
char *bios_GetAutorun( void );
void bios_UartSetEcho( uint8_t val );
uint8_t bios_UartGetEcho( void );
#else
#define bios_UartGetBaudrate()			Settings.Baudrate
#define bios_GetAutorun()				Settings.AutoRun
#define bios_UartGetEcho()				Settings.Echo
#define	bios_UartSetEcho( val )			do { Settings.Echo = val ? 1 : 0; } while( 0 )
#endif

uint8_t bios_SetAutorun( char *str );

void bios_UartPutCharEcho( char ch );
void bios_UartPutStrEcho( const char *str );
void bios_PutVersion( void );

void bios_TimerInit( void );
void bios_setWaitTick( int32_t tick );
int32_t bios_getWaitTick( void );
uint32_t bios_getSystemTick( void );
void bios_setSystemTick( uint32_t tick );
uint32_t bios_getSystemClock( void );
uint32_t bios_getSystemMilisec( void );
void bios_setSystemClock( uint32_t seconds );

inline void bios_rtc_wait_syncbusy( void ) { while( RTC->MODE0.SYNCBUSY.reg ); }
int32_t *bios_getBackupRegPtr( uint32_t n );
	
void bios_Randomize( uint32_t val );
uint32_t bios_GetRandom( void );
uint32_t bios_GetTRNG( void );

inline uint32_t bios_GetMpuId( void ) {	return DSU->DID.reg; }

void bios_SystemReset( void );
uint32_t bios_GetResetCause( void );

void bios_FlashInit( void );
uint8_t bios_FlashWrite( uint32_t adrs, uint8_t *buff, uint16_t len );
uint8_t bios_FlashProgSave( uint8_t *buff, uint16_t len );
uint8_t bios_FlashProgLoad( uint8_t *buff, uint16_t len );

uint8_t bios_GpioInput( int32_t pin, uint8_t *stat );
uint8_t bios_GpioOutput( int32_t pin, int32_t stat );

int32_t bios_GetAdc( uint32_t pin );
int32_t bios_GetTemp( void );

void bios_set_extint( uint32_t pinMux, uint32_t irqLine, enum extint_pull pull, enum extint_detect irqMode, extint_callback_t callback );
void bios_del_extint( uint32_t irqLine );

void bios_SetActive( bool act );
void bios_Wakeup_Init( void );
bool bios_Get_Wakeup_Pin( void );
void bios_Sleep_Init( enum system_sleepmode mode );

#if BIOS_WRIPPER_IS_FUNCTION
void bios_Lowpower( void );
bool bios_TimerProcess( void );
#else
#define bios_Lowpower()				system_sleep()
#define bios_TimerProcess()			TimerProcess()		// sys_timer.c -> rtc_board.c (once)
#endif

void bios_Sleep( bool deep, int32_t time, uint8_t mode );
void bios_set_exit_flag( uint8_t flag );
void bios_set_sleep_callback( void (*callback_func)(void) );
void bios_ClearWDT( void );

void bios_Initialize( void );

int32_t basic_LoRa_Get_Sn( void );

void bios_uart_trans( int32_t timeout );
bool bios_pwm_set( uint8_t pin, int32_t per_val, int32_t cc_val );
void bios_pwm_init( int32_t val_1, int32_t val_2 );
void bios_pwm_deinit( void );

void bios_SetXtal32k( int32_t val );
int32_t bios_CheckXtal32k( void );
int32_t bios_GetXtal32kStat( void );

bool bios_IsUartEnable( void );
bool bios_IsUart2Enable( void );

#include "bios_gps.h"
#include "bios_lcd.h"
#include "sys_i2c.h"

inline void bios_I2cInit( void )	{ I2cInit( ); }
inline void bios_I2cDeInit( void )	{ I2cDeInit( ); }
inline bool bios_I2cIsEnable( void ) { return I2cIsEnable( ); }
inline void bios_I2cSetBaudrate( uint32_t val ) { I2cSetBaudrate( (uint32_t)val ); }
inline uint32_t bios_I2cGetBaudrate( void )		{ return I2cGetBaudrate( ); }
inline uint8_t bios_I2cWriteMemory( uint8_t dev_adrs, int16_t reg_addr, uint8_t *reg_data, uint16_t len )	{ return I2CWriteMemory( dev_adrs, reg_addr, reg_data, len ); }
inline uint8_t bios_I2cReadMemory( uint8_t dev_adrs, int16_t reg_addr, uint8_t *reg_data, uint16_t len )	{ return I2CReadMemory( dev_adrs, reg_addr, reg_data, len ); }

#include "sys_spi.h"
bool bios_SpiInit( int32_t baud );
uint32_t bios_SpiStat( void );
bool bios_SpiTRx( uint8_t *val_out, uint8_t *val_in, int32_t len );

#endif
