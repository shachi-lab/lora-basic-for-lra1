/*!
 * \file      gpio-board.h
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
#ifndef __GPIO_BOARD_H__
#define __GPIO_BOARD_H__

#include "pinName-board.h"
//#include "pinName-ioe.h"

/*!
 * Board GPIO pin names
 */
typedef enum
{
    MCU_PINS,
    IOE_PINS,
    // Not connected
    NC = PIN_NC
}PinNames;


/*!
 * Operation Mode for the GPIO
 */
typedef enum
{
    PIN_INPUT = 0,
    PIN_OUTPUT,
    PIN_ALTERNATE_FCT,
    PIN_ANALOGIC
}PinModes;

/*!
 * Add a pull-up, a pull-down or nothing on the GPIO line
 */
typedef enum
{
    PIN_NO_PULL = 0,
    PIN_PULL_UP,
    PIN_PULL_DOWN
}PinTypes;

/*!
 * Define the GPIO as Push-pull type or Open Drain
 */
typedef enum
{
    PIN_PUSH_PULL = 0,
    PIN_OPEN_DRAIN
}PinConfigs;

/*!
 * Define the GPIO IRQ on a rising, falling or both edges
 */
typedef enum
{
    NO_IRQ = 0,
    IRQ_RISING_EDGE,
    IRQ_FALLING_EDGE,
    IRQ_RISING_FALLING_EDGE,
	IRQ_HIGH_LEVEL,
	IRQ_LOW_LEVEL
}IrqModes;

/*!
 * Define the IRQ priority on the GPIO
 */
typedef enum
{
    IRQ_VERY_LOW_PRIORITY = 0,
    IRQ_LOW_PRIORITY,
    IRQ_MEDIUM_PRIORITY,
    IRQ_HIGH_PRIORITY,
    IRQ_VERY_HIGH_PRIORITY
}IrqPriorities;

/*!
 * Structure for the GPIO
 */
typedef struct
{
    PinNames pin;
    PinTypes pull;
    uint16_t pinIndex;
    uint32_t pinMask;
    PortGroup *port;
}Gpio_t;

/*!
 * GPIO IRQ handler function prototype
 */
typedef void( GpioIrqHandler )( void );

/*!
 * GPIO Expander IRQ handler function prototype
 */
typedef void( GpioIoeIrqHandler )( void );

/*!
 * \brief Initializes the given GPIO object
 *
 * \param [IN] obj    Pointer to the GPIO object
 * \param [IN] pin    Pin name ( please look in pinName-board.h file )
 * \param [IN] mode   Pin mode [PIN_INPUT, PIN_OUTPUT,
 *                              PIN_ALTERNATE_FCT, PIN_ANALOGIC]
 * \param [IN] config Pin config [PIN_PUSH_PULL, PIN_OPEN_DRAIN]
 * \param [IN] type   Pin type [PIN_NO_PULL, PIN_PULL_UP, PIN_PULL_DOWN]
 * \param [IN] value  Default output value at initialization
 */
void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinTypes type, uint32_t value );

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
void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler );

/*!
 * \brief Removes the interrupt from the object
 *
 * \param [IN] obj Pointer to the GPIO object
 */
void GpioMcuRemoveInterrupt( Gpio_t *obj );

/*!
 * \brief Writes the given value to the GPIO output
 *
 * \param [IN] obj   Pointer to the GPIO object
 * \param [IN] value New GPIO output value
 */
void GpioMcuWrite( Gpio_t *obj, uint32_t value );

/*!
 * \brief Toggle the value to the GPIO output
 *
 * \param [IN] obj   Pointer to the GPIO object
 */
void GpioMcuToggle( Gpio_t *obj );

/*!
 * \brief Reads the current GPIO input value
 *
 * \param [IN] obj Pointer to the GPIO object
 * \retval value   Current GPIO input value
 */
uint32_t GpioMcuRead( Gpio_t *obj );

/*!
 * \brief Interrupt handler Common
 *
 */
void GpioMcuIsr_Common( uint8_t index, uint8_t offset );

/*!
 * \brief Set Port pull-up/down
 * \param [IN] obj Pointer to the GPIO object
 * \param [IN] pull   Port pin input pull [PIN_NO_PULL, PIN_PULL_UP, PIN_PULL_DOWN]
 */
void GpioMcuSetPull( Gpio_t *obj, PinTypes pull );

/*!
 * \brief Get Port Muxen bit
 * \param [IN] obj Pointer to the GPIO object
 * \retval value   Current GPIO Muxen bit value
 */
bool GpioMcuIsMuxEn( Gpio_t *obj );

/*!
 * \brief Set Port default settings
 * \param [IN] force  Forcibly set default even if it has already been initialized
 */
void GpioMcuSetDefault( bool force );

void GpioMcuSetObj( Gpio_t *obj, PinNames pin );

extern uint8_t const GpioMcu_PinList[];

#endif // __GPIO_BOARD_H__
