/*!
 * \file      rtc-board.c
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
//#include <hal_init.h>
#include <hw_timer.h>
#include "board.h"
#include "board-config.h"
#include "board-mcu.h"
#include "sys_timer.h"
#include "sys_systime.h"
#include "sys_gpio.h"
#include "utilities.h"
#include "rtc-board.h"

#define RTC_MILISEC_SHIFT			10
#define RTC_OVERTICK_SHIFT			(32 - RTC_MILISEC_SHIFT)
#define RTC_BACKUP_MILISEC			RTC_BACKUP_REG(2)
#define RTC_BACKUP_SECONDS			RTC_BACKUP_REG(3)
#define RTC_WAIT_SYNCBUSY()			while( RTC->MODE0.SYNCBUSY.reg )

/*!
 * \brief Indicates if the RTC is already Initialized or not
 */
static bool RtcInitialized = false;
static volatile bool RtcTimeoutPendingInterrupt = false;
static volatile bool RtcTimeoutPendingPolling = false;

typedef enum AlarmStates_e
{
    ALARM_STOPPED = 0,
    ALARM_RUNNING = !ALARM_STOPPED
} AlarmStates_t;

/*!
 * RTC timer context 
 */
typedef struct
{
    uint32_t Time;  // Reference time
    uint32_t Delay; // Reference Timeout duration
    uint32_t AlarmState;
}RtcTimerContext_t;

/*!
 * Keep the value of the RTC timer when the RTC alarm is set
 * Set with the \ref RtcSetTimerContext function
 * Value is kept as a Reference to calculate alarm
 */
static RtcTimerContext_t RtcTimerContext;


/*!
 * Used to store the Seconds and SubSeconds.
 * 
 * WARNING: Temporary fix fix. Should use MCU NVM internal
 *          registers
 */
uint32_t RtcBkupRegisters[] = { 0, 0 };

/*!
 * \brief Callback for the hw_timer when alarm expired
 */
static void RtcAlarmIrq( void );

/*!
 * \brief Callback for the hw_timer when counter overflows
 */
static void RtcOverflowIrq( void );

void RtcInit( void )
{
    if( RtcInitialized == false )
    {
        // RTC timer
        HwTimerInit( );
        HwTimerAlarmSetCallback( RtcAlarmIrq );
        HwTimerOverflowSetCallback( RtcOverflowIrq );

        RtcTimerContext.AlarmState = ALARM_STOPPED;
        RtcSetTimerContext( );
        RtcInitialized = true;
    }
}

uint32_t RtcSetTimerContext( void )
{
    RtcTimerContext.Time = ( uint32_t )HwTimerGetTime( );
    return ( uint32_t )RtcTimerContext.Time;
}

uint32_t RtcGetTimerContext( void )
{
    return RtcTimerContext.Time;
}

uint32_t RtcGetMinimumTimeout( void )
{
    return HwTImerGetMinimumTimeout();
}

#if 0
uint32_t RtcMs2Tick( TimerTime_t milliseconds )
{
//  tick = (time * 1024) / 1000
    uint32_t seconds = milliseconds / 1000;
	milliseconds = milliseconds % 1000;
	return ( seconds << RTC_MILISEC_SHIFT ) + ( (milliseconds << RTC_MILISEC_SHIFT) / 1000 );
}

TimerTime_t RtcTick2Ms( uint32_t tick )
{
//  time = (tick * 1000) / 1024
    uint32_t seconds = tick >> RTC_MILISEC_SHIFT;
    tick = tick & 0x3FF;
    return ( ( seconds * 1000 ) + (( tick * 1000 ) >> RTC_MILISEC_SHIFT) );
}

uint32_t RtcTick2Sec( uint32_t tick )
{
	return tick >> RTC_MILISEC_SHIFT;
}
#endif

void RtcDelayMs( TimerTime_t milliseconds )
{
    uint32_t delayTicks = milliseconds;	// = RtcMs2Tick( milliseconds );
    uint32_t refTicks = RtcGetTimerValue( );

    // Wait delay ms
    while( ( ( RtcGetTimerValue( ) - refTicks ) ) < delayTicks )
    {
        __NOP( );
    }
}

void RtcSetAlarm( uint32_t timeout )
{
    RtcStartAlarm( timeout );
}

void RtcStopAlarm( void )
{
    RtcTimerContext.AlarmState = ALARM_STOPPED;
	HwTimerDisable();
}

void RtcStartAlarm( uint32_t timeout )
{
    CRITICAL_SECTION_BEGIN( );
    RtcStopAlarm( );
 
    RtcTimerContext.Delay = timeout;
 
    RtcTimeoutPendingInterrupt = true;
    RtcTimeoutPendingPolling = false;

    RtcTimerContext.AlarmState = ALARM_RUNNING;
    if( HwTimerLoadAbsoluteTicks( RtcTimerContext.Time + RtcTimerContext.Delay ) == false )
    {
        // If timer already passed
        if( RtcTimeoutPendingInterrupt == true )
        {
            // And interrupt not handled, mark as polling
            RtcTimeoutPendingPolling = true;
            RtcTimeoutPendingInterrupt = false;
        }
    }
    CRITICAL_SECTION_END( );
}

uint32_t RtcGetTimerValue( void )
{
    return (uint32_t)HwTimerGetTime( );
}

uint32_t RtcGetTimerElapsedTime( void )
{
    return (uint32_t)( HwTimerGetTime( ) - RtcTimerContext.Time );
}

uint32_t RtcGetCalendarTime( uint16_t *milliseconds )
{
	uint32_t seconds = HwTimer_RTC_GetSec( milliseconds );
	RTC_WAIT_SYNCBUSY();
	seconds += RTC_BACKUP_SECONDS;
	*milliseconds += RTC_BACKUP_MILISEC;
	if(*milliseconds >= 1000) {
		*milliseconds -= 1000;
		seconds++;
	}
	return seconds;			
}

void RtcSetCalendarTime( uint32_t seconds, uint16_t sub_seconds )
{
	uint16_t rtc_milliseconds;
	uint32_t rtc_seconds = HwTimer_RTC_GetSec( &rtc_milliseconds );
	uint32_t milliseconds = (((uint32_t)sub_seconds) * 1000) >> RTC_MILISEC_SHIFT;
	seconds -= rtc_seconds;
	milliseconds -= rtc_milliseconds;
	if (milliseconds >= 1000) {
		milliseconds += 1000;
		seconds--;
	}
	RTC_BACKUP_SECONDS = seconds;
	RTC_BACKUP_MILISEC = milliseconds;
	RTC_WAIT_SYNCBUSY();
}

void RtcBkupWrite( uint32_t data0, uint32_t data1 )
{
    CRITICAL_SECTION_BEGIN( );
    RtcBkupRegisters[0] = data0;
    RtcBkupRegisters[1] = data1;
    CRITICAL_SECTION_END( );
}

void RtcBkupRead( uint32_t* data0, uint32_t* data1 )
{
    CRITICAL_SECTION_BEGIN( );
    *data0 = RtcBkupRegisters[0];
    *data1 = RtcBkupRegisters[1];
    CRITICAL_SECTION_END( );
}

bool RtcProcess( void )
{
    CRITICAL_SECTION_BEGIN( );

    if( (RtcTimerContext.AlarmState == ALARM_RUNNING) && (RtcTimeoutPendingPolling == true) )
    {
        if( RtcGetTimerElapsedTime( ) >= RtcTimerContext.Delay )
        {
            RtcTimerContext.AlarmState = ALARM_STOPPED;

            // Because of one shot the task will be removed after the callback
            RtcTimeoutPendingPolling = false;
            // NOTE: The handler should take less then 1 ms otherwise the clock shifts
            TimerIrqHandler( );
        }
    }
    CRITICAL_SECTION_END( );
	return RtcTimeoutPendingPolling;
}

TimerTime_t RtcTempCompensation( TimerTime_t period, float temperature )
{
    return period;
}

static void RtcAlarmIrq( void )
{
	BoardMainClockSelect( GCLK_SOURCE_OSC16M );

    RtcTimerContext.AlarmState = ALARM_STOPPED;
    // Because of one shot the task will be removed after the callback
    RtcTimeoutPendingInterrupt = false;
    // NOTE: The handler should take less then 1 ms otherwise the clock shifts
    TimerIrqHandler( );
}

static void RtcOverflowIrq( void )
{
	RTC_BACKUP_SECONDS += (1UL << RTC_OVERTICK_SHIFT);
    //RtcTimerContext.Time += ( uint64_t )( 1 << 32 );
}

bool RtcIsTimeoutPendingPolling( void )
{
	return RtcTimeoutPendingPolling;
}

void RtcSetTimeoutPendingPolling( void )
{
	if( RtcTimerContext.AlarmState == ALARM_RUNNING )
	{
		RtcTimeoutPendingPolling = true;
		RtcTimeoutPendingInterrupt = false;
	}
}
