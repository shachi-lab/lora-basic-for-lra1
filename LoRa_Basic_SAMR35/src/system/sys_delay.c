/*!
 * \file      sys_delay.c
 *
 * \brief     Delay implementation
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
#include "delay-board.h"
#include "sys_delay.h"

/*
 * The following functions have been moved inline
 */
#if 0

/*! 
 * Blocking delay of "s" seconds
 */
void Delay( float s )
{
    DelayMs( s * 1000.0f );
}

/*! 
 * Blocking delay of "ms" milliseconds
 */
void DelayMs( uint32_t ms )
{
    DelayMsMcu( ms );
}

/*! 
 * Blocking delay of "us" microseconds
 */
inline void DelayUs( uint32_t us )
{
	DelayUsMcu( us );
}
#endif

