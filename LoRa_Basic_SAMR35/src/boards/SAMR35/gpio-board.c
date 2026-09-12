/*!
 * \file      gpio-board.c
 *
 * \brief     Target board GPIO driver implementation
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
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_fifo.h"
#include "sys_spi.h"
#include "sys_timer.h"
#include "sys_uart.h"
#include "board-config.h"
#include "rtc-board.h"
#include "gpio-board.h"

inline bool _GpioMcuIsObjInvalid( Gpio_t *obj ){ return (obj == NULL || obj->pin == NC ); }

/*
 *
 */
void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinTypes type, uint32_t value )
{
	if( pin == NC )	return;

	Gpio_t obj_temp;
	if ( obj == NULL )
	{
		obj = &obj_temp;
	}

	GpioMcuSetObj( obj, pin );
	obj->pull = type;

	struct system_pinmux_config pin_conf;
	pin_conf.mux_position = SYSTEM_PINMUX_GPIO;
	pin_conf.direction = PORT_PIN_DIR_OUTPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	pin_conf.powersave = false;

	if( mode == PIN_ANALOGIC )	pin_conf.powersave = true;
	if( mode == PIN_INPUT )
	{
		pin_conf.direction = PORT_PIN_DIR_INPUT;
		switch( type )
		{
			case PIN_PULL_UP   : pin_conf.input_pull = PORT_PIN_PULL_UP; break;
			case PIN_PULL_DOWN : pin_conf.input_pull = PORT_PIN_PULL_DOWN; break;
			default : break;
		}
	}
	system_pinmux_pin_set_config( pin, &pin_conf );

	// Sets initial output value
	if( mode == PIN_OUTPUT )
	{
//		system_pinmux_pin_set_output_strength(pin,SYSTEM_PINMUX_PIN_STRENGTH_HIGH);
//		port_pin_set_output_level( pin, value );
		obj->port->PINCFG[obj->pinIndex].bit.DRVSTR = 1;
		GpioMcuWrite( obj, value );
	}
}

/*
 *
 */
void GpioMcuSetObj( Gpio_t *obj, PinNames pin )
{
	obj->pin = pin;
	obj->port = system_pinmux_get_group_from_gpio_pin( pin );
	obj->pinIndex = (pin % 32);
	obj->pinMask  = (1UL << (pin % 32));
}

/*
 *
 */
void GpioMcuSetPull( Gpio_t *obj, PinTypes pull )
{	
	if ( _GpioMcuIsObjInvalid( obj ) ) return;

	obj->pull = pull;
	obj->port->DIRCLR.reg = obj->pinMask;
	if ( pull == PIN_NO_PULL )
	{
		obj->port->PINCFG[obj->pinIndex].bit.PULLEN = 0;
	}
	else
	{
		obj->port->PINCFG[obj->pinIndex].bit.PULLEN = 1;
		if( pull == PIN_PULL_UP ) {
			obj->port->OUTSET.reg = obj->pinMask;
		} else {
			obj->port->OUTCLR.reg = obj->pinMask;
		}
	}
}

/*
 *
 */
bool GpioMcuIsMuxEn( Gpio_t *obj )
{
	if( _GpioMcuIsObjInvalid( obj )	) return false;
	return obj->port->PINCFG[obj->pinIndex].bit.PMUXEN;
}

/*
 *
 */
void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{

}

/*
 *
 */
void GpioMcuRemoveInterrupt( Gpio_t *obj )
{
	//ext_irq_register( obj->pin, NULL );
}

/*
 *
 */
void GpioMcuWrite( Gpio_t *obj, uint32_t value )
{
	if( _GpioMcuIsObjInvalid( obj )	) return;
	/* Set the pin to high or low atomically based on the requested level */
	if( value )
	{
		obj->port->OUTSET.reg = obj->pinMask;
	} else {
		obj->port->OUTCLR.reg = obj->pinMask;
	}
}

/*
 *
 */
void GpioMcuToggle( Gpio_t *obj )
{
	if( _GpioMcuIsObjInvalid( obj )	) return;
	obj->port->OUTTGL.reg = obj->pinMask;
}

/*
 *
 */
uint32_t GpioMcuRead( Gpio_t *obj )
{
	if( _GpioMcuIsObjInvalid( obj )	) return 0;
	return (obj->port->IN.reg & obj->pinMask);
}

/*!
 * \brief Interrupt handler Common
 *
 */
void GpioMcuIsr_Common( uint8_t index, uint8_t offset )
{
#if !defined( USE_NO_TIMER )
//  RtcRecoverMcuStatus( );
#endif
}

/*
 *
 */
uint8_t const GpioMcu_PinList[] =
{
	PIN_PA00, PIN_PA01,					// XOSC32
	PIN_PA04, PIN_PA05,					// UART-TX,RX
	PIN_PA06,							// WAKEUP
	PIN_PA07, PIN_PA08,	PIN_PA09, PIN_PA13,
	PIN_PA14,							// ACTIVE_IND
	PIN_PA15, 
	PIN_PA16, PIN_PA17,					// I2C
	PIN_PA18, PIN_PA19,					// LED
	PIN_PA22, PIN_PA23, PIN_PA24, PIN_PA25, PIN_PA27, PIN_PA28,
	PIN_PA30, PIN_PA31,					//  SWDCLK,SWDIO
	PIN_PB02, PIN_PB03,	PIN_PB22, PIN_PB23,
	PIN_MAX + 1
};

/*!
 * \brief Set Port default settings
 * \param [IN] force  Forcibly set default even if it has already been initialized
 */
void GpioMcuSetDefault( bool force )
{
	static bool initialized = false;

	if( initialized && (force == false))	return;
	initialized = true;

	struct system_pinmux_config pin_conf;
//	system_pinmux_get_config_defaults( &pin_conf );
	pin_conf.powersave = true;
	for( uint8_t const *p = GpioMcu_PinList; *p <= PIN_MAX; p++ )
	{
		system_pinmux_pin_set_config( *p, &pin_conf );
	}
}
