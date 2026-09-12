/*!
 * @file    basic_sub.c
 * @brief   Subroutines for Basic Interpreter
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
#include "asf.h"
#include "string.h"
#include "ctype.h"
#include "inttypes.h"
#include "basic.h"
#include "bios.h"
#include "utilities.h"

uint32_t basic_EorTimer = 0;
static uint8_t basic_CaretStrBuff[BASIC_CARET_STR_BUFF_SIZE];
static uint8_t basic_CaretStrLen = 0;
static char *last_string_buff;
static uint8_t inputStringEncode = 0;

static bool basic_GetCaretVal( var_t *val );
static void basic_SetCaret( void );
static char *basic_GetCaretStr( char *buff );
static int32_t basic_GetLabelNum( void );
static uint8_t get_bracket_type( uint8_t ch, int8_t func );
static uint8_t *str2form( uint8_t *fm, int32_t *len );
static var_t expr4th( void );
static var_t expr3rd( void );
static var_t expr2nd( void );
static char *get_ValPrint( char *buff );
static errCode_t proc_let( var_t *pvar, uint8_t ope );
static int32_t byteswap16( int32_t val );
static int32_t byteswap32( int32_t val );

#define IS_DIVZERO( val )	(val ? 0 : (errorCode = ERROR_DIVZERO))

#define INPUT_EDIT_ENABLE			1
#define INPUT_HISTORY_ENABLE		1

//*************************************************
errCode_t proc_LetVariable( var_t *pvar )
{
	return proc_let( pvar, *executionPointer );
}

//*************************************************
errCode_t proc_LetSpecial( var_t *pvar )
{
	return proc_let( pvar, 0 );
}

//*************************************************
static errCode_t proc_let( var_t *pvar, uint8_t ope )
{
	if ( ope == executionPointer[1] )
	{
		executionPointer += 2;
		if ( ope == '+' ) { if ( !checkDelimiter() ) (*pvar)++; return errorCode; }
		if ( ope == '-' ) { if ( !checkDelimiter() ) (*pvar)--; return errorCode; }
		if ( ope !='<' && ope != '>' ) return errorCode = ERROR_SYNTAX;
	} else
	if ( IS_OPERATOR_CHR( ope ) )
	{
		++executionPointer;
	}
	if ( chkST( '=' ) ) return errorCode;
	var_t val = expr();
	if ( errorCode ) return errorCode;
	switch ( ope )
	{
	case '+' : *pvar += val; break;
	case '-' : *pvar -= val; break;
	case '*' : *pvar *= val; break;
	case '/' : if ( !IS_DIVZERO( val ) ) *pvar /= val; break;
	case '%' : if ( !IS_DIVZERO( val ) ) *pvar %= val; break;
	case '|' : *pvar |= val; break;
	case '&' : *pvar &= val; break;
	case '^' : *pvar ^= val; break;
	case '<' : *pvar <<= val; break;
	case '>' : *pvar >>= val; break;
	default  : *pvar = val; break;
	}
	return errorCode;
}

//*************************************************
errCode_t get_1argVal( var_t *val )
{
	*val = 0;
	if ( !isExeDelimiter( ) )
	{
		*val = expr();
		if ( !checkDelimiter() && *val < 0 )		//�ȗ��\(val=0)�A�}�C�i�X�s��
		{
			errorCode = ERROR_PARA;
		}
	}
	return errorCode;
}

//*************************************************
errCode_t get_2argVal( var_t *val_1, var_t *val_2 )
{
	*val_1 = expr();
	if ( !errorCode )
	{
		if ( !chkST( ',' ) )
		{
			*val_2 = expr();
			checkDelimiter();
		}
	}
	return errorCode;
}

//*************************************************
errCode_t get_XargVal_def( var_t *val, uint8_t num )
{
	bool flag = false;
	while ( 1 )
	{
		if ( isExeDelimiter( ) )	return errorCode;
		if ( !num ) break;
		if ( *executionPointer == ',' )
		{
			if ( num == 1 ) break;
			executionPointer++;
			val++;
			num--;
			flag = false;
		} else
		if ( !flag )
		{
			*val = expr();
			flag = true;
		} else
		{
			break;
		}
	}
	errorCode = ERROR_SYNTAX;
	return errorCode;
}

//*************************************************
bool isLabelChar( uint8_t ch )
{
	return (isalnum(ch) || ch==ST_LABEL || ch == '.');
}

//*************************************************
bool isDelimiter( uint8_t ch )
{
	return ((ch == ':')||(ch == ST_EOL)||(ch == ST_ELSE)||(ch == ST_ELSEIF)||(ch == ST_ENDIF));
}

//*************************************************
bool isExeDelimiter( void )
{
	return isDelimiter( *executionPointer );
}

//*************************************************
errCode_t checkDelimiter( void )
{
	if ( !isExeDelimiter( ) )
	{
		errorCode = ERROR_SYNTAX;
	}
	return errorCode;
}

//*************************************************
errCode_t chkST( uint8_t ch )
{
	if ( *executionPointer++ != ch )
	{
		errorCode = ERROR_SYNTAX;
	}
	return errorCode;
}

//*************************************************
static char* copyString( char *buff, const char *ptr )
{
	if ( ptr == NULL ) return buff;
	int16_t len = strlen( ptr );
	char *str = buff + len;
	if ( str - bios_UartTxBuff >= BIOS_UART_TX_BUFF_SIZE )
	{
		errorCode = ERROR_OVERFLOW;
		return buff;
	}
	memcpy( buff, ptr, len );
	return str;
}

//*************************************************
static char *get_StringPara_Decode( int16_t func(uint8_t*, char**, char), char *buff )
{
	uint8_t ch = *executionPointer++;
	if ( ch == ST_STRING )
	{
		buff += func( (uint8_t*)buff, (char**)&executionPointer, ST_STRING );
	} else
	if ( ch == ST_CARET )
	{
		uint8_t *ptr = basic_CaretStrBuff;
		buff += func( (uint8_t*)buff, (char**)&ptr, 0 );
	} else
	{
 		errorCode = ERROR_SYNTAX;
	}
	return buff;
}

//*************************************************
static char *get_StringPara_Form( uint8_t fm )
{
	int32_t len = 0;
	var_t val;

	if ( chkST( '(' ) ) return NULL;
	if ( fm == 0 )
	{
		if ( !str2form( &fm, &len ) ) return NULL;
		if ( chkST( ',' ) ) return NULL;
		val = expr();
	} else
	{
		val = expr();
		if ( errorCode ) return NULL;
		if ( *executionPointer == ',' )
		{
			executionPointer++;
			len = expr();
		}
	}
	if ( errorCode ) return NULL;
	if ( chkST( ')' ) ) return NULL;
	return conv2str( val, fm, len );
}

//*************************************************
void clearStringBuff( void )
{
	last_string_buff = bios_UartTxBuff;
	basic_CaretStrLen = 0;
}

//*************************************************
static char *string_DateTime( char *buff, int len )
{
	var_t val;
	if ( !calcValueFunc_1( &val ) ) {
		int t[6];
		gps_ConvDatetime( val, (int32_t*)t );
		if ( len > BIOS_UART_TX_BUFF_SIZE-18 ) { errorCode = ERROR_OVERFLOW; }
		else {
			sprintf( buff, "%02d/%02d/%02d %02d:%02d:%02d", t[0], t[1], t[2], t[3], t[4], t[5] );
			buff += 17;
		}
	}
	return buff;
}

//*************************************************
int16_t get_StringPara( uint8_t flag )
{
static uint8_t *recursive_ptr = NULL;
	char *ptr, *buff = bios_UartTxBuff;
	var_t	val;
	int16_t len = 0;
	uint8_t lastCh = 0;
	uint8_t size;

	while ( errorCode == ERROR_OK )
	{
		bool val_flag = false;
		uint8_t ch = *executionPointer;
		if ( flag & STRING_PARA_FLAG_VARIABLE ) { 
			if ( IS_CLOSE_BRACKET(ch) || ch == ST_COMMA )	break;
		}
		if ( isDelimiter( ch ) )
		{
			if ((flag & STRING_PARA_FLAG_CRLF_ENABLE) && (lastCh != ST_SEMICOLON) && (lastCh != ST_COMMA))
			{
				*buff++ = '\r';
				*buff++ = '\n';
				len += 2;
			}
			break;
		}
		executionPointer++;
		switch ( ch ) {
		case ENCODE_PERCENT :
			buff = get_StringPara_Decode( Ui_Percent_Decode, buff );
			break;	
		case ENCODE_BASE64 :
			buff = get_StringPara_Decode( Ui_Base64_Decode, buff );
			break;
		case ENCODE_HEXDEC :
			if ( *executionPointer == '(' )
			{
				size = FORM_HEX;
				goto func_para_form;
			}
			buff = get_StringPara_Decode( Ui_Hexdec_Decode, buff );
			break;
		case ST_ZERO :
			if ( *executionPointer == '(' )
			{
				size = FORM_DEC;
				goto func_para_form;
			}
			val_flag = true;
			break;
		case ST_STRING :
			while ( (ch = *executionPointer++) != ST_STRING )
			{
				*buff++ = (char)ch;
			}
			break;
		case ST_COMMA :
			if ( flag & STRING_PARA_FLAG_COMMA_ENABLE )	*buff++ = '\t';
		case ST_SEMICOLON :
			break;
		case ST_PERIOD :
			buff = last_string_buff;
			break;
		case ST_CARET :
			if ( IS_OPEN_BRACKET(*executionPointer) ) val_flag = true;
			else buff = basic_GetCaretStr( buff );
			break;
		case ST_COMMENT :
			if ( flag & STRING_PARA_FLAG_COMMA_ENABLE ) buff = copyString( buff, "\033[f\033[2J" );
		case VAR_NULL :
			break;
		case FUNC_CHR :
			size = NUM_SIZE_BYTE;
			goto func_chr_conv;
		case FUNC_WCHR :
			size = NUM_SIZE_WORD;
			goto func_chr_conv;
		case FUNC_LCHR :
			size = NUM_SIZE_LONG;
			goto func_chr_conv;
		case FUNC_MCHR :
			size = NUM_SIZE_MID;
func_chr_conv:
			if ( calcValueFunc_1( &val ) ) break;
			buff = (char*)set_VarConv( val, (uint8_t*)buff, size );
			break;
		case ST_LORA_DATETIME :
			buff = string_DateTime(buff, len);
			break;
		case FUNC_FORM :
			size = FORM_NONE;
func_para_form:
			ptr = get_StringPara_Form( size );
			buff = copyString( buff, ptr );
			break;
		case VAR_AUTO :
			ptr = bios_GetAutorun();
			buff = copyString( buff, ptr );
			break;
		case VAR_GETS :
		case VAR_UGETS :
			val = 0;
			inputStringEncode = ENCODE_RAW;
			if ( *executionPointer == ENCODE_DENBUN ) { inputStringEncode = *executionPointer++; }
			if ( *executionPointer == '(' && calcValueFunc_1( &val ) ) break;
			int16_t res = val_inputString( buff, BIOS_UART_RX_BUFF_SIZE - len, val, (ch == VAR_UGETS) );
			if ( res > 0 ) buff += res;
			break;
#ifdef	__UI_LORA_WAN__
		case ST_CODE_WAN :
			ch = *executionPointer++;
			if ( (ptr = basic_Wan_Print( ch, buff )) != NULL ) { buff = ptr; break; }
			executionPointer-=2;
			if ( (ptr = get_ValPrint( buff )) != NULL )	buff = ptr;
			break;
#endif
		default:
			if ( (ptr = basic_LoRa_Print( ch, buff )) != NULL ) { buff = ptr; break; }
			val_flag = true;
			break;
		}

		if ( val_flag )
		{
			if ( executionPointer == recursive_ptr ) { errorCode = ERROR_SYNTAX; break; }
			recursive_ptr = executionPointer--;
			if ( (ptr = get_ValPrint( buff )) == NULL ) break;
			buff = ptr;
		}
		lastCh = ch;
		len = (int16_t)(buff - bios_UartTxBuff);
		if ( len > BIOS_UART_TX_BUFF_SIZE-3 ) errorCode = ERROR_OVERFLOW;
	}
	recursive_ptr = NULL;
	if ( errorCode )	return -1;
	last_string_buff = buff;
	return len;
}

//*************************************************
static char *get_ValPrint( char *buff )
{
	var_t val = expr();
	if ( errorCode ) return NULL;
	char *ptr = conv2str( val, FORM_NONE, 0 );
	buff = strcpy2( buff, ptr );
	return buff;
}

//*************************************************
var_t *getParameterPointer( void )
{
	uint8_t ch = *executionPointer++;
	if ( ch == ST_ARRAY ) {
		var_t index = getArrayIndexZero( ARRAY_INDEX_MAX );
		if ( errorCode ) return NULL;
		return ( &arrayVariables[index] );
	} else
	if ( isupper( ch ) ) {
		return ( &globalVariables[ch-'A'] );
	} else
	if ( ch == VAR_BUP1 || ch == VAR_BUP2 )
	{
		return (var_t*)bios_getBackupRegPtr( ch - VAR_BUP1 );
	}
	errorCode = ERROR_SYNTAX;
	return NULL;
}

//*************************************************
static uint8_t get_bracket_type( uint8_t ch, int8_t func )
{
	if ( ch == '(' )			return ')';
	if ( ch == '[' && !func )	return ']';
	return 0;
}

//*************************************************
static errCode_t calcValueFuncArray_n( var_t *val, int num, int8_t func )
{
	uint8_t cx = get_bracket_type( *executionPointer++, func );
	if ( cx == 0 )
	{
		errorCode = ERROR_SYNTAX;
		return errorCode;
	}
	while ( num-- )
	{
		if ( func < -1 ) {
			func = -1;
			int16_t len = get_StringPara( STRING_PARA_FLAG_VARIABLE );		
			*val = len;
		} else {
			*val = expr();
		}
		val++;
		if ( errorCode ) return errorCode;
		if ( num == 0 )	break;
		if ( func < 0 && *executionPointer == cx ) break;
		if ( chkST( ',' ) )	return errorCode;
	}
	if ( *executionPointer++ != cx )
	{
		errorCode = ERROR_SYNTAX;
	}
	return errorCode;
}

//*************************************************
errCode_t calcValueFunc_n( var_t *val, int num )
{
	return calcValueFuncArray_n( val, num, 1 );
}

//*************************************************
errCode_t calcValueFunc_1( var_t *val )
{
	return calcValueFuncArray_n( val, 1, 1 );
}

//*************************************************
var_t getArrayIndex( var_t index_min, var_t index_max )
{
	var_t index;
	if ( calcValueFuncArray_n( &index, 1, 0 ) )	return -1;
	if ( index < index_min || index > index_max ) {
		errorCode = ERROR_ARRAY;
		return -1;
	}
	return index;
}

//*************************************************
var_t getArrayIndexZero( var_t index_max )
{
	return getArrayIndex( 0, (var_t)index_max );
}

//*************************************************
static var_t basic_txBuffToInt( int16_t length, int32_t pos, int32_t num )
{
	if ( pos > length ) return 0;
	if ( !num || ((pos + num) > length) ) num = length - pos;
	char *p = bios_UartTxBuff + pos;
	if ( *p != ENCODE_HEXDEC ) return Ui_DecStr2Val_n( &p, num );
	p++;
	num--;
	return Ui_HexStr2Val_n( &p, num );
}

//*************************************************
static int16_t basic_strFunc( void )
{
	var_t val = 0;
	
	if( calcValueFuncArray_n( &val, 1, -2 ) ) return -1;
	return (int16_t)val;
}

//*************************************************
static var_t basic_ToInt( void )
{
	var_t val[3] = {0};
	
	if( calcValueFuncArray_n( val, 3, -2 ) ) return 0;
	if ( val[1] < 0 || val[2] < 0 ) {
		errorCode = ERROR_PARA;
		return 0;
	}
	return basic_txBuffToInt( val[0], val[1], val[2] );
}

//*************************************************
inline static var_t basic_GetVersion( void )
{
	var_t val = _PRODUCT_VER_MAJOR_VAL_ * 100 + _PRODUCT_VER_MINOR_VAL_;
	if( *executionPointer == '.' )
	{
		executionPointer++;
		val = (_PRODUCT_VER_MAJOR_VAL_ * 100 + _PRODUCT_VER_MINOR_VAL_) * 100 + (_PRODUCT_VER_PATCH_[0] - 'a');
	}
	return val;
}

//*************************************************
static var_t calcValue( void )
{
	uint8_t ch, stat, *str;
	var_t val, val2[2];
	int16_t len;

	ch = *executionPointer++;
	if ( isupper( ch ) ) {
		return globalVariables[ch-'A'];
	}
	switch ( ch )
	{
	case ST_EOL :
		executionPointer--;
		return 0;
	case ST_ARRAY :
		val = getArrayIndexZero( ARRAY_INDEX_MAX );
		if ( errorCode ) break;
		return arrayVariables[val];
	case ST_CARET :
		if ( basic_GetCaretVal( &val ) ) return val;
		break;
	case FUNC_BCC :
		if ( (len = basic_strFunc()) < 0 ) break;
		for ( val = 0, str = (uint8_t*)bios_UartTxBuff, ch = 0; len; len-- ) val ^= *str++;	
		return val;
	case FUNC_SUM :
		if ( (len = basic_strFunc()) < 0 ) break;
		for ( val = 0, str = (uint8_t*)bios_UartTxBuff, ch = 0; len; len-- ) val += *str++;
		return val;
	case FUNC_TOINT :
		return basic_ToInt();
	case '('	:
		executionPointer--;
		if ( calcValueFunc_1( &val ) ) break;
		return val;
	case ')'	:
	case '['	:
	case ']'	:
		errorCode = ERROR_SYNTAX;
		break;
	case '-'	:
		return -calcValue();
	case '!'	:
		return !calcValue();
	case '~'	:
		return ~calcValue();
	case FUNC_RND :
		if ( calcValueFunc_1( &val ) )	break;
		var_t tmp = bios_GetRandom();
		return val ? tmp % (val+1) : tmp ;
	case FUNC_ABS :
		if ( calcValueFunc_1( &val ) )	break;
		if ( val < 0 ) val = -val;
		return val;
	case FUNC_INP :
		if ( calcValueFunc_1( &val ) )	break;
		if ( bios_GpioInput( val, &stat ) == 0 )	return stat;
		errorCode = ERROR_PARA;
		break;
	case FUNC_ADC :
		if ( calcValueFunc_1( &val ) )	break;
		val = bios_GetAdc( val );
		if ( val <= BIOS_ADC_VAL_MAX )	return val;
		errorCode = ERROR_PARA;
		break;
	case FUNC_SQRT :
		if ( calcValueFunc_1( &val ) )	break;
		return gps_sqrt_int( val );
	case FUNC_ATAN2 :
		if ( calcValueFunc_n( val2, 2 ) ) break;
		return gps_anat2_int( val2[0], val2[1] );
	case FUNC_INT8 :
		if ( calcValueFunc_1( &val ) )	break;
		return (var_t)((int8_t)val);
	case FUNC_INT16 :
		if ( calcValueFunc_1( &val ) )	break;
		return (var_t)((int16_t)val);
	case FUNC_INT24 :
		if ( calcValueFunc_1( &val ) )	break;
		val &= 0xffffff;
		if ( val & 0x800000 ) val |= 0xff000000;
		return val;	
	case FUNC_POW :
		if ( calcValueFunc_n( val2, 2 ) ) break;
		return gps_pow_int( val2[0], val2[1] );
	case VAR_AUTO	:	return	strlen( bios_GetAutorun() );
	case VAR_TICK	:	return	bios_getSystemTick();
	case VAR_CLOCK	:	return	bios_getSystemClock();
	case VAR_MILISEC:	return	bios_getSystemMilisec();
	case VAR_INKEY	:
	case VAR_UINKEY	:
		val = 0;
		if ( *executionPointer == '(' && calcValueFunc_1( &val ) ) break;
		return val_getInkey( val, ch == VAR_UINKEY );
	case ST_PRINT	:	return	bios_UartTxBuffLen();
	case ST_UPRINT	:	return	bios_Uart2TxBuffLen();
	case ST_ZERO	:	return	0;
	case VAR_NULL	:	return	0;
	case VAR_FALSE	:	return	0;
	case VAR_TRUE	:	return	1;
	case VAR_ECHO	:	return	bios_UartGetEcho();
	case VAR_BAUD	:	return	bios_UartGetBaudrate();
	case VAR_UBAUD	:	return	bios_Uart2GetBaudrate();
	case VAR_LPOS	:	return	bios_Lcd_GetPos();
	case VAR_LCONT	:   return	bios_Lcd_GetContrust();
	case ST_VER     :	return	basic_GetVersion();
	case ST_LIST	:	return	(var_t)progLength;
	case ST_RUN		:   return	(var_t)executionPointer;
	case ST_EDIT	:   return	progEditMode;
	case ST_CATCH	:	return	errorCatchLineNumber;
	case VAR_ERROR	:	if ( *executionPointer != '#' ) return lastErrCode;
						executionPointer++;	return lastErrCode ? lastErrLineNumber : 0;
	case VAR_CAUSE  :   return bios_GetResetCause();
	case ST_LABEL	:	return basic_GetLabelNum();
	case VAR_BUP1	:
	case VAR_BUP2	:	if ( *executionPointer == '.' ) {
							executionPointer++;
							ch += 2;
						}
						bios_rtc_wait_syncbusy();
						return (uint32_t)*bios_getBackupRegPtr( ch - VAR_BUP1 );
	case FUNC_TIMEVAL:	if ( calcValueFunc_1( &val ) )	break;
						return basic_LoRa_TimeVal( val );
	case VAR_SPIBAUD :	return bios_SpiStat();
	case FUNC_SPITRX :
		val = val2[1] = 0;
		if ( calcValueFuncArray_n( val2, 2, -1 ) ) break;
		if ( !bios_SpiTRx( (uint8_t*)&val, (uint8_t*)&val2[0], val2[1] ) ) return val;
		errorCode = ERROR_DEVICE;
		break;
	case FUNC_BSWAP16 :
		if ( calcValueFunc_1( &val ) ) break;
		return byteswap16( val );
	case FUNC_BSWAP32 :
		if ( calcValueFunc_1( &val ) ) break;
		return byteswap32( val );
#ifdef __UI_LORA_WAN__
	case ST_CODE_WAN :
		basic_WanGetVar( *executionPointer++, &val );
		return val;
#endif
	}

	if ( errorCode == ERROR_OK )
	{
		if ( is_TypeNum( ch ) )
		{
			executionPointer = get_num( executionPointer, &val, ch );
			return val;
		}
		if ( basic_LoRa_GetVar( ch, &val ) )	return val;
		executionPointer--;
		val = get_StringPara( STRING_PARA_FLAG_VARIABLE );
		if ( val >= 0 )	return basic_txBuffToInt( val, 0, 0 );
		if ( !errorCode ) errorCode = ERROR_SYNTAX;
	}
	return -1;
}

//*************************************************
static int32_t basic_GetLabelNum( void )
{
	int32_t val = 0;
	executionPointer--;
	get_GotoLabel( &val );
	if( errorCode == ERROR_LABEL ) {
		errorCode = ERROR_OK;
	}
	return val;
}

//*************************************************
errCode_t basic_SetVarSub( var_t *val, var_t min, var_t max )
{
	if ( !proc_LetSpecial( val ) && (*val < min || max < *val) ) errorCode = ERROR_PARA;
	return errorCode;
}

//*************************************************
static int16_t get_ValString( void )
{
	if ( chkST( '=' ) ) return -1;
	int16_t len = get_StringPara( STRING_PARA_FLAG_CRLF_DISABLE );
	if ( len >= 0 )
	{
		memset( bios_UartTxBuff + len, 0, BIOS_UART_TX_BUFF_SIZE - len );
	}
	return len;
}

//*************************************************
uint8_t set_BiosVar( uint8_t ch )
{
	var_t val;

	switch ( ch )
	{
	case VAR_AUTO :
		if ( get_ValString() < 0 )	break;
		if ( bios_SetAutorun( bios_UartTxBuff ) ) errorCode = ERROR_PARA;
		break;
	case ST_PERIOD :
		get_ValString();
		break;
	case ST_CARET :
		basic_SetCaret();
		break;
	case VAR_BAUD :
		if ( !proc_LetSpecial( &val ) && bios_UartSetBaudrate(val) ) errorCode = ERROR_PARA;	
		break;
	case VAR_UBAUD :
		if ( !proc_LetSpecial( &val ) && bios_Uart2SetBaudrate(val) ) errorCode = ERROR_PARA;
		break;
	case VAR_ECHO :
		if ( !proc_LetSpecial( &val ) ) bios_UartSetEcho(val);
		break;
	case VAR_LPOS :
		if ( !proc_LetSpecial( &val ) ) bios_Lcd_SetPos(val);
		break;
	case VAR_LCONT :
		if ( !proc_LetSpecial( &val ) ) bios_Lcd_SetContrust(val);
		break;
	case VAR_TICK :
		if ( !proc_LetSpecial( &val ) ) bios_setSystemTick(val);
		break;
	case VAR_CLOCK :
		if ( !proc_LetSpecial( &val ) ) bios_setSystemClock(val);
		break;
	case VAR_ERROR :
		if ( *executionPointer == '#' ) { ch = *executionPointer++; }
		if ( !proc_LetSpecial( &val ) ) {
			errorCode = val;
			if ( errorCode == ERROR_OK )	lastErrCode = lastErrLineNumber = 0;
			if ( ch != VAR_ERROR )	lineNumber = lastErrLineNumber;
			setLastError();
		}
		break;
	case VAR_SPIBAUD :
		if ( !proc_LetSpecial( &val ) && bios_SpiInit(val) ) errorCode = ERROR_PARA;
		break;
	case VAR_BUP1 :
	case VAR_BUP2 :
		proc_LetVariable( (var_t*)bios_getBackupRegPtr( ch - VAR_BUP1 ) );
		bios_rtc_wait_syncbusy();
		break;
	default :
		return 1;
	}
	return 0;
}

//*************************************************
static var_t expr4th( void )
{
	var_t acc, val;
	uint8_t	ch;

	acc = calcValue();
	while ( errorCode == ERROR_OK ) {
		ch = *executionPointer++;
		switch ( ch ) {
		case '*':						// ��Z, �ݏ�
			if ( *executionPointer == ch ) {
				executionPointer++;
				val = calcValue();
				acc = gps_pow_int( acc, val );
				break;
			}
			val = calcValue();
			acc *= val;
			break;
		case '/':						// ���Z, �ΐ�
			if ( *executionPointer == ch ) {
				executionPointer++;
				val = calcValue();
				acc = gps_log_int( acc, val );
				if ( acc < 0 ) errorCode = ERROR_DIVZERO;
				break;
			}
			val = calcValue();
			if ( !IS_DIVZERO( val ) ) acc /= val;
			break;
		case '%':						// ��]
			val = calcValue();
			if ( !IS_DIVZERO( val ) ) acc %= val;
			break;
		default:
			executionPointer--;
			return acc;
		}
	}
	return -1;
}

//*************************************************
static var_t expr3rd( void )
{
	var_t acc = expr4th();
	while ( errorCode == ERROR_OK ) {
		uint8_t ch = *executionPointer++;
		switch ( ch ) {
		case '+':					// ���Z
			acc = acc + expr4th();
			break;
		case '-':					// ���Z
			acc = acc - expr4th();
			break;
		default:
			executionPointer--;
			return acc;
		}
	}
	return -1;
}

//*************************************************
static var_t expr2nd( void )
{
	var_t acc, tmp;
	uint8_t	ch, ch2;

	acc = expr3rd();
	while ( errorCode == ERROR_OK ) {
		ch = *executionPointer++;
		switch ( ch ) {
		case '>':
			ch2 = *executionPointer++;
			if ( ch2 == '=' ) {
				tmp = expr3rd();
				acc = (acc >= tmp);		// >=
			} else if ( ch2 == ch )	{
				tmp = expr3rd();
				acc = (acc >> tmp);		// >>
			} else {
				executionPointer--;
				tmp = expr3rd();
				acc = (acc > tmp);		// >
			}
			break;
		case '<':
			ch2 = *executionPointer++;
			if ( ch2 == '>' ) {
				tmp = expr3rd();
				acc = (acc != tmp);		// <>
			} else if ( ch2 == '=' ) {
				tmp = expr3rd();
				acc = (acc <= tmp);		// <=
			} else if ( ch2 == ch )	{
				tmp = expr3rd();
				acc = (acc << tmp);		// <<
			} else {
				executionPointer--;
				tmp = expr3rd();
				acc = (acc < tmp);		// <
			}
			break;
		case '=':
			if ( *executionPointer == ch ) executionPointer++;
			tmp = expr3rd();
			acc = (acc == tmp);			// =, ==
			break;
		case '!':
			if ( *executionPointer == '=' ) {
				executionPointer++;
				tmp = expr3rd();
				acc = (acc != tmp);		// !=
				break;
			}
		default:
			executionPointer--;
			return acc;
		}
	}
	return -1;
}

//*************************************************
var_t expr( void )
{
	var_t	acc, tmp;
	uint8_t	ch;

	acc = expr2nd();
	while ( errorCode == 0 ) {
		ch = *executionPointer++;
		switch ( ch ) {
		case '&' :					// �_����
			if ( *executionPointer == ch ) {
				executionPointer++;
				tmp = expr2nd();
				acc = tmp && acc;
			} else
			{
				tmp = expr2nd();
				acc = tmp & acc;
			}
			break;
		case '|' :					// �_���a
			if ( *executionPointer == ch ) {
				executionPointer++;
				tmp = expr2nd();
				acc = tmp || acc;
			} else
			{
				tmp = expr2nd();
				acc = tmp | acc;
			}
			break;
		case '^' :					// �r���I�_���a
			tmp = expr2nd();
			acc = tmp ^ acc;
			break;
		default:
			executionPointer--;
			return acc;
		}
	}
	return -1;
}

//*************************************************
static uint8_t *str2form( uint8_t *fm, int32_t *len )
{
	uint8_t ff, ch;
	int16_t cnt;

	ff = cnt = 0;
	if ( chkST( ST_STRING )) return NULL;
	while ( true ) {
		ch = *executionPointer++;
		switch ( ch ) {
		case ST_EOL :
			errorCode = ERROR_SYNTAX;
			return ( NULL );
		case ST_STRING :
			*fm = ff;
			*len = cnt;
			return ( executionPointer );
		case '-' : ff |= FORM_FLAG;		break;
		case 'X' : ff |= FORM_HEXU;		break;
		case 'x' : ff |= FORM_HEXL;		break;
		case 'd' :
		case 'D' : ff &= ~FORM_HEX;		break;
		case '0' : ff |= FORM_ZERO;		break;
		case '+' : ff |= FORM_PLUS;		break;
		default :
			if ( isdigit( ch ) ) {
				cnt = ch - '0';
			}
		}
	}
}

#if INPUT_HISTORY_ENABLE
#define	CSI_SEQ		"\x1b["
#define	CSI_CUF		CSI_SEQ "C"
#define	CSI_CUB		CSI_SEQ "D"
#define	CSI_ED		CSI_SEQ "J"
#define	CSI_SCP		CSI_SEQ "s"
#define	CSI_RCP		CSI_SEQ "u"
#define	HISTORY_BUFF_SIZE	BIOS_UART_RX_BUFF_SIZE
#define HISTORY_NUM			10
typedef struct {
	uint16_t pos;
	uint16_t len;
} history_list_t;
static char history_buff[HISTORY_BUFF_SIZE];
static history_list_t history_list[HISTORY_NUM];

#if LIST_HISTORY_ENABLE
extern char *basic_listHistory_buff;
extern uint16_t basic_listHistory_len;
#endif

static void input_history_put( char *str, uint16_t len )
{
	if ( !len ) return;
	if ( history_list[0].len == len && !memcmp( str, history_buff, len ) ) return;
	uint16_t size = HISTORY_BUFF_SIZE - len;
	char *src = &history_buff[size];
	char *dst = &history_buff[HISTORY_BUFF_SIZE];
	while ( size ) { *--dst = *--src; size--; }
	memcpy( history_buff, str, len );
	for ( uint16_t i = HISTORY_NUM-1; i > 0; i-- )
	{
		uint16_t n_pos = history_list[i-1].pos + len;
		uint16_t n_len = history_list[i-1].len;
		if ( n_len && n_pos < HISTORY_BUFF_SIZE )
		{
			history_list[i].pos = n_pos;
			history_list[i].len = n_len;
		} else {
			history_list[i].pos = 0;
			history_list[i].len = 0;
		}
	}
	history_list[0].len = len;
}

static char *input_history_get( uint16_t *len, uint8_t idx )
{
	if ( idx && idx <= HISTORY_NUM )
	{
		*len = history_list[idx-1].len;
		if ( *len )	return &history_buff[history_list[idx-1].pos];
	}
	*len = 0;
	return NULL;
}
#endif

//*************************************************
static int16_t get_utf8_bytes( char ch )
{
	if ( ch < 0b10000000 ) return 1;	// ASCII
	if ( ch < 0b11000000 ) return 0;	// UTF8 �̓r��
	if ( ch < 0b11100000 ) return 2;	// UTF8 2byte�����̐擪
	if ( ch < 0b11110000 ) return 3;	// UTF8 3byte�����̐擪
	if ( ch < 0b11111000 ) return 4;	// UTF8 4byte�����̐擪
	if ( ch < 0b11111100 ) return 5;	// UTF8 5byte�����̐擪
	return 6;							// UTF8 6byte�����̐擪				
}

//*************************************************
static int16_t get_utf8_last_len( char *ptr )
{
	int16_t n;
	while ( (n = get_utf8_bytes( *ptr-- )) == 0 );
	return n;
}

#if INPUT_EDIT_ENABLE
//*************************************************
static int16_t get_utf8_len( char *ptr )
{
	int16_t n;
	while ( (n = get_utf8_bytes( *ptr++ )) == 0 );
	return n;
}
#endif

//*************************************************
static int16_t inputStringCSI( char *ptr, const char *str )
{
	int16_t n = get_utf8_last_len( ptr );
	bios_UartPutStrEcho( str );
	if ( n > 1 ) bios_UartPutStrEcho( str );
	return n;
}

#if INPUT_EDIT_ENABLE
//*************************************************
static int16_t inputStringRight( char *buff, int16_t pos )
{
	int16_t n = inputStringCSI( &buff[pos], CSI_CUF );
	return pos + n;
}
#endif

//*************************************************
static int16_t inputStringLeft( char *buff, int16_t pos )
{
	if ( !pos ) return pos;
	int16_t n = inputStringCSI( &buff[pos-1], "\b" );
	return pos - n;
}

//*************************************************
static var_t GetUartInkey( uint8_t (*hook_func)(void), bool uart2 )
{
	while ( 1 )
	{
		int c = uart2 ? bios_Uart2GetChar() : bios_UartGetChar();
		if ( c >= 0 ) return c;
		basic_EorProc();
		bios_Lowpower();
		if ( hook_func && hook_func() ) 	return -2;
		if ( Ui_IsBreak_BK() ) 	return -3;
		if ( !hook_func ) return -1;
	}
}

//*************************************************
int16_t inputString( uint8_t (*hook_func)(void), char *buff, int16_t buff_size, bool history )
{
	int16_t len = 0;
	int16_t pos = 0;
	int c;
#if INPUT_EDIT_ENABLE
	int16_t utf_count = 0;
	int16_t utf_bytes = 0;
	int8_t esc_count = 0;
	char esc_digit = 0;
#if INPUT_HISTORY_ENABLE
	uint16_t history_idx = 0;
#endif
#endif
	buff[0] = 0;
	buff_size -= 2;
	while ( 1 )
	{
		while ( (c = GetUartInkey( hook_func, false )) < 0 )
		{
			if ( c < -1 ) return -1;
		}
		basic_EorClear();
		char ch = (char)c;
		switch ( ch )
		{
		case ASCII_CR :
			buff[len] = 0;
			if ( len )
			{
#if INPUT_EDIT_ENABLE
#if INPUT_HISTORY_ENABLE
				if ( history ) input_history_put( buff, len );
#endif
				bios_UartPutStrEcho( &buff[pos] );
#endif
			}
			bios_UartPutStrEcho( "\r\n" );
			return ( len );
#if INPUT_EDIT_ENABLE
		case ASCII_ESC :
			esc_count = 1;
			esc_digit = 0;
			break;
#endif
		case ASCII_BS :
			if ( pos == 0 || len == 0 ) break;
			if ( pos == len )
			{
				len = pos = inputStringLeft( buff, pos );
				bios_UartPutStrEcho( CSI_ED );	// ED
				buff[len] = 0;
				break;
			}
#if INPUT_EDIT_ENABLE
			pos = inputStringLeft( buff, pos );
		case ASCII_DEL :
			if ( pos == len ) break;
			uint16_t n = get_utf8_len( &buff[pos] );
			memcpy( &buff[pos], &buff[pos+n], len-pos );			
			len -= n;
			bios_UartPutStrEcho( CSI_SCP CSI_ED );	// cursor save, ED 		
			bios_UartPutStrEcho( &buff[pos] );
			bios_UartPutStrEcho( CSI_RCP );			// cursor restore
#endif
			break;
		case ASCII_HT :
			ch = ASCII_SP;
		default :
#if INPUT_EDIT_ENABLE
			if ( esc_count == 1 )
			{
				esc_count = ( ch == '[' ) ? 2 : 0;	// CSI sequence
			} else
			if ( esc_count == 2 )
			{
				if ( isdigit( ch ) )
				{
					esc_digit = ch;
					break;
				}
				esc_count = 0;
#if INPUT_HISTORY_ENABLE
				char *his_buf;
				uint16_t his_len = 0;			
				if ( ch == 'A' || ch == 'B' )	// Up/Down
				{
					if ( ch == 'B' )				// Down
					{
						if ( history_idx > 1 ) {
							his_buf = input_history_get( &his_len, history_idx - 1 );
							if ( !his_len ) break;
							history_idx--;
						}
#if LIST_HISTORY_ENABLE
						else {
							his_buf = basic_listHistory_buff;
							his_len = basic_listHistory_len;
							if ( !his_len ) break;
							history_idx = 0;
						}
#endif
					} else
					if ( ch == 'A' )				// Up
					{
						his_buf = input_history_get( &his_len, history_idx + 1);
						if ( !his_len ) break;
						history_idx++;
					}
					while ( pos ) pos = inputStringLeft( buff, pos );	
					bios_UartPutStrEcho( CSI_ED );	// ED
					memcpy( buff, his_buf, his_len );
					pos = len = his_len;
					buff[len] = '\0';
					bios_UartPutStrEcho( buff );		
				} else
#endif
				if ( ch == 'C' )					// Right
				{
					if ( pos < len ) pos = inputStringRight( buff, pos );
				} else
				if ( ch == 'D' )					// Left
				{
					pos = inputStringLeft( buff, pos );
				} else
				if ( ch == '~' )
				{
					if ( esc_digit == '1' )		// Home
					{
						while ( pos ) pos = inputStringLeft( buff, pos );
					} else
					if ( esc_digit == '4' )		// End	
					{
						if ( len ) bios_UartPutStrEcho( &buff[pos] );
						pos = len;
					}
				}
			} else
			if ( ch >= ASCII_SP )
			{		
				if ( utf_count == 0 )
				{
					utf_bytes = get_utf8_bytes( ch );
					if ( utf_bytes == 0 ) break;
					if ( len + utf_bytes > buff_size ) break;		
					utf_count = utf_bytes;
				}
				for ( int16_t i = len; i > pos; i-- )
				{			
					buff[i] = buff[i-1];
				}
				buff[pos++] = ch;
				buff[++len] = 0;
				if ( --utf_count ) break;
				if ( pos == len )
				{
					bios_UartPutStrEcho( &buff[pos - utf_bytes] );
				} else
				{
					pos -= utf_bytes;
					bios_UartPutStrEcho( CSI_SCP );	// cursor save
					bios_UartPutStrEcho( &buff[pos] );
					bios_UartPutStrEcho( CSI_RCP );	// cursor restore
					pos = inputStringRight( buff, pos );
				}
			}
#else
			if ( ch >= ASCII_SP )
			{
				if ( len >= buff_size ) break;
				buff[pos++] = ch;
				buff[++len] = 0;
				bios_UartPutCharEcho( ch );
			}
#endif
			break;
		}
	}
}

//*************************************************
int16_t GetUartString( uint8_t (*hook_func)(void), char *buff, int16_t buff_size, uint8_t code, bool uart2 )
{
	int c;
	int16_t len = 0;
	bool denbun_flag = false;

	if ( uart2 == false && code == ENCODE_DENBUN ) bios_UartSetBreakCh( 0 );
	buff_size -= 2;
	while ( 1 )
	{
		basic_EorClear();
		c = GetUartInkey( hook_func, uart2 );
		if( c < -1 ) { len = -1; break; }
		if( c < 0 ) continue;
		char ch = (char)c;
		if ( code == ENCODE_DENBUN ) {
			if ( !uart2 ) bios_UartPutCharEcho( ch );
			if ( ch == ASCII_STX ) {
				denbun_flag = true;
				len = 0;
			} else
			if ( ch == ASCII_ETX ) {
				if ( denbun_flag ) {
					buff[len] = 0;
					break;
				}
			} else
			if ( denbun_flag ) {
				if ( len < buff_size ) buff[len++] = ch;
				else denbun_flag = false;
			}
		} else
		if ( ch == ASCII_CR )
		{
			buff[len] = 0;
			if ( !uart2 ) bios_UartPutStrEcho( "\r\n" );
			if ( code == ENCODE_PERCENT)	len = Ui_Percent_Decode( (uint8_t*)buff, &buff, 0 ); else
			if ( code == ENCODE_BASE64 )	len = Ui_Base64_Decode ( (uint8_t*)buff, &buff, 0 ); else
			if ( code == ENCODE_HEXDEC )	len = Ui_Hexdec_Decode ( (uint8_t*)buff, &buff, 0 );
			break;
		} else
		if ( ch >= ASCII_SP )
		{
			if ( len < buff_size ) {
				buff[len++] = ch;
				if ( !uart2 ) bios_UartPutCharEcho( ch );
			}
		}
	}
	if ( uart2 == false && code == ENCODE_DENBUN ) bios_UartSetBreakCh( bios_GetBreakCh() );
	return len;
}

//*************************************************
static uint8_t val_inputString_hook( void )
{
	return !bios_getWaitTick();
}

//*************************************************
void *val_initInkey( var_t timeout, bool uart2 )
{
	if ( uart2 ) bios_Uart2Begin( UART2_RX );
	if ( !timeout ) return NULL;
	bios_setWaitTick( timeout );
	return val_inputString_hook;
}

//*************************************************
int16_t val_inputString( char *buff, int16_t buff_size, int32_t timeout, bool uart2 )
{
	uint8_t (*hook_func)(void) = val_initInkey( timeout, uart2 );
	return GetUartString( hook_func, buff, buff_size, inputStringEncode, uart2 );
}

//*************************************************
var_t val_getInkey( var_t timeout, bool uart2 )
{
	uint8_t (*hook_func)(void) = val_initInkey( timeout, uart2 );
	int c = GetUartInkey( hook_func, uart2 );
	if ( c < 0 ) c = -1;
	return c;
}

//*************************************************
char *conv2str( var_t para, uint8_t ff, int32_t len )
{
	static char str[13];
	char *s, ch , flag, fx;
	uint32_t val;
	int32_t	dot = -1;

	if ( len < 0 )	ff |= FORM_ZERO;
	len = abs( len );

	if( len > 9 )
	{
		dot = len / 100;	
		len = len % 100;
		if ( len > 10 )	len = 10;
	}

	fx = str[12] = 0;
	if ((para < 0) && (ff & FORM_FHEX) != FORM_HEX ) {
		fx = flag = '-';
		val = (uint32_t)(-para);
	} else {
		flag = ' ';
		if ( ff & FORM_PLUS ) fx = flag = '+';
		val = (uint32_t)para;
	}
	s = &str[10];
	while ( 1 )
	{
		if ( ff & FORM_HEX ) {
			ch = (val & 0x0f) + '0';
			if ( ch > '9' ) {
				ch += 0x07 + (ff & FORM_LOWER);
			}
			val >>= 4;
		} else {
			ch = ( val % 10 ) + '0';
			val /= 10;
		}
		*s-- = ch;
		if ( dot >= 0 && (--dot == 0 ) ) *s-- = '.';
		
		if ( len > 0 && (--len == 0) ) break;

		if ( dot < 0 && val == 0 ) break;
	}

	if ( ff & FORM_FLAG ) {
		while ( len > 0 ) {
			len--;
			*s-- = 0x20 + (ff & FORM_ZERO);
		}
		*s = flag;
		return ( s );
	}

	if ( ff & FORM_ZERO ) {
		if ( len == 0 && fx ) {
			*s = flag;
			return (s);
		}
		while ( len > 0 ) {
			len--;
			*s-- = ( len == 0 && fx ) ? flag : '0';
		}
	} else {
		if ( fx ) {
			*s-- = flag;
			if ( len > 0 )
				len--;
		}
		while ( len > 0 ) {
			len--;
			*s-- = ' ';
		}
	}
	return (s + 1);
}

// size = 1(Byte)/2(Word)/3(Long)/4(Mid)
//*************************************************
uint8_t get_sizeBytes( uint8_t size )
{
	static const uint8_t size_bytes[] = { 0, 1, 2, 4, 3 };
	return size_bytes[size];
}

//*************************************************
uint8_t get_numBytes( uint8_t type )
{
	return get_sizeBytes( type & NUM_SIZE_MASK );
}

// size = 1(Byte)/2(Word)/3(Long)/4(Mid)
//*************************************************
uint8_t *get_VarConv( int32_t *val, uint8_t *src, uint8_t size )
{
	var_size_conv_t	x = { .n = 0 };
	do {
		x.b[0] = *src++;
		if ( size < NUM_SIZE_WORD )	break;
		x.b[1] = *src++;
		if ( size < NUM_SIZE_LONG )	break;
		x.b[2] = *src++;
		if ( size == NUM_SIZE_MID )	break;
		x.b[3] = *src++;
	} while (0);
	*val = x.n;
	return src;
}

// size = 1(Byte)/2(Word)/3(Long)/4(Mid)
//*************************************************
uint8_t *set_VarConv( int32_t val, uint8_t *dst, uint8_t size )
{
	var_size_conv_t	x = { .n = val };
	do {
		*dst++ = x.b[0];
		if ( size < NUM_SIZE_WORD )	break;
		*dst++ = x.b[1];
		if ( size < NUM_SIZE_LONG )	break;
		*dst++ = x.b[2];
		if ( size == NUM_SIZE_MID )	break;
		*dst++ = x.b[3];
	} while (0);
	return dst;
}


//*************************************************
uint8_t *get_num( uint8_t *ptr, var_t *val, uint8_t type )
{
	uint8_t size = (type & NUM_SIZE_MASK);
	ptr = get_VarConv( val, ptr, size );
	return ptr;
}

//*************************************************
uint8_t *put_num( uint8_t *dst, var_t val, uint8_t type, int16_t *len )
{
	uint8_t size = (type & NUM_SIZE_MASK);

	if ( size == NUM_SIZE_AUTO )
	{
		if ((val < INT16_MIN || INT16_MAX < val)||(type == ST_NUM_HEX && val < 0)) {
			size = NUM_SIZE_LONG;
		} else
		if ( val < INT8_MIN || INT8_MAX < val ) {
			size = NUM_SIZE_WORD;
		} else
		{
			size = NUM_SIZE_BYTE;
		}
	}
	uint8_t *ptr = dst;
	*ptr++ = ST_NUM | type | size;
	ptr = set_VarConv( val, ptr, size );
	(*len) += ptr - dst;
	return ptr;
}

//*************************************************
errCode_t basic_SetKey( void *buff, uint8_t len )
{
	if ( len > sizeof(int32_t) )
	{
		if ( get_ValString() < 0 ) return errorCode;
		memcpy( buff ,bios_UartTxBuff, len );
	} else
	{
		var_t val;
		if ( proc_LetSpecial( &val ) ) return errorCode;
		switch ( len )
		{
//		case sizeof(int8_t)  : *((int8_t  *)buff) = val; break;
//		case sizeof(int16_t) : *((int16_t *)buff) = val; break;
			case sizeof(int32_t) : *((int32_t *)buff) = val; break;
			default : return errorCode;
		}
	}
	return errorCode;
}

//*************************************************
bool basic_IsNotZero( uint8_t *ptr, uint8_t size )
{
	while ( size-- ) { if ( *ptr++ ) return true; }
	return false;
}

//*************************************************
static char *basic_GetCaretStr( char *buff )
{
	memcpy( buff, basic_CaretStrBuff, basic_CaretStrLen );
	return buff + basic_CaretStrLen;
}

//*************************************************
static bool basic_GetCaretVal( var_t *val )
{
	uint8_t cx = get_bracket_type( *executionPointer, 0 );
	if ( cx == 0 ) return false;
	if ( cx == executionPointer[1] )
	{
		executionPointer += 2;
		*val = basic_CaretStrLen;
	} else
	{
		var_t index = getArrayIndexZero( BASIC_CARET_STR_BUFF_SIZE );
		if ( errorCode ) return true;
		*val = (index >= basic_CaretStrLen) ? -1 : basic_CaretStrBuff[index];
	}
	return true;
}

//*************************************************
static void basic_SetCaret( void )
{
	uint8_t cx = get_bracket_type( *executionPointer, 0 );
	if ( cx == 0 )
	{
		int16_t len = get_ValString();
		if ( len < 0 ) return;
		if ( len >= BASIC_CARET_STR_BUFF_SIZE ) { errorCode = ERROR_PARA; return; }
		basic_CaretStrLen = len;
		memcpy( (void*)basic_CaretStrBuff, (void*)bios_UartTxBuff, basic_CaretStrLen );
		basic_CaretStrBuff[basic_CaretStrLen] = 0;
	} else {
		var_t val;
		if ( cx == executionPointer[1] )
		{
			executionPointer += 2;
			if ( !proc_LetSpecial( &val ) )
			{
				if ( val < 0 )
				{
					val = -val;
					if ( val > basic_CaretStrLen )  basic_CaretStrLen = 0;
					else {
						basic_CaretStrLen -= val;
						memcpy( basic_CaretStrBuff, basic_CaretStrBuff + val, basic_CaretStrLen );
					}
				} else {
					if ( val >= BASIC_CARET_STR_BUFF_SIZE ) { errorCode = ERROR_PARA; return; }
					if ( val > basic_CaretStrLen ) memset( basic_CaretStrBuff + basic_CaretStrLen, ' ', val - basic_CaretStrLen );
					basic_CaretStrLen = val;
				}
			}
		} else {
			var_t index = getArrayIndexZero( basic_CaretStrLen - 1 );
			if ( errorCode ) return;		
			if ( !proc_LetSpecial( &val ) ) basic_CaretStrBuff[index] = val;
		}
	}
}

//*************************************************
int32_t byteswap16( int32_t val )
{
	var_size_conv_t x, y;
	x.n = val;
	y.b[0] = x.b[1];
	y.b[1] = x.b[0];
	y.b[2] = x.b[3];
	y.b[3] = x.b[2];
	return y.n;
}

//*************************************************
int32_t byteswap32( int32_t val )
{
	var_size_conv_t x, y;
	x.n = val;
	y.b[0] = x.b[3];
	y.b[1] = x.b[2];
	y.b[2] = x.b[1];
	y.b[3] = x.b[0];
	return y.n;
}
