/*!
 * @file    trace_log.c
 * @brief   Trace logging module for debug
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
#include <asf.h>
#include "trace_log.h"

#if _LOG_ENABLE_

struct trace_log
{
	enum trace_log_id id;
	uint16_t	time;
	uint32_t	val;
};

struct trace_log LogBuff[LOG_BUFF_SIZE];
uint8_t LogCnt = 0;
extern volatile uint32_t HwTimer_SysTick_Count;
/*
 *
 */
void TraceLog( enum trace_log_id id, uint32_t value )
{
	if( LogCnt >= LOG_BUFF_SIZE ) return;
	LogBuff[LogCnt].id = id;
	LogBuff[LogCnt].time = (uint16_t)HwTimer_SysTick_Count;
	LogBuff[LogCnt].val = value;
	LogCnt++;
}
#endif
