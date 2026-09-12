/*!
 * \file      delay-board.c
 *
 * \brief     Target board delay implementation
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
#include "delay-board.h"
#include "cycle_counter.h"

/*!
 * \brief Blocking delay of "sec" seconds
 *
 * \param [IN] ms    delay in seconds
 */
void DelaySecMcu( uint32_t sec )
{
	cpu_delay_s( sec );
}

/*!
 * \brief Blocking delay of "ms" milliseconds
 *
 * \param [IN] ms    delay in milliseconds
 */
void DelayMsMcu( uint32_t ms )
{
	cpu_delay_ms( ms );
}

/*!
 * \brief Blocking delay of "us" microseconds
 *
 * \param [IN] us    delay in microseconds
 */
void DelayUsMcu( uint32_t us )
{
	cpu_delay_us( us );
}
