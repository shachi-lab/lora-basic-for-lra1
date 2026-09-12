/*!
 * \file      bme280-board.c
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
#include "asf.h"
#include <stdlib.h>
#include "utilities.h"
#include "sys_delay.h"
#include "board-config.h"
#include "sys_gpio.h"
#include "sys_i2c.h"
#include "bme280/bme280.h"
#include "bme280-board.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define	BME280_I2C_SEL		0			// 0:Primary / 1:Second

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#if BME280_I2C_SEL == 0
static uint8_t dev_addr = BME280_I2C_ADDR_PRIM;
#else
static uint8_t dev_addr = BME280_I2C_ADDR_SEC;
#endif

struct bme280_data BME280_Comp_Data;

/* Private function prototypes -----------------------------------------------*/
static int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev);
static BME280_INTF_RET_TYPE bme280_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
static BME280_INTF_RET_TYPE bme280_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 * Delay function map to COINES platform
 */
static void bme280_delay_us(uint32_t period, void *intf_ptr)
{
	DelayUs( period );
}

//-----------------------------------------------
//	BME280�Ōv������ (I2C)
//-----------------------------------------------
int8_t bme280_measurement(void)
{
	I2cInit( );

	struct bme280_dev dev;
//	dev.intf_ptr = &dev_addr;
	dev.intf = BME280_I2C_INTF;
	dev.read = bme280_i2c_read;
	dev.write = bme280_i2c_write;
	dev.delay_us = bme280_delay_us;

	int8_t rslt;
	rslt = bme280_init(&dev);
	if( rslt == BME280_OK )
	{
		rslt = stream_sensor_data_forced_mode(&dev);
	}
	bme280_set_sensor_mode(BME280_POWERMODE_SLEEP, &dev);	
//	I2cDeInit( );
	return rslt;
}

//-----------------------------------------------
//	�v���l�𑗐M�f�[�^�ɒP�ʕϊ�����
//-----------------------------------------------
void bme280_to_data( bme280_data_t *data )
{
	data->temperature = BME280_Comp_Data.temperature / 10;	// �C�� (0.1deg)
 	data->pressure    = BME280_Comp_Data.pressure / 10;		// �C�� (0.1hPa)
	uint32_t hum_temp = (BME280_Comp_Data.humidity * 10) / 1024;	// ���x (0.1%)
 	data->humidity    = hum_temp;
}

//-----------------------------------------------
//	BME280�Ōv�����J�n����
//-----------------------------------------------
static int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev)
{
    int8_t rslt;
	uint8_t status;
	
    struct bme280_settings settings;
    /* Configuring the over-sampling rate, filter coefficient and standby time */
    /* Overwrite the desired settings */
    settings.filter = BME280_FILTER_COEFF_8;
    /* Over-sampling rate for humidity, temperature and pressure */
    settings.osr_h = BME280_OVERSAMPLING_8X;
    settings.osr_p = BME280_OVERSAMPLING_8X;
    settings.osr_t = BME280_OVERSAMPLING_8X;
    /* Setting the standby time */
    settings.standby_time = BME280_STANDBY_TIME_0_5_MS;

    uint8_t settings_sel = BME280_SEL_OSR_PRESS | BME280_SEL_OSR_TEMP | BME280_SEL_OSR_HUM | BME280_SEL_FILTER;
    rslt = bme280_set_sensor_settings(settings_sel, &settings, dev);
	if( rslt != BME280_OK )	return rslt;

	rslt = bme280_set_sensor_mode(BME280_POWERMODE_FORCED, dev);
	if( rslt != BME280_OK )	return rslt;

    /* Wait for the measurement to complete and print data @25Hz */
	for( int i = 0; i < 10 ; i++ )
	{
	    DelayMs(10);
        rslt = bme280_get_regs(BME280_REG_STATUS, &status, 1, dev);
		if( rslt != BME280_OK )	break;
		if((status & BME280_STATUS_MEAS_DONE) == 0)
		{
		    rslt = bme280_get_sensor_data(BME280_ALL, &BME280_Comp_Data, dev);
/* ------------------------------
 * 	����l�͈ȉ��Ɋi�[�����
 * 
 *	BME280_Comp_Data.temperature
 *	BME280_Comp_Data.pressure
 *	BME280_Comp_Data.humidity
 * ------------------------------ */
			break;
 		}
		 rslt = -1;
	}
    return rslt;
}

//-----------------------------------------------
//
//-----------------------------------------------
static BME280_INTF_RET_TYPE bme280_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
//	dev_addr = *(uint8_t*)intf_ptr;
	return I2CReadMemory( dev_addr, reg_addr, reg_data, length );
}

//-----------------------------------------------
//
//-----------------------------------------------
static BME280_INTF_RET_TYPE bme280_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
//	dev_addr = *(uint8_t*)intf_ptr;
	return I2CWriteMemory( dev_addr, reg_addr, reg_data, length );
}
