/*!
 * @file    basic_sub.h
 * @brief   HeaderFile of Subroutines for Basic Interpreter
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
#ifndef	__BASIC_SUB_H__
#define	__BASIC_SUB_H__

#define STRING_PARA_FLAG_CRLF_DISABLE		0
#define STRING_PARA_FLAG_CRLF_ENABLE		1
#define STRING_PARA_FLAG_COMMA_ENABLE		2
#define STRING_PARA_FLAG_VARIABLE			4

#define BASIC_CARET_STR_BUFF_SIZE			256
//extern uint8_t basic_CaretStrBuff[BASIC_CARET_STR_BUFF_SIZE];
//extern uint8_t basic_CaretStrLen;

errCode_t proc_LetSpecial( var_t *pvar );
errCode_t proc_LetVariable( var_t *pvar );
errCode_t get_1argVal( var_t *val );
errCode_t get_2argVal( var_t *val_1, var_t *val_2 );
errCode_t get_XargVal_def( var_t *val, uint8_t num );
errCode_t checkDelimiter( void );
bool isExeDelimiter( void );
bool isDelimiter( uint8_t ch );
bool isLabelChar( uint8_t ch );
errCode_t chkST( uint8_t ch );
int16_t get_StringPara( uint8_t flag );
var_t *getParameterPointer( void );
var_t getArrayIndex( var_t index_min, var_t index_max );
var_t getArrayIndexZero( var_t index_max );
var_t expr( void );
int16_t inputString( uint8_t (*hook_func)(void), char *buff, int16_t buff_size, bool history );
int16_t GetUartString( uint8_t (*hook_func)(void), char *buff, int16_t buff_size, uint8_t code, bool uart2 );
int16_t val_inputString( char *buff, int16_t buff_size, int32_t timeout, bool uart2 );
void *val_initInkey( var_t timeout, bool uart2 );
var_t val_getInkey( var_t timeout, bool uart2 );
uint8_t set_BiosVar( uint8_t ch );
errCode_t calcValueFunc_n( var_t *val, int num );
errCode_t calcValueFunc_1( var_t *val );
uint8_t *get_num( uint8_t *ptr, var_t *val, uint8_t type );
uint8_t *put_num( uint8_t *dst, var_t val, uint8_t type, int16_t *len );
uint8_t get_numBytes( uint8_t type );
uint8_t get_sizeBytes( uint8_t size );
char *conv2str( var_t para, uint8_t ff, int32_t len );
errCode_t basic_SetVarSub( var_t *val, var_t min, var_t max );
errCode_t basic_SetKey( void *buff, uint8_t len );
bool basic_IsNotZero( uint8_t *ptr, uint8_t size );
uint8_t *get_VarConv( int32_t *val, uint8_t *src, uint8_t size );
uint8_t *set_VarConv( int32_t val, uint8_t *dst, uint8_t size );
void clearStringBuff( void );

#include "board-config.h"
#include "bios.h"
extern uint32_t basic_EorTimer;
inline void basic_EorClear( void ) { basic_EorTimer = 0; }
inline void basic_EorProc( void ) { if ( basic_EorTimer && --basic_EorTimer == 0 ) bios_SystemReset(); }
inline void basic_EorStart( void ) { if (IS_CTRL_RESET_ON_ERROR) basic_EorTimer = ERROR_ON_RESET_TIME * SYSTICK_PERIOD; }

typedef union
{
	uint8_t	b[4];
	uint16_t w[2];
	int32_t n;
} var_size_conv_t;

#endif