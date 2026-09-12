/*!
 * \file      sys_spi.h
 *
 * \brief     HeaderFile of SPI driver implementation
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
#ifndef __SPI_H__
#define __SPI_H__

#include "sys_gpio.h"

/*!
 * SPI peripheral ID
 */
typedef enum
{
    SPI_1 = 0,
    SPI_2 = 1,
}SpiId_t;

#define SPI_BAUD_MAX	8000000		// 8MHz

/*!
 * SPI object type definition
 */
typedef struct Spi_s
{
    SpiId_t SpiId;
	bool enable;
	uint32_t baudrate;
	struct spi_module *module;
    Gpio_t Mosi;
    Gpio_t Miso;
    Gpio_t Sclk;
    Gpio_t Nss;
}Spi_t;

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
 void SpiInit( Spi_t *obj, SpiId_t spiId, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss, uint32_t baud );

/*!
 * \brief De-initializes the SPI object and MCU peripheral
 *
 * \param [IN] obj SPI object
 */
void SpiDeInit( Spi_t *obj );

/*!
 * \brief Configures the SPI peripheral
 *
 * \remark Slave mode isn't currently handled
 *
 * \param [IN] obj   SPI object
 * \param [IN] bits  Number of bits to be used. [8 or 16]
 * \param [IN] cpol  Clock polarity
 * \param [IN] cpha  Clock phase
 * \param [IN] slave When set the peripheral acts in slave mode
 */
void SpiFormat( Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave );

/*!
 * \brief Sets the SPI speed
 *
 * \param [IN] obj SPI object
 * \param [IN] hz  SPI clock frequency in hz
 */
void SpiFrequency( Spi_t *obj, uint32_t hz );

/*!
 * \brief Sends outData and receives inData
 *
 * \param [IN] obj     SPI object
 * \param [IN] outData Byte to be sent
 * \retval inData      Received byte.
 */
uint16_t SpiInOut( Spi_t *obj, uint16_t outData );

/*!
 * \brief Sends buffer data
 *
 * \param [IN] obj     SPI object
 * \param [IN] buff    Buffer to be sent
 * \param [IN] len     Length of data in buffer
 */
void SpiOutBuffer( Spi_t *obj, uint8_t *buff, uint16_t len );

/*!
 * \brief Receives buffer data
 *
 * \param [IN] obj     SPI object
 * \param [IN] buff    Buffer to be receive
 * \param [IN] len     Length of data in buffer
 */
void SpiInBuffer( Spi_t *obj, uint8_t *buff, uint16_t len );

/*!
 * \brief Sends buffer data
 *
 * \param [IN] obj     SPI object
 * \param [IN] outBuff Buffer to be sent
 * \param [IN] inBuff  Buffer to be receive
 * \param [IN] len     Length of data in buffer
 */
void SpiInOutBuffer( Spi_t *obj, uint8_t *outBuff, uint8_t *inBuff, uint16_t len );

void SpiEnable( Spi_t *obj );
void SpiDisable( Spi_t *obj );

#endif // __SPI_H__
