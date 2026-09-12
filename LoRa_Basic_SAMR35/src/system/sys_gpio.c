/*!
 * \file      sys_gpio.c
 *
 * \brief     GPIO driver implementation
 *
 * @details
 *          Relies on the specific board GPIO implementation as well as on
 *          IO expander driver implementation if one is available on the target
 *          board.
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
#include "sys_gpio.h"

/* Include the following functions in "sys_gpio.h" as inline functions */
#if SYS_GPIO_WRAPPER_IS_INLINE == 0
void GpioInit( Gpio_t *obj, PinNames pin, PinModes mode, PinTypes type, uint8_t value )
{
	GpioMcuInit( obj, pin, mode, type, value );
}

void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    GpioMcuSetInterrupt( obj, irqMode, irqPriority, irqHandler );
}

void GpioRemoveInterrupt( Gpio_t *obj )
{
    GpioMcuRemoveInterrupt( obj );
}

void GpioWrite( Gpio_t *obj, uint8_t value )
{
    GpioMcuWrite( obj, value );
}

void GpioToggle( Gpio_t *obj )
{
    GpioMcuToggle( obj );
}

uint8_t GpioRead( Gpio_t *obj )
{
    return GpioMcuRead( obj );
}

void GpioSetPull( Gpio_t *obj, PinTypes pull )
{
	GpioMcuSetPull( obj, pull );
}
#endif