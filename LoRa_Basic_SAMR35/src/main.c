/*!
 * @file    main.c
 * @brief   main program of i2 LoRa module
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
#include <asf.h>
#include "board-config.h"
#include "board-mcu.h"
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_timer.h"
#include "sys_uart.h"
#include "basic.h"
#include "bios.h"
#include "hw_timer.h"


uint32_t serial[4];

/*
 *
 */
int main (void)
{
	system_init();
	BoardMclkMaskClear();

	Ui_Settings_Init( false );
	Ui_Adjust_Init();
#ifdef	__UI_LORA_WAN__
	WanUi_Conf_Init( false );
#endif
	BoardInitMcu();
	Enable_global_interrupt();
	Ui_Led_Init();
	LED_TRX_ON();
	bios_Initialize();
	if( !IS_CTRL_TITLE_DISABLE )
	{
		bios_PutVersion();
		bios_CheckXtal32k();
	}
	DelayMs( 500 );
	LED_TRX_OFF();
	basic_LoRa_Init();
	basicMain( NULL );

	for(;;)
	{
		BoardLowPowerHandler( );
	}
}
