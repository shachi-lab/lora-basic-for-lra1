/*!
 * @file    basic_list.c
 * @brief   Identifier List for Basic Interpreter
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
#include "string.h"
#include "ctype.h"
#include "basic.h"

const char 	Wan_pre_str[] = "Wan_";

const BasicProc_t procCodeList[] = {
	proc_print			,	// 0x80 : ST_PRINT
	proc_input			,	// 0x81 : ST_INPUT
	proc_goto			,	// 0x82 : ST_GOTO
	proc_gosub			,	// 0x83 : ST_GOSUB
	proc_return			,	// 0x84 : ST_RETURN
	proc_for			,	// 0x85 : ST_FOR
	proc_next			,	// 0x86 : ST_NEXT
	proc_do				,	// 0x87 : ST_DO
	proc_loop			,	// 0x88 : ST_LOOP
	proc_while			,	// 0x89 : ST_WHILE
	proc_if				,	// 0x8a : ST_IF
	proc_run			,	// 0x8b : ST_RUN
	proc_resume			,	// 0x8c : ST_RESUME
	proc_stop			,	// 0x8d : ST_STOP
	proc_end			,	// 0x8e : ST_END
	proc_new			,	// 0x8f : ST_NEW
	proc_list			,	// 0x90 : ST_LIST
	proc_renum			,	// 0x91 : ST_RENUM
	proc_edit			,	// 0x92 : ST_EDIT
	proc_delete			,	// 0x93 : ST_DELETE
	proc_pload   		,   // 0x94 : ST_PLOAD
	proc_psave			,	// 0x95 : ST_PSAVE
	proc_ver			,	// 0x96 : ST_VER
	proc_delay			,	// 0x97 : ST_DELAY
	proc_pause			,	// 0x98 : ST_PAUSE
	proc_exit			,	// 0x99 : ST_EXIT
	proc_continue		,	// 0x9a : ST_CONTINUE
	proc_randomize		,	// 0x9b : ST_RONDOMIZE
	proc_data			,	// 0x9c : ST_DATA
	proc_read			,	// 0x9d : ST_READ
	proc_restore		,	// 0x9e : ST_RESTORE
	proc_reset			,	// 0x9f : ST_RESET
	proc_sleep			,	// 0xa0 : ST_SLEEP
	proc_deep			,	// 0xa1 : ST_DEEP
	proc_outp			,	// 0xa2 : ST_OUTP
	proc_swap			,	// 0xa3 : ST_SWAP
	proc_else			,	// 0xa4 : ST_ELSE
	proc_elseif			,	// 0xa5 : ST_ELSEIF
	proc_endif			,	// 0xa6 : ST_ENDIF
	proc_lclr			,	// 0xa7 : ST_LCLR
	proc_lprint			,	// 0xa8 : ST_LPRINT
	proc_LoRa_Disp		,	// 0xa9 : ST_LORA_DISP
	proc_nop			,	// 0xaa : ST_NOP
	proc_LoRa_Default	,	// 0xab : ST_LORA_DEFAULT
	proc_LoRa_Sload		,	// 0xac : ST_LORA_SLOAD
	proc_LoRa_Ssave		,	// 0xad : ST_LORA_SSAVE
	proc_LoRa_Send		,	// 0xae : ST_LORA_SEND
	proc_LoRa_Recv		,	// 0xaf : ST_LORA_RECV
	proc_LoRa_RxStop	,	// 0xb0 : ST_LORA_RXSTOP
	proc_LoRa_Comm		,	// 0xb1 : ST_LORA_COMM
	proc_LoRa_Txif		,	// 0xb2 : ST_LORA_TXIF
	proc_LoRa_TxWave	,	// 0xb3 : ST_LORA_TXWAVE
	proc_LoRa_Sens		,	// 0xb4 : ST_LORA_SENS
	proc_LoRa_Bme		,	// 0xb5 : ST_LORA_BME
	proc_LoRa_I2cR		,	// 0xb6 : ST_LORA_I2CR
	proc_LoRa_I2cW		,	// 0xb7 : ST_LORA_I2CW
	proc_LoRa_Aload		,	// 0xb8 : ST_LORA_ALOAD
	proc_LoRa_Asave		,	// 0xb9 : ST_LORA_ASAVE
	proc_LoRa_Utrans	,	// 0xba : ST_LORA_TRANS
	proc_LoRa_Pwm		,	// 0xbb : ST_LORA_PWM
	proc_LoRa_Ugps		,	// 0xbc : ST_LORA_GPS
	proc_LoRa_Datetime	,	// 0xbd : ST_LORA_DATETIME
	proc_ext_cmd		,   // 0xbe : ST_EXT_CMD
	proc_print			,	// 0xbf : ST_UPRINT
	proc_error			,	// 0xc0 : ST_THEN
	proc_error			,	// 0xc1 : ST_TO
	proc_error			,	// 0xc2 : ST_STEP
	proc_LoRa_Distance	,	// 0xc3 : ST_DISTANCE
	proc_catch			,	// 0xc4 : ST_CATCH
	proc_LoRa_Cad		,	// 0xc5 : ST_LORA_CAD
	proc_memdump		,	// 0xc6 : ST_MEMDUMP
};

//
const BasicProc_t procWanCodeList[] =
{
	proc_wan_join		,	// 0xff,0x80 : ST_WAN_JOIN
	proc_wan_unjoin		,	// 0xff,0x81 : ST_WAN_JOIN
	proc_wan_linkchk	,	// 0xff,0x82 : ST_WAN_LINKCHK
	proc_wan_tx			,	// 0xff,0x83 : ST_WAN_TX
	proc_wan_rx			,	// 0xff,0x84 : ST_WAN_RX
	proc_wan_save		,	// 0xff,0x85 : ST_WAN_SAVE
	proc_wan_chlist		,	// 0xff,0x86 : ST_WAN_CHLIST
	proc_wan_rxstop		,	// 0xff,0x87 : ST_WAN_RXSTOP
	proc_wan_chadd		,	// 0xff,0x88 : ST_WAN_CHADD
	proc_wan_chremove	,	// 0xff,0x89 : ST_WAN_CHREMOVE
};

const basic_CmdList_t basic_CmdList_1 =
{
	ST_PRINT	,
	ST_MEMDUMP	,
	LIST_SP_BOTH,
	{
		"Print"		,	// 0x80
		"Input"		,	// 0x81
		"Goto"		,	// 0x82
		"Gosub"		,	// 0x83
		"Return"	,	// 0x84
		"For"		,	// 0x85
		"Next"		,	// 0x86
		"Do"		,	// 0x87
		"Loop"		,	// 0x88
		"While"		,	// 0x89
		"If"		,	// 0x8a
		"Run"		,	// 0x8b
		"Resume"	,	// 0x8c
		"Stop"		,	// 0x8d
		"End"		,	// 0x8e
		"New"		,	// 0x8f
		"List"		,	// 0x90
		"Renum"		,	// 0x91
		"Edit"		,	// 0x92
		"Delete"	,	// 0x93
		"Pload"		,	// 0x94
		"Psave"		,	// 0x95
		"Ver"		,	// 0x96
		"Delay"		,	// 0x97
		"Pause"		,	// 0x98
		"Exit"		,	// 0x99
		"Continue"	,	// 0x9a
		"Randomize"	,	// 0x9b
		"Data"		,	// 0x9c
		"Read"		,	// 0x9d
		"Restore"	,	// 0x9e
		"Reset" 	,	// 0x9f
		"Sleep"  	,	// 0xa0
		"Deep"  	,	// 0xa1
		"Outp"  	,	// 0xa2
		"Swap"  	,	// 0xa3
		"Else"		,	// 0xa4
		"ElseIf"	,	// 0xa5
		"EndIf"		,	// 0xa6
		"Lclr"  	,	// 0xa7
		"Lprint" 	,	// 0xa8
		"#?" 		,	// 0xa9
		"Nop"		,	// 0xaa
		"Default"	,	// 0xab
		"Sload" 	,	// 0xac
		"Ssave" 	,	// 0xad
		"Send"		,	// 0xae
		"Recv"		,	// 0xaf
		"RxStop"	,	// 0xb0
		"Comm"		,	// 0xb1
		"Txif" 		,	// 0xb2
		"TxWave"	,	// 0xb3
		"Sens"		,	// 0xb4
		"Bme"		,	// 0xb5
		"I2cR"		,	// 0xb6
		"I2cW"		,	// 0xb7
		"Aload"		,	// 0xb8
		"Asave"		,	// 0xb9
		"Utrans"	,	// 0xba
		"Pwm"		,	// 0xbb
		"Ugps"		,	// 0xbc
		"Datetime"	,	// 0xbd
		EXT_CMD_STR	, 	// 0xbe
		"Uprint"	,	// 0xbf
		"Then"		,	// 0xc0
		"To"		,	// 0xc1
		"Step"		,	// 0xc2
		"Distance"	,	// 0xc3
		"Catch"		,	// 0xc4
		"Cad"		,	// 0xc5
		"MemDump"	,	// 0xc6
	}
};

const basic_CmdList_t basic_CmdList_2 =
{
	VAR_NULL	,
	FUNC_POW	,
	LIST_SP_NONE,
	{
		"Null"		,	// 0xc7
		"Tick"		,	// 0xc8
		"Clock"		,	// 0xc9
		"Inkey"		,	// 0xca
		"Auto"		,	// 0xcb
		"False"		,	// 0xcc
		"True"		,	// 0xcd
		"Baud"		,	// 0xce
		"Echo"		,	// 0xcf
		"Lpos"		,	// 0xd0
		"Lcont"		,	// 0xd1
		"Modem"		,	// 0xd2
		"Pwr"		,	// 0xd3
		"Sf" 		,	// 0xd4
		"Bw" 		,	// 0xd5
		"Cr" 		,	// 0xd6
		"Ch" 		,	// 0xd7
		"Frq"		,	// 0xd8
		"Gid"		,	// 0xd9
		"Own"		,	// 0xda
		"Dst"		,	// 0xdb
		"Snr"		,	// 0xdc
		"Rssi"		,	// 0xdd
		"DevId"		,	// 0xde
		"Stat"		,	// 0xdf
		"Period"	,	// 0xe0
		"Ctrl"		,	// 0xe1
		"Txd"		,	// 0xe2
		"Rxd"		,	// 0xe3
		"Txdw"		,	// 0xe4
		"Rxdw"		,	// 0xe5
		"Txdl"		,	// 0xe6
		"Rxdl"		,	// 0xe7
		"Pass"		,	// 0xe8
		"BrkCh"		,	// 0xe9
		"Level"		,	// 0xea
		"I2cD"		,	// 0xeb
		"Sn"		,	// 0xec
		"Drift"		,	// 0xed
		"MpuId"		,	// 0xee
		"Xtal32k"	,	// 0xef
		"Rnd"		,	// 0xf0
		"Abs"		,	// 0xf1
		"Chr"		,	// 0xf2
		"WChr"  	,	// 0xf3
		"Form"		,	// 0xf4
		"Inp"		,	// 0xf5
		"Adc"		,	// 0xf6
		"Int8"		,	// 0xf7
		"Int16"		,	// 0xf8
		"IsFree"	,	// 0xf9
		"Ubaud"		,	// 0xfa
		"Uinkey"	,	// 0xfb
		"Atan2"		,	// 0xfc
		"Sqrt"		,	// 0xfd
		"Pow"		,	// 0xfe
//		"Wan_"		,	// 0xff	// Wan
	}
};

const basic_CmdList_t basic_CmdList_3 =
{
	VAR_AESKEY		,
	FUNC_TOINT,
	LIST_SP_NONE	,
	{
		"AesKey"	,	// 0x60
		"Error"		,	// 0x61
		"Gets"		,	// 0x62
		"Ugets"		,	// 0x63
		"TxLen"		,	// 0x64
		"Hop"		,	// 0x65
		"FwdConf"	,	// 0x66
		"Cause"		,	// 0x67
		"SpiBaud"	,	// 0x68
		"SpiTRx"	,	// 0x69
		"Bswap16"	,	// 0x6a
		"Bswap32"	,	// 0x6b
		"Bup1"		,	// 0x6c
		"Bup2"		,	// 0x6d
		"TimeVal"	,	// 0x6e
		"Milisec"	,	// 0x6f
		"LChr"		,	// 0x70
		"MChr"		,	// 0x71	
		"Int24"		,	// 0x72
		"Txdm"		,	// 0x73
		"Rxdm"		,	// 0x74
		"Bcc"		,	// 0x75
		"Sum"		,	// 0x76
		"ProtId"	,	// 0x77
		"SyncWord"	,	// 0x78
		"Debug"		,	// 0x79
		"ToInt"		,	// 0x7a
	}
};

const basic_CmdList_t *basic_CmdList[] =
{
	&basic_CmdList_1,
	&basic_CmdList_2,
	&basic_CmdList_3,
	NULL,
};


const basic_CmdList_t basic_Wan_CmdList_1 =
{
	ST_WAN_JOIN		,
	ST_WAN_CHREMOVE	,
	LIST_SP_POST	,
	{
		"Join"		,	// 0x80
		"UnJoin"	,	// 0x81
		"LinkChk"	,	// 0x82
		"Tx"		,	// 0x83
		"Rx"		,	// 0x84
		"Save"		,	// 0x85
		"ChList"	,	// 0x86
		"RxStop"	,	// 0x87
		"ChAdd"		,	// 0x88
		"ChRemove"	,	// 0x89
						// 0x8a <--- ST_WAN_CODE_END
	}
};

const basic_CmdList_t basic_Wan_CmdList_2 =
{
	VAR_WAN_MODE	,
	VAR_WAN_BATLEVEL,
	LIST_SP_NONE	,
	{
		"Mode"		,	// 0xa0
		"Class"		,	// 0xa1
		"TxPwr"		,	// 0xa2
		"FPort"		,	// 0xa3
		"Cnf"		,	// 0xa4
		"Adr"		,	// 0xa5
		"Dr"		,	// 0xa6
		"Trial"		,	// 0xa7
		"DevEui"	,	// 0xa8
		"JoinEui"	,	// 0xa9
		"AppKey"	,	// 0xaa
		"DevAddr"	,	// 0xab
		"NetID"		,	// 0xac
		"AppSkey"	,	// 0xad
		"NwkSkey"	,	// 0xae
		"JoinStat"	,	// 0xaf
		"UpCnt"		,	// 0xb0
		"DownCnt"	,	// 0xb1
		"Length"	,	// 0xb2
		"JoinMode"	,	// 0xb3
		"ChMask"	,	// 0xb4
		"Pending"	,	// 0xb5
		"RecvDelay1",	// 0xb6
		"RecvDelay2",	// 0xb7
		"JoinDelay1",	// 0xb8
		"JoinDelay2",	// 0xb9
		"TimeReq"	,	// 0xba
		"DutyTime"	,	// 0xbb
		"DwellStat"	,	// 0xbc
		"LastTxCh"	,	// 0xbd
		"MacCmd"	,	// 0xbe
		"BatLevel"  ,	// 0xbf
	}
};

const basic_CmdList_t *basic_Wan_CmdList[] =
{
	&basic_Wan_CmdList_1,
	&basic_Wan_CmdList_2,
	NULL,
};
