/*!
 * \file      board-samr35.c
 *
 * \brief     Target board general functions implementation
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
#include "utilities.h"
#include "system.h"
#include "rtc-board.h"
#include "board-mcu.h"
#include "gpio-board.h"
#include "uart-board.h"
#include "i2c-board.h"
#include "board-config.h"
#include "sx1276-board.h"
#include "sx1276.h"
#include "ui_common.h"

/*
 * MCU objects
 */
Gpio_t Led1;
Gpio_t Led2;

static uint32_t dev_serial_number[4];
static uint8_t BattLevel = 0;	//  Battery level [0: node is connected to an external power source ...

int _write( int fd, const void *buf, size_t count );
int _read( int fd, const void *buf, size_t count );

/*
 * unique 128-bit serial number for ATSAM mcu
 */
#define SEREAL_NUMBER_WORD_0		*((uint32_t *)0x0080A00C)
#define SEREAL_NUMBER_WORD_1		*((uint32_t *)0x0080A040)
#define SEREAL_NUMBER_WORD_2		*((uint32_t *)0x0080A044)
#define SEREAL_NUMBER_WORD_3		*((uint32_t *)0x0080A048)

/*!
 * Flag to indicate if the MCU is Initialized
 */
static bool McuInitialized = false;

/*!
 * \brief Disable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardDisableIrq( void )
{
	cpu_irq_enter_critical();
}

/*!
 * \brief Enable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardEnableIrq( void )
{
	cpu_irq_leave_critical();
}

void BoardCriticalSectionBegin( uint32_t *mask )
{
	*mask = __get_PRIMASK( );
	__disable_irq( );
}

void BoardCriticalSectionEnd( uint32_t *mask )
{
	__set_PRIMASK( *mask );
}

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph( void )
{
//  GpioInit( &Led1, LED_1, PIN_OUTPUT, PIN_NO_PULL, LED_OFF );
//	GpioInit( &Led2, LED_2, PIN_OUTPUT, PIN_NO_PULL, LED_OFF );
}

/*!
 * \brief Initializes the mcu.
 */
void BoardInitMcu( void )
{
	BoardMainClockSelect( GCLK_SOURCE_OSC16M );
//  UartInit( &Uart1, UART_1 );
	GpioMcuSetDefault( false );
    SX1276IoInit( );
    SpiInit( &SX1276.Spi, SPI_1, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, RADIO_NSS, SPI_BAUD_MAX );
    RtcInit( );
	BoardInitWDT( !IS_DEBUG_WDT_DISABLE );
    McuInitialized = true;
}

/*!
 * \brief Clear Clock mask.
 */
void BoardMclkMaskClear( void )
{
	MCLK->APBCMASK.reg = 0;
	MCLK->APBDMASK.reg = 0;
//	MCLK->APBEMASK.reg = 0;
}

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu( void )
{
    if( McuInitialized == true )
    {
		I2cDeInit( );
        SpiDeInit( &SX1276.Spi );
	    SX1276IoDeInit( );
		UartDeInit( &Uart1 );
		BoardInitWDT( false );
    }
    McuInitialized = false;
}

/*
 *
 */
void BoardInitWDT( bool flag )
{
	system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBA, MCLK_APBAMASK_WDT);
	WDT->CTRLA.reg &= ~WDT_CTRLA_ENABLE;
	while( WDT->SYNCBUSY.reg );
	if( flag == false ) return;
	WDT->CONFIG.reg = WDT_CONFIG_PER_CYC8192;
	WDT->CTRLA.reg |= WDT_CTRLA_ENABLE;
	while( WDT->SYNCBUSY.reg );
}

/*
 *
 */
void BoardClearWDT( void )
{
	if( WDT->SYNCBUSY.bit.CLEAR ) return;
	WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
}

/**
 * \brief Setect main clock source.
 * clock_src = GCLK_SOURCE_OSC16M, GCLK_SOURCE_OSC32K, GCLK_SOURCE_XOSC32K, GCLK_SOURCE_OSCULP32K
 */
void BoardMainClockSelect( uint8_t clock_src )
{
	static bool initialized = false;

	if( initialized && ((GCLK->GENCTRL[0].reg & GCLK_GENCTRL_SRC_Msk) == clock_src) ) return;
	initialized = true;

	uint32_t div = 0;
	if( clock_src == GCLK_SOURCE_OSC16M )
	{
		OSCCTRL->OSC16MCTRL.reg = OSCCTRL_OSC16MCTRL_ENABLE | OSCCTRL_OSC16MCTRL_RUNSTDBY | OSCCTRL_OSC16MCTRL_FSEL_16;
		while( !(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_OSC16MRDY) );
		div = GCLK_GENCTRL_DIV( BOADR_MCU_CLK_DIV );
	}
 
	while( GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL0 );
	cpu_irq_enter_critical();

//	GCLK->GENCTRL[0].reg = GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | clock_src | div;
	GCLK->GENCTRL[0].reg = GCLK_GENCTRL_GENEN | clock_src | div;

	while( GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL0 );
	cpu_irq_leave_critical();

	if( clock_src != GCLK_SOURCE_OSC16M )
	{
		OSCCTRL->OSC16MCTRL.reg = 0;
	}
}

/*
 *
 */
void BoardSercomDisableMclk( Sercom *const hw )
{
	switch( (uint32_t)hw )
	{
		case (uint32_t)SERCOM0 : MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM0;	break;
		case (uint32_t)SERCOM1 : MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM1;	break;
		case (uint32_t)SERCOM2 : MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM2;	break;
		case (uint32_t)SERCOM3 : MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM3;	break;
		case (uint32_t)SERCOM4 : MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_SERCOM4;	break;
		case (uint32_t)SERCOM5 : MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_SERCOM5;	break;
	}
}

/*
 *
 */
void BoardSercomEnableMclk( Sercom *const hw )
{
	switch( (uint32_t)hw )
	{
		case (uint32_t)SERCOM0 : MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM0;	break;
		case (uint32_t)SERCOM1 : MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM1;	break;
		case (uint32_t)SERCOM2 : MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM2;	break;
		case (uint32_t)SERCOM3 : MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM3;	break;
		case (uint32_t)SERCOM4 : MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM4;	break;
		case (uint32_t)SERCOM5 : MCLK->APBDMASK.reg |= MCLK_APBDMASK_SERCOM5;	break;
	}
}

void BoardEnableExtint( uint32_t irqLine )
{
	EIC->INTFLAG.reg = (1UL << irqLine);
	EIC->INTENSET.reg = (1UL << irqLine);
}

void BoardDisableExtint( uint32_t irqLine )
{
	EIC->INTENCLR.reg = (1UL << irqLine);
}

/*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * \retval seed Generated pseudo random seed
 */
uint32_t BoardGetRandomSeed( void )
{
	uint32_t seed;
	seed = SEREAL_NUMBER_WORD_0 ^ SEREAL_NUMBER_WORD_1 ^ SEREAL_NUMBER_WORD_2 ^ SEREAL_NUMBER_WORD_3;
	return seed;
}

/*!
 * \brief Gets the board 128 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
uint32_t *BoardGetUniqueId( void )
{
	dev_serial_number[0] = SEREAL_NUMBER_WORD_0;
	dev_serial_number[1] = SEREAL_NUMBER_WORD_1;
	dev_serial_number[2] = SEREAL_NUMBER_WORD_2;
	dev_serial_number[3] = SEREAL_NUMBER_WORD_3;
	return dev_serial_number;
}

/*!
 * \brief Get the current battery level
 *
 * \retval value  battery level [  0: USB,
 *                                 1: Min level,
 *                                 x: level
 *                               254: fully charged,
 *                               255: Error]
 */
uint8_t BoardGetBatteryLevel( void )
{
	return BattLevel;
}

/*!
 * \brief Set the current battery level
 */
void BoardSetBatteryLevel( uint8_t level )
{
	BattLevel = level;
}

/*!
 * \brief Get the board power source
 *
 * \retval value  power source [0: USB_POWER, 1: BATTERY_POWER]
 */
uint8_t GetBoardPowerSource( void )
{
    return BATTERY_POWER;
}

void BoardLowPowerHandler( void )
{
    __disable_irq( );
    /*!
     * If an interrupt has occurred after __disable_irq( ), it is kept pending
     * and cortex will not enter low power anyway
     */

    // Call low power handling function.

    __enable_irq( );
}

#if !defined ( __CC_ARM )

/*
 * Function to be used by stdout for printf etc
 */
int _write( int fd, const void *buf, size_t count )
{
    while( UartPutBuffer( &Uart1, ( uint8_t* )buf, ( uint16_t )count ) != 0 ){ };
    return count;
}

/*
 * Function to be used by stdin for scanf etc
 */
int _read( int fd, const void *buf, size_t count )
{
    size_t bytesRead = 0;
    while( UartGetBuffer( &Uart1, ( uint8_t* )buf, count, ( uint16_t* )&bytesRead ) != 0 ){ };
    // Echo back the character
    while( UartPutBuffer( &Uart1, ( uint8_t* )buf, ( uint16_t )bytesRead ) != 0 ){ };
    return bytesRead;
}

#endif

#ifdef USE_FULL_ASSERT
/*
 * Function Name  : assert_failed
 * Description    : Reports the name of the source file and the source line number
 *                  where the assert_param error has occurred.
 * Input          : - file: pointer to the source file name
 *                  - line: assert_param error line source number
 * Output         : None
 * Return         : None
 */
void assert_failed( uint8_t* file, uint32_t line )
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %u\r\n", file, line) */

    printf( "Wrong parameters value: file %s on line %u\r\n", ( const char* )file, line );
    /* Infinite loop */
    while( 1 )
    {
    }
}
#endif

