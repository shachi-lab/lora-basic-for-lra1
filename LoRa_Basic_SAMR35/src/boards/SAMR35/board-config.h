/*!
 * \file      board-config.h
 *
 * \brief     Board configuration
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
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "asf.h"

#if !(SAMR34) && !(SAMR35)
#error	"This project is ATSAMR34,35 only !!"
#endif

/*!
 * Board MCU main clock definitions
 */
#define BOADR_MCU_CLK_DIV							0		// 0:16MHz(Default), 1:8MHz, 2:4MHz

#define SYSTICK_PERIOD								100		// 100Hz

/*!
 * Defines the time required for the TCXO to wakeup [ms].
 */
#define BOARD_TCXO_WAKEUP_TIME                      10

/*!
 * Board MCU pins definitions
 */

// Internal Multiplexed Signals
#define RADIO_RESET                                 PIN_PB15		// RF_RST

#define RADIO_MOSI                                  PIN_PB30		// RF_MOSI
#define RADIO_MISO                                  PIN_PC19		// RF_MISO
#define RADIO_SCLK                                  PIN_PC18		// RF_SCLK
#define RADIO_NSS                                   PIN_PB31		// RF_SEL

#define RADIO_DIO_0                                 PIN_PB16
#define RADIO_DIO_1                                 PIN_PA11
#define RADIO_DIO_2                                 PIN_PA12
#define RADIO_DIO_3                                 PIN_PB17
#define RADIO_DIO_4                                 PIN_PA10
#define RADIO_DIO_5                                 PIN_PB00

#define RADIO_ANT_SWITCH							PIN_NC

#ifdef	__SAMR34J18B__ 												// for SAMR34 Xplained Pro
#define RADIO_BAND_SEL								PIN_PA13
#define	RADIO_TCXO_POWER							PIN_PA09	
#else
#define RADIO_BAND_SEL								PIN_NC
#define	RADIO_TCXO_POWER							PIN_NC	
#endif

// for Evalution board
#define LED_POLARITY								0				// 0:Active-High, 1:Active-Low
#define LED_1                                       PIN_PA18
#define LED_2                                       PIN_PA19
#define LED_ON										(LED_POLARITY ^ 1)
#define LED_OFF										LED_POLARITY

#define	PUSH_SWITCH									PIN_PA06
#define PUSH_ON										true
#define PUSH_OFF									false

#define UART1_TX                                    PIN_PA04
#define UART1_RX                                    PIN_PA05

#define UART2_TX                                    PIN_PA22
#define UART2_RX                                    PIN_PA23

#define I2C_SCL										PIN_PA16
#define I2C_SDA										PIN_PA17

#define BIOS_MOSI									PIN_PB22		// BIOS_MOSI
#define BIOS_MISO									PIN_PB02		// BIOS_MISO
#define BIOS_SCLK									PIN_PB23		// BIOS_SCLK
#define BIOS_NSS									PIN_NC			// BIOS_NSS(NC)

typedef struct {
	Sercom *const hw;
	uint32_t muxsettimg;
	uint32_t pinmux_pad[4];
	union{ uint32_t tx_pin; uint32_t sda_pin; };
	union{ uint32_t rx_pin; uint32_t scl_pin; };
} sercom_settings_t;

void BoardMclkMaskClear( void );
void BoardEnableExtint( uint32_t irqLine );
void BoardDisableExtint( uint32_t irqLine );
void BoardSercomDisableMclk( Sercom *const hw );
void BoardSercomEnableMclk( Sercom *const hw );

extern uint8_t Padac_value;

#endif // __BOARD_CONFIG_H__
