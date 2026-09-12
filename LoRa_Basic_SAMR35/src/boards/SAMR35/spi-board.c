/*!
 * \file      spi-board.c
 *
 * \brief     Target board SPI driver implementation
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
#include "sys_fifo.h"
#include "sys_spi.h"
#include "sys_timer.h"
#include "sys_uart.h"
#include "spi-board.h"

struct spi_module spi_master_instance[2];

const sercom_settings_t spi_board_settings[2] =
{//                                       MISO                     , (Slave SS)   , MOSI                     , SCK
	{ SERCOM4, SPI_SIGNAL_MUX_SETTING_E, {PINMUX_PC19F_SERCOM4_PAD0, PINMUX_UNUSED, PINMUX_PB30F_SERCOM4_PAD2, PINMUX_PC18F_SERCOM4_PAD3} },
	{ SERCOM5, SPI_SIGNAL_MUX_SETTING_E, {PINMUX_PB02D_SERCOM5_PAD0, PINMUX_UNUSED, PINMUX_PB22D_SERCOM5_PAD2, PINMUX_PB23D_SERCOM5_PAD3} },
};

const enum spi_transfer_mode spi_xfer_mode[4] = { SPI_TRANSFER_MODE_0, SPI_TRANSFER_MODE_1, SPI_TRANSFER_MODE_2, SPI_TRANSFER_MODE_3};

/*!
 * \brief Initializes the SPI object and MCU peripheral
 *
 * \remark When NSS pin is software controlled set the pin name to NC otherwise
 *         set the pin name to be used.
 *
 * \param [IN] obj  SPI object
 * \param [IN] mosi SPI MOSI pin name to be used
 * \param [IN] miso SPI MISO pin name to be used
 * \param [IN] sclk SPI SCLK pin name to be used
 * \param [IN] nss  SPI NSS pin name to be used
 * \param [IN] baud SPI baudrate
 */
void SpiInit( Spi_t *obj, SpiId_t spiId, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss, uint32_t baud )
{
	obj->SpiId = spiId;
	obj->baudrate = baud;
	obj->enable = false;
	GpioMcuSetObj( &(obj->Mosi), mosi );
	GpioMcuSetObj( &(obj->Miso), miso );
	GpioMcuSetObj( &(obj->Sclk), sclk );
	GpioMcuSetObj( &(obj->Nss ), nss  );

	if( nss != PIN_NC )
	{
		/* Get default config for pin */
		struct port_config pin_conf;
		port_get_config_defaults(&pin_conf);
		/* Edit config to set the pin as output */
		pin_conf.direction = PORT_PIN_DIR_OUTPUT;
		/* Set config on Slave Select pin */
		port_pin_set_config(nss, &pin_conf);
		port_pin_set_output_level(nss, true);
	}
	sercom_settings_t const *spi_set_p = &spi_board_settings[spiId];
	obj->module = &spi_master_instance[obj->SpiId];

	/* Configure, initialize and enable SERCOM SPI module */
	struct spi_config config_spi_master;
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.data_order = (baud & 0x04) ? SPI_DATA_ORDER_LSB : SPI_DATA_ORDER_MSB;
	config_spi_master.transfer_mode = spi_xfer_mode[baud & 0x03];
	config_spi_master.mode_specific.master.baudrate = baud & ~0x07;
	config_spi_master.mux_setting = spi_set_p->muxsettimg;
	config_spi_master.pinmux_pad0 = spi_set_p->pinmux_pad[0];
	config_spi_master.pinmux_pad1 = spi_set_p->pinmux_pad[1];
	config_spi_master.pinmux_pad2 = spi_set_p->pinmux_pad[2];
	config_spi_master.pinmux_pad3 = spi_set_p->pinmux_pad[3];
	spi_init(obj->module, spi_set_p->hw, &config_spi_master);
//	spi_enable(spip);
}

/*!
 * \brief De-initializes the SPI object and MCU peripheral
 *
 * \param [IN] obj SPI object
 */
void SpiDeInit( Spi_t *obj )
{
//	if( obj->SpiId < SPI_1 || obj->SpiId > SPI_2 ) return;
    GpioInit( &obj->Miso, obj->Miso.pin, PIN_INPUT , PIN_PULL_DOWN, 0 );
    GpioInit( &obj->Mosi, obj->Mosi.pin, PIN_OUTPUT, PIN_NO_PULL  , 0 );
    GpioInit( &obj->Sclk, obj->Sclk.pin, PIN_OUTPUT, PIN_NO_PULL  , 0 );
    GpioInit( &obj->Nss , obj->Nss.pin , PIN_OUTPUT, PIN_NO_PULL  , 1 );
	SpiDisable( obj );
	obj->enable = false;
	obj->baudrate = 0;
	obj->SpiId = -1;
}

/*!
 * \brief Sends outData and receives inData
 *
 * \param [IN] obj     SPI object
 * \param [IN] outData Byte to be sent
 * \retval inData      Received byte.
 */
uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
	SercomSpi *spi_module = &(obj->module->hw->SPI);

	if( !obj->enable )	SpiEnable( obj );

	// Wait for bus idle (ready to write)
	while( ( SERCOM_SPI_INTFLAG_DRE & spi_module->INTFLAG.reg ) == 0 );
	spi_module->INTFLAG.reg = SERCOM_SPI_INTFLAG_DRE;

	// Write byte
	spi_module->DATA.reg = outData;	

	// Wait for ready to read
	while( ( SERCOM_SPI_INTFLAG_RXC & spi_module->INTFLAG.reg ) == 0 );
	spi_module->INTFLAG.reg = SERCOM_SPI_INTFLAG_RXC;

	// Read byte
	outData = ( uint16_t )spi_module->DATA.reg;

	if( !obj->enable )	SpiDisable( obj );

	return outData;
}

/*
 *
 */
void SpiOutBuffer( Spi_t *obj, uint8_t *buff, uint16_t len )
{
	SpiInOutBuffer( obj, buff, NULL, len );
}

/*
 *
 */
void SpiInBuffer( Spi_t *obj, uint8_t *buff, uint16_t len )
{
	SpiInOutBuffer( obj, NULL, buff, len );
}

//
inline void Spi_Write( SercomSpi *spi, uint8_t data )
{
	// Wait for bus idle (ready to write)
#if 0
	for( uint8_t i = 100; i ; i-- ){ if( SERCOM_SPI_INTFLAG_DRE & spi->INTFLAG.reg ) break; }
#else
	while( (SERCOM_SPI_INTFLAG_DRE & spi->INTFLAG.reg ) == 0 );
#endif
	spi->INTFLAG.reg = SERCOM_SPI_INTFLAG_DRE;
	spi->DATA.reg = data;
}

//
inline uint8_t Spi_Read( SercomSpi *spi )
{
#if 0
	for( uint8_t i = 100; i ; i-- ){ if( SERCOM_SPI_INTFLAG_RXC & spi->INTFLAG.reg ) break; }
#else
	while( (SERCOM_SPI_INTFLAG_RXC & spi->INTFLAG.reg ) == 0 );
#endif
	spi->INTFLAG.reg = SERCOM_SPI_INTFLAG_RXC;
	return spi->DATA.reg;
}

/*
 *
 */
void SpiInOutBuffer( Spi_t *obj, uint8_t *outBuff, uint8_t *inBuff, uint16_t len )
{
//	if( obj->Initialized == false ) return;
	SercomSpi *spi_module = &(obj->module->hw->SPI);
	SpiEnable( obj );
	
	uint8_t InOutData;

	while( len )
	{
		// Write byte
		InOutData = (outBuff == NULL) ? 0 : *outBuff++;
		Spi_Write( spi_module, InOutData );

		// Read byte
		InOutData = Spi_Read( spi_module );
		if( inBuff != NULL )
		{
			*inBuff++ = InOutData;
		}
		len--;
	}
	SpiDisable( obj );
}

/*
 *
 */
void SpiEnable( Spi_t *obj )
{
	BoardSercomEnableMclk( obj->module->hw );
	spi_enable( obj->module );
}

/*
 *
 */
void SpiDisable( Spi_t *obj )
{
	spi_disable( obj->module );
	BoardSercomDisableMclk( obj->module->hw );
}
