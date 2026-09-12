/*!
 * @file    ui_code.h
 * @brief   HeaderFile of encode/decode functions
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
#ifndef __UI_CODE_H
#define __UI_CODE_H

int32_t Ui_HexStr2Val( char **src );
int32_t Ui_DecStr2Val( char **src ); 

int32_t Ui_HexStr2Val_n( char **src, int32_t len );
int32_t Ui_DecStr2Val_n( char **src, int32_t len );

char Ui_Byte2Hex( uint8_t n );
uint8_t Ui_Hex2Byte( char c );

char *Ui_SetHexBuff( char *dst, uint8_t *src, int16_t len );
int16_t Ui_GetHexBuff( uint8_t *dst, char *src, int16_t size );

int16_t Ui_Percent_Encode( void (*putc_func)(char), uint8_t *src, int16_t size );
int16_t Ui_Percent_Decode( uint8_t *dst, char **src, char term );

int16_t Ui_Base64_Encode( void (*putc_func)(char), uint8_t *src, int16_t size );
int16_t Ui_Base64_Decode( uint8_t *dst, char **src, char term );

int16_t Ui_Hexdec_Encode( void (*putc_func)(char), uint8_t *src, int16_t size );
int16_t Ui_Hexdec_Decode( uint8_t *dst, char **src, char term );

int16_t Ui_Denbun_Encode( void (*putc_func)(char), uint8_t *src, int16_t size );

int16_t Ui_Encode_Print( void (*putc_func)(char), uint8_t *src, int16_t size, uint8_t code );

bool Ui_IsEncode( uint8_t code );

#endif
