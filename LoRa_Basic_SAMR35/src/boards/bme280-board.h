/*!
 * \file      bme280-board.h
 *
 * \brief     Target board BME280 driver implementation
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
#ifndef __BME280_BOARD_H__
#define __BME280_BOARD_H__

typedef struct  
{
	int16_t	temperature;	// �C�� (0.1deg)
	int16_t	pressure;		// �C�� (hPa)
	int16_t	humidity;		// ���x (%)
} bme280_data_t;


int8_t bme280_measurement( void );
void bme280_to_data( bme280_data_t *data );

#endif // __BME280_BOARD_H__
