/*!
 * \file      crypto_hw_aes.c
 *
 * \brief     Crypto H/W Driver 
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
#include <stdio.h>
#include <string.h>
#include "board-config.h"
#include "board-mcu.h"
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_timer.h"
#include "radio.h"
#include "bme280-board.h"
#include "basic.h"
#include "bios.h"

#include "crypto_aes.h"

#if defined(_HW_AES_MODE_) && (_HW_AES_MODE_ == true)

static union{
	uint8_t b[16];
	uint32_t l[4];
}hw_aes_buff;

/*
 * hw AES Initialize
 */
void hw_aes_init( aes_context *ctx )
{
	/* Enable clock for AES */
	MCLK->APBCMASK.reg |= MCLK_APBCMASK_AES;

	/* Perform a software reset */
	AES->CTRLA.reg = AES_CTRLA_SWRST;

	/* Initialize the AES with new configurations */
	uint32_t ul_mode = AES_CTRLA_CIPHER 	/* ENCRYPTION	*/
			| AES_CTRLA_STARTMODE			/* AUTO_START	*/
			| AES_CTRLA_KEYSIZE(0)			/* KEY_SIZE_128	*/
			| AES_CTRLA_AESMODE(0)			/* ECB_MODE		*/
			| AES_CTRLA_CFBS(0)				/* CFB_SIZE_128	*/
			| AES_CTRLA_CTYPE(0x0f);		/* COUNTERMEASURE_TYPE_ALL	*/

	AES->CTRLA.reg = ul_mode;
	AES->CTRLA.reg |= AES_CTRLA_ENABLE;
}


/*
 * hw AES DeInitialize
 */
void hw_aes_deinit( void )
{
	/* Disbale interrupt */
	AES->INTENCLR.reg = AES_INTENCLR_MASK;
	/* Clear interrupt flag */
	AES->INTFLAG.reg = AES_INTFLAG_MASK;
	AES->CTRLA.reg &= (~AES_CTRLA_ENABLE);

	MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_AES;
}

/*
 *  Set the cipher key for the pre-keyed version
 */
void hw_aes_set_key( const uint8_t *key, uint8_t keylen )
{
	memcpy( hw_aes_buff.b, key, keylen );

	AES->KEYWORD[0].reg = hw_aes_buff.l[0];
	AES->KEYWORD[1].reg = hw_aes_buff.l[1];
	AES->KEYWORD[2].reg = hw_aes_buff.l[2];
	AES->KEYWORD[3].reg = hw_aes_buff.l[3];
}

/*
 *  Encrypt a single block of 16 bytes
 */
void hw_aes_encrypt( const uint8_t *inp, uint8_t *out )
{
	memcpy( hw_aes_buff.b, inp, 16 );

	/* Write the data to be deciphered to the input data registers. */
	AES->DATABUFPTR.reg = 0;
	AES->INDATA.reg = hw_aes_buff.l[0];
	AES->INDATA.reg = hw_aes_buff.l[1];
	AES->INDATA.reg = hw_aes_buff.l[2];
	AES->INDATA.reg = hw_aes_buff.l[3];

	/* Wait for the end of the decryption process. */
	while( !(AES->INTFLAG.reg & AES_INTFLAG_ENCCMP) );

	AES->DATABUFPTR.reg = 0;
	hw_aes_buff.l[0] = AES->INDATA.reg;
	hw_aes_buff.l[1] = AES->INDATA.reg;
	hw_aes_buff.l[2] = AES->INDATA.reg;
	hw_aes_buff.l[3] = AES->INDATA.reg;

	memcpy( out, hw_aes_buff.l, 16 );
}

#endif

