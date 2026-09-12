/*!
 * \file      sys_gpio.h
 *
 * \brief     HeaderFile of GPIO driver implementation
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
#ifndef __GPIO_H__
#define __GPIO_H__

#include "gpio-board.h"

#define SYS_GPIO_WRAPPER_IS_INLINE		1

/*!
 * \brief Initializes the given GPIO object
 *
 * \param [IN] obj    Pointer to the GPIO object
 * \param [IN] pin    Pin name ( please look in pinName-board.h file )
 * \param [IN] mode   Pin mode [PIN_INPUT, PIN_OUTPUT,
 *                              PIN_ALTERNATE_FCT, PIN_ANALOGIC]
 * \param [IN] type   Pin type [PIN_NO_PULL, PIN_PULL_UP, PIN_PULL_DOWN]
 * \param [IN] value  Default output value at initialization
 */
inline void GpioInit( Gpio_t *obj, PinNames pin, PinModes mode, PinTypes type, uint8_t value )
{
	GpioMcuInit( obj, pin, mode, type, value );
}

/*!
 * \brief GPIO IRQ Initialization
 *
 * \param [IN] obj         Pointer to the GPIO object
 * \param [IN] irqMode     IRQ mode [NO_IRQ, IRQ_RISING_EDGE,
 *                                   IRQ_FALLING_EDGE, IRQ_RISING_FALLING_EDGE]
 * \param [IN] irqPriority IRQ priority [IRQ_VERY_LOW_PRIORITY, IRQ_LOW_PRIORITY
 *                                       IRQ_MEDIUM_PRIORITY, IRQ_HIGH_PRIORITY
 *                                       IRQ_VERY_HIGH_PRIORITY]
 * \param [IN] irqHandler  Callback function pointer
 */
inline void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
	GpioMcuSetInterrupt( obj, irqMode, irqPriority, irqHandler );
}

/*!
 * \brief Removes the interrupt from the object
 *
 * \param [IN] obj Pointer to the GPIO object
 */
inline void GpioRemoveInterrupt( Gpio_t *obj )
{
	GpioMcuRemoveInterrupt( obj );
}

/*!
 * \brief Writes the given value to the GPIO output
 *
 * \param [IN] obj   Pointer to the GPIO object
 * \param [IN] value New GPIO output value
 */
inline void GpioWrite( Gpio_t *obj, uint8_t value )
{
	GpioMcuWrite( obj, value );
}

/*!
 * \brief Toggle the value to the GPIO output
 *
 * \param [IN] obj   Pointer to the GPIO object
 */
inline void GpioToggle( Gpio_t *obj )
{
	GpioMcuToggle( obj );
}

/*!
 * \brief Reads the current GPIO input value
 *
 * \param [IN] obj Pointer to the GPIO object
 * \retval value   Current GPIO input value
 */
inline uint8_t GpioRead( Gpio_t *obj )
{
	return GpioMcuRead( obj );
}

/*!
 * \brief Set Port pull-up/down
 * \param [IN] obj Pointer to the GPIO object
 * \param [IN] pull   Port pin input pull [PIN_NO_PULL, PIN_PULL_UP, PIN_PULL_DOWN]
 */
inline void GpioSetPull( Gpio_t *obj, PinTypes pull )
{
	GpioMcuSetPull( obj, pull );
}

/*!
 * \brief Get Port Muxen bit
 * \param [IN] obj Pointer to the GPIO object
 * \retval value   Current GPIO Muxen bit value
 */
inline bool GpioIsMuxEn( Gpio_t *obj )
{
	return GpioMcuIsMuxEn( obj );
}

#endif // __GPIO_H__
