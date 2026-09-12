/*!
 * @file    pvt_fwd.h
 * @brief   HeaderFile of "LRA1 LoRa P2P protocol (Forwarding Extension)"
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
#ifndef __PVT_FWD_H__
#define __PVT_FWD_H__

#if __LORA_FWD_ENABLE__

#define FORWARD_KEEP_TIME			30000		// 30 sec > (4sec * 7hop = 28)
#define FORWARD_LIST_SIZE			16			// 16 entries
#define FORWARD_DELAY_MIN			60			// 60 ms
#define FORWARD_DELAY_MASK			63			// 63 ms ( delay = (RND & MASK) + MIN )
#define FORWARD_TIME_RETRY			1000
#define FORWARD_TIME_MAX			6000

#define FORWARD_DISABLE		0
#define FORWARD_NORMAL		1
#define FORWARD_ONLY		2
#define FORWARD_BOTH		3

inline bool Pvt_Is_Forward_Enable( void )	{ return (GET_CTRL_FORWARD() != FORWARD_DISABLE); }	// �]���@�\�L���H
inline bool Pvt_Is_Forward_FwdRxOnly( void ){ return (GET_CTRL_FORWARD() == FORWARD_ONLY);	}	// �]��Pkt�̂ݎ�M�H
inline bool Pvt_Is_Forward_FwdRxBoth( void ){ return (GET_CTRL_FORWARD() == FORWARD_BOTH);	}	// ������M�H�i�����X�g�ɓ���Ȃ��j

bool Pvt_Forward_Check( DataFrame_t *frame_p );
UiRes_t Pvt_Forward_Send( void );
uint32_t Pvt_Forward_SetTxConf( int32_t val );
uint32_t Pvt_Forward_GetTxConf( void );

typedef union
{
	struct {
	uint8_t		Sf;
	uint8_t		Bw;
	uint8_t		Cr;
	uint8_t		Ch;
	};
	uint32_t	parm;
} fwdTxConf_t;

#endif
#endif
