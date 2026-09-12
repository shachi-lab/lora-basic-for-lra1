/*!
 * \file      uart-board.h
 *
 * \brief     Target board UART driver implementation
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
#ifndef __UART_BOARD_H__
#define __UART_BOARD_H__

#include <stdint.h>
#include "sys_uart.h"

#define UART_BAUDRATE_DEFAULT		115200
#define UART_BAUDRATE_MAX			115200
#define UART_BAUDRATE_MIN			300

#define	UART_BAUD_OPT_BITS			0x3f
#define	UART_BAUD_OPT_PARITY_NONE	0x00
#define	UART_BAUD_OPT_PARITY_ODD	0x01
#define	UART_BAUD_OPT_PARITY_EVEN	0x02
#define	UART_BAUD_OPT_PARITY_BITS	0x03
#define	UART_BAUD_OPT_STOP_1		0x00
#define	UART_BAUD_OPT_STOP_2		0x04
#define	UART_BAUD_OPT_SIZE_8		0x00
#define	UART_BAUD_OPT_SIZE_7		0x08
#define	UART_BAUD_OPT_RX_PULL_DOWN	0x10
#define	UART_BAUD_OPT_TX_PULL_UP	0x20
#define	UART_BAUD_OPT_PULL_BITS		0x30

extern Uart_t Uart_obj[];

#define Uart1	Uart_obj[UART_1]
#define Uart2	Uart_obj[UART_2]

/*!
 * \brief Initializes the UART object and MCU peripheral
 *
 * \param [IN] obj    UART object
 * \param [IN] uartId UART ID
 */
void UartMcuInit( Uart_t *obj, UartId_t uartId );

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
 void UartMcuConfig( Uart_t *obj, uint32_t baudrate );

/*!
 * \brief DeInitializes the UART object and MCU pins
 *
 * \param [IN] obj  UART object
 */
void UartMcuDeInit( Uart_t *obj );

/*!
 * \brief Sends a character to the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Character to be sent
 * \retval status    [0: OK, 1: Busy]
 */
uint8_t UartMcuPutChar( Uart_t *obj, uint8_t data );

/*!
 * \brief Sends a buffer to the UART
 *
 * \param [IN] obj    UART object
 * \param [IN] buffer Buffer to be sent
 * \param [IN] size   Buffer size
 * \retval status     [0: OK, 1: Busy]
 */
uint8_t UartMcuPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size );

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Received character
 * \retval status    [0: OK, 1: Busy]
 */
uint8_t UartMcuGetChar( Uart_t *obj, uint8_t *data );

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj          UART object
 * \param [IN] buffer       Received buffer
 * \param [IN] size         Number of bytes to be received
 * \param [OUT] nbReadBytes Number of bytes really read
 * \retval status           [0: OK, 1: Busy]
 */
uint8_t UartMcuGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size, uint16_t *nbReadBytes );

uint8_t UartMcuIsBreak( Uart_t *obj );
void UartMcuEnable( Uart_t *obj );
void UartMcuDisable( Uart_t *obj );
void UartMcuWaitTxEmpty( Uart_t *obj );
void UartMcuFlush( Uart_t *obj );
uint16_t UartMcuTxBuffLen( Uart_t *obj );
uint16_t UartMcuRxBuffLen( Uart_t *obj );

#endif // __UART_BOARD_H__
