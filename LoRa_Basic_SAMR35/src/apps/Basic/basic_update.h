/*!
 * @file    basic_update.h
 * @brief   HeaderFile of "BASIC App Update"
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
#ifndef BASIC_UPDATE_H_
#define BASIC_UPDATE_H_

#define	UPDATE_CONF_PROID		0x3269			// '2i'
#define UPDATE_CONF_SF			7
#define UPDATE_CONF_CH			0				// Free
#define UPDATE_CONF_BW			LORA_BW_125k
#define UPDATE_CONF_CR			LORA_CR_4_5
#define UPDATE_SEND_RETRY		5
#define UPDATE_RES_TIMEOUT		2000
#define UPDATE_SEND_DELAY		200
#define UPDATE_BLOCK_SIZE		64

PROC( proc_Update );
bool basic_psave_update( uint8_t *top, uint32_t size );
bool basic_pload_update( bool append, int32_t timeout );
uint16_t basic_sum( uint8_t *ptr, uint32_t len );

#endif /* BASIC_UPDATE_H_ */