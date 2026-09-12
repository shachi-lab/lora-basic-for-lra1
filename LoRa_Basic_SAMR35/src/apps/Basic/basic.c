/*!
 * @file    basic.c
 * @brief   Embedded BASIC Interpreter
 *
 * @details
 * This BASIC interpreter is designed for embedded systems and is
 * used in the LRA1 LoRa module.
 * Originally forked from an early nanoBASIC implementation and
 * extended and maintained by Shachi-lab.
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

#include "asf.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include "basic.h"
#include "bios.h"
#include "utilities.h"

#define	INTERNAL_CODE_PRINT			0
#ifndef AUTORUN_WAIT_TIME
#define AUTORUN_WAIT_TIME			5
#endif

LPRAM uint8_t program_area[PROGRAM_AREA_SIZE];
VARBSS var_t globalVariables[VARIABLE_SIZE];
ARYBSS var_t arrayVariables[ARRAY_INDEX_SIZE];
uint8_t InternalcodeLine[CODE_LINE_SIZE];
Stack_t stacks[STACK_SIZE];

int32_t lineNumber;
uint8_t *executionPointer;
errCode_t errorCode;
errCode_t lastErrCode;
int32_t  lastErrLineNumber;
uint8_t *errorCatchPointer;
int32_t errorCatchLineNumber;
request_t returnReq;
uint8_t *dataReadPointer;
uint8_t *resumePointer;
int32_t resumeLineNumber;
uint32_t progLength = 0;
uint8_t progEditMode = 0;
uint8_t stackPointer = 0;

static int32_t pload_base64_passkey = 0;

#define CHECK_RUN_MODE()	if (lineNumber)  { errorCode = ERROR_RUNMODE; return; }
#define CHECK_EDIT_MODE()	if (!progEditMode) { errorCode = ERROR_EDIT; return; }
#define CHECK_PROTECT()		if (Settings.Protect) { errorCode = ERROR_PERMISSION; return; }

static void interactiveMain( char *str );
static uint8_t convertInternalCode( uint8_t *dst, char *src, int16_t dst_size );
static void interpreterMain( uint8_t *ptr );
static void executeBreak( void );
static uint8_t *proc_exit_cont_sub( uint8_t err );
static void printError( void );
static uint8_t *goto_Jump( void );
static Stack_t *pushStack( uint8_t st );
static Stack_t *popStack( uint8_t st );
static void sweepStack( uint8_t *ptr );
static void prog_regist( uint8_t *ptr );
static void move_line( uint8_t *dst, uint8_t *src );
static char *convertCmdToCode( const basic_CmdList_t *lptr_p[], char *src, uint8_t *dst, uint8_t ext );
static uint8_t *find_LineNumber( uint8_t *ptr );
static char *Get_AutorunStr( uint32_t timeout );
static void proc_print_encode( uint8_t st, uint8_t code );
static var_t str2var( char *ptr );
static void proc_sleep_and_deep( bool deep );
static uint8_t *get_line_num( uint8_t *ptr, var_t *num );
static uint8_t isSTwithLineNumber( uint8_t ch );
static uint8_t *findST( const uint8_t *st_list, uint8_t *st );
static uint8_t *findLoop( uint8_t *ptr );
static uint8_t *findNext( uint8_t *ptr );
static bool interpreterCode( void );
static uint8_t *find_line( int32_t *lnum, uint8_t **str );
static uint8_t basic_exec_cmd( uint8_t codeBegin, uint8_t codeEnd, uint8_t ch, const BasicProc_t *codeList );
static bool basic_psave_hexdec( uint8_t *top, uint32_t size );
static bool basic_psave_base64( uint8_t *top, uint32_t size );
static bool basic_pload_base64( bool append );

//
const char* const errorSting[] = {
	"OK"					,	// 0x00 : ERROR_OK
	"Syntax"				,	// 0x01 : ERROR_SYNTAX
	"Division by 0"			,	// 0x02 : ERROR_DIVZERO
	"Array index over"		,	// 0x03 : ERROR_ARRAY
	"Parameter"				,	// 0x04 : ERROR_PARA
	"Stack overflow"		,   // 0x05 : ERROR_STACK
	"Can't resume"			,	// 0x06 : ERROR_RESUME
	"Label not found"		,	// 0x07 : ERROR_LABEL
	"Unless from run-mode"	,	// 0x08 : ERROR_RUNMODE
	"Program area overflow"	,	// 0x09 : ERROR_PGOVER
	"Loop nothing"			,	// 0x0a : ERROR_NOLOOP
	"Endif not found"		,	// 0x0b : ERROR_NOENDIF
	"Device access"			,	// 0x0c : ERROR_DEVICE
	"Edit mode"				,	// 0x0d : ERROR_EDIT
	"Invalid mode"			,   // 0x0e : ERROR_MODE
	"Permission"			,   // 0x0f : ERROR_PERMISSION
	"Overflow"				,   // 0x10 : ERROR_OVERFLOW
	"Join mode"				,   // 0x11 : ERROR_JOINMODE
	" Next"					,	// 0x12 : ERROR_UXNEXT
	" Return"				,	// 0x13 : ERROR_UXRETURN
	" Loop"					,	// 0x14 : ERROR_UXLOOP
	" Exit"					,	// 0x15 : ERROR_UXEXIT
	" Continue"				,	// 0x16 : ERROR_UXCONTINUE
	" Read"					,	// 0x17 : ERROR_UXREAD
	"Flash write"			,	// 0x18 : ERROR_FLASH
	"Program nothing"		,	// 0x19 : ERROR_NOPROG
};

//*************************************************
void basicMain( char *str )
{
#if UART_INIT_BAUDRATE
//	bios_UartInit( Settings.Baudrate );
#endif
	initializeVariables( true );
	setLastError();
	if ( str == NULL )
	{
		str = Get_AutorunStr( AUTORUN_WAIT_TIME );
		if ( errorCode )	printError();
	}
	while ( errorCode != ERROR_QUIT )
	{
		interactiveMain( str );
		str = NULL;
	}
}

//*************************************************
static char *Get_AutorunStr( uint32_t timeout )
{
	char *str = bios_GetAutorun();
	if ( *str == 0 ) return NULL;

	if( IS_CTRL_FAST_START_ON_DEEP && (RSTC->RCAUSE.reg & RSTC_RCAUSE_BACKUP) )	return str;

	while ( timeout )
	{
		bios_UartPutChar( '.' );
		bios_setWaitTick( 1000 );
		while ( bios_getWaitTick() > 0 )
		{
			if ( Ui_IsBreak_LP() ) return NULL;
		}
		timeout--;
	}
	return str;
}

//*************************************************
static void interactiveMain( char *str )
{
	lineNumber = 0;
	returnReq = REQUEST_NONE;
	bios_UartPutLine( "OK" );
	while ( true )
	{
		errorCode = ERROR_OK;
		if ( str == NULL )
		{
			bios_UartPutChar( '>' );
			if ( progEditMode )
			{
				bios_UartPutChar( '>' );
			}
			str = bios_UartRxBuff;
			inputString( NULL, str, BIOS_UART_RX_BUFF_SIZE, true );
		} else
		{
			bios_UartPutLine( str );
		}
		if ( errorCode == ERROR_OK )
		{
			errorCode = convertInternalCode( InternalcodeLine, str, CODE_LINE_SIZE );	// �����R�[�h�ɕϊ�
		}
		if ( errorCode == ERROR_OK && InternalcodeLine[0] > 1 )
		{
#if INTERNAL_CODE_PRINT
			printInternalcode();
#endif
			if ( is_TypeNumDec( InternalcodeLine[1] ) == 0 )// �s�ԍ��Ȃ��H
			{
				interpreterMain( InternalcodeLine );		// Direct ���s
				return;
			}		
			if ( progEditMode )
			{
				prog_regist( InternalcodeLine );			// Prog�s�o�^
			}
			else
			{
				errorCode = ERROR_EDIT;
			}
		}
		if ( errorCode != ERROR_OK )
		{
			printError();
			return;
		}
		str = NULL;
	}
}

#if	INTERNAL_CODE_PRINT
//*************************************************
static void printInternalcode( void )
{
	uint8_t len, ch, *ptr;

	ptr = executionPointer;
	len = *ptr;
	if ( len > 0 ) {
		len++;
		while ( len-- ) {
			ch = *ptr++;
			bios_UartPuts(conv2str( ch, FORM_HEX|FORM_ZERO, 2 ));
			bios_UartPutChar( 0x20 );
		}
		bios_UartPutCrLf();
	}
}
#endif

//*************************************************
void initializeVariables( bool flag )
{
	if ( flag )
	{
		lineNumber = 0;
		executionPointer = InternalcodeLine;
		memset((void*)PROGRAM_AREA_TOP, 0xff, PROGRAM_AREA_SIZE);
		*PROGRAM_AREA_TOP = ST_EOL;
		progLength = 0;
	}
	errorCode = ERROR_OK;
	stackPointer = 0;
	resumePointer = NULL;
	resumeLineNumber = 0;
	dataReadPointer = 0;
	lastErrCode = ERROR_OK;
	lastErrLineNumber = 0;
	errorCatchPointer = NULL;
	errorCatchLineNumber = 0;
	clearStringBuff();
	memset( InternalcodeLine, 0, sizeof(InternalcodeLine) );
	memset( &_svar_bss, 0, (size_t)(&_evar_bss - &_svar_bss) );
//	memset( globalVariables, 0, sizeof(globalVariables) );
//	memset( arrayVariables , 0, sizeof(arrayVariables ) );
}

//*************************************************
static void printError( void )
{
	if ( errorCode ) {
		setLastError();
		if ( errorCode == ERROR_QUIT ) {
			return;
		}
		bios_UartPutCrLf();
		if ( errorCode == ERROR_BREAK ) {
			bios_UartPuts( "Break" );
		} else
		{
			if ( IS_VALID_ERROR( errorCode ) )
			{
				char const *s = errorSting[errorCode];
				if ( *s == ' ' ) {
					bios_UartPuts( "Unexpected" );
				}
				bios_UartPuts( s );
			} else
			{
				bios_UartPrintf( "[%d]", errorCode );
			}
			bios_UartPuts( " error" );
		}
		if ( lineNumber ) {
			bios_UartPrintf( " in %"PRId32, lineNumber );
		}
	}
	bios_UartPutCrLf();
}

//*************************************************
void setLastError( void )
{
	if ( errorCode == ERROR_OK ) return;
	lastErrCode = errorCode;
	lastErrLineNumber = lineNumber;
}

//************************************************
static void errLineBup( void )
{
	if ( IS_DEBUG_SAVE_LINE_NUMBER ) {
		 RTC->MODE0.GP[2].reg = lineNumber;
		 RTC->MODE0.GP[3].reg = errorCode;
	}
}
	
//*************************************************
static void interpreterMain( uint8_t *ptr )
{
	uint8_t	ch;

	executionPointer = ptr;
	while ( true )
	{
		ch = *executionPointer++;
		if ( ch == ST_EOL || returnReq == REQUEST_EXIT )
		{
			resumePointer = NULL;
			lineNumber = 0;
			return;
		}
		if ( lineNumber )
		{
			ch = *executionPointer;
			if ( is_TypeNumDec(ch) )
			{
				var_t num;
				executionPointer = get_num( &executionPointer[1], &num, ch );
				lineNumber = num;
				errLineBup();
			}
		}
		for ( returnReq = REQUEST_NONE; returnReq == REQUEST_NONE; )
		{
			if ( Ui_IsBreak_BK() == 0 )
			{
				if ( interpreterCode() )
				{
					if ( lineNumber == 0 ) return;
					break;			
				}
			}
			if ( errorCode )
			{
				setLastError();
				if ( lineNumber == 0 || errorCatchPointer == NULL )
				{
					if ( lineNumber ){ basic_EorStart(); errLineBup(); }
					printError();
					return;
				}
				executionPointer = errorCatchPointer;
				lineNumber = errorCatchLineNumber;
				errorCatchPointer = NULL;
				errorCode = ERROR_OK;
				returnReq = REQUEST_GOTO;
			}
		}
	}
}

//*************************************************
static void st_skip( uint8_t ch )
{
	while ( *executionPointer != ch && *executionPointer != ST_EOL )	executionPointer++;
}

//*************************************************
static bool interpreterCode( void )
{
	uint8_t ch;

	do {
		ch = *executionPointer++;
		if ( ch == ST_EOL )	return true;
		if ( ch == ST_INDENT ) {
			 executionPointer++; 
			 ch = ' ';
		}
	} while ((ch == ' ')||(ch == '\t')||( ch == ':'));

	if ( ch == ST_ARRAY )
	{
		var_t index = getArrayIndexZero( ARRAY_INDEX_MAX );
		if ( errorCode == ERROR_OK )	proc_LetVariable( &arrayVariables[index] );
		return false;
	}
	if ( isupper( ch ) )
	{
		uint8_t index = ch - 'A';
		proc_LetVariable( &globalVariables[index] );
		return false;
	}
	if ( ch == '=' )
	{
		expr();
		return false;
	}
	if ( ch == ST_COMMENT )
	{
		st_skip( ST_EOL );
		return false;
	}
	if ( ch == ST_LABEL )
	{
		st_skip( ':' );
		return false;
	}
	if ( Ui_IsEncode( ch ) )
	{
		uint8_t code = ch;
		ch = *executionPointer++;
		proc_print_encode( ch, code );
		return false;
	}
#ifdef __UI_LORA_WAN__
	if ( !proc_wan_exec( ch ) )	return false;
#endif
	if ( !basic_exec_cmd( ST_CODE_BEGIN, ST_CODE_END, ch, procCodeList ) )	return false;
	if ( !set_BiosVar( ch ) ) return false;
	if ( !basic_LoRa_SetVar( ch ) )	return false;

	errorCode = ERROR_SYNTAX;
	return false;
}

//*************************************************
static uint8_t basic_exec_cmd( uint8_t codeBegin, uint8_t codeEnd, uint8_t ch, const BasicProc_t *codeList )
{
	if ( ch < codeBegin || ch > codeEnd )	return 1;
	uint8_t index = ch - codeBegin;
	if ( codeList[index] )
	{
		(*codeList[index])(ch);
	}
	return 0;
}

//*************************************************
static var_t str2var( char *ptr )
{
	var_t	val = 0;
	char ch, flag = 0;

	while ( (ch = *ptr) <= 0x20 )
	{
		if ( ch == 0 )	return ( val );
		ptr++;
	}
	if ( ch == '-' ) {
		flag = ch;
		ch = *++ptr;
	}
	if ( ch == ST_HEXCHR ) {
		ptr++;
		val = Ui_HexStr2Val( &ptr );
	} else {
		val = Ui_DecStr2Val( &ptr );
	}
	if ( flag )	return -val;
	return ( val );
}

//*************************************************
static char *strcmp_case( const char *s1, char *s2 )
{
	while( *s1 != '\0' )
	{
		if ( toupper( *s1++ ) != toupper( *s2++ ) )	return NULL;
	}
	return s2;
}

//*************************************************
static char *convertCmdToCode( const basic_CmdList_t *lptr_p[], char *src, uint8_t *dst, uint8_t ext )
{
	while ( *lptr_p ) {
		basic_CmdList_t const *lptr = *lptr_p;
		char **ss = (char**)lptr->cmd_str;
		for ( uint8_t code = lptr->code_from; code <= lptr->code_to; code++)
		{	
			char *s2 = strcmp_case( *ss, src );
			if ( s2 != NULL &&  !isalpha( *s2 ) )
			{
				if ( ext )	*dst++ = ext;
				*dst = code;
				return s2;				
			}
			ss++;
		}
		lptr_p++;
	}
	return NULL;
}

#define CODE_PUT_GET( ch )		do { *dst++=ch; ch=*++src; len++; } while (0)
#define CODE_PUT_CHR( ch )		do { *dst++=ch; src++; len++; } while (0)

//*************************************************
static uint8_t convertInternalCode( uint8_t *dst, char *src, int16_t dst_size )
{
	char ch;
	uint8_t	*topptr, num_type, next_type = 0;
	int16_t len = 0;
	var_t val;
	bool term_flag = false;
	bool line_num_flag = true;

	num_type = ST_NUM_WORD;
	topptr = dst++;
	dst_size -= 2;
	while ( len < dst_size )
	{
		while ((ch = *src) <= 0x20) {
			if ( *src == '\0' ) {
				*dst++ = ST_EOL;
				len++;
				*topptr = len;
				return ERROR_OK;
			}
			src++;
		}
		do {
			if ( ch == ':' ) {
				CODE_PUT_CHR( ch );
				term_flag = false;
				break;
			}
			if ( ch == '?' ) {
				CODE_PUT_CHR( ST_PRINT );
				break;
			}
			if ( isalpha( ch ) || ch == ST_LORA ) {	// A-Z,a-z,#
				char wan_flag = 0;
				char *ptr = &src[1];
				do {
					if ( ch == ST_LORA ) {
						if ( src[1] == '?' )			// #?
						{
							ptr++;
							*dst = ST_LORA_DISP;
							break;
						}
						wan_flag = ch;
					} else
					if ( !isalnum( src[1] ) ) {		// ��낪�p���ȊO -> �ϐ� ?
						*dst = toupper( ch );
						break;
					}
#ifdef __UI_LORA_WAN__
					if( strcmp_case( "WAN_", src ) )
					{
						ptr += 3;
						wan_flag = ch;
					}
					if ( wan_flag )
					{
						ptr = convertCmdToCode( basic_Wan_CmdList, ptr, dst, ST_CODE_WAN );
						if ( ptr != NULL )
						{
							dst++;	len++;
							break;
						}
						if ( wan_flag != ST_LORA ) return ERROR_SYNTAX;
						*dst = ST_LORA;
						ptr = &src[1];
						break;
					}
#endif
					ptr = convertCmdToCode( basic_CmdList, src, dst, 0 );
					if ( ptr == NULL ) return ERROR_SYNTAX;
					if ( isSTwithLineNumber( *dst )) next_type = ST_NUM_WORD;
					if ( isDelimiter( *dst ) ) term_flag = 0;

				} while ( 0 );
				src = ptr;
				dst++;	len++;
				break;
			}

			if ( ch == ST_ARRAY && (src[1]=='(' || src[1]=='[' )) {	// �z�� ?
				CODE_PUT_CHR( ch );
				break;
			}
			if ( isdigit( ch ) ) {					// ���l ?
				val = Ui_DecStr2Val( &src );
				if( val == 0 && next_type == 0 ) {
					*dst++ = ST_ZERO;
					len++;
					break;
				}
				dst = put_num( dst, val, num_type, &len );
				if ( line_num_flag )
				{
					uint8_t indent = 0;
					while( *src == ' ' ) {
						indent++;
						src++;					
					}
					if ( progEditMode == 2 && indent > 1 ) {
						*dst++ = ST_INDENT;
						*dst++ = indent - 1;							
						len += 2;
					}
				}			
				break;
			}
			if ( ch == ST_HEXCHR ) {				// HEX ?
				src++;
				if ( isxdigit( *src ) )
				{
					val = Ui_HexStr2Val( &src );
					dst = put_num( dst, val, ST_NUM_HEX, &len );
					break;
				}
				*dst++ = ch;
				len++;
				break;
			}
			if ( ch == ST_STRING ) {				// ������ ?
				do {
					CODE_PUT_GET( ch );
					if ( ch < 0x20 )	return ERROR_SYNTAX;
				} while ( ch != ST_STRING );
				CODE_PUT_GET( ch );
				break;
			}
			if ( ch == ST_COMMENT ) {				// �R�����g ?
				while ( ch >= 0x20 ) {
					CODE_PUT_GET( ch );
				}
				break;
			}
			if ( ch == ST_LABEL ) {				// �����񃉃x��
				while ( isLabelChar( ch ) ) {
					CODE_PUT_GET( ch );
				}
				term_flag = true;
				break;
			}
			if ( term_flag && isLabelChar( ch ) )	return ERROR_SYNTAX;
			term_flag = 0;
			if ( IS_VALID_CHR( ch ) ) {			// �L���ȕ��� ?
				CODE_PUT_GET( ch );
				break;
			}
			return ERROR_SYNTAX;
		} while ( 0 );
		num_type = next_type;
		next_type = 0;
		line_num_flag = false;
	}
	return ERROR_PGOVER;
}

//*************************************************
static uint8_t *find_line( int32_t *lnum, uint8_t **str )
{
	uint8_t len, ch, *ptr, *p, *s;
	var_t	dec;

	uint8_t *z = *str;
	while( z && isLabelChar( *z ) ) z++;

	ptr = PROGRAM_AREA_TOP;
	while ( true )
	{
		p = ptr;
		len = *p++;
		if ( len == ST_EOL )
		{
			*str = z;
			errorCode = ERROR_LABEL;
			return NULL;
		}
		ch = *p++;
		if ( is_TypeNumDec( ch ) )
		{
			p = get_num( p, &dec, ch );
			if ( z == NULL ) {				// �s�ԍ� ?
				if ( dec == *lnum )
				{
					return ptr;
				}
			} else
			{
				if ( *p == ST_INDENT ) { p += 2; }	
				if ( *p == ST_LABEL )		// �����񃉃x�� ?
				{
					p++;
					s = *str;
					while ( *p++ == *s++ )
					{
						if ( !isLabelChar( *s ) && !isLabelChar( *p ) )
						{
							*lnum = dec;
							*str = s;
							return ptr;
						}
					}
				}
			}
		}
		ptr += len + 1;
	}
}

//*************************************************
static uint8_t *findST( const uint8_t *st_list, uint8_t *st )
{
	uint8_t ch, count_if, *ptr;

	count_if = 0;
	ptr = executionPointer;
	int32_t num = lineNumber;
	while ( true ) {
		do {
			ch = *ptr++;
			switch ( ch )
			{
#ifdef __UI_LORA_WAN__
			case ST_CODE_WAN :
				ptr++;
				break;
#endif
			case ST_COMMENT :
				while ( *ptr++ != ST_EOL );
				ch = ST_EOL;
				break;
			case ST_STRING :
				while ( *ptr++ != ST_STRING );
				break;
			case ST_IF :
				count_if++;
				break;
			case ST_ENDIF :
				if ( count_if ) {
					count_if--;
					break;
				}
			default :
				if ( is_TypeNum( ch ) )
				{
					ptr += get_numBytes( ch );
				}
				else
				if ( count_if == 0 ) {
					for ( const uint8_t *lp = st_list; *lp; lp++ )
					{
						if ( *lp == ch )
						{
							lineNumber = num;
							*st = ch;
							return ( ptr );
						}
					}
				}
			}
		} while ( ch != ST_EOL );
		if ( lineNumber == 0 ) {
			break;
		}
		if ( *ptr++ == ST_EOL ) {
			break;
		}
		if ( is_TypeNumDec( *ptr ) )
		{
			ptr = get_num( &ptr[1], (var_t*)&num, *ptr );
		}
	}
	return ( NULL );
}

//*************************************************
static Stack_t *pushStack( uint8_t st )
{
	if ( stackPointer >= STACK_SIZE ) {
		errorCode = ERROR_STACK;
		return NULL;
	}
	Stack_t *prevsp = &stacks[stackPointer++];
	prevsp->type = st;
	prevsp->returnPointer = executionPointer;
	prevsp->returnLineNumber = lineNumber;
	return prevsp;
}

//*************************************************
static Stack_t *popStack( uint8_t st )
{
	if ( stackPointer == 0 ) return NULL;
	Stack_t *prevsp = &stacks[--stackPointer];
	if ( prevsp->returnPointer == NULL ) return NULL;
	if ( st != 0 && prevsp->type != st ) return NULL;
	return prevsp;
}

//*************************************************
static void sweepStack( uint8_t *ptr )
{
	Stack_t *sp = stacks;
	for( uint8_t i = stackPointer; i; i--, sp++ )
	{
		if( sp->returnPointer >= ptr ){
			sp->returnPointer = NULL;
			sp->returnLineNumber = 0;
		}
	}
}

//*************************************************
PROC( proc_print )
{
	proc_print_encode( st_code, ENCODE_RAW );
}

//*************************************************
static void proc_print_encode( uint8_t st, uint8_t code )
{
	if ( st == ST_UPRINT )
	{
		bios_Uart2Begin( UART2_TX );
		int16_t len = get_StringPara( STRING_PARA_FLAG_CRLF_ENABLE | STRING_PARA_FLAG_COMMA_ENABLE );
		if ( len >= 0 ) Ui_Encode_Print( bios_Uart2PutChar, (uint8_t*)bios_UartTxBuff, len, code );
	} else
	if ( st == ST_PRINT )
	{
		int16_t len = get_StringPara( STRING_PARA_FLAG_CRLF_ENABLE | STRING_PARA_FLAG_COMMA_ENABLE );
		if ( len >= 0 ) Ui_Encode_Print( bios_UartPutChar, (uint8_t*)bios_UartTxBuff, len, code );
	} else
	{
		errorCode = ERROR_SYNTAX;
	}
}

//*************************************************
PROC( proc_input )
{
	var_t *pvar = getParameterPointer();
	if ( pvar == NULL )	return;

	uint8_t flg = *executionPointer;
	if ( flg == ST_HEXCHR ) {
		executionPointer++;
	}
	if ( checkDelimiter() )	return;

	if ( inputString( NULL, bios_UartRxBuff, BIOS_UART_RX_BUFF_SIZE, false ) == 0 ) return;

	if ( flg == ST_HEXCHR ) {
		*pvar = bios_UartRxBuff[0];
	} else {
		*pvar = str2var( bios_UartRxBuff );
	}
}

//*************************************************
uint8_t *get_GotoLabel( int32_t *lnum )
{
	var_t val = 0;
	uint8_t *str = NULL;

	if ( *executionPointer == ST_LABEL ) {
		if ( !isLabelChar( *++executionPointer ) ) val = lineNumber;
		else str = executionPointer;
	}
	else {
		val = expr();
	}
	if ( errorCode ) return NULL;
	if ( val == 0 && *lnum == 0 && str == NULL ) return NULL;
	uint8_t *goto_ptr = find_line( &val, &str );
	if ( str ) executionPointer = str;
	*lnum = val;
	return goto_ptr;
}

//*************************************************
static uint8_t *goto_Jump( void )
{
	int32_t lnum = 1;
	uint8_t *goto_ptr = get_GotoLabel( &lnum );
	if ( goto_ptr == NULL )	return NULL;

	lineNumber = lnum;
	uint8_t *next_ptr = executionPointer;
	executionPointer = goto_ptr;
	returnReq = REQUEST_GOTO;
	return next_ptr;
}

//*************************************************
PROC( proc_goto )
{
	goto_Jump();
}

//*************************************************
PROC( proc_gosub )
{
	Stack_t *prevsp = pushStack( ST_GOSUB );
	if ( prevsp == NULL )	return;
	prevsp->returnPointer = goto_Jump();
	if ( errorCode ) {
		stackPointer--;
	}
}

//*************************************************
PROC( proc_return )
{
	Stack_t *prevsp;

	if ( checkDelimiter() )	return;
	do {
		prevsp = popStack( 0 );
		if ( prevsp == NULL ) {
			errorCode = ERROR_UXRETURN;
			return;
		}
	} while( prevsp->type != ST_GOSUB );

	executionPointer = prevsp->returnPointer;
	lineNumber = prevsp->returnLineNumber;
}

//*************************************************
PROC( proc_for )
{
	uint8_t ch;
	var_t from, to, step, *pvar;
	Stack_t *prevsp;

	pvar = getParameterPointer();
	if ( pvar == NULL )	return;
	if ( chkST( '=' ) ) return;
	from = expr();
	if ( errorCode )	return;
	if ( chkST( ST_TO ) ) return;
	to = expr();
	if ( errorCode )	return;
	ch = *executionPointer++;
	if ( ch == ST_STEP ) {
		step = expr();
		if ( errorCode )	return;
	} else {
		step = 1;
		executionPointer--;	/* unget it */
	}
	prevsp = pushStack( ST_FOR );
	if ( prevsp == NULL )	return;
	*pvar = from;
	prevsp->pvar = pvar;
	prevsp->limit = to;
	prevsp->step = step;
}

//*************************************************
PROC( proc_next )
{
	if ( checkDelimiter() )	return;
	Stack_t *prevsp = popStack( ST_FOR );
	if ( prevsp == NULL ) {
		errorCode = ERROR_UXNEXT;
		return;
	}
	if ( prevsp->limit == *(prevsp->pvar) ) {
		return;
	}
	*(prevsp->pvar) += prevsp->step;
	if ( prevsp->step > 0 ) {
		if ( prevsp->limit < *(prevsp->pvar) ) {
			return;
		}
	} else {
		if ( prevsp->limit > *(prevsp->pvar) ) {
			return;
		}
	}
	stackPointer++;
	executionPointer = prevsp->returnPointer;
	lineNumber = prevsp->returnLineNumber;
}

//*************************************************
PROC( proc_do )
{
	if ( checkDelimiter() ) return;
	Stack_t *prevsp = pushStack( ST_DO );
	if ( prevsp == NULL )	return;
	prevsp->returnPointer = executionPointer - 1;
}

//*************************************************
PROC( proc_loop )
{
	var_t val;
	Stack_t *prevsp;
	uint8_t ch;

	prevsp = popStack( ST_DO );
	if ( prevsp == NULL ) {
		errorCode = ERROR_UXLOOP;
		return;
	}
	ch = *executionPointer;
	if ( ch == ST_WHILE ) {
		executionPointer++;
		val = expr();
		if ( checkDelimiter() ) return;
		if ( !val ) {
			return;
		}
	} else
	if ( checkDelimiter() ) return;
	executionPointer = prevsp->returnPointer;
	lineNumber = prevsp->returnLineNumber;
}

//*************************************************
static uint8_t *findNext( uint8_t *ptr )
{
	static const uint8_t st_list[] = { ST_NEXT, ST_FOR, 0 };
	uint8_t ch;;

	for ( uint8_t count = 1; count ; )
	{
		executionPointer = ptr;
		ptr = findST( st_list, &ch );
		if ( ptr == NULL )	break;
		if ( ch == ST_NEXT ) {
			count--;
		} else
		{
			count++;
		}
	}
	return ptr;
}

//*************************************************
static uint8_t *findLoop( uint8_t *ptr )
{
	static const uint8_t st_list[] = { ST_LOOP, ST_WHILE, ST_DO, 0 };
	uint8_t ch;

	for ( uint8_t count = 1; count ; )
	{
		executionPointer = ptr;
		ptr = findST( st_list, &ch );
		if ( ptr == NULL )	break;
		if ( ch == ST_LOOP ) {
			if ( --count && *ptr == ST_WHILE )	ptr++;
		} else
		{
			count++;
		}
	}
	return ptr;
}

//*************************************************
static uint8_t *skipToDelimiter( uint8_t *ptr )
{
	while ( !isDelimiter( *ptr ) ) ptr++;
	return ptr;
}

//*************************************************
PROC( proc_while )
{
	var_t val;
	Stack_t *prevsp;
	uint8_t *ptr;

	ptr = executionPointer;
	val = expr();
	if ( checkDelimiter() ) return;
	if ( val ) {
		prevsp = pushStack( ST_DO );
		if ( prevsp == NULL ) return;
		prevsp->returnPointer = ptr - 1;
	} else {
		ptr = findLoop( ptr );
		if ( ptr == NULL ) {
			errorCode = ERROR_NOLOOP;
			return;
		}
		executionPointer = skipToDelimiter( ptr );
	}
}

//*************************************************
PROC( proc_exit )
{
	uint8_t *ptr = proc_exit_cont_sub( ERROR_UXEXIT );
	if ( ptr == NULL ) return;
	stackPointer--;
	executionPointer = skipToDelimiter( ptr );
}

//*************************************************
PROC( proc_continue )
{
	uint8_t *ptr = proc_exit_cont_sub( ERROR_UXCONTINUE );
	if ( ptr == NULL ) return;
	executionPointer = ptr - 1;
}

//*************************************************
static uint8_t *proc_exit_cont_sub( errCode_t err )
{
	Stack_t *prevsp;
	uint8_t *ptr;

	if ( checkDelimiter() )	return NULL;
	if ( stackPointer )
	{
		prevsp = &stacks[stackPointer-1];
		if ( prevsp->type == ST_FOR )
		{
			ptr = findNext( executionPointer );
			if ( ptr )	return ptr;
		}
		else
		if ( prevsp->type == ST_DO  ) {
			ptr = findLoop( executionPointer );
			if ( ptr )	return ptr;
		}
	}
	errorCode = err;
	return NULL;
}

//*************************************************
PROC( proc_if )
{
	var_t val;
	uint8_t ch, *ptr;
	static const uint8_t st_list[] = { ST_ENDIF, ST_ELSE, ST_ELSEIF, 0 };

	do {
		val = expr();
		if ( errorCode )	return;
		if ( chkST( ST_THEN )) return;
		if ( val ) {
			ch = *executionPointer;
			if ( is_TypeNum( ch ) || ch == ST_LABEL ) {
				goto_Jump();
			}
			return;
		}
		ptr = findST( st_list, &ch );
		if ( ptr == NULL ) {
			errorCode = ERROR_NOENDIF;
			return;
		}
		executionPointer = ptr;
	} while ( ch == ST_ELSEIF );

	if ( ch == ST_ELSE ) {
		ch = *executionPointer;
		if ( is_TypeNum( ch ) || ch == ST_LABEL ) {
			goto_Jump();
		}
	}
}

//*************************************************
PROC( proc_else )
{
	static const uint8_t st_list[] = { ST_ENDIF, 0 };
	uint8_t ch, *ptr;

	ptr = findST( st_list, &ch );
	if ( ptr == NULL ) {
		errorCode = ERROR_NOENDIF;
		return;
	}
	executionPointer = ptr;
}

//*************************************************
PROC( proc_elseif )
{
	proc_else( ST_ELSE );
}

//*************************************************
PROC( proc_endif )
{
	checkDelimiter();
}

//*************************************************
PROC( proc_run )
{
	if ( isExeDelimiter() )
	{
		lineNumber = -1;
		executionPointer = PROGRAM_AREA_TOP;
	}
	else
	if ( goto_Jump() == NULL ) return;

	initializeVariables( false );
	returnReq = REQUEST_GOTO;
}

//*************************************************
PROC( proc_resume )
{
	if ( checkDelimiter() ) return;
	if ( resumePointer == NULL ) {
		errorCode = ERROR_RESUME;
		return;
	}
	executionPointer = resumePointer;
	lineNumber = resumeLineNumber;
}

//*************************************************
PROC( proc_stop )
{
	if ( checkDelimiter() )	return;
	executeBreak();
}

//*************************************************
static void executeBreak( void )
{
	while ( bios_UartGetChar() >= 0 );
	if ( lineNumber ) {
		resumePointer = executionPointer;
		resumeLineNumber = lineNumber;
	}
	errorCode = ERROR_BREAK;
}

//*************************************************
uint8_t basic_checkBreak( void )
{
	bios_ClearWDT();
	bios_TimerProcess();
	uint8_t brk = bios_UartGetBreak();
	if ( brk )
	{
		bios_UartFlush();
		executeBreak();
	}
	return brk;
}

//*************************************************
PROC( proc_end )
{
	if ( checkDelimiter() )	return;
	returnReq = REQUEST_EXIT;
	lineNumber = 0;
}

//*************************************************
PROC( proc_new )
{
//	CHECK_PROTECT();
	CHECK_RUN_MODE();
	CHECK_EDIT_MODE();
	if ( checkDelimiter() )	return;
	initializeVariables( true );
}

//*************************************************
static const char *basic_get_cmd_str( basic_CmdList_t const *lptr_p[], uint8_t code, uint8_t *sp )
{
	while ( *lptr_p )
	{
		basic_CmdList_t const *lptr = *lptr_p;
		if ((lptr->code_from <= code)&&(code <= lptr->code_to)) {
			code -= lptr->code_from;
			*sp = lptr->sp;
			if ( *sp == LIST_SP_BOTH ) {
				if ((code != ST_ELSE)&&(code != ST_ELSEIF)&&(code == ST_ENDIF)&&(code != ST_THEN)&&(code != ST_TO)&&(code != ST_STEP)) {
					*sp = LIST_SP_POST;
				}
			}
			return lptr->cmd_str[code];
		}
		lptr_p++;
	}
	return NULL;
}

#if LIST_HISTORY_ENABLE
static char list_history_buff[BIOS_UART_RX_BUFF_SIZE];
#define LIST_HISTORY_BUFF	list_history_buff
char *basic_listHistory_buff = LIST_HISTORY_BUFF;
uint16_t basic_listHistory_len = 0;
static char *list_buff_ptr;
#define list_BuffInit()		list_buff_ptr = basic_listHistory_buff
#define list_PutChar(c)		do { *list_buff_ptr++ = c; } while (0)
#define list_PutStr(str)	do { list_buff_ptr = strcpy2(list_buff_ptr, str); } while (0)
#define	list_BuffOut()		do { *list_buff_ptr = '\0';					\
	basic_listHistory_len = list_buff_ptr - basic_listHistory_buff;		\
	list_buff_ptr = basic_listHistory_buff;								\
	bios_UartPutLine(basic_listHistory_buff);} while (0)

#else
#define list_BuffInit()
#define list_PutChar(c)		bios_UartPutChar(c)
#define list_PutStr(str)	bios_UartPuts(str)
#define	list_BuffOut()		bios_UartPutCrLf();
#endif

static errCode_t basic_get_lnum_2( int32_t *num_s, int32_t *num_e )
{
	if ( !isExeDelimiter() )
	{
		if ( *executionPointer != ',' )
		{
			get_GotoLabel( num_s );
			if( *num_s == 0 ) return errorCode;
			errorCode = ERROR_OK;
		}
		if ( *executionPointer == ',' )
		{
			executionPointer++;
			if ( !isExeDelimiter() )
			{
				get_GotoLabel( num_e );
				if( *num_s == 0 ) return errorCode;
				errorCode = ERROR_OK;
			}
		} else
		{
			*num_e = *num_s;	
		}
		if ( checkDelimiter() ) return errorCode;
	}
	return errorCode;
}

//*************************************************
PROC( proc_list )
{
	var_t val;

	uint8_t *ptr = PROGRAM_AREA_TOP;
	int32_t	num_s = 1;
	int32_t	num_e = UINT16_MAX;

	CHECK_PROTECT();

	if ( basic_get_lnum_2( &num_s, &num_e) ) return;
	if ( num_s > num_e ) { errorCode = ERROR_PARA; }
	if ( errorCode ) return;

	list_BuffInit();
	while ( 1 )
	{
		uint8_t len = *ptr++;
		if ( len == ST_EOL ) break;
		uint8_t *next = ptr + len;
		uint8_t flag = 0x20;
		while ( true )
		{
			uint8_t ch = *ptr++;
			if ( ch == ST_EOL )
			{
				list_BuffOut();
				break;
			}
			flag >>= 1;
			if ( flag & 0x08 ) { list_PutChar( 0x20 ); flag = 0x01; }
			if ( ch == ':' )	{
				list_PutChar( ch );
				flag = 0x02;
				continue;
			}
			if ( flag & 0x04 ) { list_PutChar( 0x20 ); flag = 0x01; }

			if ( ch == ST_INDENT ) {
				uint8_t n = *ptr++;
				do {
					list_PutChar( 0x20 );
				} while( --n );
			} else
			if ( is_TypeNumDec( ch ) ) {			// ���l
				ptr = get_num( ptr, &val, ch );
				if ((flag & 0x10) && ((val < num_s) || (val > num_e))) {
					ptr = next;
					break;
				}
				list_PutStr(conv2str( val, FORM_NONE, 0 ));
			} else
			if ( is_TypeNumHex( ch ) ) {			// 16�i��
				ptr = get_num( ptr, &val, ch );
				list_PutChar( ST_HEXCHR );
				list_PutStr(conv2str( val, FORM_HEX, 0 ));
			} else
			if ( ch == ST_STRING ) {				// ������
				list_PutChar( ST_STRING );
				do {
					list_PutChar( *ptr );
				} while ( *ptr++ != ST_STRING );
			} else
			if ( ch == ST_COMMENT ) {				// �R�����g
				list_PutChar( ST_COMMENT );
				while ( *ptr != ST_EOL )
					list_PutChar( *ptr++ );
			} else
			if ( ch == ST_LABEL ) {					// ���x��
				list_PutChar( ST_LABEL );
				while ( !isDelimiter( *ptr ) )
					list_PutChar( *ptr++ );
			} else
			{
				char const *pre_str = NULL;
				basic_CmdList_t const **lptr = basic_CmdList;
#ifdef __UI_LORA_WAN__
				if ( ch == ST_CODE_WAN )
				{
					pre_str = Wan_pre_str;
					lptr = basic_Wan_CmdList;
					ch = *ptr++;
				}
#endif
				uint8_t sp;
				char const *cmd_str = basic_get_cmd_str( lptr, ch, &sp );
				if ( cmd_str )
				{
					if ((sp & LIST_SP_PRE) && !(flag & 0x01)) list_PutChar( 0x20 );
					list_PutStr( pre_str );
					list_PutStr( cmd_str );
					if ((sp & LIST_SP_POST) && (*ptr != '.')) flag = 0x08;
				} else {
					list_PutChar( ch );
					if ( ((*ptr == ST_PRINT) || (*ptr == ST_UPRINT)) && Ui_IsEncode( ch ) ) flag = 0x02;
				}
			}
		}
	}
	bios_UartPrintfLine( "[%ld/%u bytes]", progLength, PROGRAM_AREA_SIZE );
}

//*************************************************
PROC( proc_renum )
{
	uint8_t len, *ppp, *ptr = PROGRAM_AREA_TOP;
	var_t num, new_num, old_num;
	int16_t tmp;

//	CHECK_PROTECT();
	CHECK_RUN_MODE();
	CHECK_EDIT_MODE();
	if ( get_1argVal( &new_num ) )	return;
	if ( new_num == 0 )	new_num = 10;

	while ( true )
	{
		len = *ptr++;
		if ( len == ST_EOL )	break;
		if ( is_TypeNumDec( *ptr ) )
		{
			get_num( ptr+1, &old_num, *ptr );
			put_num( ptr, new_num, *ptr, &tmp );		// �V�ԍ��Z�b�g
			ppp = NULL;
			while ( (ppp = find_LineNumber( ppp )) != NULL )
			{
				if ( is_NumRenum( *ppp ) )	continue;		// Renum �� ?
				get_num( &ppp[1], &num, *ppp );				// �ԍ��擾
				if ( num != old_num )	continue;			// ��v ?
				*ppp |= NUM_RENUM_FLAG;
				ppp = put_num( ppp, new_num, *ppp, &tmp );	// �V�ԍ��Z�b�g <=== RENUM_FLAG
			}
			new_num += 10;
		}
		ptr += len;
	}
	ppp = NULL;
	while ( (ppp = find_LineNumber( ppp )) != NULL )
	{
		*ppp &= ~NUM_RENUM_FLAG;
	}
}

//*************************************************
static uint8_t isSTwithLineNumber( uint8_t ch )
{
	return ( ch==ST_GOTO||ch==ST_GOSUB||ch==ST_THEN||ch==ST_ELSE||ch==ST_CATCH||ch==ST_RUN||ch==ST_RESTORE );
}

//*************************************************
static uint8_t *find_LineNumber( uint8_t *ptr )
{
	if ( ptr == NULL )
	{
		ptr = PROGRAM_AREA_TOP + 1;
	}
	do {
		while ( true )
		{
			uint8_t ch = *ptr++;
			if ( ch == ST_EOL )	break;
			if ( isSTwithLineNumber( ch ) ) {
				if ( is_TypeNumDec( *ptr ) )	return ptr;
			} else
			if ( ch == ST_COMMENT ) {
				while ( *ptr++ != ST_EOL );
				break;
			} else
			if ( ch == ST_STRING ) {
				while ( *ptr++ != ST_STRING );
			} else
			if ( ch == ST_LABEL ) {
				ptr = skipToDelimiter( ptr );
			} else
			if ( is_TypeNum( ch ) )
			{
				ptr += get_numBytes( ch );
			}
		}
	} while ( *ptr++ != ST_EOL );
	return NULL;
}

//*************************************************
uint32_t get_prog_length( uint8_t *ptr )
{
	uint8_t len, *p = ptr;

	if ( *ptr == ST_EOL ) return 0;
	while ( (len = *p ) != ST_EOL )
	{
		p += len + 1;
	}
	return (uint32_t)(p - ptr + 1);
}

//*************************************************
PROC( proc_edit )
{
	var_t val;

	CHECK_PROTECT();
	CHECK_RUN_MODE();
	if ( get_1argVal( &val ) )	return;
	if ( val < 0 || val > 2 ) { errorCode = ERROR_PARA; return; }
	progEditMode = val;
}

//*************************************************
static void move_line( uint8_t *dst, uint8_t *src )
{
	if ( dst == src )	return;

	uint32_t len = get_prog_length( src );

#if _USE_UTILITY_MEMCPY2_
	dst = memcpy2( dst, src, len );
	*dst = ST_EOL;
#else
	if ( src > dst )		// �l�߂�
	{
		while ( len-- ) { *dst++ = *src++; }
		*dst = ST_EOL;
	}
	else				// �L����
	{
		src += len;
		dst += len;
		*dst = ST_EOL;
		while ( len-- ) { *--dst = *--src; }
	}
#endif
}

//*************************************************
static void prog_regist( uint8_t *line )
{
	uint8_t	*line_next, *ptr = PROGRAM_AREA_TOP;
	var_t	ins_num, now_num, del_len, ins_len;

	line_next = get_num( line+2, &ins_num, line[1] );
	if ( line_next == NULL || ins_num < 1 )
	{
		errorCode = ERROR_SYNTAX;
		return;
	}

	while ( true )
	{
		del_len = ptr[0];
		if ( del_len == ST_EOL ) break;				// �s�ǉ� del_len = 0
		del_len++;
		if ( is_TypeNumDec( ptr[1] ) )
		{
			get_num( &ptr[2], &now_num, ptr[1] );
			if ( now_num >= ins_num ) break;		// �폜 or �}�� or �u��
		}
		ptr += del_len;
	}

	ins_len = (*line_next == ST_EOL) ? 0 : *line + 1;	// �}���s�� 0=�폜�̂�
	if ( (progLength - del_len + ins_len) >  PROGRAM_AREA_SIZE-2 )
	{
		errorCode = ERROR_PGOVER;
		return;
	}

	if ( del_len )								// �ǉ��ł͂Ȃ� ?
	{
		if ( now_num == ins_num )				// �폜 or �u�� ?
		{
			move_line( ptr + ins_len, ptr + del_len );
			progLength -= del_len;
			if ( progLength == 1 ) progLength = 0;
		}
		else
		{
			if ( ins_len == 0 )	return;
			move_line( ptr + ins_len, ptr );	// �}��
		}
	}

	if ( ins_len )
	{
		if ( progLength == 0 ) progLength = 1;
		progLength += ins_len;
	}
	memcpy( ptr, line, ins_len );				// �R�s�[
	if ( del_len == 0 )							// �ǉ��̂Ƃ�
	{
		ptr[ins_len] = ST_EOL;
	}
}

//*************************************************
static uint8_t *get_line_num( uint8_t *ptr, var_t *num )
{
	uint8_t ch, len, *p;

	*num = 0;
	if ( *ptr == ST_EOL )	return NULL;
	p = ptr++;
	len = *p++;
	ch = *p++;
	if ( is_TypeNumDec( ch ) )
	{
		get_num( p, num, ch );
	}
	return ptr + len;
}

//*************************************************
static uint32_t basic_get_prog_range( uint8_t **ptr_s, uint8_t **ptr_e )
{
	uint8_t *ptr = PROGRAM_AREA_TOP;
	uint32_t len = 0;
	var_t num = 0;
	var_t num_s = 0;
	var_t num_e = 0;

	if ( basic_get_lnum_2( &num_s, &num_e ) ) return 0;
	if ( num_s > num_e ) { errorCode = ERROR_PARA; }
	if ( errorCode ) return 0;
	if ( !num_s && !num_e ) return 0;

	while ( true )
	{
		*ptr_s = ptr;
		if ( !num_s ) break;
		if ( (ptr = get_line_num( ptr, &num )) == NULL ) { errorCode = ERROR_LABEL; return 0; }
		if ( num >= num_s )	break;
	}
	if ( num > num_e ) { errorCode = ERROR_LABEL; return 0; }
	while ( true )
	{
		*ptr_e = ptr;
		if ( (ptr = get_line_num( ptr, &num )) == NULL ) break;
		if ( num > num_e )	break;
	}
	len = PROGRAM_AREA_END - *ptr_e;
	return len;
}

//*************************************************
PROC( proc_delete )
{
	uint8_t *ptr_s;
	uint8_t *ptr_e;

//	CHECK_PROTECT();
	CHECK_EDIT_MODE();

	uint32_t len = basic_get_prog_range( &ptr_s, &ptr_e );
	if ( errorCode ) return;
	if ( len == 0 )	{ errorCode = ERROR_PARA; return; }

	if( ptr_s <= executionPointer ) {
		CHECK_RUN_MODE();
	}

	memcpy( ptr_s, ptr_e, len );
	progLength = get_prog_length( PROGRAM_AREA_TOP );
	sweepStack( ptr_s );
}

//*************************************************
PROC( proc_pload )
{
	var_t val = 0;
	bool res = false;
	bool run_flag = false;
	uint8_t ch = *executionPointer;

	if( ch == ENCODE_BASE64 || ch == ENCODE_PERCENT ) {
		executionPointer++;
		if( *executionPointer == '+' ) {
			executionPointer++;
			res = true;
		}
		if( ch == ENCODE_PERCENT ) {
			if ( get_XargVal_def( &val, 1) ) return;
//			if ( val != 0 && val < 10 )	{ errorCode = ERROR_PARA; return; }
		}
	} else
	if( ch == '!' ) {
		executionPointer++;	
		run_flag = true;	
	}

	if ( checkDelimiter() )	return;
	if( res == false ) {
		CHECK_RUN_MODE();
	}
	pload_base64_passkey = 0;
	switch( ch )
	{
		case ENCODE_BASE64 :			// &
			res = basic_pload_base64( res );
			if ( !res ) {			
				pload_base64_passkey = Settings.PassKey;
				if ( pload_base64_passkey < 0 ) {
					Settings.Protect = true;
				}
			}
			break;
		case ENCODE_PERCENT :			// %
			res = basic_pload_update( res, val * 1000 );
			break;
		default :
			res = bios_FlashProgLoad( PROGRAM_AREA_TOP, PROGRAM_AREA_SIZE );
			break;
	}
	if ( res ) {
		errorCode = ERROR_NOPROG;
		return;
	}
	progLength = get_prog_length( PROGRAM_AREA_TOP );
	resumePointer = NULL;
	if( run_flag ) proc_run( st_code );
}

//*************************************************
PROC( proc_psave )
{
	CHECK_RUN_MODE();

	bool protect_flag = false;
	uint8_t ch = *executionPointer;
	if(	(ch == ENCODE_BASE64 ) ||
		(ch == ENCODE_PERCENT) ||
		(ch == ENCODE_HEXDEC && basic_PassLevel >= PassLevel_2) )
	{
		CHECK_PROTECT();

		executionPointer++;
		uint8_t *ptr_s;
		uint8_t *ptr_e;
		uint32_t len = basic_get_prog_range( &ptr_s, &ptr_e );
		if( errorCode ) return;
		if ( checkDelimiter() )	return;
		if( len == 0 ) {
			ptr_s = PROGRAM_AREA_TOP;
			len = progLength;
		} else {
			len = ptr_e - ptr_s;
		}
		if ( !len ) ch = 0;
		switch( ch )
		{
			case ENCODE_HEXDEC :			// $
				if( basic_psave_hexdec( ptr_s, len ) ) break;
				return;
			case ENCODE_BASE64 :			// &
				if( basic_psave_base64( ptr_s, len ) ) break;
				return;
			case ENCODE_PERCENT :			// %
				if( basic_psave_update( ptr_s, len ) ) break;
				return;
		}
		errorCode = ERROR_NOPROG;
		return;
	}

	if( pload_base64_passkey == 0 ) {
		CHECK_PROTECT();
		CHECK_EDIT_MODE();
	}

	if ( ch == ENCODE_IGNORE ) {
		executionPointer++;
		protect_flag = true;
	}

	var_t val = 0;
	if ( get_XargVal_def( &val, 1) ) return;

	uint8_t *buff = NULL;
	uint16_t len = PROGRAM_AREA_SIZE;
	if ( val == 0 ) {
		if ( progLength == 0 ) {
			if ( basic_PassLevel < PassLevel_2 ) {
				errorCode = ERROR_NOPROG;
				return;
			}
		} else {
			len = progLength;
			buff = PROGRAM_AREA_TOP;
		}
	} else
	if ( val > 0 ) {
		errorCode = ERROR_PARA;
		return;
	}
	if ( bios_FlashProgSave( buff, len ) )
	{
		errorCode = ERROR_FLASH;
		return;
	}
	Settings.Protect |= protect_flag;
	if ( pload_base64_passkey < 0 && FLASH_SETTINGS->Protect == false ) {
		Ui_Settings_Save();
	}
}

//*************************************************
PROC( proc_delay )
{
	var_t val;

	if ( get_1argVal( &val ) )	return;
	bios_setWaitTick( val );
	while ( !Ui_IsBreak_BK() && bios_getWaitTick() > 0 );
}

//*************************************************
PROC( proc_pause )
{
	if ( checkDelimiter() )	return;
	while ( !Ui_IsBreak_BK() && bios_UartGetChar() < 0 );
}

//*************************************************
PROC( proc_randomize )
{
	var_t val;

	if ( get_1argVal( &val ) )	return;
	bios_Randomize( val );
}

//*************************************************
PROC( proc_data )
{
	while ( true ) {
		uint8_t ch = *executionPointer;
		if ( isDelimiter( ch ) ) {
			return;
		}
		if ( is_TypeNum( ch ) ) {
			executionPointer += get_numBytes( ch );
		}
		executionPointer++;
	}
}

//*************************************************
PROC( proc_read )
{
	var_t *pvar, val;
	uint8_t ch, *ptr, *ptrsave;
	int32_t numsave;
	static const uint8_t st_list[] = { ST_DATA, 0 };

	while ( true ) {
		pvar = getParameterPointer();
		if ( pvar == NULL )	return;

		numsave = lineNumber;
		ptrsave = executionPointer;
		executionPointer = ( dataReadPointer ) ? dataReadPointer : (PROGRAM_AREA_TOP + 1);

		do {
			if ( *executionPointer != ',' )
			{
				ptr = findST( st_list, &ch );
				if ( ptr == NULL ) {
					errorCode = ERROR_UXREAD;
					break;
				}
				executionPointer = ptr;
			} else {
				executionPointer++;
			}
			val = expr();
			if ( errorCode ) {
				break;
			}
			*pvar = val;
			ch = *executionPointer;
			if ( isDelimiter( ch ) || ch == ',' ) {
				break;
			}
			errorCode = ERROR_PARA;
		} while ( false );

		dataReadPointer = executionPointer;
		executionPointer = ptrsave;
		lineNumber = numsave;
		if ( errorCode ) return;
		if ( *executionPointer != ',' ) {
			checkDelimiter();
			return;
		}
		executionPointer++;
	}
}

//*************************************************
PROC( proc_restore )
{
	if ( isExeDelimiter() )
	{
		dataReadPointer = NULL;
		return;
	}
	int32_t lnum = 0;
	uint8_t *goto_ptr = get_GotoLabel( &lnum );
	if ( errorCode )	return;
	dataReadPointer = goto_ptr;
}

//*************************************************
PROC( proc_catch )
{
	if ( isExeDelimiter() )
	{
		errorCode = ERROR_PARA;
		return;
	}
	int32_t lnum = 0;
	uint8_t *goto_ptr = get_GotoLabel( &lnum );
	if ( errorCode )	return;
	errorCatchPointer = goto_ptr;
	errorCatchLineNumber = lnum;
}

//*************************************************
PROC( proc_reset )
{
	if ( checkDelimiter() )	return;
	bios_SystemReset();
}

//*************************************************
static void proc_sleep_and_deep( bool deep )
{
	var_t val[2] = {0};

	if ( !get_XargVal_def( val, 2 ) )
	{
		bios_Sleep( deep, val[0], val[1] );
	}
}

//*************************************************
PROC( proc_sleep )
{
	proc_sleep_and_deep( false );
}

//*************************************************
PROC( proc_deep )
{
	proc_sleep_and_deep( true );
}

//*************************************************
PROC( proc_ver )
{
	if ( checkDelimiter() )	return;
	bios_PutVersion();
}

//*************************************************
PROC( proc_outp )
{
	var_t val_1, val_2;

	if ( get_2argVal( &val_1, &val_2 ) )	return;
	if ( bios_GpioOutput( val_1, val_2 ) )
	{
		errorCode = ERROR_PARA;
	}
}

//*************************************************
PROC( proc_swap )
{
	var_t *p1 = getParameterPointer();
	if ( p1 == NULL )	return;
	if ( chkST( ',' ) )	return;
	var_t *p2 = getParameterPointer();
	if ( p2 == NULL )	return;
	var_t temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}

//*************************************************
PROC( proc_lclr )
{
	bios_Lcd_cls();
}

//*************************************************
PROC( proc_lprint )
{
	bios_Lcd_print();
}

/*
 * Nop
 */
PROC( proc_nop )
{
}

/*
 * Error
 */
PROC( proc_error )
{
	errorCode = ERROR_SYNTAX;
}

static bool basic_psave_hexdec( uint8_t *top, uint32_t size )
{
#if 0
	if( top == NULL ) {
		top = PROGRAM_AREA_TOP;
		size = progLength;
	}
	if ( !size ) return true;
#endif
	uint8_t *ptr = top;

	while ( size )
	{
		int16_t len = size > 16 ? 16 : size;
		Ui_Hexdec_Encode( bios_UartPutChar, ptr, len );
		bios_UartPutCrLf();
		ptr += len;
		size -= len;
	}
	return false;
}

typedef struct {
	uint32_t addr;
	uint32_t cont;
} prog_aes_key_t;

typedef struct
{
	union {
	  prog_aes_key_t key;
	  struct {
		uint32_t nonce;
		uint16_t length;
		uint16_t sum;
	  };
	};
	uint8_t version[28];
	uint8_t dev_id[8];
	int32_t passkey;
} prog_base64_header_t;

#define	PSAVE_BASE64_ENCRYPT_ENABLE		1
#define PSAVE_LINE_SIZE					64
#define	PSAVE_BLOCK_SIZE				((PSAVE_LINE_SIZE * 3) / 4)

#if PSAVE_BASE64_ENCRYPT_ENABLE && __LORA_AES_ENABLE__
#include "LoRaMacCrypto.h"
#else
#undef PSAVE_BASE64_ENCRYPT_ENABLE
#define	PSAVE_BASE64_ENCRYPT_ENABLE		0
#endif

static void prog_base64_Encrypt( uint8_t *src, uint8_t *dst, uint32_t size, prog_aes_key_t *key, uint8_t seq )
{
#if PSAVE_BASE64_ENCRYPT_ENABLE
	union {
		uint8_t  buff_b[AESKEY_LEN];
		uint32_t buff_l;
	} aes;

	uint8_t *ptr = Settings.AesEnable ? Settings.AesKey : (uint8_t*)&_Product_.Right[46];
	memcpy( aes.buff_b, ptr, sizeof(aes) );
	aes.buff_l ^= Settings.PassKey;

	LoRaMacPayloadEncrypt( src, size, aes.buff_b, key->addr, seq, key->cont, dst );
#else
	memcpy( dst, src, size );
#endif
}

/*
 *
 */
static bool basic_psave_base64( uint8_t *top, uint32_t size )
{
	uint32_t len;
	prog_base64_header_t hdr;
	uint8_t seq = 0;
	prog_aes_key_t enc_key;

#if 0
	if( top == NULL ) {
		top = PROGRAM_AREA_TOP;
		size = progLength;
	}
	if ( !size ) return true;
#endif
	uint8_t *ptr = top;

	memcpy( hdr.dev_id , Adjust.Device_Id, sizeof(hdr.dev_id) );
	memcpy( hdr.version, (uint8_t*)&_Product_, sizeof(hdr.version) );
	hdr.passkey = Settings.PassKey;
	hdr.nonce = bios_GetRandom();
	hdr.length = size;
	hdr.sum = basic_sum( ptr, size );
	len = sizeof(prog_base64_header_t);

	enc_key.addr = hdr.nonce;
	enc_key.cont = 0;
	uint8_t *p = set_VarConv( hdr.nonce, (uint8_t*)bios_UartRxBuff, NUM_SIZE_LONG );
	prog_base64_Encrypt( (uint8_t*)&hdr.length, p, len - sizeof(hdr.nonce), &enc_key, seq );

	goto put;

	while ( size )
	{
		len = (size > PSAVE_BLOCK_SIZE) ? PSAVE_BLOCK_SIZE : size;
		prog_base64_Encrypt( ptr, (uint8_t*)bios_UartRxBuff ,len, &hdr.key, ++seq );
		ptr += len;
		size -= len;
put:
		bios_UartPutChar( ST_COMMENT );
		Ui_Base64_Encode( bios_UartPutChar, (uint8_t*)bios_UartRxBuff, len );
		bios_UartPutCrLf();
	}
	return false;
}

/*
 *
 */
static bool basic_pload_base64( bool append )
{
	uint8_t *top = PROGRAM_AREA_TOP;
	uint32_t size = 0;
	uint32_t len;
	prog_base64_header_t hdr;
	uint8_t seq = 0;
	prog_aes_key_t enc_key;

	if ( append && progLength ) {
		top = &top[progLength] - 1;
	}
	uint8_t *ptr = top;

	int16_t res = 0;
	while ( 1 )
	{
		res = inputString( NULL, bios_UartRxBuff, BIOS_UART_RX_BUFF_SIZE-2, false );
		if ( res < 0 ) return false;
		if ( res < 1 ) return true;
		char *src = bios_UartRxBuff;
		res = Ui_Base64_Decode( (uint8_t*)&hdr, &src, 0 );
		if ( res >= sizeof(hdr.nonce) )
		{
			len = res;
			enc_key.addr = hdr.nonce;
			enc_key.cont = 0;
			prog_base64_Encrypt( (uint8_t*)&hdr.length, (uint8_t*)&hdr.length, len - sizeof(hdr.nonce), &enc_key, seq );
			if ( !memcmp( hdr.version, (uint8_t*)&_Product_, 22 ) && ( hdr.passkey == Settings.PassKey) ) seq = 1;
			break;
		}
	}

	while ( 1 )
	{
		res = inputString( NULL, bios_UartRxBuff, BIOS_UART_RX_BUFF_SIZE-2, false );
		if ( res < 1 ) break;
		if ( !seq ) continue;
		char *src = bios_UartRxBuff;
		res = Ui_Base64_Decode( (uint8_t*)src, &src, 0 );
		if ( res > 0 )
		{
			len = res;
			if( (ptr + len) > (PROGRAM_AREA_TOP + PROGRAM_AREA_SIZE) ) {
				errorCode = ERROR_OVERFLOW;
				break;
			}
			prog_base64_Encrypt( (uint8_t*)bios_UartRxBuff, (uint8_t*)ptr, len, &hdr.key, seq++ );
			ptr += len;
			size += len;
		}
	}
	if ( res == 0 )
	{
		if ( !seq )
		{
			bios_UartPutLine( "Not an LRA1 program or invalid key." );
			return false;
		}
		if ( size == 0 ) return true;
		if ( size == hdr.length )
		{
			uint16_t sum = basic_sum( top, size );
			if ( sum == hdr.sum ) return false;
		}
	}
	bios_UartPutLine( "Invalid program." );
	*top = ST_EOL;
	return false;
}

//*************************************************
PROC( proc_memdump )
{
	if( basic_CheckPassLevel_2() )	return;
	
	var_t val[2] = { 0, -1 };
	if ( get_XargVal_def( val, 2 ) ) return;

	uint32_t p = val[0];
	uint32_t n = (val[1] > 0) ? val[1] : 16 - (p & 0x0f);
	
	if( p == 0x00804000 || p == 0x00806020 || p == 0x00806030 ) { n = 8; }
	else
	if (( n < 1 ) || (
	    ( (p < FLASH_ADDR ) || ((p + n) > (FLASH_ADDR + FLASH_SIZE)) ) &&
		( (p < HSRAM_ADDR ) || ((p + n) > (HSRAM_ADDR + HSRAM_SIZE)) ) &&
		( (p < LPRAM_ADDR ) || ((p + n) > (LPRAM_ADDR + LPRAM_SIZE)) ) ))
	{
		errorCode = ERROR_PARA;
		return;
	}

	uint8_t f = 0;
	for( ;n ; n--, p++ )
	{
		if( !f ) {
			bios_UartPrintf( "%07x0 :"	, (unsigned int)(p >> 4) );
			uint32_t x = p & 0x0f;
			f = 16 - x;
			while( x ) {
				bios_UartPuts("   ");
				x--;
			}
		}
		bios_UartPrintf( " %02x" , *((uint8_t*)p) );
		f--;
		if( !f ) bios_UartPutCrLf();
	}
	if( f ) bios_UartPutCrLf();
}
