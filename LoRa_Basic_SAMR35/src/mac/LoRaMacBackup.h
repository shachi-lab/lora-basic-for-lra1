/*!
 * \file      LoRaMac_backup.h
 *
 * \brief     Heeaderfile of Backup module for LoRa-Mac
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
#ifndef __LORAMAC_BACKUP_H__
#define __LORAMAC_BACKUP_H__

#include "basic_conf.h"

#define MACZERO	__attribute__ ((section(".maczero"),aligned(4)))

#define MAC_BACKUP_BSS_VERSION	0x0001

#if __WAN_BACKUP_ENABLE__

void LoRaMacBackup_Initialize( void );
void LoRaMacBackup_SaveRAM( void );
void LoRaMacBackup_SaveFLASH( void );
bool LoRaMacBackup_Recover( bool *isJoin );
extern bool LoRaMacBackup_RequestFlag;

#define MAC_BACKUP_INITIALIZE()			LoRaMacBackup_Initialize()		// Clear : bss->RAM->Flash
#define MAC_BACKUP_JOIN_SAVE_RAM()		LoRaMacBackup_SaveRAM()			// bss->RAM
#define	MAC_BACKUP_JOIN_SAVE_FLASH()	LoRaMacBackup_SaveFLASH()		// bss->RAM->Flash
#define	MAC_BACKUP_JOIN_RECOVER(x)		LoRaMacBackup_Recover(x)		// bss<-RAM<-Flash
#define MAC_BACKUP_SET_REQUEST()		LoRaMacBackup_RequestFlag = true
#define MAC_BACKUP_REQUEST_SAVE()		do{ if( IS_CTRL_WAN_BACKUP_0 && LoRaMacBackup_RequestFlag ){ MAC_BACKUP_JOIN_SAVE_FLASH(); } } while(0)
#else

bool LoRaMacBackup_Recover( bool *isJoin );

#define MAC_BACKUP_INITIALIZE()
#define MAC_BACKUP_JOIN_SAVE_RAM()
#define	MAC_BACKUP_JOIN_SAVE_FLASH()
#define	MAC_BACKUP_JOIN_RECOVER(x)		LoRaMacBackup_Recover(x)
#define MAC_BACKUP_SET_REQUEST()
#define MAC_BACKUP_REQUEST_SAVE()

#endif

#if __WAN_BACKUP_ENABLE__
typedef union
{
	uint32_t	value[4];
	struct
	{
		uint8_t	mark;                   // 1 byte  ff:no-use / 7f:new / 77:old
		uint8_t channel;                // 1 byte
		uint8_t	lastTxChannel;          // 1 byte
		uint8_t adrAckCounter;          // 1 byte
		uint8_t channelsNbRepCounter;	// 1 byte
		uint8_t nodeAckRequested;       // 1 byte
		uint8_t x;                      // 1 byte
		uint8_t y;                      // 1 byte
		uint32_t upCounter;             // 4 bytes
		uint32_t downCounter;           // 4 bytes
	};
} MacBackupCounters_t;

MacBackupCounters_t* LoRaMacBackup_CounterLoad( void );
void LoRaMacBackup_CounterSave( MacBackupCounters_t *item );
bool LoRaMacBackup_IsUpCountFixed( void );
bool LoRaMacBackup_IsBackupCounter( void );
void LoRaMacCountSave( void );
void LoRaMacCountLoad( void );
#define MAC_BACKUP_COUNT_SAVE_FLASH()	LoRaMacCountSave()
#define MAC_BACKUP_COUNT_LOAD_FLASH()	LoRaMacCountLoad()

#else

#define MAC_BACKUP_COUNT_SAVE_FLASH()
#define MAC_BACKUP_COUNT_LOAD_FLASH()

#endif

#define	WAN_BACKUP_ALL			0x00
#define	WAN_BACKUP_JOIN_ONLY	0x01
#define	WAN_BACKUP_RAM_ONLY		0x02
#define	WAN_BACKUP_DISABLE		0x03

#endif
