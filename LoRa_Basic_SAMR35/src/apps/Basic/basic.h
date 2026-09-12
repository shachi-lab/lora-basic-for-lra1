/*!
 * @file    basic.c
 * @brief   HeaderFile of Basic Interpreter
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
#ifndef	__BASIC_H__
#define	__BASIC_H__

#include "basic_ext.h"
#include "version.h"
#include "basic_conf.h"
#include "bios.h"

extern errCode_t errorCode;
extern uint8_t *executionPointer;
extern uint8_t *resumePointer;
extern uint32_t progLength;
extern int32_t lineNumber;
extern uint8_t	progEditMode;
extern errCode_t lastErrCode;
extern int32_t  lastErrLineNumber;
extern int32_t errorCatchLineNumber;

extern var_t globalVariables[VARIABLE_SIZE];
extern var_t arrayVariables[ARRAY_INDEX_SIZE];
extern uint8_t program_area[PROGRAM_AREA_SIZE];
#define PROGRAM_AREA_TOP	(uint8_t*)program_area
#define PROGRAM_AREA_END	(uint8_t*)&program_area[PROGRAM_AREA_SIZE-1]

#define IS_EXTERN_ARRAY_VARIABLES

#include "ui_common.h"
#include "ui_devid.h"
#include "ui_code.h"
#include "pvt_pro.h"
#include "pvt_fwd.h"
#include "pvt_ui.h"
#include "wan_pro.h"
#include "wan_ui.h"
#include "basic_list.h"
#include "basic_sub.h"
#include "basic_lora.h"
#include "basic_wan.h"
#include "basic_update.h"
#include "debug_log.h"

void basicMain( char *str );
uint8_t basic_checkBreak( void );
void setLastError( void );
uint8_t *get_GotoLabel( int32_t *lnum );
uint32_t get_prog_length( uint8_t *ptr );
void initializeVariables( bool flag );

#define LIST_HISTORY_ENABLE		1

#endif
