/*!
 * \file      LoRaMac_backup.c
 *
 * \brief     Backup module for LoRa-Mac
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
#include <string.h>
#include <stdio.h>
#include "utilities.h"
#include "basic.h"
#include "bios.h"

/*
 * LoRaMAC Backup
 */
#include "cmsis_gcc.h"		// for use __NOP();

extern uint32_t _smac_bss, _emac_bss;
extern uint32_t _smac_backup, _emac_backup;
extern uint16_t _mac_bss_version;
extern uint32_t _mac_bss_crc;
extern const uint32_t _mac_bss_size;

bool LoRaMacBackup_RequestFlag = false;

/*
 * Backup2Flash : [Bss] .. [Backup] -> [Flash] / -
 * Flash2Backup : [Bss] .. [Backup] <- [Flash] / -
 * Bss2Backup   : [Bss] -> [Backup] .. [Flash] / MAC_BACKUP_JOIN_SAVE_RAM
 * Backup2Bss   : [Bss] <- [Backup] .. [Flash] / -
 * SaveFlash    : [Bss] -> [Backup] -> [Flash] / MAC_BACKUP_JOIN_SAVE_FLASH
 * Recover      : [Bss] <- [Backup] <- [Flash] / MAC_BACKUP_JOIN_RECOVER
 */

#if __WAN_BACKUP_ENABLE__

static bool LoRaMacBackup_BssCheck( void );
static void LoRaMacBackup_Bss2Backup( void );
static void LoRaMacBackup_Backup2Bss( void );
static void LoRaMacBackup_Backup2Flash( void );
static void LoRaMacBackup_Flash2Backup( void );

//
static bool LoRaMacBackup_BssCheck( void )
{
	if( _mac_bss_version != MAC_BACKUP_BSS_VERSION ) return false;
	uint32_t crc = CalculateCRC32( &_smac_bss, (_mac_bss_size/4)-1, CRCBEATSIZE_WORD );
	return ( crc == _mac_bss_crc );
}


static void LoRaMacBackup_Bss2Backup_Copy( void )
{
	memcpy32( &_smac_backup, &_smac_bss, _mac_bss_size/4 );
}

//
static void LoRaMacBackup_Bss2Backup( void )
{
	_mac_bss_version = MAC_BACKUP_BSS_VERSION;
	_mac_bss_crc = CalculateCRC32( &_smac_bss, (_mac_bss_size/4)-1, CRCBEATSIZE_WORD );
	LoRaMacBackup_Bss2Backup_Copy();
}

//
static void LoRaMacBackup_Backup2Bss( void )
{
	memcpy32( &_smac_bss, &_smac_backup, _mac_bss_size/4 );
}

//
static void LoRaMacBackup_Backup2Flash( void )
{
	if( _mac_bss_size > FLASH_LORAMAC_SIZE ) return;
	bios_FlashWrite( FLASH_LORAMAC_PARM_ADRS, (uint8_t*)&_smac_backup, _mac_bss_size );
}

//
static void LoRaMacBackup_Flash2Backup( void )
{
	memcpy32( (uint32_t*)&_smac_backup, (uint32_t*)FLASH_LORAMAC_PARM_ADRS, _mac_bss_size/4 );
}

//
void LoRaMacBackup_Initialize( void )
{
	_mac_bss_version = -1;
	_mac_bss_crc = -1;
	LoRaMacBackup_Bss2Backup_Copy();
	LoRaMacBackup_Backup2Flash();
}


//
void LoRaMacBackup_SaveRAM( void )
{
	if( GET_CTRL_WAN_BACKUP() > WAN_BACKUP_RAM_ONLY ) return;
	LoRaMacBackup_Bss2Backup();
}

//
void LoRaMacBackup_SaveFLASH( void )
{
	LoRaMacBackup_RequestFlag = false;
	if( GET_CTRL_WAN_BACKUP() > WAN_BACKUP_RAM_ONLY ) return;
	LoRaMacBackup_Bss2Backup();
	if( GET_CTRL_WAN_BACKUP() > WAN_BACKUP_JOIN_ONLY ) return;
	LoRaMacBackup_Backup2Flash();
}

//
// return : true=Flash����Join��Ԃ�Recover���ꂽ, false=����ȊO
//
bool LoRaMacBackup_Recover( bool *isJoin )
{
	bool res = false;
	do{
		if( GET_CTRL_WAN_BACKUP() < WAN_BACKUP_DISABLE )
		{
			// �d��ON,BOR,DEEP ��RAM�̓��e���s��
			if( !(RSTC->RCAUSE.reg & (RSTC_RCAUSE_BOD33 | RSTC_RCAUSE_POR | RSTC_RCAUSE_BACKUP)) ) {
				LoRaMacBackup_Backup2Bss();
				if( LoRaMacBackup_BssCheck() ) break;
			}
		}
		if( GET_CTRL_WAN_BACKUP() < WAN_BACKUP_RAM_ONLY )
		{
			res = true;
			LoRaMacBackup_Flash2Backup();
			LoRaMacBackup_Backup2Bss();
			if( LoRaMacBackup_BssCheck() ) break;
		}
		*isJoin = false;
	}while(0);
	if( *isJoin )	return res;
	memset32( &_smac_bss, 0, _mac_bss_size/4 );
	return false;
}
#else

//
bool LoRaMacBackup_Recover( bool *isJoin )
{
	memset32( &_smac_bss, 0, _mac_bss_size/4 );
	*isJoin = false;
	return false;
}
#endif

#if __WAN_BACKUP_ENABLE__

#define	MAC_BACKUP_USED			0x77
#define	MAC_BACKUP_LATEST		0x7f
#define	MAC_BACKUP_BLANK		0xff
#define	MAC_BACKUP_SIG			0x12345678
#define	MAC_BACKUP_UNIT_NUM		16
#define MAC_BACKUP_UNIT_SIZE	sizeof(MacBackupCounters_t)
#define MAC_BACKUP_BUFF_SIZE	(MAC_BACKUP_UNIT_SIZE * 4)

//
MacBackupCounters_t* LoRaMacBackup_CounterLoad( void )
{
	MacBackupCounters_t *page = (MacBackupCounters_t*)FLASH_LORAMAC_COUNT_ADRS;

    if( page->value[0] != MAC_BACKUP_SIG )	return NULL;
	for( uint8_t i = 1; i < MAC_BACKUP_UNIT_NUM; i++ )
	{
		page++;
		if( page->mark == MAC_BACKUP_LATEST )	return page;
	}
	return NULL;
}

//
void LoRaMacBackup_CounterSave( MacBackupCounters_t *items )
{
	uint32_t page_address;
	uint8_t num = 0x0f;
	MacBackupCounters_t page_buff[4];
	MacBackupCounters_t *ptr = LoRaMacBackup_CounterLoad();

	if( ptr != NULL )
	{
		items->mark = MAC_BACKUP_LATEST;
		if( !memcmp( (uint8_t*)ptr, (uint8_t*)items, MAC_BACKUP_UNIT_SIZE) ) return;	// ������e�Ȃ炻�̂܂� ?

		page_address = (uint32_t)ptr;
		num = (page_address >> 4 ) & 0x0f;
		if( num < 0x0f )
		{
			page_address &= ~0x3f;
			memcpy( (uint8_t*)page_buff, (uint8_t*)page_address, MAC_BACKUP_BUFF_SIZE );
			num &= 0x03;
			page_buff[num++].mark = MAC_BACKUP_USED;
		}
	}
	
	if( num >= 0x0f )
	{
		page_address = FLASH_LORAMAC_COUNT_ADRS;
		while( nvm_erase_row( page_address ) == STATUS_BUSY );
		memset( (uint8_t*)page_buff, 0xff, MAC_BACKUP_BUFF_SIZE );
		page_buff[0].value[0] = MAC_BACKUP_SIG;
		items->mark = MAC_BACKUP_LATEST;
		num = 1;
	}
	
	if( num > 3 )
	{
		/* �� Page �������� */
		while( nvm_write_buffer( page_address, (uint8_t*)page_buff, NVMCTRL_PAGE_SIZE ) == STATUS_BUSY );
		memset( (uint8_t*)page_buff, 0xff, MAC_BACKUP_BUFF_SIZE );
		page_address += 0x40;
		num = 0;
	}

	memcpy( (uint8_t*)&page_buff[num], (uint8_t*)items, MAC_BACKUP_UNIT_SIZE );
	/* �V Page �������� */
	while( nvm_write_buffer( page_address, (uint8_t*)page_buff, NVMCTRL_PAGE_SIZE ) == STATUS_BUSY );
}

//
bool LoRaMacBackup_IsUpCountFixed( void )
{
	return IS_CTRL_WAN_UPCOUNT_FIX;
}

//
bool LoRaMacBackup_IsBackupCounter( void )
{
	return ( GET_CTRL_WAN_BACKUP() == WAN_BACKUP_ALL );
}

#endif
