/*!
 * \file      sx1276-board.c
 *
 * \brief     Target board ATSAM35 Module driver implementation
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
#include <stdlib.h>
#include "utilities.h"
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_fifo.h"
#include "sys_spi.h"
#include "sys_timer.h"
#include "sys_uart.h"
#include "board-config.h"
#include "board-mcu.h"
#include "radio.h"
#include "sx1276-board.h"
#include "hw_timer.h"

/*!
 * \brief Gets the board PA selection configuration
 *
 * \param [IN] channel Channel frequency in Hz
 * \retval PaSelect RegPaConfig PaSelect value
 */
static uint8_t SX1276GetPaSelect( uint32_t channel );

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    SX1276Init,
    SX1276GetStatus,
    SX1276SetModem,
    SX1276SetChannel,
    SX1276IsChannelFree,
    SX1276Random,
    SX1276SetRxConfig,
    SX1276SetTxConfig,
    SX1276CheckRfFrequency,
    SX1276GetTimeOnAir,
    SX1276Send,
    SX1276SetSleep,
    SX1276SetStby,
    SX1276SetRx,
    SX1276StartCad,
    SX1276SetTxContinuousWave,
    SX1276ReadRssi,
    SX1276Write,
    SX1276Read,
    SX1276WriteBuffer,
    SX1276ReadBuffer,
    SX1276SetMaxPayloadLength,
    SX1276SetPublicNetwork,
    SX1276GetWakeupTime,
};

/*!
 * Antenna switch GPIO pins objects
 */
#if RADIO_ANT_SWITCH != PIN_NC
Gpio_t AntSwitch;
#endif

/*!
 * Tcxo Power GPIO pins objects
 */
#if	RADIO_TCXO_POWER != PIN_NC
Gpio_t TcxoPower;
#endif

/*!
 * Band Sel GPIO pins objects
 */
#if	RADIO_BAND_SEL != PIN_NC
Gpio_t BandSel;
#endif

uint8_t Padac_value;				// RF_PADAC_20DBM_OFF <- Preset in SX1276Reset()
uint8_t SX1276CurrentOscValue;		// SX1276_OSC_DEFAULT <- Preset in SX1276Reset()
/*
 *
 */
void SX1276IoInit( void )
{
/*
	GpioInit( &SX1276.Spi.Miso, RADIO_MISO, PIN_INPUT , PIN_NO_PULL, 0 );
	GpioInit( &SX1276.Spi.Mosi, RADIO_MOSI, PIN_OUTPUT, PIN_PULL_UP, 1 );
	GpioInit( &SX1276.Spi.Sclk, RADIO_SCLK, PIN_OUTPUT, PIN_PULL_UP, 1 );
	GpioInit( &SX1276.Spi.Nss , RADIO_NSS , PIN_OUTPUT, PIN_PULL_UP, 1 );
*/
    GpioInit( NULL, RADIO_DIO_0, PIN_INPUT, PIN_PULL_DOWN, 0 );
    GpioInit( NULL, RADIO_DIO_1, PIN_INPUT, PIN_PULL_DOWN, 0 );
    GpioInit( NULL, RADIO_DIO_2, PIN_INPUT, PIN_PULL_DOWN, 0 );
    GpioInit( NULL, RADIO_DIO_3, PIN_INPUT, PIN_PULL_DOWN, 0 );
    GpioInit( NULL, RADIO_DIO_4, PIN_INPUT, PIN_PULL_DOWN, 0 );
//  GpioInit( NULL, RADIO_DIO_5, PIN_INPUT, PIN_PULL_DOWN, 0 );
}

static const uint32_t IoIrq_pin_mux[6] =
{
	PINMUX_PB16A_EIC_EXTINT0, PINMUX_PA11A_EIC_EXTINT11, PINMUX_PA12A_EIC_EXTINT12, PINMUX_PB17A_EIC_EXTINT1, PINMUX_PA10A_EIC_EXTINT10, PINMUX_PB00A_EIC_EXTINT0
};
static const uint32_t IoIrq_line[6] =
{
	PIN_PB16A_EIC_EXTINT_NUM, PIN_PA11A_EIC_EXTINT_NUM, PIN_PA12A_EIC_EXTINT_NUM, PIN_PB17A_EIC_EXTINT_NUM, PIN_PA10A_EIC_EXTINT_NUM, PIN_PB00A_EIC_EXTINT_NUM
};

/*
 *
 */
void SX1276IoIrqInit( DioIrqHandler **irqHandlers )
{
	struct extint_chan_conf config_extint_chan;

	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_NONE;
	config_extint_chan.detection_criteria = EXTINT_DETECT_RISING;
	
    for( int8_t i = 0; i < 5; i++ )
    {
		if( irqHandlers[i] != NULL )
		{
			config_extint_chan.gpio_pin = IoIrq_pin_mux[i] >> 16;
			config_extint_chan.gpio_pin_mux = IoIrq_pin_mux[i];
			extint_chan_set_config( IoIrq_line[i], &config_extint_chan );
			extint_register_callback( irqHandlers[i],  IoIrq_line[i], EXTINT_CALLBACK_TYPE_DETECT );
			BoardEnableExtint( IoIrq_line[i] );
//			extint_chan_enable_callback(IoIrq_line[i], EXTINT_CALLBACK_TYPE_DETECT);
		}
	}
}

/*
 *
 */
void SX1276IoDeInit( void )
{
//  GpioInit( &SX1276.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );	// in SpiDeInit() function
    for( int8_t i = 0; i < 5; i++ )
    {
		BoardDisableExtint( IoIrq_line[i] );
//		extint_chan_disable_callback( IoIrq_line[i], EXTINT_CALLBACK_TYPE_DETECT );
	}
    GpioInit( NULL, RADIO_DIO_0, PIN_INPUT, PIN_PULL_DOWN, 0 );
    GpioInit( NULL, RADIO_DIO_1, PIN_INPUT, PIN_PULL_DOWN, 0 );
    GpioInit( NULL, RADIO_DIO_2, PIN_INPUT, PIN_PULL_DOWN, 0 );
    GpioInit( NULL, RADIO_DIO_3, PIN_INPUT, PIN_PULL_DOWN, 0 );
    GpioInit( NULL, RADIO_DIO_4, PIN_INPUT, PIN_PULL_DOWN, 0 );
//	GpioInit( NULL, RADIO_DIO_5, PIN_INPUT, PIN_PULL_DOWN, 0 );
}

/*!
 * \brief Enables/disables the TCXO if available on board design.
 *
 * \param [IN] state TCXO enabled when true and disabled when false.
 */
void SX1276SetBoardTcxo( uint8_t state )
{
#if RADIO_TCXO_POWER != PIN_NC
	if( state == true )
	{
		GpioInit( NULL, RADIO_TCXO_POWER, PIN_OUTPUT, PIN_PULL_UP, 1 );
		DelayMs( BOARD_TCXO_WAKEUP_TIME );
	}
	else
	{
		GpioInit( NULL, RADIO_TCXO_POWER, PIN_OUTPUT, PIN_PULL_UP, 0 );
	}
#endif
	BoardClearWDT();
}

/*
 *
 */
uint32_t SX1276GetBoardTcxoWakeupTime( void )
{
    return BOARD_TCXO_WAKEUP_TIME;
}

/*
 *
 */
void SX1276Reset( void )
{
    // Disable the TCXO
    SX1276SetBoardTcxo( false );
	SX1276SetAntSwLowPower( true );
	SX1276CurrentOscValue = SX1276_OSC_DEFAULT;
	Padac_value = RF_PADAC_20DBM_OFF;

    // Set RESET pin to 0
    GpioInit( NULL, RADIO_RESET, PIN_OUTPUT, PIN_NO_PULL, 0 );
    // Wait 1 ms
    DelayMs( 1 );
    // Configure RESET as input
    GpioInit( NULL, RADIO_RESET, PIN_INPUT, PIN_NO_PULL, 1 );
    // Wait 6 ms
    DelayMs( 6 );
}

/*
 *
 */
void SX1276SetClkout( bool enable )
{
	uint8_t	div = enable ? SX1276_CLKOUT_DIV : SX1276_OSC_DEFAULT;
	if( div == SX1276CurrentOscValue )	return;
	SX1276CurrentOscValue = div;
	uint8_t modem = SX1276.Settings.Modem;

    SX1276SetModem( MODEM_FSK );
	SX1276Write( REG_OSC, SX1276CurrentOscValue );
    SX1276SetModem( modem );
}

/*
 *
 */
void SX1276SetRfTxPower( int8_t power )
{
	power = SX1276GetOffsetTxPower( power );
	uint8_t paDac = SX1276Read( REG_PADAC ) & RF_PADAC_20DBM_MASK;
	uint8_t paConfig = SX1276GetPaSelect( SX1276.Settings.Channel );

	if( ( paConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
	{
		if( power > 17 )
		{
			paDac |= RF_PADAC_20DBM_ON;
			if( power < 5 )
			{
				power = 5;
			}
			if( power > 20 )
			{
				power = 20;
			}
			power -= 5;
		}
		else
		{
			paDac |= RF_PADAC_20DBM_OFF;
			if( power < 2 )
			{
				power = 2;
			}
			if( power > 17 )
			{
				power = 17;
			}
			power -= 2;
		}
		paConfig |= 0x70;
	}
	else
	{
//		paDac |= RF_PADAC_20DBM_OFF;
		paDac |= Padac_value;
		if( power > 0 )
		{
			if( power > 15 )
			{
				power = 15;
			}
			paConfig |= 0x70;
		}
		else
		{
			if( power < -4 )
			{
				power = -4;
			}
			power += 4;
		}
	}
	paConfig |= power;
	SX1276Write( REG_PACONFIG, paConfig );
	SX1276Write( REG_PADAC, paDac );
}

/*
 *
 */
static uint8_t SX1276GetPaSelect( uint32_t channel )
{
#if defined( __PABOOST__ )
    // RFM95/96/97/98 does not have RFO pins connected to anything. 
    // Only PA_BOOST pin is connected, so must use PA_BOOST Pout = 2 + OutputPower.
    // The documentation is pretty confusing on this topic: PaSelect says the max power is 20dBm,
    // but OutputPower claims it would be 17dBm. My measurements show 20dBm is correct
    return RF_PACONFIG_PASELECT_PABOOST;
#else
    return RF_PACONFIG_PASELECT_RFO;
#endif
}

/*
 *
 */
void SX1276SetAntSwLowPower( bool status )
{
    if( RadioIsActive != status )
    {
        RadioIsActive = status;
        if( status == false )
        {
			HwTimer_TC2_Init( HW_TIMER_TC2_RFXTAL );
            SX1276AntSwInit( );
        }
        else
        {
			HwTimer_TC2_Init( HW_TIMER_TC2_GCLK0 );
            SX1276AntSwDeInit( );
        }
    }
}

/*
 *
 */
void SX1276AntSwInit( void )
{
#if RADIO_ANT_SWITCH != PIN_NC
    GpioInit( &AntSwitch, RADIO_ANT_SWITCH, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
#endif
#if RADIO_BAND_SEL != PIN_NC
	GpioInit( &BandSel, RADIO_BAND_SEL, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
#endif
}

/*
 *
 */
void SX1276AntSwDeInit( void )
{
#if RADIO_ANT_SWITCH != PIN_NC
	GpioInit( &AntSwitch, RADIO_ANT_SWITCH, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
#endif
#if RADIO_BAND_SEL != PIN_NC
	GpioInit( &BandSel, RADIO_BAND_SEL, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );
#endif
}

/*
 *
 */
void SX1276SetAntSw( uint8_t opMode )
{
#if RADIO_ANT_SWITCH != PIN_NC
    switch( opMode )
    {
    case RFLR_OPMODE_TRANSMITTER:
		GpioWrite( &AntSwitch, 1 );
        break;
    case RFLR_OPMODE_RECEIVER:
    case RFLR_OPMODE_RECEIVER_SINGLE:
    case RFLR_OPMODE_CAD:
    default:
		GpioWrite( &AntSwitch, 0 );
        break;
    }
#endif
#if RADIO_BAND_SEL != PIN_NC
	GpioWrite( &BandSel, ((opMode == RF_OPMODE_SLEEP) ? 0 : 1) );
#endif
}

/*
 *
 */
bool SX1276CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

/*
 *
 */
__attribute__((weak)) int8_t SX1276GetOffsetTxPower( int8_t power )
{
	return power;
}

/*
 *
 */
__attribute__((weak)) uint32_t SX1276GetDriftFreq( uint32_t freq )
{
	return freq;
}