/*!
 * @file    trace_log.h
 * @brief   HeaderFile of Trace logging module for debug
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
#ifndef __TRACE_LOG_H__
#define __TRACE_LOG_H__

#define _LOG_ENABLE_		0

#if _LOG_ENABLE_
#define LOG_BUFF_SIZE		32
enum trace_log_id
{
	LOG_NONE = 0,
	LOG_REQ   	,
	LOG_TXDONE	,
	LOG_RXDONE	,
	LOG_MACCHK	,
	LOG_WIN1	,
	LOG_WIN2	,
	LOG_ACKT	,
};
void TraceLog( enum trace_log_id id, uint32_t value );
#define TRACE_LOG( id, value )	TraceLog( id, (uint32_t)value )
#define MAC_LOG_VAL		((LoRaMacState<<8)|LoRaMacFlags.Value)
#else
#define	TRACE_LOG( id, value )
#endif

#endif


