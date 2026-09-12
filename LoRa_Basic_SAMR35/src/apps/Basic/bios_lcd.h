/*!
 * @file    bios_lcd.h
 * @brief   HeaderFile of LCD driver for Basic Interpreter
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
#ifndef __BIOS_LCD_H__
#define __BIOS_LCD_H__

void bios_Lcd_cls( void );
void bios_Lcd_print( void );
uint8_t bios_Lcd_GetContrust( void );
void bios_Lcd_SetContrust( int32_t contrust );
void bios_Lcd_SetPos( int32_t pos );
uint8_t bios_Lcd_GetPos( void );
void bios_Lcd_Disable( void );

#endif /* BIOS_LCD_H_ */