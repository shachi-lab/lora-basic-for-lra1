/*!
 * \file      crypto_hw_aes.h
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
#ifndef _CRYPTO_HW_AES_H_
#define	_CRYPTO_HW_AES_H_

#if defined(_HW_AES_MODE_) && (_HW_AES_MODE_ == true)

typedef uint32_t aes_context;		// Dummy

void hw_aes_init( aes_context *ctx );
void hw_aes_deinit( void );
void hw_aes_set_key( const uint8_t *key, uint8_t keylen );
void hw_aes_encrypt( const uint8_t *inp, uint8_t *out );

#define aes_initialize( ctx )			hw_aes_init( ctx )
#define aes_deinit( ctx )				hw_aes_deinit( )
#define aes_set_key( key, len, ctx )	hw_aes_set_key( key, len )
#define aes_encrypt( inp, out, ctx )	hw_aes_encrypt( inp, out )

#endif

#endif
