/*!
 * \file      sys_delay.h
 *
 * \brief     HeaderFile of Delay implementation
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
#ifndef __DELAY_H__
#define __DELAY_H__

#include <stdint.h>
#include "delay-board.h"

/*! 
 * Blocking delay of "sec" seconds
 */
inline void DelaySec( uint32_t sec ) { DelaySecMcu( sec ); }

/*! 
 * Blocking delay of "ms" milliseconds
 */
inline void DelayMs( uint32_t ms ) { DelayMsMcu( ms ); }

/*! 
 * Blocking delay of "us" microseconds
 */
inline void DelayUs( uint32_t us ) { DelayUsMcu( us ); }

#endif // __DELAY_H__

