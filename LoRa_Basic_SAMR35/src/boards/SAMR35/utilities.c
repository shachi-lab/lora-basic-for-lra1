/*!
 * \file      utilities.c
 *
 * \brief     Helper functions implementation
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
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_fifo.h"
#include "sys_spi.h"
#include "sys_timer.h"
#include "sys_uart.h"
#include "utilities.h"
#include "bios.h"

/*!
 * Redefinition of rand() and srand() standard C functions.
 * These functions are redefined in order to get the same behavior across
 * different compiler toolchains implementations.
 */
// Standard random functions redefinition start
#define RAND_LOCAL_MAX 2147483647L

static uint32_t next = 1;

int32_t rand1( void )
{
	next = bios_GetRandom();
	return next % RAND_LOCAL_MAX;
}

void srand1( uint32_t seed )
{
	next = seed;
	bios_Randomize( seed );
}
// Standard random functions redefinition end

int32_t randr( int32_t min, int32_t max )
{
    return ( int32_t )rand1( ) % ( max - min + 1 ) + min;
}

void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    dst += ( size - 1 );
    while( size-- ) { *dst-- = *src++; }
}

#if _USE_UTILITY_MEMCPY_
void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    while( size-- ) { *dst++ = *src++; }
}
#endif

#if _USE_UTILITY_MEMSET_
void memset1( uint8_t *dst, uint8_t value, uint16_t size )
{
    while( size-- ) { *dst++ = value; }
}
#endif

char *strcpy2( char *dst, const char *src )
{
	if( src ) while( *src ) { *dst++ = *src++; }
	return dst;
}

#if _USE_UTILITY_MEMCPY2_
uint8_t *memcpy2( uint8_t *dst, const uint8_t *src, uint16_t size )
{
	if( src >= dst )
	{
		while( size-- ) { *dst++ = *src++; }
		return dst;
	}
	dst += size;
	src += size;
	uint8_t *ptr = dst;
	while( size-- ) { *--dst = *--src; }
	return ptr;
}
#endif

int8_t Nibble2HexChar( uint8_t a )
{
    if( a < 10 ) return '0' + a;
	if( a < 16 ) return 'A' + ( a - 10 );
    return '?';
}

/*!
 * calculate CRC32 (IEEE 802.3) with SAMR34/35 DMA module
 * 
 * \param [IN]  buff  Pointer to a buffer
 * \param [IN]  len   Length for unit of beat size to be calculated
 * \param [IN]  beatsize  beat size (CRCBEATSIZE_BYTE, CRCBEATSIZE_HWORD, CRCBEATSIZE_WORD)
 * \retval Calculated CRC checksum value
 */
uint32_t CalculateCRC32( void *buff, uint32_t len, enum crc_beatsize beatsize )
{
	MCLK->AHBMASK.reg |= MCLK_AHBMASK_DMAC;
	DMAC->CRCCTRL.reg = DMAC_CRCCTRL_CRCPOLY_CRC32 | DMAC_CRCCTRL_CRCSRC_IO | DMAC_CRCCTRL_CRCBEATSIZE(beatsize);
	DMAC->CRCCHKSUM.reg = -1;
	DMAC->CTRL.reg |= DMAC_CTRL_CRCENABLE;
	switch( beatsize )
	{
	case CRCBEATSIZE_BYTE :
		for( uint8_t *ptr8 = buff; len; len-- ) { DMAC->CRCDATAIN.reg = *ptr8++; } break;
	case CRCBEATSIZE_HWORD :
		for( uint16_t *ptr16 = buff; len; len-- ) { DMAC->CRCDATAIN.reg = *ptr16++; nop();}	break;
	case CRCBEATSIZE_WORD :
		for( uint32_t *ptr32 = buff; len; len-- ) { DMAC->CRCDATAIN.reg = *ptr32++; nop(); nop();}	break;
	}
	DMAC->CRCSTATUS.reg = DMAC_CRCSTATUS_CRCBUSY;
	uint32_t sum = DMAC->CRCCHKSUM.reg;
	DMAC->CRCCTRL.reg = 0;
	while( DMAC->CRCSTATUS.reg & DMAC_CRCSTATUS_CRCBUSY );
	DMAC->CTRL.reg &= ~DMAC_CTRL_CRCENABLE;
//	MCLK->AHBMASK.reg &= ~MCLK_AHBMASK_DMAC;
	return sum;
}
//
void memcpy32( uint32_t *dst, uint32_t *src, uint32_t size )
{
	while( size-- ){ *dst++ = *src++; }
}
//
void memset32( uint32_t *dst, uint32_t val, uint32_t size )
{
	while( size-- ){ *dst++ = val; }
}
