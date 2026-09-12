/*!
 * \file      rtc-board.h
 *
 * \brief     Target board RTC timer and low power modes management
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
#ifndef __RTC_BOARD_H__
#define __RTC_BOARD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "sys_timer.h"

/*!
 * \brief Temperature coefficient of the clock source
 */
#define RTC_TEMP_COEFFICIENT                            ( -0.035f )

/*!
 * \brief Temperature coefficient deviation of the clock source
 */
#define RTC_TEMP_DEV_COEFFICIENT                        ( 0.0035f )

/*!
 * \brief Turnover temperature of the clock source
 */
#define RTC_TEMP_TURNOVER                               ( 25.0f )

/*!
 * \brief Turnover temperature deviation of the clock source
 */
#define RTC_TEMP_DEV_TURNOVER                           ( 5.0f )


#define RTC_BACKUP_REG(n)		(RTC->MODE0.GP[n].reg)
#define RTC_WAIT_SYNCBUSY()		while( RTC->MODE0.SYNCBUSY.reg )

/*!
 * \brief Initializes the RTC timer
 *
 * \remark The timer is based on the RTC
 */
void RtcInit( void );

/*!
 * \brief Returns the minimum timeout value
 *
 * \retval minTimeout Minimum timeout value in in ticks
 */
uint32_t RtcGetMinimumTimeout( void );

/*!
 * \brief converts time in ms to time in ticks
 *
 * \param[IN] milliseconds Time in milliseconds
 * \retval returns time in timer ticks
 */
uint32_t RtcMs2Tick( TimerTime_t milliseconds );

/*!
 * \brief converts time in ticks to time in ms
 *
 * \param[IN] time in timer ticks
 * \retval returns time in milliseconds
 */
TimerTime_t RtcTick2Ms( uint32_t tick );

/*!
 * \brief converts time in ticks to time in sec
 *
 * \param[IN] time in timer ticks
 * \retval returns time in seconds
 */
uint32_t RtcTick2Sec( uint32_t tick );

/*!
 * \brief Performs a delay of milliseconds by polling RTC
 *
 * \param[IN] milliseconds Delay in ms
 */
void RtcDelayMs( TimerTime_t milliseconds );

/*!
 * \brief Calculates the wake up time between wake up and MCU start
 *
 * \note Resolution in RTC_ALARM_TIME_BASE
 */
void RtcSetMcuWakeUpTime( void );

/*!
 * \brief Returns the wake up time in ticks
 *
 * \retval wakeUpTime The WakeUpTime value in ticks
 */
int16_t RtcGetMcuWakeUpTime( void );

/*!
 * \brief Sets the alarm
 *
 * \note The alarm is set at now (read in this funtion) + timeout
 *
 * \param timeout [IN] Duration of the Timer ticks
 */
void RtcSetAlarm( uint32_t timeout );

/*!
 * \brief Stops the Alarm
 */
void RtcStopAlarm( void );

/*!
 * \brief Starts wake up alarm
 *
 * \note  Alarm in RtcTimerContext.Time + timeout
 *
 * \param [IN] timeout Timeout value in ticks
 */
void RtcStartAlarm( uint32_t timeout );

/*!
 * \brief Sets the RTC timer reference
 *
 * \retval value Timer reference value in ticks
 */
uint32_t RtcSetTimerContext( void );
  
/*!
 * \brief Gets the RTC timer reference
 *
 * \retval value Timer value in ticks
 */
uint32_t RtcGetTimerContext( void );

/*!
 * \brief Gets the system time with the number of seconds elapsed since epoch
 *
 * \param [OUT] milliseconds Number of milliseconds elapsed since epoch
 * \retval seconds Number of seconds elapsed since epoch
 */
uint32_t RtcGetCalendarTime( uint16_t *milliseconds );

/*!
 * \brief Sets the system time with the number of seconds elapsed since epoch
 *
 * \param [IN] seconds Number of seconds elapsed since epoch
 * \param [IN] sub_seconds Number of 1/1024 seconds
 */
void RtcSetCalendarTime( uint32_t seconds, uint16_t sub_seconds );

/*!
 * \brief Get the RTC timer value
 *
 * \retval RTC Timer value
 */
uint32_t RtcGetTimerValue( void );

/*!
 * \brief Get the RTC timer elapsed time since the last Alarm was set
 *
 * \retval RTC Elapsed time since the last alarm in ticks.
 */
uint32_t RtcGetTimerElapsedTime( void );

/*!
 * \brief Writes data0 and data1 to the RTC backup registers
 *
 * \param [IN] data0 1st Data to be written
 * \param [IN] data1 2nd Data to be written
 */
void RtcBkupWrite( uint32_t data0, uint32_t data1 );

/*!
 * \brief Reads data0 and data1 from the RTC backup registers
 *
 * \param [OUT] data0 1st Data to be read
 * \param [OUT] data1 2nd Data to be read
 */
void RtcBkupRead( uint32_t* data0, uint32_t* data1 );

/*!
 * \brief Processes pending timer events
 */
bool RtcProcess( void );

/*!
 * \brief Computes the temperature compensation for a period of time on a
 *        specific temperature.
 *
 * \param [IN] period Time period to compensate in milliseconds
 * \param [IN] temperature Current temperature
 *
 * \retval Compensated time period
 */
TimerTime_t RtcTempCompensation( TimerTime_t period, float temperature );

bool RtcIsTimeoutPendingPolling( void );
void RtcSetTimeoutPendingPolling( void );

#ifdef __cplusplus
}
#endif

#endif // __RTC_BOARD_H__
