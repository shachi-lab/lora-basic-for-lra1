/*!
 * \file      delay-board.h
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
#ifndef __DELAY_BOARD_H__
#define __DELAY_BOARD_H__

#include <stdint.h>

/*!
 * \brief Blocking delay of "sec" seconds
 *
 * \param [IN] sec    delay in seconds
 */
void DelaySecMcu( uint32_t sec );

/*!
 * \brief Blocking delay of "ms" milliseconds
 *
 * \param [IN] ms    delay in milliseconds
 */
void DelayMsMcu( uint32_t ms );

/*!
 * \brief Blocking delay of "us" microseconds
 *
 * \param [IN] us    delay in microseconds
 */
void DelayUsMcu( uint32_t us );

#endif // __DELAY_BOARD_H__
