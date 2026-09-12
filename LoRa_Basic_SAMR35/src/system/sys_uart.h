/*!
 * \file      uart.h
 *
 * \brief     HeaderFile of UART driver implementation
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
#ifndef __UART_H__
#define __UART_H__

#include "sys_fifo.h"
#include "sys_gpio.h"

/*!
 * UART peripheral ID
 */
typedef enum
{
    UART_1 = 0,
    UART_2 = 1,
}UartId_t;

typedef enum
{
	IS_BREAK_NONE  = 0,
	IS_BREAK_BREAK = 1,
	IS_BREAK_CHAR  = 2,
}UartIsBreak_t;

/*!
 * UART object type definition
 */
typedef struct
{
    UartId_t UartId;
	struct{
    bool IsInitialized : 1;
	bool IsSendData    : 1;
	bool BrkDisable    : 1;
	bool IsGpioTxInit  : 1;
	bool IsGpioRxInit  : 1;
	};
	UartIsBreak_t IsBreak;
	uint8_t BreakChar;
	uint8_t	Options;
	uint32_t Baudrate;
    Gpio_t Tx;
    Gpio_t Rx;
	uint8_t TxData;
	uint8_t RxData;
    Fifo_t TxFifo;
    Fifo_t RxFifo;
	struct usart_module *Instance;
}Uart_t;

#define	SYS_UART_WRAPPER_IS_FUNCTION	0

/*!
 * \brief Initializes the UART object and MCU peripheral
 *
 * \param [IN] obj    UART object
 * \param [IN] uartId UART ID
 */
#if	SYS_UART_WRAPPER_IS_FUNCTION
void UartInit( Uart_t *obj, UartId_t uartId );
#else
#define UartInit( obj, uartId )		UartMcuInit( obj, uartId )
#endif

/*!
 * \brief Configures the UART object and MCU peripheral
 *
 * \remark UartInit function must be called first.
 *
 * \param [IN] obj          UART object
 * \param [IN] baudrate     UART baudrate
 * \                        stop bits     (baudrate bit-2)
 * \                        packet parity (baudrate bit-0,1)
 */
#if	SYS_UART_WRAPPER_IS_FUNCTION
void UartConfig( Uart_t *obj, uint32_t baudrate );
#else
#define	UartConfig( obj, baudrate )	UartMcuConfig( obj, baudrate )
#endif

/*!
 * \brief DeInitializes the UART object and MCU pins
 *
 * \param [IN] obj  UART object
 */
#if	SYS_UART_WRAPPER_IS_FUNCTION
void UartDeInit( Uart_t *obj );
#else
#define	UartDeInit( obj )	    UartMcuDeInit( obj )
#endif

/*!
 * \brief Sends a character to the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Character to be sent
 * \retval status    [0: OK, 1: Busy]
 */
#if	SYS_UART_WRAPPER_IS_FUNCTION
uint8_t UartPutChar( Uart_t *obj, uint8_t data );
#else
#define	UartPutChar( obj, data )	UartMcuPutChar( obj, data )
#endif

/*!
 * \brief Sends a buffer to the UART
 *
 * \param [IN] obj    UART object
 * \param [IN] buffer Buffer to be sent
 * \param [IN] size   Buffer size
 * \retval status     [0: OK, 1: Busy]
 */
#if	SYS_UART_WRAPPER_IS_FUNCTION
uint8_t UartPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size );
#else
#define	UartPutBuffer( obj, buffer, size )	UartMcuPutBuffer( obj, buffer, size )
#endif

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Received character
 * \retval status    [0: OK, 1: Busy]
 */
#if	SYS_UART_WRAPPER_IS_FUNCTION
uint8_t UartGetChar( Uart_t *obj, uint8_t *data );
#else
#define	UartGetChar( obj, data )	UartMcuGetChar( obj, data )
#endif

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj          UART object
 * \param [IN] buffer       Received buffer
 * \param [IN] size         Number of bytes to be received
 * \param [OUT] nbReadBytes Number of bytes really read
 * \retval status           [0: OK, 1: Busy]
 */
#if	SYS_UART_WRAPPER_IS_FUNCTION
uint8_t UartGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size, uint16_t *nbReadBytes );
#else
#define	UartGetBuffer( obj, buffer, size, nbReadBytes )		UartMcuGetBuffer( obj, buffer, size, nbReadBytes )
#endif

/*!
 * \brief Set Baudrate to the UART
 *
 * \remark UartInit function must be called first.
 *
 * \param [IN] obj          UART object
 * \param [IN] baudrate     UART baudrate
 * \                        stop bits     (baudrate 2bit)
 * \                        packet parity (baudrate 0,1bit)
 */
#if	SYS_UART_WRAPPER_IS_FUNCTION
void UartSetBaudrate( Uart_t *obj, uint32_t baudrate );
#else
#define UartSetBaudrate( obj, baudrate )	if ( (obj)->Baudrate != baudrate ) { UartMcuConfig( obj, baudrate ); }
#endif

#endif // __UART_H__
