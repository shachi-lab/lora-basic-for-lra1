/*!
 * \file      i2c-board.c
 *
 * \brief     Target board I2C driver implementation
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
#include "board-mcu.h"
#include "board-config.h"
#include "utilities.h"
#include "sys_gpio.h"
#include "sys_delay.h"
#include "i2c-board.h"
#include "sys_gpio.h"
#include "sys_i2c.h"

#define I2C_TIMEOUT					1000
#define I2C_BUSY_RETRY_MAX			100
#define	I2C_LEN_MAX					32	

#define I2C_INSTANCE_MAX			1

static struct 
{
	bool IsInitialized;
	uint8_t Id;
	uint8_t Sda_pin;
	uint8_t Scl_pin;
	uint32_t Baudrate;
	struct i2c_master_module Instance;
} i2c_obj;

sercom_settings_t const i2c_board_settings[I2C_INSTANCE_MAX] =
{
	{ SERCOM1, 0, {PINMUX_PA16C_SERCOM1_PAD0, PINMUX_PA17C_SERCOM1_PAD1, PINMUX_UNUSED, PINMUX_UNUSED}, {I2C_SDA}, {I2C_SCL} },
};

/*!
 * \brief Initializes the I2C object and MCU peripheral
 *
 * \remark 
 *
 */
void I2cInit( void )
{
	if( i2c_obj.IsInitialized != false )	return;
	i2c_obj.IsInitialized = true;

	const sercom_settings_t *sp = &i2c_board_settings[I2C_1];
	struct i2c_master_config config_i2c_master;

	i2c_obj.Id = I2C_1;
	i2c_obj.Sda_pin = sp->sda_pin;
	i2c_obj.Scl_pin = sp->scl_pin;

	i2c_master_get_config_defaults( &config_i2c_master );
	config_i2c_master.buffer_timeout = I2C_TIMEOUT;
	config_i2c_master.pinmux_pad0 = sp->pinmux_pad[0];
	config_i2c_master.pinmux_pad1 = sp->pinmux_pad[1];
	config_i2c_master.scl_stretch_only_after_ack_bit = true;
	config_i2c_master.baud_rate = I2cGetBaudrate( );
	i2c_master_init( &i2c_obj.Instance, sp->hw, &config_i2c_master );
	i2c_master_enable( &i2c_obj.Instance );
}

/*
 *
 */
bool I2cIsEnable( void )
{
	return	i2c_obj.IsInitialized;
}

/*
 *
 */
void I2cSetBaudrate( uint32_t baudrate )
{
	if( (int32_t)baudrate < 0 )
	{
		I2cDeInit();
		return;
	}
	uint32_t old = I2cGetBaudrate( );
	i2c_obj.Baudrate = baudrate;
	i2c_obj.Baudrate = I2cGetBaudrate( );
	if( old != i2c_obj.Baudrate && i2c_obj.IsInitialized )
	{
		I2cDeInit();
		I2cInit();
	}
}

/*
 *
 */
uint32_t I2cGetBaudrate( void )
{
	if( i2c_obj.Baudrate == 0 )	return I2C_MASTER_BAUD_RATE_100KHZ;
	if( i2c_obj.Baudrate > I2C_MASTER_BAUD_RATE_400KHZ )	return I2C_MASTER_BAUD_RATE_400KHZ;
	return 	i2c_obj.Baudrate;
}

/*!
 * \brief De-initializes the I2C object and MCU peripheral
 *
 * \param [IN] obj I2C object
 */
void I2cDeInit( void )
{
	if( i2c_obj.IsInitialized == false )	return;
    i2c_obj.IsInitialized = false;
	I2cDisable( );

	GpioMcuInit( NULL, i2c_obj.Sda_pin, PIN_ANALOGIC, PIN_PULL_DOWN, 0 );
	GpioMcuInit( NULL, i2c_obj.Scl_pin, PIN_ANALOGIC, PIN_PULL_DOWN, 0 );
}

/*
 *
 */
void I2cEnable( void )
{
	BoardSercomEnableMclk( i2c_obj.Instance.hw );
	i2c_master_enable( &i2c_obj.Instance );
}

/*
 *
 */
void I2cDisable( void )
{
	i2c_master_disable( &i2c_obj.Instance );
	BoardSercomDisableMclk( i2c_obj.Instance.hw );
}

//-----------------------------------------------
//
//-----------------------------------------------
uint8_t I2CReadMemory( uint8_t dev_adrs, int16_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Stop       | -                   |
     * | Start      | -                   |
     * | Read       | (reg_data[0])       |
     * | Read       | (....)              |
     * | Read       | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */
	enum status_code stat = STATUS_ERR_NOT_INITIALIZED;
	if( i2c_obj.IsInitialized == false )	return stat;

	uint16_t timeout = 0;
	struct i2c_master_module *module = &i2c_obj.Instance;
	struct i2c_master_packet packet;
	packet.address = dev_adrs;
	packet.ten_bit_address = false;
	packet.high_speed = false;
	packet.hs_master_code = 0x0;
	if( reg_addr >= 0 )
	{
		uint8_t reg_temp = (uint8_t)reg_addr;
		packet.data = &reg_temp;
		packet.data_length = 1;
		do{
			stat = i2c_master_write_packet_wait_no_stop( module, &packet );
		}while( stat == STATUS_BUSY && ++timeout < I2C_BUSY_RETRY_MAX );
		if( stat != STATUS_OK )	len = 0;
	}
	if( len == 0 )
	{
		i2c_master_send_stop( module );
		return stat;
	}
	packet.data = reg_data;
	packet.data_length = len;
	do{
		stat = i2c_master_read_packet_wait( module, &packet );
	}while( stat == STATUS_BUSY && ++timeout < I2C_BUSY_RETRY_MAX );

	return stat;
}

//-----------------------------------------------
//
//-----------------------------------------------
uint8_t I2CWriteMemory( uint8_t dev_adrs, int16_t reg_addr, const uint8_t *reg_data, uint16_t len)
{
    /*
     * Data on the bus should be like
     * |------------+---------------------|
     * | I2C action | Data                |
     * |------------+---------------------|
     * | Start      | -                   |
     * | Write      | (reg_addr)          |
     * | Write      | (reg_data[0])       |
     * | Write      | (....)              |
     * | Write      | (reg_data[len - 1]) |
     * | Stop       | -                   |
     * |------------+---------------------|
     */
	enum status_code stat = STATUS_ERR_NOT_INITIALIZED;
	if( i2c_obj.IsInitialized == false )	return stat;

	uint8_t	buff[I2C_LEN_MAX+1];
	uint16_t timeout = 0;
	struct i2c_master_module *module = &i2c_obj.Instance;
	struct i2c_master_packet packet;

	if( len > I2C_LEN_MAX )	len = I2C_LEN_MAX;
	if( reg_addr < 0 )
	{
		packet.data = (uint8_t*)reg_data;
	}else
	{
		memcpy( &buff[1], reg_data, len );
		buff[0] = (uint8_t)reg_addr;
		packet.data = buff;
		len++;
	}
	packet.data_length = len;
	packet.address = dev_adrs;
	packet.ten_bit_address = false;
	packet.high_speed = false;
	packet.hs_master_code = 0x0;
	do 
	{
		stat = i2c_master_write_packet_wait( module, &packet );
	}while( stat == STATUS_BUSY && ++timeout < I2C_BUSY_RETRY_MAX );
	return stat;
}
