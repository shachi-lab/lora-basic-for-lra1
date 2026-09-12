/*!
 * @file    bios_lcd.c
 * @brief   LCD driver for Basic Interpreter
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
#include "sys_delay.h"
#include "board-config.h"
#include "sys_gpio.h"
#include "basic.h"
#include "bios.h"

static uint8_t basic_Lcd_Initialized = false;

#define LCD_I2C_ADDR			0b00111110
#define LCD_CTRL_CMD			0b00000000		// RS=0
#define	LCD_CTRL_DATA			0b01000000		// RS=1
#define	LCD_CONTRUST_DEFAULT	25				// �R���g���X�g (1�`63)
#define	LCD_CONTRUST_MIN		1
#define	LCD_CONTRUST_MAX		63
#define LCD_ENB_CHECK()			if (!bios_Lcd_Enable()) return;

static uint8_t bios_Lcd_cmd( uint8_t cmd );
static bool bios_Lcd_Enable( void );
static uint8_t lcd_contrust = LCD_CONTRUST_DEFAULT;
static uint8_t lcd_position = 0;

//*************************************************
static void bios_Lcd_cls_sub( void )
{
    bios_Lcd_cmd(0x01);				// Clear Display    ��ʏ���
    DelayMs(2);
}

//*************************************************
void bios_Lcd_cls( void )
{
	LCD_ENB_CHECK();
	lcd_position = 0;
	bios_Lcd_cls_sub();
}

//*************************************************
void bios_Lcd_print( void )
{
	LCD_ENB_CHECK();

	int16_t len = get_StringPara( STRING_PARA_FLAG_CRLF_DISABLE );
	if ( len == 0 ) return;
	if ( bios_I2cWriteMemory( LCD_I2C_ADDR, LCD_CTRL_DATA, (uint8_t*)bios_UartTxBuff, len ) )
	{
		errorCode = ERROR_DEVICE;
		return;
	}
	lcd_position += len;
	DelayUs(26);
}

//*************************************************
void bios_Lcd_SetPos( int32_t pos )
{
	LCD_ENB_CHECK();
	lcd_position = pos;
	lcd_position &= 0x7f;
    bios_Lcd_cmd( lcd_position | 0x80 );
}

//*************************************************
uint8_t bios_Lcd_GetPos( void )
{
	lcd_position &= 0x7f;
	return lcd_position;
}

//*************************************************
static bool bios_Lcd_SetContrust_sub( void )
{
	if ( bios_Lcd_cmd(0x70|(lcd_contrust & 15)) ) return true;	// Contrast set �R���g���X�g�����f�[�^(����4�r�b�g)
	if ( bios_Lcd_cmd(0x54|(lcd_contrust >> 4)) ) return true;	// Contrast set ������H�L���A�R���g���X�g�����f�[�^(���2�r�b�g)
	return false;
}

//*************************************************
void bios_Lcd_SetContrust( int32_t contrust )
{
	if ( contrust > LCD_CONTRUST_MAX )	contrust = LCD_CONTRUST_MAX;
	if ( contrust < LCD_CONTRUST_MIN )	contrust = LCD_CONTRUST_MIN;
	lcd_contrust = contrust;
	LCD_ENB_CHECK();
	do {
		if ( bios_Lcd_cmd(0x39) ) break;			// function set �g���R�}���h��L���ɂ���, �W�r�b�g���[�h�Q�s
		if ( bios_Lcd_SetContrust_sub() ) break;	// Contrast set
		if ( bios_Lcd_cmd(0x38) ) break;			// function set �g���R�}���h�𖳌��ɂ���
	} while ( 0 );
}

//*************************************************
uint8_t bios_Lcd_GetContrust( void )
{
	return lcd_contrust;
}

//*************************************************
static uint8_t bios_Lcd_cmd( uint8_t cmd )
{
	uint8_t rslt = bios_I2cWriteMemory( LCD_I2C_ADDR, LCD_CTRL_CMD, &cmd, 1 );
	if ( rslt )	errorCode = ERROR_DEVICE;
	DelayUs(26);
	return errorCode;
}

//*************************************************
static bool bios_Lcd_Enable( void )
{
	if ( bios_I2cIsEnable() == false )
	{
		bios_I2cInit();
		basic_Lcd_Initialized = false;
	}
	if ( basic_Lcd_Initialized == false )
	{
		do {
			DelayMs(40);								// 40ms�҂�
			if ( bios_Lcd_cmd(0x39) ) break;			// function set     �g���R�}���h��L���ɂ���, �W�r�b�g���[�h�Q�s
			if ( bios_Lcd_cmd(0x14) ) break;			// �o�C�A�X�̑I���Ɠ������U���g���̒���
			if ( bios_Lcd_SetContrust_sub() ) break;	// Contrast set
			if ( bios_Lcd_cmd(0x6C) ) break;			// Follower control �t�H���A��H��ON���������̒������s��
			DelayMs(200);								// �d�͂����肷��܂ő҂�
			if ( bios_Lcd_cmd(0x38) ) break;			// function set     �g���R�}���h�𖳌��ɂ���
			if ( bios_Lcd_cmd(0x0c) ) break;			// display control  ��ʕ\����ON�E�J�[�\���\����OFF
			bios_Lcd_cls_sub();							// Clear Display    ��ʏ���
		} while (0);
		lcd_position = 0;
	}
	if ( errorCode == ERROR_OK ) basic_Lcd_Initialized = true;
	if ( !basic_Lcd_Initialized ) errorCode = ERROR_DEVICE;
	return basic_Lcd_Initialized;
}

//*************************************************
void bios_Lcd_Disable( void )
{
	lcd_position = 0;
	basic_Lcd_Initialized = false;
	bios_I2cDeInit();
}
