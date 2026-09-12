/*!
 * \file      hw_timer.c
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
/**************************************** INCLUDES*****************************/
#include "asf.h"
#include "hw_timer.h"
#include "board-config.h"
#include "sys_delay.h"
#include "utilities.h"
#include "ui_common.h"
#include "debug_log.h"

/**************************************** MACROS*****************************/
#define COMPARE_COUNT_MAX_VALUE				UINT32_MAX
#define COMPARE_COUNT_HALF_VALUE			INT32_MAX
#define HWTIMER_XOSC32K_STARTUP				2000
#define HWTIMER_OSC32K_STARTUP				100

#define HW_TIMER_RUNNING_STOP				0
#define HW_TIMER_RUNNING_CMP0				1
#define HW_TIMER_RUNNING_OVF				2
	
/**************************************** GLOBALS*****************************/
volatile uint8_t HwTimer_mode = HW_TIMER_MODE_OFF;
volatile uint8_t HwTimerRunning = HW_TIMER_RUNNING_STOP;
volatile uint32_t HwTimer_SysTick_Count;
volatile uint32_t HwTimer_ext32k_wait;

uint32_t HwTimer_RTC_SaveTime = 0;
uint32_t HwTimer_TC2_SaveTime = 0;
uint32_t HwTimer_TC2_RTC_Time = 0;

/************************************** IMPLEMENTATION************************/
#ifdef _X32K_ENABLE_
static uint32_t HwTimer_Check32k( uint32_t count );
#endif

static void HwTimer_TC2_DisableTimer( void );
static void HwTimer_RTC_DisableTimer( void );
static void HwTimer_RTC_DisableInterrupt( void );
static void HwTimer_RTC_EnableOverflow( void );

typedef void (*HwTimer_callback_t)(void);
HwTimer_callback_t	HwTimer_CMP_CallbeckHandler = NULL;
HwTimer_callback_t	HwTimer_OVF_CallbeckHandler = NULL;

inline void HwTimer_TC0_Syncing( void )			{ while( TC0->COUNT16.SYNCBUSY.reg ); }
inline void HwTimer_TC2_Syncing( void )			{ while( TC2->COUNT32.SYNCBUSY.reg ); }

/*
 * clksel
 * =0 : Disable       (HW_TIMER_TC2_DISABLE)
 * =1 : GCLK0 - 16MHz (HW_TIMER_TC2_GCLK0  )
 * =2 : SX1276 - 1MHz (HW_TIMER_TC2_RFXTAL )
 */
void HwTimer_TC2_Init( const int8_t clksel )
{
	static int8_t last_sel = -1;

	if( HwTimer_mode != HW_TIMER_MODE_TC2 )	return;

	if( last_sel < 0 )
	{
		struct system_pinmux_config pin_conf;
		pin_conf.powersave    = false;
		pin_conf.input_pull   = SYSTEM_PINMUX_PIN_PULL_NONE;
		pin_conf.direction    = SYSTEM_PINMUX_PIN_DIR_INPUT;
		pin_conf.mux_position = MUX_PB00I_CCL_IN1;
		system_pinmux_pin_set_config( PIN_PB00 , &pin_conf );		// PB00-SX1276.DIO5

#if HWTIMER_CLKOUT_1M1K_ENABLE	
		if( IS_CTRL_CLKOUT_TC1K )
		{
			pin_conf.direction    = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
			pin_conf.mux_position = MUX_PA22E_TC0_WO0;
			system_pinmux_pin_set_config( PIN_PA22 , &pin_conf );	// PA22-TC.1kHz
		}
		if( IS_CTRL_CLKOUT_RF1M )
		{
			pin_conf.direction    = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
			pin_conf.mux_position = MUX_PB23I_CCL_OUT0;
			system_pinmux_pin_set_config( PIN_PB23 , &pin_conf );	// PB23-RF.1MHz
		}
#endif
	}
	if( clksel == last_sel )	return;

	CRITICAL_SECTION_BEGIN();
	if( clksel == HW_TIMER_TC2_DISABLE )
	{
		if( last_sel > HW_TIMER_TC2_DISABLE )
		{
			HwTimer_TC2_DisableTimer();
			CCL->CTRL.reg = 0;
			TC0->COUNT16.CTRLA.reg = 0;
			HwTimer_TC0_Syncing();
			TC2->COUNT32.CTRLA.reg = 0;
			HwTimer_TC2_Syncing();
			EVSYS->CHANNEL[0].reg = 0;
			EVSYS->CHANNEL[1].reg = 0;
			EVSYS->USER[EVSYS_ID_USER_TC0_EVU].reg = 0;
			EVSYS->USER[EVSYS_ID_USER_TC2_EVU].reg = 0;
			GCLK->PCHCTRL[TC0_GCLK_ID].reg = 0;
			GCLK->PCHCTRL[TC2_GCLK_ID].reg = 0;
			MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_CCL;
			MCLK->APBCMASK.reg &= ~(MCLK_APBDMASK_EVSYS|MCLK_APBCMASK_TC0|MCLK_APBCMASK_TC2|MCLK_APBCMASK_TC3);
		}
	} else
	{
		uint32_t tc_evctrl = (clksel == HW_TIMER_TC2_RFXTAL) ? (TC_EVCTRL_EVACT_COUNT|TC_EVCTRL_TCEI|TC_EVCTRL_MCEO0):(TC_EVCTRL_EVACT_OFF|TC_EVCTRL_MCEO0);
		if( last_sel > HW_TIMER_TC2_DISABLE )
		{
			TC0->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
			HwTimer_TC0_Syncing();
			TC0->COUNT16.EVCTRL.reg = tc_evctrl;
			TC0->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
			HwTimer_TC0_Syncing();
		} else
		{
			MCLK->APBDMASK.reg |= MCLK_APBDMASK_CCL;
			CCL->LUTCTRL[0].reg = CCL_LUTCTRL_TRUTH(0x04) + CCL_LUTCTRL_LUTEO + CCL_LUTCTRL_INSEL1(0x04);
			CCL->LUTCTRL[0].reg |= CCL_LUTCTRL_ENABLE;
			CCL->CTRL.reg = CCL_CTRL_ENABLE;
			MCLK->APBDMASK.reg |= MCLK_APBDMASK_EVSYS;
			EVSYS->CHANNEL[0].reg = EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_CCL_LUTOUT_0) + EVSYS_CHANNEL_PATH_ASYNCHRONOUS;	// CCL LUTOUT0
			EVSYS->USER[EVSYS_ID_USER_TC0_EVU].reg = EVSYS_USER_CHANNEL(0x01);				// TC0 <- Channel.0
			MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC0;
			GCLK->PCHCTRL[TC0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN + GCLK_PCHCTRL_GEN_GCLK0;	// GCLK_TC0
			TC0->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 + TC_CTRLA_PRESCALER( TC_CTRLA_PRESCALER_DIV16_Val - BOADR_MCU_CLK_DIV );
			HwTimer_TC0_Syncing();
			TC0->COUNT16.DRVCTRL.reg = 0;
			TC0->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;			// Match frequency Zero->CC0
			TC0->COUNT16.EVCTRL.reg = tc_evctrl;
			TC0->COUNT16.CC[0].reg = 1000-1;
			TC0->COUNT16.CC[1].reg = -1;
			HwTimer_TC0_Syncing();
			TC0->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
			HwTimer_TC0_Syncing();
			EVSYS->CHANNEL[1].reg = EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_TC0_MCX_0) + EVSYS_CHANNEL_PATH_ASYNCHRONOUS;	// TC0 MC0
			EVSYS->USER[EVSYS_ID_USER_TC2_EVU].reg = EVSYS_USER_CHANNEL(0x02);				// TC2 <- Channel.1
			MCLK->APBCMASK.reg |= (MCLK_APBCMASK_TC2 | MCLK_APBCMASK_TC3);
			GCLK->PCHCTRL[TC2_GCLK_ID].reg = GCLK_PCHCTRL_CHEN + GCLK_PCHCTRL_GEN_GCLK0;	// GCLK_TC2
			TC2->COUNT32.CTRLA.reg = TC_CTRLA_MODE_COUNT32;
			HwTimer_TC2_Syncing();
			TC2->COUNT32.WAVE.reg = TC_WAVE_WAVEGEN_NFRQ;			// Normal frequency Zero->CC0->Max
			TC2->COUNT32.EVCTRL.reg = TC_EVCTRL_EVACT_COUNT|TC_EVCTRL_TCEI;
			TC2->COUNT16.CC[0].reg = -1;
			TC2->COUNT16.CC[1].reg = -1;
			HwTimer_TC2_Syncing();
			HwTimer_TC2_DisableTimer();
			TC2->COUNT32.CTRLA.reg |= TC_CTRLA_ENABLE;
			HwTimer_TC2_Syncing();
		}
	}
	CRITICAL_SECTION_END();
	last_sel = clksel;
}

/*
 *
 */
static uint32_t HwTimer_TC2_GetTime( void )
{
	CRITICAL_SECTION_BEGIN();
	TC2->COUNT32.CTRLBCLR.reg = TC_CTRLBCLR_CMD_NONE;
	HwTimer_TC2_Syncing();
	TC2->COUNT32.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
	while(TC2->COUNT32.CTRLBSET.reg & TC_CTRLBSET_CMD_READSYNC);
	HwTimer_TC2_Syncing();
	uint32_t count = TC2->COUNT32.COUNT.reg;
	CRITICAL_SECTION_END();
	return count;
}

/*
 *
 */
static void HwTimer_TC2_SetTime( uint32_t miliseconds )
{
	TC2->COUNT32.COUNT.reg = miliseconds;
	HwTimer_TC2_Syncing();
}

/*
 *
 */
static void HwTimer_TC2_SetTimer( uint32_t miliseconds )
{
	TC2->COUNT32.CC[0].reg = miliseconds;
	HwTimer_TC2_Syncing();
	TC2->COUNT32.INTFLAG.reg = TC_INTENSET_MC0;
}

/*
 *
 */
static void HwTimer_TC2_EnableTimer( void )
{
	TC2->COUNT32.INTFLAG.reg = TC_INTENSET_MC0;
	TC2->COUNT32.INTENSET.reg = TC_INTENSET_MC0;
	system_interrupt_enable( SYSTEM_INTERRUPT_MODULE_TC2 );
}

/*
 *
 */
static void HwTimer_TC2_DisableTimer( void )
{
	system_interrupt_disable( SYSTEM_INTERRUPT_MODULE_TC2 );
	TC2->COUNT32.INTENCLR.reg = TC_INTENSET_MC0;
	TC2->COUNT32.INTFLAG.reg = TC_INTENSET_MC0;
}

/*
 *
 */
void TC2_Handler( void )
{
	uint8_t flag = TC2->COUNT32.INTFLAG.reg;
	if(	flag & TC_INTFLAG_MC0 )
	{
		if( HwTimer_CMP_CallbeckHandler )	HwTimer_CMP_CallbeckHandler();
	}
	if(	flag & TC_INTFLAG_OVF )
	{
#if 0
		if( HwTimer_OVF_CallbeckHandler )	HwTimer_OVF_CallbeckHandler();
#endif
	}
	TC2->COUNT32.INTFLAG.reg = flag;
}

/*
 *
 */
void RTC_Handler( void )
{
	uint16_t flag = RTC->MODE0.INTFLAG.reg;

#if _DEBUG_LOG_ENABLE_
  DEBUG_LOG_SET(7, DEBUG_LOG_GET(0));	
  DEBUG_LOG_SET(8, flag);
  DEBUG_LOG_SET(9, RTC->MODE0.COUNT.reg);
  DEBUG_LOG_SET(10, RTC->MODE0.COMP[0].reg);
  RTC->MODE0.GP[0].reg = SCB->ICSR;
  RTC->MODE0.GP[1].reg = flag;
#endif

	if( flag & RTC_MODE0_INTFLAG_CMP0 )
	{
		if( HwTimer_CMP_CallbeckHandler )	HwTimer_CMP_CallbeckHandler();
	}
	if( flag & RTC_MODE0_INTFLAG_OVF )
	{
		if( HwTimer_OVF_CallbeckHandler )	HwTimer_OVF_CallbeckHandler();
	}
	RTC->MODE0.INTFLAG.reg = flag;
}

/*
 *
 */
static void HwTimer_RTC_Enable( void )
{
	if( RTC->MODE0.CTRLA.reg & RTC_MODE0_CTRLA_ENABLE )	return;
	RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_MODE(0) | RTC_MODE0_CTRLA_COUNTSYNC | RTC_MODE0_CTRLA_PRESCALER_DIV1;
	while( RTC->MODE0.SYNCBUSY.reg );
	RTC->MODE0.CTRLA.reg |= RTC_MODE0_CTRLA_ENABLE;
	while( RTC->MODE0.SYNCBUSY.reg & RTC_MODE0_SYNCBUSY_ENABLE);
}

/*
 *
 */
static void HwTimer_RTC_Disable( void )
{
	if( !(RTC->MODE0.CTRLA.reg & RTC_MODE0_CTRLA_ENABLE) )	return;
//	HwTimer_RTC_DisableTimer();

	RTC->MODE0.CTRLA.reg &= ~RTC_MODE0_CTRLA_ENABLE;
	while( RTC->MODE0.SYNCBUSY.reg );
}

/*
 *
 */
void HwTimer_RTC_Init( void )
{
	MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC;
	if( RTC->MODE0.CTRLA.reg == 0 )
	{
		RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_SWRST;
		while( RTC->MODE0.SYNCBUSY.bit.SWRST );
	}
	HwTimer_RTC_DisableInterrupt();
	HwTimer_UL32k_Select( false );			// osc32k or xosc32k
	HwTimer_RTC_Enable();

#ifdef _X32K_ENABLE_
	if( HwTimer_ext32k_wait && HwTimer_Check32k( 20000 ) < 4 )
	{
		HwTimer_ext32k_wait = 0;
		HwTimer_UL32k_Select( false );
	}
#endif
	HwTimer_RTC_EnableOverflow();
}

/*
 *
 */
inline static void HwTimer_X32k_Enable( uint8_t flag )
{
	HwTimer_ext32k_wait = flag;
}

/*
 *
 */
void HwTimer_X32k_Reset( uint8_t flag )
{
	HwTimer_X32k_Enable( flag );
	HwTimer_UL32k_Select( false );
	if( HwTimer_ext32k_wait && HwTimer_Check32k( 20000 ) < 4 )
	{
		HwTimer_ext32k_wait = 0;
		HwTimer_UL32k_Select( false );	
	}
}

/*
 *
 */
static uint32_t HwTimer_RTC_GetTick( void )
{
	while( RTC->MODE0.SYNCBUSY.bit.COUNT );
	return RTC->MODE0.COUNT.reg;
}

/*
 *
 */
void HwTimer_RTC_SetTick( uint32_t ticks )
{
	RTC->MODE0.COUNT.reg = ticks;
	while( RTC->MODE0.SYNCBUSY.bit.COUNT );	
}

/*
 *
 */
static uint32_t HwTimer_RTC_Tick2Sec( uint32_t ticks, uint16_t *milliseconds )
{
	uint32_t seconds = ticks >> 10;
	ticks = ((ticks & 0x3FF) * 1000) >> 10;
	*milliseconds = (uint16_t)ticks;
	return seconds;
}

/*
 * milliseconds = (ticks * 1000) / 1024
 */
static uint32_t HwTimer_RTC_Tick2Ms( uint32_t ticks )
{
	uint16_t milliseconds;
	uint32_t seconds = HwTimer_RTC_Tick2Sec( ticks, &milliseconds );
	return (seconds * 1000) + (uint32_t)milliseconds;
}

/*
 * tick = (milliseconds * 1024) / 1000
 */
inline uint32_t HwTimer_RTC_Ms2Tick( uint32_t milliseconds )
{
	uint32_t seconds = milliseconds / 1000;
	milliseconds = milliseconds % 1000;
	uint32_t ticks = ( seconds << 10 ) + ( (milliseconds << 10 ) / 1000 );
	return ticks;
}

/*
 *
 */
uint32_t HwTimer_RTC_GetTime( void )
{
	uint32_t ticks = HwTimer_RTC_GetTick();
	return HwTimer_RTC_Tick2Ms( ticks );
}

/*
 *
 */
uint32_t HwTimer_RTC_GetSec( uint16_t *milliseconds )
{
	uint32_t ticks = HwTimer_RTC_GetTick();
	uint32_t seconds = HwTimer_RTC_Tick2Sec( ticks, milliseconds );
	return seconds;
}

/*
 *
 */
static void HwTimer_RTC_SetTimer( uint32_t milliseconds )
{
	uint32_t ticks = HwTimer_RTC_Ms2Tick( milliseconds );
	RTC->MODE0.COMP[0].reg = ticks;
	while( RTC->MODE0.SYNCBUSY.bit.COMP0 );
	RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
}

/*
 *
 */
static void HwTimer_RTC_DisableInterrupt( void )
{
	RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_OVF | RTC_MODE0_INTFLAG_CMP0;
	RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTFLAG_OVF | RTC_MODE0_INTFLAG_CMP0;
	system_interrupt_disable( SYSTEM_INTERRUPT_MODULE_RTC );
}

/*
 *
 */
static void HwTimer_RTC_EnableOverflow( void )
{
	RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_OVF;
	RTC->MODE0.INTENSET.reg = RTC_MODE0_INTFLAG_OVF;
	system_interrupt_enable( SYSTEM_INTERRUPT_MODULE_RTC );
}

/*
 *
 */
static void HwTimer_RTC_EnableTimer( void )
{
	RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
	RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;
//	system_interrupt_enable( SYSTEM_INTERRUPT_MODULE_RTC );
}

/*
 *
 */
static void HwTimer_RTC_DisableTimer( void )
{
//	system_interrupt_disable( SYSTEM_INTERRUPT_MODULE_RTC );
	RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
	RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0;
}

/*
 * sel: OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K
 *		OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K
 *		OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K
 */
static void HwTimer_RTC_Sel( uint32_t sel )
{
	OSC32KCTRL->RTCCTRL.reg = sel;
	DelayMs( 5 );
	if( sel != OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K )	OSC32KCTRL->XOSC32K.reg = 0;
	if( sel != OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K  )	OSC32KCTRL->OSC32K.reg = 0;
}

/*
 * true:UL32K / false:OSC32,XOSC32K
 * CLK_RTC_OSC = 1kHz
 */
void HwTimer_UL32k_Select( bool ul32k_flag )
{
	uint32_t regsave = RTC->MODE0.CTRLA.reg;
	uint32_t wait_count;

	if( ul32k_flag )
	{
		if( OSC32KCTRL->RTCCTRL.reg == OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K )	return;
		HwTimer_RTC_Disable();
		/* RTC uses Ultra Low Power internal 32,768KHz Oscillator */
		HwTimer_RTC_Sel( OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K );
		MCLK->APBAMASK.reg &= ~MCLK_APBAMASK_OSC32KCTRL;	
	}
	else
	{
		MCLK->APBAMASK.reg |= MCLK_APBAMASK_OSC32KCTRL;
#ifndef _X32K_ENABLE_
		HwTimer_ext32k_wait = 0;
#else
		if( HwTimer_ext32k_wait )
		{
			if( OSC32KCTRL->RTCCTRL.reg == OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K )	return;
			HwTimer_RTC_Disable();
			OSC32KCTRL->XOSC32K.reg = 0;
			/* RTC uses External 32.768KHz Oscillator */
			OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_STARTUP(3)	// 1sec
									| OSC32KCTRL_XOSC32K_XTALEN
									| OSC32KCTRL_XOSC32K_EN1K
									| OSC32KCTRL_XOSC32K_EN32K
									| OSC32KCTRL_XOSC32K_RUNSTDBY;
			OSC32KCTRL->XOSC32K.reg |= OSC32KCTRL_XOSC32K_ENABLE;
			/* Wait XOSC32K Ready ? */
			DelayMs( 500 );
			for( wait_count = HWTIMER_XOSC32K_STARTUP; wait_count; wait_count-- )
			{
				if( OSC32KCTRL->STATUS.bit.XOSC32KRDY )	break;
				DelayMs( 1 );
			}
			HwTimer_ext32k_wait = wait_count;
		}
		if( HwTimer_ext32k_wait )
		{
			/* RTC source clock is external oscillator at 1kHz */
			HwTimer_RTC_Sel( OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K );
		}else
#endif
		{
			if( OSC32KCTRL->RTCCTRL.reg == OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K )	return;
			HwTimer_RTC_Disable();

			/* Read OSC32KCAL, calibration data for OSC32 !!! */
			uint32_t osc32kcal = ( (*(uint32_t*)NVMCTRL_OTP5) & 0x1FC0 ) >> 6;
			/* RTC use Low Power Internal Oscillator at 1kHz */
			OSC32KCTRL->OSC32K.reg =  OSC32KCTRL_OSC32K_STARTUP(3)		// 0.305 msec
									| OSC32KCTRL_OSC32K_CALIB(osc32kcal)
									| OSC32KCTRL_OSC32K_EN1K
									| OSC32KCTRL_OSC32K_EN32K
									| OSC32KCTRL_OSC32K_RUNSTDBY
									| OSC32KCTRL_OSC32K_ENABLE;
			/* Wait OSC32K Ready */
			DelayMs( 10 );
			for( wait_count = HWTIMER_OSC32K_STARTUP; wait_count; wait_count-- )
			{
				if( OSC32KCTRL->STATUS.bit.OSC32KRDY )	break;
				DelayMs( 1 );
			}
			/* RTC uses internal 32.768KHz Oscillator at 1kHz */
			HwTimer_RTC_Sel( OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K );
		}
	}
	if( regsave & RTC_MODE0_CTRLA_ENABLE )
	{
		HwTimer_RTC_Enable();
	}
}

/*
 *
 */
void HwTimer_X32k_Output( void )
{
#if HWTIMER_CLKOUT_X32K_ENABLE
	if( OSC32KCTRL->XOSC32K.reg == 0 ) return;
	while( GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL(1 << 7) );
	OSC32KCTRL->XOSC32K.reg |= OSC32KCTRL_XOSC32K_EN32K;
	GCLK->GENCTRL[7].reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC | GCLK_GENCTRL_SRC_XOSC32K;
	PORT->Group[0].PMUX[23/2].reg &= 0x0f;
	PORT->Group[0].PMUX[23/2].reg |= PORT_PMUX_PMUXO(MUX_PA23H_GCLK_IO7);
	PORT->Group[0].PINCFG[23].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN ;
#endif
}

/*
 *
 */
void HwTimer_SetXtal32k( uint8_t *x32_flag )
{
	if( *x32_flag != X32K_EXT )
	{
		*x32_flag = X32K_INT;
	}
	HwTimer_X32k_Enable( *x32_flag );
}

/*
 *
 */
int32_t HwTimer_Get_X32k_Stat( void )
{
	if( HwTimer_ext32k_wait == 0 )	return X32K_STAT_INT;
#if HWTIMER_CLKOUT_X32K_ENABLE
	if( GCLK->GENCTRL[7].reg )		return X32K_STAT_OUTPUT;
#endif
	return X32K_STAT_EXT;
}

#ifdef _X32K_ENABLE_
/*
 *
 */
static uint32_t HwTimer_Check32k( uint32_t count )
{
	uint32_t old = HwTimer_RTC_GetTick();
	while( --count )	__NOP();
	old = HwTimer_RTC_GetTick() - old;
	return old;
}
#endif

/**
* \brief Initializes the hw timer module
*/
void HwTimerInit( void )
{
	HwTimer_mode = HW_TIMER_MODE_OFF;
	HwTimerRunning = HW_TIMER_RUNNING_STOP;
	HwTimer_RTC_Init();
	if( !IS_CTRL_FORCE_INT_TIMER || (HwTimer_Get_X32k_Stat() == X32K_STAT_INT) )
	{
		HwTimer_mode = HW_TIMER_MODE_TC2;
		HwTimer_TC2_Init( HW_TIMER_TC2_GCLK0 );
	}
#if SYSTICK_PERIOD
	HwTimer_SysTick_Count = 0;
	SysTick_Config( system_gclk_gen_get_hz( GCLK_GENERATOR_0 ) / SYSTICK_PERIOD );
#endif
}

/**
* \brief This function is used to set the callback when the hw timer
* expires.
* \param callback Callback to be registered
*/
void HwTimerAlarmSetCallback( HwTimerCallback_t callback )
{
	HwTimer_CMP_CallbeckHandler = callback;
}

/**
* \brief This function is used to set the callback when the hw timer
* overflows.
* \param callback Callback to be registered
*/
void HwTimerOverflowSetCallback( HwTimerCallback_t callback )
{
	HwTimer_OVF_CallbeckHandler = callback;
}

volatile uint32_t debug_timer_tick = 0;
volatile uint32_t debug_timer_current = 0;

/**
* \brief Loads the timeout in terms of ticks into the hardware
* \ticks Time value in terms of timer ticks
*/
bool HwTimerLoadAbsoluteTicks( uint32_t ticks )
{
	CRITICAL_SECTION_BEGIN();
	uint32_t current = HwTimerGetTime( );
	HwTimerSetTimer( ticks );
	CRITICAL_SECTION_END();

	debug_timer_tick = ticks;
	debug_timer_current = current;

	uint32_t delay = ticks - current;
	uint32_t t_min = HwTImerGetMinimumTimeout() + 1;
	if( delay < t_min )
	{
        // if too close the matching interrupt does not trigger, so handle same as passed
        return false;
    }
    if( delay-1 >= COMPARE_COUNT_HALF_VALUE )
	{
        // if difference is more than half of max assume timer has passed
        return false;
    }
	HwTimerEnable();
	return true;
}

/**
*
*
*/
void HwTimerSetTimer( uint32_t ticks )
{
	if( HwTimer_mode == HW_TIMER_MODE_TC2 ) {
		HwTimer_TC2_SetTimer( ticks );
	}else{
		HwTimer_RTC_SetTimer( ticks - HwTimer_TC2_RTC_Time );
	}
}

/**
* \brief Gets the absolute time value
* \retval Absolute time in ticks
*/
uint32_t HwTimerGetTime( void )
{
	if( HwTimer_mode == HW_TIMER_MODE_TC2 ) {
		return HwTimer_TC2_GetTime();
	}else{
		return HwTimer_RTC_GetTime() + HwTimer_TC2_RTC_Time;
	}
}

/**
* \brief Enables the hw timer module
*/
void HwTimerEnable( void )
{
	if( HwTimerRunning != HW_TIMER_RUNNING_STOP )	return;
	if( HwTimer_mode == HW_TIMER_MODE_TC2 ) {
		HwTimer_TC2_EnableTimer();
	}else{
		HwTimer_RTC_EnableTimer();
	}
	HwTimerRunning = HW_TIMER_RUNNING_CMP0;
}

/**
* \brief Disables the hw timer module
*/
void HwTimerDisable( void )
{
	if( HwTimerRunning == HW_TIMER_RUNNING_STOP )	return;
	if( HwTimer_mode == HW_TIMER_MODE_TC2 ) {
		HwTimer_TC2_DisableTimer();
	}else{
		HwTimer_RTC_DisableTimer();
	}
	HwTimerRunning = HW_TIMER_RUNNING_STOP;
}

/**
*
*
*/
void HwTimer_TC2_to_RTC( void )
{
	if( HwTimer_mode != HW_TIMER_MODE_TC2 )	return;
	CRITICAL_SECTION_BEGIN();
	HwTimer_RTC_SaveTime = HwTimer_RTC_GetTime();
	HwTimer_TC2_SaveTime = HwTimer_TC2_GetTime();
	HwTimer_TC2_RTC_Time = HwTimer_TC2_SaveTime - HwTimer_RTC_SaveTime;
	if( HwTimerRunning != HW_TIMER_RUNNING_STOP )
	{
		HwTimer_TC2_DisableTimer();
		uint32_t tc2_cmp = TC2->COUNT32.CC[0].reg;
		tc2_cmp -= HwTimer_TC2_SaveTime;
		if( tc2_cmp < HW_TIMER_MIN_ALARM_DELAY_RTC ) tc2_cmp = HW_TIMER_MIN_ALARM_DELAY_RTC;
		HwTimer_RTC_SetTimer( HwTimer_RTC_SaveTime + tc2_cmp );
		HwTimer_RTC_EnableTimer();

#if _DEBUG_LOG_ENABLE_
  DEBUG_LOG_SET(15,HwTimer_RTC_SaveTime);
  DEBUG_LOG_SET(16,tc2_cmp);
#endif

	}
	CRITICAL_SECTION_END();
	HwTimer_mode = HW_TIMER_MODE_RTC;
	HwTimer_TC2_Init( HW_TIMER_TC2_DISABLE );
}

/**
*
*
*/
void HwTimer_RTC_to_TC2( void )
{
	if( HwTimer_mode != HW_TIMER_MODE_RTC )	return;
	HwTimer_TC2_Init( HW_TIMER_TC2_GCLK0 );
	CRITICAL_SECTION_BEGIN();
	uint32_t rtc_now = HwTimer_RTC_GetTime();
	uint32_t tc2_now = HwTimer_TC2_RTC_Time + rtc_now;
	HwTimer_TC2_SetTime( tc2_now );
	if( HwTimerRunning != HW_TIMER_RUNNING_STOP )
	{
		HwTimer_RTC_DisableTimer();
		uint32_t rtc_cmp = RTC->MODE0.COMP[0].reg;
		rtc_cmp = HwTimer_RTC_Tick2Ms( rtc_cmp );
		rtc_cmp -= rtc_now;
		if( rtc_cmp < HW_TIMER_MIN_ALARM_DELAY_INT ) rtc_cmp = HW_TIMER_MIN_ALARM_DELAY_INT;
		HwTimer_TC2_SetTimer( tc2_now + rtc_cmp );
		HwTimer_TC2_EnableTimer();
	}
	CRITICAL_SECTION_END();
	HwTimer_mode = HW_TIMER_MODE_TC2;
}

/**
* \brief SysTick interrupt handler
*/
void SysTick_Handler( void )
{
	SysTick->CTRL;
	++HwTimer_SysTick_Count;
}

/**
*
*/
bool HwTimerIsRtcTimerTooShort( void )
{
	if( HwTimerRunning == HW_TIMER_RUNNING_STOP ) return false;
	return (((int32_t)(RTC->MODE0.COMP[0].reg - RTC->MODE0.COUNT.reg)) < HW_TIMER_MIN_ALARM_DELAY_RTC );
}

/* eof hw_timer.c */
