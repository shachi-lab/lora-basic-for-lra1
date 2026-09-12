/*!
 * \file      hw_timer.h
 *
 * \brief     Wrapper used by sw_timer utility using SysTick
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
#ifndef HW_TIMER_H_INCLUDED
#define HW_TIMER_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************************************** INCLUDES*****************************/

/**************************************** MACROS******************************/

#define TIMER_CORRECTION_TICKS          0U //32
#define HW_TIMER_MIN_ALARM_DELAY_RTC	6
#define HW_TIMER_MIN_ALARM_DELAY_INT	2

/**************************************** TYPES*****************************/

typedef void (*HwTimerCallback_t) (void);

/***************************************PROTOTYPES**************************/

/**
* \brief Initializes the hw timer module
*/
void HwTimerInit(void);

/**
* \brief This function is used to set the callback when the hw timer
* expires.
* \param callback Callback to be registered
*/
void HwTimerAlarmSetCallback(HwTimerCallback_t callback);

/**
* \brief This function is used to set the callback when the hw timer
* overflows.
* \param callback Callback to be registered
*/
void HwTimerOverflowSetCallback(HwTimerCallback_t callback);

/**
* \brief This function is used to set the callback when the hw timer
* Tick.
* \param callback Callback to be registered
*/
void HwTimerTickSetCallback(HwTimerCallback_t callback);

/**
* \brief Loads the absolute timeout in terms of ticks into the hardware
* \ticks Absolute time value in terms of timer ticks
*/
bool HwTimerLoadAbsoluteTicks(uint32_t ticks);

/**
* \brief Gets the absolute time value
* \retval Absolute time in ticks
*/
uint32_t HwTimerGetTime(void);

/**
* \brief Disables the hw timer module
*/
void HwTimerDisable(void);

/**
* \brief Enables the hw timer module
*/
void HwTimerEnable(void);
void HwTimerSetTimer(uint32_t ticks);
void HwTimer_RTC_to_TC2( void );
void HwTimer_TC2_to_RTC( void );

/**
* \brief Add time to hw timer
*/
void HwTimer_RTC_Init( void );
uint32_t HwTimer_RTC_GetTime( void );
uint32_t HwTimer_RTC_GetSec( uint16_t *milliseconds );
void HwTimer_EventProcess( void );
void HwTimer_UL32k_Select( bool flag );
int32_t HwTimer_Get_X32k_Stat( void );
void HwTimer_X32k_Output( void );
void HwTimer_X32k_Reset( uint8_t flag );
bool HwTimer_Is_Force_16M( void );
void HwTimer_RTC_SetTick( uint32_t ticks );
void HwTimer_SetXtal32k( uint8_t *x32_flag );

#define X32K_STAT_INT			0	
#define X32K_STAT_EXT			1
#define X32K_STAT_FAIL			-1
#define X32K_STAT_OUTPUT		32768

void HwTimer_TC2_Init( const int8_t clksel );

#define HW_TIMER_TC2_DISABLE			0
#define HW_TIMER_TC2_GCLK0				1
#define HW_TIMER_TC2_RFXTAL				2

#define HW_TIMER_MODE_OFF				0
#define HW_TIMER_MODE_TC2				1
#define HW_TIMER_MODE_RTC				2

extern volatile uint8_t	HwTimer_mode;

#define HWTIMER_CLKOUT_X32K_ENABLE		1		// =0:Disable, =1:Enable(PA23) 
#define HWTIMER_CLKOUT_1M1K_ENABLE		1		// =0:Disable, =1:Enable(PA22,PB23) 

inline bool HwTimer_Is_IntClk( void )			{ return (HwTimer_mode == HW_TIMER_MODE_TC2); }
inline uint32_t HwTImerGetMinimumTimeout(void)	{ return HwTimer_Is_IntClk() ? HW_TIMER_MIN_ALARM_DELAY_INT : HW_TIMER_MIN_ALARM_DELAY_RTC; }

bool HwTimerIsRtcTimerTooShort( void );
	
extern volatile uint32_t HwTimer_SysTick_Count;

#ifdef	__cplusplus
}
#endif
#endif /* HW_TIMER_H_INCLUDED */
