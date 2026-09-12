/*!
 * @file    ui_code.c
 * @brief   encode/decode functions
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "basic.h"

#define UI_CODE_HEX_UPPER		0		// 1:Upper(0-9,A-F), 0:Lower(0-9,a-f)
#define B64_USE_TABLE			0

//*************************************************
int32_t Ui_HexStr2Val( char **src )
{
	return Ui_HexStr2Val_n( src, -1 );
}

//*************************************************
int32_t Ui_DecStr2Val( char **src )
{
	return Ui_DecStr2Val_n( src, -1 );
}

//*************************************************
int32_t Ui_HexStr2Val_n( char **src, int32_t len )
{
	uint32_t val = 0;

	while ( len )
	{
		char ch = Ui_Hex2Byte( **src );
		if ( ch >= 0x10 ) break;
		val = (val << 4) + ch;
		len--;
		(*src)++;
	}
	return (int32_t)val;
}

//*************************************************
int32_t Ui_DecStr2Val_n( char **src, int32_t len )
{
	int32_t val = 0;
	char flag = 0;
	
	while ( len )
	{
		char ch = **src;
		if ( flag == 0 )
		{
			flag = ch;
			if( ch == '-' || ch == '+' ) goto next;
		}
		if( !isdigit( ch ) ) break;
		val = val*10 + (ch - '0');
next:
		len--;
		(*src)++;
	}
	return (flag == '-') ? -val : val;
}

/*
 *
 */
char Ui_Byte2Hex( uint8_t n )
{
#if UI_CODE_HEX_UPPER
	if( n > 9 ) n += 0x07;
#else
	if( n > 9 ) n += 0x27;
#endif
   return n + '0';
}

/*
 *
 */
uint8_t Ui_Hex2Byte( char c )
{
	if( c >= '0' && c <= '9' )    return c -'0';
	if( c >= 'A' && c <= 'F' )    return c -'7';
	if( c >= 'a' && c <= 'f' )    return c - ('7' + 0x20);
	return 0xff;
}

/*
 *
 */
char *Ui_SetHexBuff( char *dst, uint8_t *src, int16_t len )
{
    while( len )
    {
        *dst++ = Ui_Byte2Hex( *src >> 4 );
        *dst++ = Ui_Byte2Hex( *src & 15 );
        src++;
        len--;
    }
    *dst = 0;
	return dst;
}

/*
 *
 */
int16_t Ui_GetHexBuff( uint8_t *dst, char *src, int16_t size )
{
	uint8_t c;
	int16_t len = 0;

	if( src == NULL )   return len;
	size *= 2;
	while( len < size )
	{
		c = *src++;
		if( c <= 0x20 )	break;
		if( (c = Ui_Hex2Byte( c )) > 0x0f )	return -1;
		if( len & 1 )	{ *dst++ |= c;	}
		else			{ *dst = c << 4;}
		len++;
	}
	len = (len + 1) / 2;
	return len;
}

/**
* @brief   �p�[�Z���g �G���R�[�h����
* @param[in]  putc_func �o�͊֐��|�C���^
* @param[in]  src �G���R�[�h�Ώۂ̃o�C�i���f�[�^
* @param[in]  size �o�C�i���f�[�^�̃T�C�Y
* @return     �ϊ��㕶�����byte��
* @remark
*/
int16_t Ui_Percent_Encode( void (*putc_func)(char), uint8_t *src, int16_t size )
{
    char c;
	int16_t len = 0;

    while( size )
    {
        c = *src++;
        if( (c>='0' && c<='9')||(c>='A' && c<='Z')||(c>='a' && c<='z')
          ||(c == '-' || c == '_' || c == '.' || c == '~')                          // RFC2396, RFC3986
          ||(c == '!' || c == '(' || c == ')' || c == '*' || c == 0x27 )            // RFC2396 only
          )
        {
            putc_func( c );
        }
        else
        {
			len += 2;
            putc_func( '%' );
            putc_func( Ui_Byte2Hex(c >> 4 ) );
            putc_func( Ui_Byte2Hex(c & 0xf) );
        }
		len++;
        size--;
    }
	return len;
}

/**
* @brief   �p�[�Z���g �f�R�[�h����
* @param[in]  dst �f�R�[�h��f�[�^�[�̊i�[�o�b�t�@
* @param[in]  src �f�R�[�h�Ώۂ̕�����
* @return     �ϊ����ꂽbyte��
* @remark
*/
int16_t Ui_Percent_Decode( uint8_t *dst, char **src, char term )
{
    uint8_t cc, ch, *str = (uint8_t*)(*src);
	int16_t len = 0;

    while( 1 )
    {
		ch = *str++;
		if( ch == 0 || ch == term )	break;
		if( ch == '+' )	ch = 0x20;
		else
        if( ch == '%' )
        {
			if((cc = Ui_Hex2Byte( *str )) < 0x10 )
			{
				ch = cc;
				str++;
				if((cc = Ui_Hex2Byte( *str )) < 0x10 )
				{
					str++;
					ch = (ch << 4) + cc;
				}
			}
		}
        *dst++ = ch;
        len++;
    }
	*src = (char*)str;
    return len;
}

#if B64_USE_TABLE

//! BASE64�G���R�[�h�e�[�u��
static const char b64_enc[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//! BASE64�f�R�[�h�e�[�u��
static const uint8_t b64_dec[] = {
/*	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                                                */
                                    62, 0, 0, 0,63,52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
	0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51 
/*                                                                                 , 0, 0, 0, 0, 0  */
};

#else
static uint8_t b64_dec_func( uint8_t n )
{
	if ( n >= 'A' && n <= 'Z' ) return n - 'A' + 0x00;
	if ( n >= 'a' && n <= 'z' ) return n - 'a' + 0x1a;
	if ( n >= '0' && n <= '9' ) return n - '0' + 0x34;
	if ( n == '+' ) return 0x3e;
	if ( n == '/' ) return 0x3f;
	return 0xff;
}

static char b64_enc_func( uint8_t n )
{
	n &= 0x3f;
	if ( n < 0x1a ) return (char)(n - 0x00 + 'A');
	if ( n < 0x34 ) return (char)(n - 0x1a + 'a');
	if ( n < 0x3e ) return (char)(n - 0x34 + '0');
	if ( n == 0x3e ) return '+';
	return '/';
}
#endif

/**
* @brief   BASE64 �f�R�[�h����
* @param[in]  dst �f�R�[�h��f�[�^�[�̊i�[�o�b�t�@
* @param[in]  src �f�R�[�h�Ώۂ̕�����
* @return     �ϊ����ꂽbyte��
* @remark
*/
int16_t Ui_Base64_Decode( uint8_t *dst, char **src, char term )
{
	uint8_t ch, c[4], *str = (uint8_t*)(*src);
	int32_t j, k, i = 0;

	while( 1 )
	{
		if(*str == 0 || *str == '=' || *str == term) break; 		
		for( j = 0; j < 4 ;)			/* 4�������ϊ� */
		{
			if(*str == 0 || *str == '=' || *str == term) break; 
			uint8_t n = *str++;
#if B64_USE_TABLE
			if( n < 0x2b || n > 0x7a ) continue;
			n = b64_dec[n - 0x2b];
#else		
			n = b64_dec_func( n );
			if ( n > 0x3f ) continue;
#endif
			c[j] = n;
			j++;
		}
		for(k = 0; k < j - 1; k++)	dst[i++] = c[k]<<((k<<1)+2)|c[k+1]>>((2-k)<<1);
	}
	
	do{
		ch = *str++;
	}while( ch != 0 && ch != term );
	*src = (char*)str;
	return i;
}

/**
* @brief   BASE64 �G���R�[�h����
* @param[in]  putc_func �o�͊֐��|�C���^
* @param[in]  src �G���R�[�h�Ώۂ̃o�C�i���f�[�^
* @param[in]  size �o�C�i���f�[�^�̃T�C�Y
* @return     �ϊ��㕶�����byte��
* @remark
*/
int16_t Ui_Base64_Encode( void (*putc_func)(char), uint8_t *src, int16_t size )
{
    int32_t j, reg, len;

	j = len = reg = 0;
    while( size )
    {
        reg = (reg << 8) | *src++;
        len += 8;
        while( len >= 6 )
        {
            len -= 6;
			j++;
#if B64_USE_TABLE
			putc_func( b64_enc[(reg >> len) & 0x3f] );
#else
			putc_func( b64_enc_func(reg >> len) );		
#endif
        }
		size--;
    }
    if( len > 0 )
    {
		len = 6 - len;
		j++;
#if B64_USE_TABLE
        putc_func( b64_enc[(reg << len) & 0x3f] );
#else
        putc_func( b64_enc_func(reg << len) );
#endif
    }
    while( j & 0x03 )
    {
		j++;
        putc_func( '=' );
    }
    return j;
}

/**
* @brief   HEX������ �f�R�[�h����
* @param[in]  dst �f�R�[�h��f�[�^�[�̊i�[�o�b�t�@
* @param[in]  src �f�R�[�h�Ώۂ̕�����
* @return     �ϊ����ꂽbyte��
* @remark
*/
int16_t Ui_Hexdec_Decode( uint8_t *dst, char **src, char term )
{
	uint8_t ch, *str = (uint8_t*)(*src);
	int32_t len = 0;
	bool f = false;
	while( 1 )
	{
		ch = *str++;
		if( ch == 0 || ch == term )	break;
		if((ch = Ui_Hex2Byte( ch )) < 0x10 )
		{
			if ( f )
			{
				*dst++ |= ch;
			} else
			{
				*dst = ch << 4;	
				len++;		
			}
			f = !f;
		}
	}
	*src = (char*)str;
	return len;
}

/**
* @brief   HEX������ �G���R�[�h����
* @param[in]  putc_func �o�͊֐��|�C���^
* @param[in]  src �G���R�[�h�Ώۂ̃o�C�i���f�[�^
* @param[in]  size �o�C�i���f�[�^�̃T�C�Y
* @return     �ϊ��㕶�����byte��
* @remark
*/
int16_t Ui_Hexdec_Encode( void (*putc_func)(char), uint8_t *src, int16_t size )
{
	for( int16_t len = size; len; len-- )
	{
		char c = *src++;
		putc_func( Ui_Byte2Hex(c >> 4 ) );
		putc_func( Ui_Byte2Hex(c & 0xf) );
	}
	return size * 2;
}

/**
* @brief   BINARY������ �G���R�[�h����
* @param[in]  putc_func �o�͊֐��|�C���^
* @param[in]  src �G���R�[�h�Ώۂ̃o�C�i���f�[�^
* @param[in]  size �o�C�i���f�[�^�̃T�C�Y
* @return     �ϊ��㕶�����byte��
* @remark
*/
static int16_t Ui_Binary_Encode( void (*putc_func)(char), uint8_t *src, int16_t size )
{
	for( int16_t len = size; len; len-- )
	{
		putc_func( *src++ );
	}
	return size;
}

/**
* @brief   DENBUN������ �G���R�[�h����
* @param[in]  putc_func �o�͊֐��|�C���^
* @param[in]  src �G���R�[�h�Ώۂ̃o�C�i���f�[�^
* @param[in]  size �o�C�i���f�[�^�̃T�C�Y
* @return     �ϊ��㕶�����byte��
* @remark
*/
int16_t Ui_Denbun_Encode( void (*putc_func)(char), uint8_t *src, int16_t size )
{
	putc_func( ASCII_STX );
	Ui_Binary_Encode( putc_func, src, size );
	putc_func( ASCII_ETX );
	return size + 2;
}

/**
* @brief   ENCODE������ �G���R�[�h����
* @param[in]  putc_func �o�͊֐��|�C���^
* @param[in]  src �G���R�[�h�Ώۂ̃o�C�i���f�[�^
* @param[in]  size �o�C�i���f�[�^�̃T�C�Y
* @param[in]  code ENCODE����
* @return     �ϊ��㕶�����byte��
* @remark
*/
int16_t Ui_Encode_Print( void (*putc_func)(char), uint8_t *src, int16_t size, uint8_t code )
{
	switch( code )
	{
		case ENCODE_PERCENT : return Ui_Percent_Encode( putc_func, src, size );
		case ENCODE_BASE64  : return Ui_Base64_Encode ( putc_func, src, size );
		case ENCODE_HEXDEC  : return Ui_Hexdec_Encode ( putc_func, src, size );
		case ENCODE_DENBUN  : return Ui_Denbun_Encode ( putc_func, src, size );
		case ENCODE_IGNORE  : return 0;
		case ENCODE_BINARY  :
		case ENCODE_RAW     :
		default             : return Ui_Binary_Encode ( putc_func, src, size );
	}
}

/*
 *
 */
bool Ui_IsEncode( uint8_t code )
{
	return (
//	(code == ENCODE_RAW    ) ||
	(code == ENCODE_BINARY ) ||
	(code == ENCODE_PERCENT) ||
	(code == ENCODE_BASE64 ) ||
	(code == ENCODE_HEXDEC ) ||
	(code == ENCODE_DENBUN ) ||
	(code == ENCODE_IGNORE )
	);
}
