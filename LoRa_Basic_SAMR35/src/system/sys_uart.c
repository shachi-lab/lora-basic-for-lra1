/*!
 * \file      uart.c
 *
 * \brief     UART driver implementation
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
#include "uart-board.h"
#include "sys_uart.h"

#if SYS_UART_WRAPPER_IS_FUNCTION
void UartInit( Uart_t *obj, UartId_t uartId )
{
	UartMcuInit( obj, uartId );
}

void UartConfig( Uart_t *obj, uint32_t baudrate )
{
    UartMcuConfig( obj, baudrate );
}

void UartDeInit( Uart_t *obj )
{
    UartMcuDeInit( obj );
}

uint8_t UartPutChar( Uart_t *obj, uint8_t data )
{
     return UartMcuPutChar( obj, data );
}

uint8_t UartGetChar( Uart_t *obj, uint8_t *data )
{
    return UartMcuGetChar( obj, data );
}

uint8_t UartPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size )
{
    return UartMcuPutBuffer( obj, buffer, size );
}

uint8_t UartGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size, uint16_t *nbReadBytes )
{
    return UartMcuGetBuffer( obj, buffer, size, nbReadBytes );
}

void UartSetBaudrate( Uart_t *obj, uint32_t baudrate )
{
	if ( obj->Baudrate != baudrate )
	{
		UartMcuConfig( obj, baudrate );
	}
}

#endif