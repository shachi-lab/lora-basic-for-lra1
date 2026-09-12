 /*!
 * @file    bios_gps.c
 * @brief   GPS for Basic Interpreter
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
#include <stdarg.h>
#include "string.h"
#include "ctype.h"
#include "board-config.h"
#include "board-mcu.h"
#include "uart-board.h"
#include "hw_timer.h"
#include "basic.h"
#include "bios.h"

typedef enum {
	GPS_RX_NONE		= 0x00,
	GPS_RX_TIME		= 0x01,
	GPS_RX_LOCATION	= 0x02,
	GPS_RX_DONE		= (GPS_RX_TIME | GPS_RX_LOCATION),
} gps_stat_t;

static gps_stat_t gps_GetNMEA( char *buf );
static int32_t gps_GetLocation( char *pdeg, char *psig );
static int32_t gps_GatDatetime( char *pdate, char *ptime );
static int32_t gps_GetDecimal( char *str );
static int32_t gps_Dec2Val( char *str );
static int32_t gps_datetime_to_tm( gps_Datetime_t *ptm );
static void gps_tm_to_datetime( int32_t tm, gps_Datetime_t *ptm );

#define	YEAR_ORIGIN			20
#define	DAY_PER_Y			(int32_t)365
#define	MONTH_PER_Y			(int32_t)12
#define	DAY_PER_M			(int32_t)30
#define	HOUR_PER_D			(int32_t)24
#define	MIN_PER_H			(int32_t)60
#define	SEC_PER_N			(int32_t)60
#define	SEC_PER_H			(SEC_PER_N * MIN_PER_H)		// 60*60
#define	SEC_PER_D			(SEC_PER_H * HOUR_PER_D)	// 60*60*24
#define	SEC_PER_M			(SEC_PER_D * DAY_PER_M)		// 60*60*24*30
#define SEC_PER_Y			(SEC_PER_M * MONTH_PER_Y)	// 60*60*24*30*12

#define GPS_SQRT_USE_LIBRALY	0
#define GPS_POW_USE_LIBRALY		0

static gps_stat_t gps_stat;
static int32_t gps_altitude;
static int32_t gps_latitude;
static int32_t gps_longitude;
static int32_t gps_datetime;
static int32_t gps_hdop;
static gps_Datetime_t gps_tm;

static int16_t const gps_year_day[] = { 0,  0, 31, 59, 90,120,151,181,212,243,273,304,334,365 };

/*
 * gps Main
 */
void gps_GetRecv( int32_t timeout, int32_t hdop )
{
	arrayVariables[0] = arrayVariables[1] = arrayVariables[2] = arrayVariables[3] = 0;
	gps_hdop = 0;

	uint8_t (*hook_func)(void) = val_initInkey( timeout, true );
	gps_stat = GPS_RX_NONE;
	while ( 1 )
	{	
		int len = GetUartString( hook_func, bios_UartRxBuff, BIOS_UART_RX_BUFF_SIZE, ENCODE_RAW, true );
		if ( len == 0 ) continue;
		if ( len < 0 ) break;
		gps_stat |= gps_GetNMEA( bios_UartRxBuff );
		if ( gps_stat == GPS_RX_DONE )
		{
			arrayVariables[0] = gps_datetime;
			arrayVariables[1] = gps_latitude;
			arrayVariables[2] = gps_longitude;
			arrayVariables[3] = gps_altitude;
			arrayVariables[4] = gps_hdop;
			if ( hdop == 0 || gps_hdop <= hdop ) break;
		}
	}
    bios_Uart2Deinit();
}

/*
 * NMEA�Z���e���X�̃g�[�J�[���`�F�b�N
 * (NMEA 0183 Standard Version 4.11)
 */
static bool gps_IsTalker( char *buf )
{
	if ( buf[0] != '$' )	return false;
	if ( buf[1] != 'G' )	return false;
	if ( buf[2] != 'P' )	return true;	// GPS
	if ( buf[2] != 'Q' )	return true;	// �݂��т�(QZSS)
	if ( buf[2] != 'L' )	return true;	// GLONASS
	if ( buf[2] != 'A' )	return true;	// Galileo
	if ( buf[2] != 'B' )	return true;	// �k�l(BeiDou)
	if ( buf[2] != 'N' )	return true;	// GNSS
	return false;
}

/*
 * NMEA�Z���e���X����\���̂Ƀf�[�^�[���i�[
 */
static gps_stat_t gps_GetNMEA( char *buf )
{
	char *split[12];
	do {
		if ( gps_IsTalker( buf ) == false ) break;

		gps_SplitPara( &buf[3], split, 12 );

		if ( !strcmp( split[0], "RMC" ) )
		{
			if ( *split[1] == '\0' )	break;
			if ( *split[2] != 'A'  )	break;
			if ( *split[3] == '\0' )	break;
			if ( *split[5] == '\0' )	break;
			if ( *split[9] == '\0' )	break;
//			gps_latitude  = gps_GetLocation( split[3], split[4] );
//			gps_longitude = gps_GetLocation( split[5], split[6] );
			gps_datetime  = gps_GatDatetime( split[9], split[1] );
			return GPS_RX_TIME;
		}
		if ( !strcmp( split[0], "GGA" ) )
		{
			if ( *split[1] == '\0' )	break;
			if ( *split[2] == '\0' )	break;
			if ( *split[4] == '\0' )	break;
			if ( *split[9] == '\0' )	break;
			gps_latitude  = gps_GetLocation( split[2], split[3] );
			gps_longitude = gps_GetLocation( split[4], split[5] );
			gps_altitude  = gps_GetDecimal( split[9] );
			gps_hdop      = gps_GetDecimal( split[8] );
			return GPS_RX_LOCATION;
		}
	} while (0);
	return 0;
}

/*
 * �J���}��؂�̕�����𕪉�����
 */
uint8_t gps_SplitPara( char *str, char **para, uint8_t siz )
{
	char n, *ss;;
	uint8_t	i, cnt = 0, mode = 0, nx = 0;

	para[cnt++] = ss = str;
	while ( *str != '\0' ) {
		n = *str++;
		if ( n == 0x5c ) {
			if ( nx == 0x5c ) {
				*ss++ = n;
				n = 0;
			}
		} else
		if ( n == 0x22 ) {
			if ( nx == 0x5c ) {
				*ss++ = n;
			} else {
				mode ^= 1;
			}
		} else {
			if ( mode == 0 ) {
				if (n == '#')	break;
				else
				if ((n == ',')||((cnt == 1)&&(n == '='))) {
					*ss++ = '\0';
					if ( cnt < siz )	para[cnt++] = ss;
				} else
				if ((n > 0x20)&&(n < 0x80))	*ss++ = n;
			} else {
				if (n >= 0x20) 	*ss++ = n;
			}
		}
		nx = n;
	}
	*ss = '\0';
	for ( i = cnt; i < siz; i++ )	para[i] = ss;
	return ( cnt );
}

/*
 *
 */
static int32_t gps_getdec( char *str, int len )
{
	int32_t val = 0;
	for ( ; len; len-- )
	{
		val *= 10;
		if ( isdigit( *str ) )
		{
			val += *str++ - '0';
		}
	}
	return val;
}

/*
 *
 */
static int32_t gps_Dec2Val( char *str )
{
	return gps_getdec( str, 2 );
}

/*
 * GPS�ܓx�o�x����10�i���`���ɕϊ�����
 * (dddmm.mmmm -> ddd.dddddd)
 */
static int32_t gps_GetLocation( char *pdeg, char *psig )
{
	int32_t val, deg, min, n, m = 0;

	if ( *pdeg == 0 || *psig == 0 )	return 0;

	n = Ui_DecStr2Val( &pdeg );
	if ( *pdeg++ == '.' )
	{
		m = gps_getdec( pdeg, 4 );
	}
	deg = n / 100;
	min = n % 100;
	min = (min * 10000) + m;
	min = (min * 10) / 6;
	val = (deg * 1000000) + min;
	if ( *psig == 'W' || *psig == 'S' )	val = -val; 

	return val;
}

/*
 * GPS�̐��l����int32_t�ϊ�����(10�{�����)
 * �����_�ȉ��P���̂ݗL��
 */
static int32_t gps_GetDecimal( char *str )
{
	int32_t val = 0;
	bool flag = false;

	if ( *str == 0 )	return 0;
	if ( *str == '-' )
	{
		flag = true;
		str++;
	}

	val = Ui_DecStr2Val( &str ) * 10;
	if ( *str++ == '.' )
	{
		val += gps_getdec( str, 1 );
	}
	if ( flag )	val = -val;
	return val;
}

/*
 * GPS���������擾����
 */
static int32_t gps_GatDatetime( char *pdate, char *ptime )
{
	gps_tm.year = gps_Dec2Val( &pdate[4] );
	gps_tm.mon  = gps_Dec2Val( &pdate[2] );
	gps_tm.day  = gps_Dec2Val( &pdate[0] );
	gps_tm.hour = gps_Dec2Val( &ptime[0] );
	gps_tm.min  = gps_Dec2Val( &ptime[2] );
	gps_tm.sec  = gps_Dec2Val( &ptime[4] );
	int32_t tm = gps_datetime_to_tm( &gps_tm );
	return tm;
}

/*
 * �V���A������������ɕ�������
 */
void gps_ConvDatetime( int32_t tm, int32_t *array_p )
{
	gps_tm_to_datetime( tm, &gps_tm );
	*array_p++ = gps_tm.year;
	*array_p++ = gps_tm.mon;
	*array_p++ = gps_tm.day;
	*array_p++ = gps_tm.hour;
	*array_p++ = gps_tm.min;
	*array_p++ = gps_tm.sec; 
}

/*
 * �������V���A�������ɂ���
 */
int32_t gps_ConvTimeVal( int32_t *array_p )
{
	gps_tm.year = *array_p++;
	gps_tm.mon  = *array_p++;
	gps_tm.day  = *array_p++;
	gps_tm.hour = *array_p++;
	gps_tm.min  = *array_p++;
	gps_tm.sec  = *array_p++;
	return gps_datetime_to_tm( &gps_tm );
}

/*
 * ���t��������V���A���b���v�Z���� �iDAY,MONTH��1�`�Ƃ���j
 */
static int32_t gps_datetime_to_tm( gps_Datetime_t *ptm )
{
	int32_t	tm, yy;

	if( ptm->mon > MONTH_PER_Y ) return 0;
	yy = ptm->year - YEAR_ORIGIN;
#if YEAR_ORIGIN > 50
	if ( ptm->year < YEAR_ORIGIN ) yy += 100;
#else
	if ( ptm->year < YEAR_ORIGIN ) yy = 0;
#endif
	tm = (yy * DAY_PER_Y) + ((yy + 3) / 4);
	tm += gps_year_day[ ptm->mon ];
	if (!(yy & 3) && (ptm->mon > 2)) tm++;
	if ( ptm->day ) tm += ptm->day - 1;
	tm = ptm->hour + tm * HOUR_PER_D;
	tm = ptm->min + tm * MIN_PER_H;
	tm = ptm->sec + tm * SEC_PER_N;
	return tm;
}

/*
 * �V���A���b������t�������v�Z����(DAY,MONTH��1�`�Ƃ���j
 */
static void gps_tm_to_datetime( int32_t tm, gps_Datetime_t *ptm )
{
	int32_t leap, yd, md = 0;

	if( tm < 0 ) tm = 0;
	ptm->sec = tm % SEC_PER_N;
	tm /= SEC_PER_N;
	ptm->min = tm % MIN_PER_H;
	tm /= MIN_PER_H;
	ptm->hour = tm % HOUR_PER_D;
	tm /= HOUR_PER_D;
	for ( ptm->year = YEAR_ORIGIN; ;ptm->year++ )
	{
		leap = (ptm->year & 0x03) ? 0 : 1;
		yd = 365 + leap;
		if ( tm < yd )	break;
		tm -= yd;
	}
#if YEAR_ORIGIN > 50
	if ( ptm->year >= 100 )	ptm->year -= 100;	
#endif
	for ( ptm->mon = 2; ; ptm->mon++ )
	{
		yd = gps_year_day[ptm->mon];
		if ( ptm->mon > 2 )	yd += leap;
		if ( tm < yd )	break;
		md = yd;
	}
	ptm->mon--;
	ptm->day = tm - md + 1;	
}

#include <float.h>
#include <math.h>

#if 0
#define DEGtoRAD( deg )		(((double)deg)*(M_PI/(180.0*1000000.0)))
#define RADtoDEG( rad )		(uint32_t)((rad)*((180.0*1000000.0)/M_PI))
#else
static double DEGtoRAD( int32_t deg ) { return (((double)deg)*(M_PI/(180.0*1000000.0))); }
static int32_t RADtoDEG( double rad ) { return	(uint32_t)((rad)*((180.0*1000000.0)/M_PI)); }
#endif

/*
 *
 */
static double gps_atan2( double x, double y )
{
	if ( fabs( x ) < DBL_EPSILON )	return 0; 
	double ang = atan( y / x );
	if ( x < 0 )	ang += ( y > 0 ) ? M_PI : -M_PI;
	return ang;
}

/*
 * �Q�n�_�Ԃ̋����ƕ��ʊp���v�Z����
 *
 * input  : �n�_�P(�ܓx=val[0], �o�x=val[1])�A�n�_�Q(�ܓx=val[2], �o�x=val[3])
 *          1000000�{�̒l
 * output : val[4]=����(m), val[5]=�p�x(deg)
 */
void gps_CalcDistance( int32_t val[] )
{ 
	double lat1 = DEGtoRAD(val[0]);
	double lat2 = DEGtoRAD(val[2]);
	double dx   = DEGtoRAD(val[3]-val[1]);
	double sin_dx = sin(dx);
	double cos_dx = cos(dx);
	double sin_lat1 = sin(lat1);
	double cos_lat1 = cos(lat1);
	double sin_lat2 = sin(lat2);
	double cos_lat2 = cos(lat2);
	double dis, ang;
	dis = 6378137.0 * acos( sin_lat1 * sin_lat2 + cos_lat1 * cos_lat2 * cos_dx );
	val[4] = (uint32_t)dis;
	ang = gps_atan2( sin_dx, (cos_lat1 * tan(lat2) - sin_lat1 * cos_dx) );
	ang = ((ang < M_PI_2) ? M_PI_2 : (M_PI_2 * 5)) - ang;
	val[5] = RADtoDEG( ang );
}

/*
 *
 */
int32_t gps_anat2_int( int32_t x, int32_t y )
{
	double xx = (double)x;
	double yy = (double)y;
	double val = gps_atan2( xx, yy );
	val *= 1000000.0;
	return (int32_t)val;
}

/*
 *
 */
int32_t gps_sqrt_int( int32_t x )
{
#if GPS_SQRT_USE_LIBRALY
	double val = sqrt( (double)x );
	return (int32_t)val;
#else
	if ( 0 > x ) return INT32_MAX;
	if ( 3 > x ) return 1;

	int32_t c = 3;		// �������̉���1�ɂȂ鐮���̐�(1��2��3��3��)
	int32_t val = 1; 	// ���[�v��(�������̉�)
    while ( true ) {
		x -= c;
    	if ( 0 >= x ) break;
    	c += 2;
    	val++;
   	}
   	return val;
#endif
 }

/*
 *
 */
int32_t gps_pow_int( int32_t x, int32_t y )
{
#if GPS_POW_USE_LIBRALY
	double val = pow( (double)x, (double)y );
	return (int32_t)val;
#else
    int32_t val = 1;
    while ( y > 0 )
	{
		if (y & 1)	val *= x;  // n �̍ŉ���bit�� 1 �Ȃ�� x^(2^i) ��������
	    x *= x;
	    y >>= 1;	// n ��1bit ���ɂ��炷
    }
    return val;
#endif
}

/*
 * log2 x => N.8bit
 */
static int32_t gps_log2_int( int32_t x )
{
	int32_t n = 31;

	while ( n && !(x & (1 << --n)) );
	if ( n > 8 ) { x >>= n - 8; } else { x <<= 8 - n; }
	n = (n << 8) + (x & 0xff);
	return n;
}

/*
 * log x / log y
 */
int32_t gps_log_int( int32_t x, int32_t y )
{
	if ( x < 1 || y < 2 ) return -1; 

	x = gps_log2_int( x );
	y = gps_log2_int( y );
	x /= y;
	return x;
}

