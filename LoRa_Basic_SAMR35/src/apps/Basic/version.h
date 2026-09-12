/*!
 * @file    version.h
 * @brief   HeaderFile of Version for System
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

#ifndef __VERSION_H__
#define __VERSION_H__

#define	_PRODUCT_VER_MAJOR_VAL_		1
#define	_PRODUCT_VER_MINOR_VAL_		28
#define	_PRODUCT_VER_PATCH_VAL_		a

#include "stdbool.h"

#if defined(_DEBUG_LOG_) && (_DEBUG_LOG_ == true)
#define	_DEBUG_LOG_ENABLE_			1
#define _PRODUCT_VER_DEBUG_			"!"
#else
#define	_DEBUG_LOG_ENABLE_			0
#define _PRODUCT_VER_DEBUG_
#endif

#define TOSTRING(x)					#x
#define EXPAND(x)					TOSTRING(x)
#define	_PRODUCT_VER_MAJOR_			EXPAND( _PRODUCT_VER_MAJOR_VAL_ )
#define	_PRODUCT_VER_MINOR_			EXPAND( _PRODUCT_VER_MINOR_VAL_ )
#define	_PRODUCT_VER_PATCH_			EXPAND( _PRODUCT_VER_PATCH_VAL_ )

#ifdef	__UI_LORA_WAN__
#define _PRODUCT_WAN_				"+"
#else
#define _PRODUCT_WAN_				"-"
#endif


#if defined(_EXT_ENABLE_) && _EXT_ENABLE_
#define _PRODUCT_EXT_				"x"
#else
#define _PRODUCT_EXT_
#endif

#define _PRODUCT_VER_NUMBER_		_PRODUCT_VER_MAJOR_ "." _PRODUCT_VER_MINOR_ "." _PRODUCT_VER_PATCH_

#define _PRODUCT_NAME_				"LoRa-Basic / Shachi-lab"
#define _PRODUCT_VERSION_           "Ver " _PRODUCT_VER_NUMBER_ _PRODUCT_WAN_ _PRODUCT_EXT_ _PRODUCT_VER_DEBUG_
#define _PRODUCT_RIGHT_				"Copyright (C) 2026 Shachi-lab. All Rights Reserved."
#define _PRODUCT_DATETIME_          __DATE__ "-" __TIME__

typedef struct
{
	char	Name[24];
	char	Version[16];
	char	Right[];
} Product_types;

extern Product_types _Product_;

#endif
