/*!
 * @file    bacic_lora.h
 * @brief   HeaderFile of LoRa extension for Basic Interpreter
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
#ifndef	__BASIC_LORA_H__
#define	__BASIC_LORA_H__

#define LORA_TXPWR_OFFSET	1

#define LORA_FORCE_INIT_NUMBER		PassLevel_Protect

void basic_LoRa_Init( void );
uint8_t basic_LoRa_SetVar( uint8_t ch );
bool basic_LoRa_GetVar( uint8_t ch, var_t *val );
char *basic_LoRa_Print( uint8_t ch, char *buff );
int16_t basic_LoRa_SetTxd( void );
bool basic_LoRa_IsEncode( uint8_t ch );
bool basic_LoRa_GetCodecPara( uint8_t *code );
errCode_t basic_LoRa_GetRecvPara( uint8_t *code, var_t *val );
errCode_t basic_LoRa_GetCommPara( uint8_t *code );
void basic_LoRa_Process( void );
int32_t basic_LoRa_TimeVal( int32_t val );
errCode_t basic_LoRa_Pvt_Check( void );

extern DataFrame_t basic_LoRa_TxFrame;
extern DataFrame_t basic_LoRa_RxFrame;
extern uint8_t	basic_LoRa_stat;

#endif
