/*!
 * \file      pinName-board.h
 *
 * \brief     Target board GPIO pins definitions
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
#include <compiler.h>

#ifndef __PIN_NAME_BOARD_H__
#define __PIN_NAME_BOARD_H__

#if (SAMR34) || (SAMR35)
/*!
 * ATSAMR34,35 Pin Names
 */
#define MCU_PINS \
    PA_00 = 0, PA_01 = 1, PA_04 = 4, PA_05 = 5, PA_06 = 6, PA_07 = 7, PA_08 = 8, PA_09 = 9,	\
    PA_10 =10, PA_11 =11, PA_12 =12, PA_13 =13, PA_14 =14, PA_15 =15, PA_16 =16, PA_17 =17,	\
    PA_18 =18, PA_19 =19, PA_22 =22, PA_23 =23, PA_24 =24, PA_25 =25, PA_27 =27, PA_28 =28,	\
    PA_30 =30, PA_31 =31, PB_00 =32, PB_02 =34, PB_03 =35, PB_15 =47, PB_16 =48, PB_17 =49,	\
    PB_22 =54, PB_23 =55, PB_30 =62, PB_31 =63, PC_18 =82, PC_19 =83
#else
#error	"This project is ATSAMR34,35 only !!"
#endif

#define PIN_NC		0xFFFFFFFF
#define PIN_MAX		PIN_PC19

#endif // __PIN_NAME_BOARD_H__
