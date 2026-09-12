/*!
 * @file    basic_list.h
 * @brief   HeaderFile of Identifier List for Basic Interpreter
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
#ifndef __BASIC_LIST_H__
#define __BASIC_LIST_H__

extern const BasicProc_t procCodeList[];
extern const BasicProc_t procWanCodeList[];
extern const basic_CmdList_t *basic_CmdList[];
extern const basic_CmdList_t *basic_Wan_CmdList[];


extern const char Wan_pre_str[];		// "Wan_"

#define	ST_CODE_BEGIN		0x80
enum StCode
{
	ST_EOL			=	0x00,	// End of line
	ST_INDENT		=	0x01,	// Indent
	// Number
	ST_NUM			=	0x10,	// Number
	// Special character
	ST_STRING		=	0x22,	// '"'
	ST_LORA			=	0x23,	// '#'
	ST_HEXCHR		=	0x24,	// '$'
	ST_PERCENT		=	0x25,	// '%'
	ST_ANPERSAND	=	0x26,	// '&'
	ST_COMMENT		=	0x27,	// '''
	ST_COMMA		=	0x2c,	// ','
	ST_PERIOD		=	0x2e,	// '.'
	ST_ZERO			=	0x30,	// '0'
	ST_SEMICOLON	=   0x3b,	// ';'
	ST_QUESTION		=	0x3f,	// '?'
	ST_ARRAY		=	0x40,	// '@'
	ST_VAR_A		=	0x41,	// 'A'
	ST_VAR_Z		=	0x5a,	// 'Z'
	ST_CARET		=	0x5e,	// '^'
	ST_LABEL		=	0x5f,	// '_'
	ST_VBAR			=	0x7c,	// '|'
	ST_TILDE		=	0x7e,	// '~'

	// Command
	ST_PRINT 		= 	0x80,	// 0x80	<-- BIGEN
	ST_INPUT		,			// 0x81
	ST_GOTO			,			// 0x82
	ST_GOSUB		,			// 0x83
	ST_RETURN		,			// 0x84
	ST_FOR			,			// 0x85
	ST_NEXT			,			// 0x86
	ST_DO			,			// 0x87
	ST_LOOP			,			// 0x88
	ST_WHILE		,			// 0x89
	ST_IF			,			// 0x8a
	ST_RUN			,			// 0x8b
	ST_RESUME		,			// 0x8c
	ST_STOP			,			// 0x8d
	ST_END			,			// 0x8e
	ST_NEW			,			// 0x8f
	ST_LIST			,			// 0x90
	ST_RENUM		,			// 0x91
	ST_EDIT			,			// 0x92
	ST_DELETE		,			// 0x93
	ST_PLOAD		,			// 0x94
	ST_PSAVE		,			// 0x95
	ST_VER			,			// 0x96
	ST_DELAY		,			// 0x97
	ST_PAUSE		,			// 0x98
	ST_EXIT			,			// 0x99
	ST_CONTINUE		,			// 0x9a
	ST_RONDOMIZE	,			// 0x9b
	ST_DATA			,			// 0x9c
	ST_READ			,			// 0x9d
	ST_RESTORE		,			// 0x9e
	ST_RESET		,			// 0x9f
	ST_SLEEP		,			// 0xa0
	ST_DEEP			,			// 0xa1
	ST_OUTP			,			// 0xa2
	ST_SWAP			,			// 0xa3
	ST_ELSE			,			// 0xa4
	ST_ELSEIF		,			// 0xa5
	ST_ENDIF		,			// 0xa6
	ST_LCLR			,			// 0xa7
	ST_LPRINT		,			// 0xa8
	ST_LORA_DISP	,			// 0xa9
	ST_NOP			,			// 0xaa
	ST_LORA_DEFAULT ,			// 0xab
	ST_LORA_SLOAD	,			// 0xac
	ST_LORA_SSAVE	,			// 0xad
	ST_LORA_SEND	,			// 0xae
	ST_LORA_RECV	,			// 0xaf
	ST_LORA_RXSTOP	,			// 0xb0
	ST_LORA_COMM	,			// 0xb1
	ST_LORA_TXIF	,			// 0xb2
	ST_LORA_TXWAVE	,			// 0xb3
	ST_LORA_SENS    ,			// 0xb4
	ST_LORA_BME		,			// 0xb5
	ST_LORA_I2CR	,			// 0xb6
	ST_LORA_I2CW	,			// 0xb7
	ST_LORA_ALOAD	,			// 0xb8
	ST_LORA_ASAVE	,			// 0xb9
	ST_LORA_UTRANS	,			// 0xba
	ST_LORA_PWM		,			// 0xbb
	ST_LORA_UGPS	,			// 0xbc
	ST_LORA_DATETIME,			// 0xbd
	ST_EXT_CMD		,			// 0xbe
	ST_UPRINT		,			// 0xbf
	ST_THEN			,			// 0xc0
	ST_TO			,			// 0xc1
	ST_STEP			,			// 0xc2
	ST_DISTANCE		,			// 0xc3
	ST_CATCH		,			// 0xc4
	ST_LORA_CAD		,			// 0xc5
	ST_MEMDUMP		,			// 0xc6
	ST_CODE_END		,			//      <-- END

	VAR_NULL		= 0xc7,		// 0xc7
	VAR_TICK		,			// 0xc8
	VAR_CLOCK		,			// 0xc9
	VAR_INKEY		,			// 0xca
	VAR_AUTO		,			// 0xcb
	VAR_FALSE		,			// 0xcc
	VAR_TRUE		,			// 0xcd
	VAR_BAUD		,			// 0xce
	VAR_ECHO		,			// 0xcf
	VAR_LPOS		,			// 0xd0
	VAR_LCONT		,			// 0xd1
	VAR_LORA_MODEM	,			// 0xd2
	VAR_LORA_PWR	,			// 0xd3
	VAR_LORA_SF		,			// 0xd4
	VAR_LORA_BW		,			// 0xd5
	VAR_LORA_CR		,			// 0xd6
	VAR_LORA_CH		,			// 0xd7
	VAR_LORA_FRQ	,			// 0xd8
	VAR_LORA_GID	,			// 0xd9
	VAR_LORA_OWN	,			// 0xda
	VAR_LORA_DST	,			// 0xdb
	VAR_LORA_SNR	,			// 0xdc
	VAR_LORA_RSSI   ,			// 0xdd
	VAR_LORA_DEVID	,			// 0xde
	VAR_LORA_STAT	,			// 0xdf
	VAR_LORA_PERIOD	,			// 0xe0
	VAR_LORA_CTRL	,			// 0xe1
	VAR_LORA_TXD	,			// 0xe2
	VAR_LORA_RXD	,			// 0xe3
	VAR_LORA_TXDW	,			// 0xe4
	VAR_LORA_RXDW	,			// 0xe5
	VAR_LORA_TXDL	,			// 0xe6
	VAR_LORA_RXDL	,			// 0xe7
	VAR_LORA_PASS	,			// 0xe8
	VAR_LORA_BRKCH	,			// 0xe9
	VAR_LORA_LEVEL	,			// 0xea
	VAR_LORA_I2CD	,			// 0xeb
	VAR_LORA_SN		,			// 0xec
	VAR_LORA_DRIFT	,			// 0xed
	VAR_LORA_MPUID	,			// 0xee
	VAR_LORA_XTAL32K,			// 0xef
	FUNC_RND		,			// 0xf0
	FUNC_ABS		,			// 0xf1
	FUNC_CHR		,			// 0xf2
	FUNC_WCHR		,			// 0xf3
	FUNC_FORM		,			// 0xf4
	FUNC_INP		,			// 0xf5
	FUNC_ADC		,			// 0xf6
	FUNC_INT8		,			// 0xf7
	FUNC_INT16		,			// 0xf8
	FUNC_LORA_ISFREE,			// 0xf9
	VAR_UBAUD		,			// 0xfa
	VAR_UINKEY		,			// 0xfb
	FUNC_ATAN2		,			// 0xfc
	FUNC_SQRT		,			// 0xfd
	FUNC_POW		,			// 0xfe
	ST_CODE_WAN = 0xff ,		// 0xff

	VAR_AESKEY  = 0x60 ,		// 0x60
	VAR_ERROR		,			// 0x61
	VAR_GETS		,			// 0x62
	VAR_UGETS		,			// 0x63
	VAR_LORA_TXLEN	,			// 0x64
	VAR_HOP			,			// 0x65
	VAR_FWDCONF		,			// 0x66
	VAR_CAUSE		,			// 0x67
	VAR_SPIBAUD		,			// 0x68
	FUNC_SPITRX		,			// 0x69
	FUNC_BSWAP16	,			// 0x6a
	FUNC_BSWAP32	,			// 0x6b
	VAR_BUP1		,			// 0x6c
	VAR_BUP2		,			// 0x6d
	FUNC_TIMEVAL	,			// 0x6e
	VAR_MILISEC		,			// 0x6f
	FUNC_LCHR		,			// 0x70
	FUNC_MCHR		,			// 0x71
	FUNC_INT24		,			// 0x72
	VAR_LORA_TXDM	,			// 0x73
	VAR_LORA_RXDM	,			// 0x74
	FUNC_BCC		,			// 0x75
	FUNC_SUM		,			// 0x76
	VAR_LORA_PROTID ,			// 0x77
	VAR_LORA_SYNCWORD,			// 0x78
	VAR_LORA_DEBUG	,			// 0x79
	FUNC_TOINT		,			// 0x7a
};

#define ST_WAN_CODE_BEGIN	0x80
// Statments
enum StWanCode
{
	ST_WAN_JOIN		= 0x80,		// 0x80 <-- BEGIN
	ST_WAN_UNJOIN	,			// 0x81
	ST_WAN_LINKCHK	,			// 0x82
	ST_WAN_TX		,			// 0x83
	ST_WAN_RX		,			// 0x84
	ST_WAN_SAVE		,			// 0x85
	ST_WAN_CHLIST	,			// 0x86
	ST_WAN_RXSTOP	,			// 0x87
	ST_WAN_CHADD	,			// 0x88
	ST_WAN_CHREMOVE	,			// 0x89
	ST_WAN_CODE_END	,			//      <-- END

	VAR_WAN_MODE	= 0xa0,		// 0xa0
	VAR_WAN_CLASS	,			// 0xa1
	VAR_WAN_TXPWR	,			// 0xa2
	VAR_WAN_FPORT	,			// 0xa3
	VAR_WAN_CNF		,			// 0xa4
	VAR_WAN_ADR		,			// 0xa5
	VAR_WAN_DR		,			// 0xa6
	VAR_WAN_TRIAL	,			// 0xa7
	VAR_WAN_DEVEUI	,			// 0xa8
	VAR_WAN_JOINEUI	,			// 0xa9
	VAR_WAN_APPKEY	,			// 0xaa
	VAR_WAN_DEVADDR	,			// 0xab
	VAR_WAN_NETID	,			// 0xac
	VAR_WAN_APPSKEY	,			// 0xad
	VAR_WAN_NWKSKEY	,			// 0xae
	VAR_WAN_JOINSTAT,			// 0xaf
	VAR_WAN_UPCNT	,			// 0xb0
	VAR_WAN_DOWNCNT	,			// 0xb1
	VAR_WAN_LENGTH	,			// 0xb2
	VAR_WAN_JOINMODE,			// 0xb3
	VAR_WAN_CHMASK	,			// 0xb4
	VAR_WAN_PENDING ,			// 0xb5
	VAR_WAN_RECVDELAY1,			// 0xb6
	VAR_WAN_RECVDELAY2,			// 0xb7
	VAR_WAN_JOINDELAY1,			// 0xb8
	VAR_WAN_JOINDELAY2,			// 0xb9
	VAR_WAN_TIMEREQ	,			// 0xba
	VAR_WAN_DUTYTIME,			// 0xbb
	VAR_WAN_DWELLSTAT,			// 0xbc
	VAR_WAN_LASTTXCH,			// 0xbd
	VAR_WAN_MACCMD	,			// 0xbe
	VAR_WAN_BATLEVEL,			// 0xbf
};

//
//
// Basic-cmd
PROC( proc_print			);
PROC( proc_input 			);
PROC( proc_goto 			);
PROC( proc_gosub 			);
PROC( proc_return 			);
PROC( proc_for 				);
PROC( proc_next 			);
PROC( proc_do 				);
PROC( proc_loop 			);
PROC( proc_while 			);
PROC( proc_if 				);
PROC( proc_else 			);
PROC( proc_elseif 			);
PROC( proc_endif 			);
PROC( proc_run 				);
PROC( proc_resume 			);
PROC( proc_stop 			);
PROC( proc_end 				);
PROC( proc_new 				);
PROC( proc_list 			);
PROC( proc_renum 			);
PROC( proc_edit 			);
PROC( proc_delete 			);
PROC( proc_ver 				);
PROC( proc_delay 			);
PROC( proc_pause 			);
PROC( proc_exit 			);
PROC( proc_continue 		);
PROC( proc_randomize	 	);
PROC( proc_data 			);
PROC( proc_read 			);
PROC( proc_restore 			);
PROC( proc_reset 			);
PROC( proc_sleep 			);
PROC( proc_deep 			);
PROC( proc_pload 			);
PROC( proc_psave 			);
PROC( proc_outp 			);
PROC( proc_swap 			);
PROC( proc_lclr 			);
PROC( proc_lprint 			);
PROC( proc_uinkey 			);
PROC( proc_nop				);
PROC( proc_error			);
PROC( proc_catch			);
PROC( proc_memdump			);
PROC( proc_ext_cmd			);

// LoRa-cmd
PROC( proc_LoRa_Disp		);
PROC( proc_LoRa_Default		);
PROC( proc_LoRa_Sload		);
PROC( proc_LoRa_Ssave		);
PROC( proc_LoRa_Send		);
PROC( proc_LoRa_Recv		);
PROC( proc_LoRa_RxStop		);
PROC( proc_LoRa_Comm		);
PROC( proc_LoRa_Txif		);
PROC( proc_LoRa_TxWave		);
PROC( proc_LoRa_Sens		);
PROC( proc_LoRa_Bme			);
PROC( proc_LoRa_I2cR		);
PROC( proc_LoRa_I2cW		);
PROC( proc_LoRa_Aload		);
PROC( proc_LoRa_Asave		);
PROC( proc_LoRa_Utrans		);
PROC( proc_LoRa_Pwm			);
PROC( proc_LoRa_Ugps		);
PROC( proc_LoRa_Datetime	);
PROC( APL_EXT_FUNC			);
PROC( proc_LoRa_Distance	);
PROC( proc_LoRa_Cad			);

// Wan-cmd
PROC( proc_wan_join			);
PROC( proc_wan_unjoin		);
PROC( proc_wan_linkchk		);
PROC( proc_wan_tx			);
PROC( proc_wan_rx			);
PROC( proc_wan_save			);
PROC( proc_wan_chlist		);
PROC( proc_wan_rxstop		);
#if __WAN_CH_ADD_ENABLE__
PROC( proc_wan_chadd		);
PROC( proc_wan_chremove		);
#endif
#endif
