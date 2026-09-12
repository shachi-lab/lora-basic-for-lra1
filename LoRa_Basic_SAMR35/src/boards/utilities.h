/*!
 * \file      utilities.h
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
#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#define	_USE_UTILITY_MEMCPY_						0
#define	_USE_UTILITY_MEMSET_						0
#define _USE_UTILITY_MEMCPY2_						0

#include <stdint.h>
#include <string.h>
/*!
 * Generic definition
 */
#ifndef SUCCESS
#define SUCCESS                                     1
#endif

#ifndef FAIL
//#define FAIL										0
#endif

#ifndef NULL
#define NULL                                        0
#endif

/*
 *
 */
#define assert_param( x )


/*!
 * \brief Returns the minimum value between a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval minValue Minimum value
 */
#undef MIN
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )

/*!
 * \brief Returns the maximum value between a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval maxValue Maximum value
 */
#undef MAX
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )

/*!
 * \brief Returns 2 raised to the power of n
 *
 * \param [IN] n power value
 * \retval result of raising 2 to the power n
 */
#define POW2( n ) ( 1 << n )

/*!
 * \brief Initializes the pseudo random generator initial value
 *
 * \param [IN] seed Pseudo random generator initial value
 */
void srand1( uint32_t seed );

/*!
 * \brief Computes a random number between min and max
 *
 * \param [IN] min range minimum value
 * \param [IN] max range maximum value
 * \retval random random value in range min..max
 */
int32_t randr( int32_t min, int32_t max );

int32_t rand1( void );

/*!
 * \brief Copies size elements of src array to dst array
 *
 * \remark STM32 Standard memcpy function only works on pointers that are aligned
 *
 * \param [OUT] dst  Destination array
 * \param [IN]  src  Source array
 * \param [IN]  size Number of bytes to be copied
 */
 #if _USE_UTILITY_MEMCPY_
void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size );
#else
inline void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size ) { memcpy( dst, src, size ); }
#endif

/*!
 * \brief Copies size elements of src array to dst array reversing the byte order
 *
 * \param [OUT] dst  Destination array
 * \param [IN]  src  Source array
 * \param [IN]  size Number of bytes to be copied
 */
void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size );

/*!
 * \brief Set size elements of dst array with value
 *
 * \remark STM32 Standard memset function only works on pointers that are aligned
 *
 * \param [OUT] dst   Destination array
 * \param [IN]  value Default value
 * \param [IN]  size  Number of bytes to be copied
 */
#if _USE_UTILITY_MEMSET_
void memset1( uint8_t *dst, uint8_t value, uint16_t size );
#else
inline void memset1( uint8_t *dst, uint8_t value, uint16_t size  ) { memset( dst, value, size ); }
#endif

/*!
 * \brief Converts a nibble to an hexadecimal character
 *
 * \param [IN] a   Nibble to be converted
 * \retval hexChar Converted hexadecimal character
 */
int8_t Nibble2HexChar( uint8_t a );

/*!
 * Begins critical section
 */
//#define CRITICAL_SECTION_BEGIN( )	uint32_t mask; BoardCriticalSectionBegin( &mask );
#define CRITICAL_SECTION_BEGIN( )	cpu_irq_enter_critical()

/*!
 * Ends critical section
 */
//#define CRITICAL_SECTION_END( )	BoardCriticalSectionEnd( &mask )
#define CRITICAL_SECTION_END( )		cpu_irq_leave_critical()

/*
 * ============================================================================
 * Following functions must be implemented inside the specific platform 
 * board.c file.
 * ============================================================================
 */
/*!
 * Disable interrupts, begins critical section
 * 
 * \param [IN] mask Pointer to a variable where to store the CPU IRQ mask
 */
void BoardCriticalSectionBegin( uint32_t *mask );

/*!
 * Ends critical section
 * 
 * \param [IN] mask Pointer to a variable where the CPU IRQ mask was stored
 */
void BoardCriticalSectionEnd( uint32_t *mask );


enum crc_beatsize
{
	CRCBEATSIZE_BYTE	= 0,
	CRCBEATSIZE_HWORD	= 1,
	CRCBEATSIZE_WORD	= 2,
};
#define CRC32_MAGIC_NUMBER		0x2144df1c
/*!
 * calculate CRC32 (IEEE 802.3) with SAMR34/35 DMA module
 * 
 * \param [IN]  buff  Pointer to a buffer
 * \param [IN]  len   Length for unit of beat size to be calculated
 * \param [IN]  beatsize  beat size (CRCBEATSIZE_BYTE, CRCBEATSIZE_HWORD, CRCBEATSIZE_WORD)
 * \retval Calculated CRC checksum value
 */
uint32_t CalculateCRC32( void *buff, uint32_t len, enum crc_beatsize beatsize );
void memcpy32( uint32_t *dst, uint32_t *src, uint32_t size );
void memset32( uint32_t *dst, uint32_t val, uint32_t size );
char *strcpy2( char *dst, const char *src );
#if _USE_UTILITY_MEMCPY2_
uint8_t *memcpy2( uint8_t *dst, const uint8_t *src, uint16_t size );
#endif

#endif // __UTILITIES_H__
