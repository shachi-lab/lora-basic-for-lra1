/*!
 * @file    basic_conf.h
 * @brief   HeaderFile of Configuration for Basic Interpreter
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
 
#ifndef	__BASIC_CONF_H__
#define	__BASIC_CONF_H__

#define	__LORA_AES_ENABLE__			1
#define	__LORA_FWD_ENABLE__			1
#define __WAN_BACKUP_ENABLE__		1
#define __WAN_RXDELAY_ENABLE__		1
#define __WAN_CH_ADD_ENABLE__		1

#ifndef __UI_LORA_WAN__
#define __UI_LORA_WAN__
#endif

#define ERROR_ON_RESET_TIME	180		// sec

#define UART_INIT_BAUDRATE	0		// 0:initialize at system boot, other:Baudrate

typedef signed long var_t;

#ifndef NULL
#define NULL				0
#endif

#define	CODE_LINE_SIZE		256
#define STACK_SIZE 			16
#define	VARIABLE_SIZE		('Z'-'A' + 1)
#define	ARRAY_INDEX_SIZE	256
#define	ARRAY_INDEX_MAX		(ARRAY_INDEX_SIZE - 1)
#define	BUFFER_INDEX_SIZE	256
#define	BUFFER_INDEX_MAX	(BUFFER_INDEX_SIZE - 1)

#define	ASCII_NUL			0x00
#define ASCII_STX			0x02
#define ASCII_ETX			0x03
#define	ASCII_BS			0x08
#define	ASCII_HT			0x09
#define	ASCII_LF			0x0a
#define	ASCII_CR			0x0d
#define ASCII_ESC			0x1b
#define	ASCII_SP			0x20
#define ASCII_DEL			0x7f

// Internal Code
#define	ST_TYPE_MASK		0xf0

// Numbers
#define	NUM_SIZE_AUTO		0x00	// Auto
#define	NUM_SIZE_BYTE		0x01	//  8 bits
#define	NUM_SIZE_WORD		0x02	// 16 bits
#define	NUM_SIZE_LONG		0x03	// 32 bits
#define	NUM_SIZE_MID		0x04	// 24 bits
#define	NUM_SIZE_MASK		0x03
#define	ST_NUM_BYTE			(ST_NUM|NUM_SIZE_BYTE)
#define	ST_NUM_WORD			(ST_NUM|NUM_SIZE_WORD)
#define	ST_NUM_LONG			(ST_NUM|NUM_SIZE_LONG)

#define NUM_RENUM_FLAG		0x04
#define NUM_RENUM_MASK		0xfb

#define NUM_BASE_DEC		0x00
#define NUM_BASE_HEX		0x08
#define	NUM_BASE_MASK		0x08
#define	ST_TYPE_BASE_MASK	(ST_TYPE_MASK|NUM_BASE_MASK)
#define	ST_NUM_DEC			(ST_NUM|NUM_BASE_DEC)
#define	ST_NUM_HEX			(ST_NUM|NUM_BASE_HEX)

#define	is_TypeNum( n )		(((n) & ST_TYPE_MASK) == ST_NUM)
#define	is_TypeNumDec( n )	(((n) & ST_TYPE_BASE_MASK) == ST_NUM_DEC)
#define	is_TypeNumHex( n )	(((n) & ST_TYPE_BASE_MASK) == ST_NUM_HEX)
#define is_NumRenum( n )	((n) & NUM_RENUM_FLAG)

#define IS_OPERATOR_CHR( n )	((n)=='+' || (n)=='-' || (n)=='*' || (n)=='/' || (n)=='%' || (n)=='|' || (n)=='&' || (n)=='^')
#define IS_VALID_CHR( n )		((n)<0x3f || (n)=='^' || (n)=='|' || (n)=='~' || (n)=='[' || (n)==']')
#define IS_OPEN_BRACKET( n )	((n == '(') || (n == '['))
#define IS_CLOSE_BRACKET( n )	((n == ')') || (n == ']'))

#define	ENCODE_PERCENT		'%'
#define	ENCODE_BASE64		'&'
#define	ENCODE_HEXDEC		'$'
#define ENCODE_DENBUN		'!'
#define ENCODE_BINARY		'~'
#define ENCODE_IGNORE		'*'
#define ENCODE_RAW			0
#define ENCODE_HIDDEN		0x80

// Space print control for LIST
#define LIST_SP_NONE		0x00
#define LIST_SP_PRE			0x01
#define LIST_SP_POST		0x02
#define LIST_SP_BOTH		0x03

// Error Code
typedef enum
{
// Ok
	ERROR_OK			=	0x00,
// Error
	ERROR_SYNTAX		=	0x01,
	ERROR_DIVZERO		=	0x02,
	ERROR_ARRAY			=	0x03,
	ERROR_PARA			=	0x04,
	ERROR_STACK			=	0x05,
	ERROR_RESUME		=	0x06,
	ERROR_LABEL			=	0x07,
	ERROR_RUNMODE		=	0x08,
	ERROR_PGOVER		=	0x09,
	ERROR_NOLOOP		=	0x0a,
	ERROR_NOENDIF		=	0x0b,
	ERROR_DEVICE		=	0x0c,
	ERROR_EDIT			=	0x0d,
	ERROR_MODE			=	0x0e,
	ERROR_PERMISSION	=	0x0f,
	ERROR_OVERFLOW		=	0x10,
	ERROR_JOINMODE		=	0x11,
// Unexpected
	ERROR_UXNEXT		=	0x12,
	ERROR_UXRETURN		=	0x13,
	ERROR_UXLOOP		=	0x14,
	ERROR_UXEXIT		=	0x15,
	ERROR_UXCONTINUE	=	0x16,
	ERROR_UXREAD		=	0x17,
// Extend
	ERROR_FLASH			=	0x18,
	ERROR_NOPROG		=	0x19,
	ERROR_CODE_MAX				,
// Quit/Break
	ERROR_QUIT			=	0xfe,
	ERROR_BREAK			=	0xff,
} errCode_t;

#define	IS_VALID_ERROR( n )		((n)<ERROR_CODE_MAX)

// Internal Request Code
typedef enum
{
	REQUEST_NONE		=	0x00,
	REQUEST_GOTO		=	0x01,
	REQUEST_EXIT		=	0x02,
} request_t;

// Stack frame
typedef struct {
	uint8_t		type;
	uint8_t		*returnPointer;
	int32_t		returnLineNumber;
	var_t		*pvar;				/* counter variable */
	var_t		limit;				/* limit value */
	var_t		step;				/* step value */
} Stack_t;

// FORM flags

typedef void (*BasicProc_t)( uint8_t st_code );

#define	PROC( func )	void func( uint8_t st_code )
typedef struct {
#if 0
	const char  *cmd_str;
	uint8_t code;
	uint8_t sp;
#else
	uint8_t code_from;
	uint8_t code_to;
	uint8_t sp;
	const char *cmd_str[];
#endif
} basic_CmdList_t;
#else

#define FORM_NONE		0x00
#define FORM_FLAG		0x01
#define FORM_PLUS		0x02
#define FORM_HEX		0x04
#define FORM_DEC		0x80
#define FORM_ZERO		0x10
#define FORM_LOWER		0x20
#define FORM_HEXU		FORM_HEX
#define FORM_HEXL		(FORM_HEX | FORM_LOWER)
#define	FORM_FHEX		(FORM_HEX | FORM_FLAG)

#endif
