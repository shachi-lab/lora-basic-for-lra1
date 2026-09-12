/*!
 * @file    bacic_wan.h
 * @brief   HeaderFile of LoRa-Wan extension for Basic Interpreter
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
#ifndef	__BASIC_WAN_H__
#define	__BASIC_WAN_H__

uint8_t proc_wan_exec( uint8_t ch );
bool basic_WanInit( void );
errCode_t basic_WanSetVar( uint8_t ch );
void basic_WanGetVar( uint8_t ch, var_t *val );
char *basic_Wan_Print( uint8_t ch, char *buff );
void basic_Wan_Process( void );

#endif
