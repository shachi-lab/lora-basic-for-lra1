/*!
 * \file      sys_i2c.h
 *
 * \brief     HeaderFile of I2C driver implementation
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
#ifndef __SYS_I2C_H__
#define __SYS_I2C_H__

/*!
 * I2C peripheral ID
 */
typedef enum
{
    I2C_1 = 0
}I2cId_t;

void I2cInit( void );
void I2cDeInit( void );
bool I2cIsEnable( void );
uint8_t I2CWriteMemory( uint8_t dev_adrs, int16_t reg_addr, const uint8_t *reg_data, uint16_t len );
uint8_t I2CReadMemory( uint8_t dev_adrs, int16_t reg_addr, uint8_t *reg_data, uint16_t len );
void I2cSetBaudrate( uint32_t baudrate );
uint32_t I2cGetBaudrate( void );
void I2cDisable( void );
void I2cEnable( void );

#endif // __SYS_I2C_H__
