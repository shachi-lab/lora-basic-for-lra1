/*!
 * @file    bios_gps.h
 * @brief   HeaderFile of GPS for Basic Interpreter
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
#ifndef	__BIOS_GPS_H__
#define	__BIOS_GPS_H__

typedef struct
{
	int8_t	year;
	int8_t	mon;
	int8_t	day;
	int8_t	hour;
	int8_t	min;
	int8_t	sec;
} gps_Datetime_t;

#define GPS_EPOCH_TO_20JAN01	(1261872000 + 18)	// from GPS-Epoch(80Jan06) + Leap Seconds(18)

void gps_GetRecv( int32_t timeout, int32_t hdop );
void gps_ConvDatetime( int32_t tm, int32_t *array_p );
int32_t gps_ConvTimeVal( int32_t *array_p );
uint8_t gps_SplitPara( char *str, char **para, uint8_t siz );
void gps_CalcDistance( int32_t val[] );
int32_t gps_anat2_int( int32_t x, int32_t y );
int32_t gps_sqrt_int( int32_t x );
int32_t gps_pow_int( int32_t x, int32_t y );
int32_t gps_log_int( int32_t x, int32_t y );

#endif