#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

#include "version.h"

/*
 * デバッグを有効にするには、 このヘッダファイルをincludeして、
 * "_DEBUG_LOG_=true" を定義すること
 */

#if _DEBUG_LOG_ENABLE_

#ifndef IS_EXTERN_ARRAY_VARIABLES
#define IS_EXTERN_ARRAY_VARIABLES
extern uint32_t arrayVariables[];
#endif

#define DEBUG_LOG_SET(idx,val)	arrayVariables[idx] = val
#define DEBUG_LOG_GET(idx)		arrayVariables[idx]
#define DEBUG_LOG_INC(idx)		arrayVariables[idx]++

#else

#define DEBUG_LOG_SET(idx,val)
#define DEBUG_LOG_GET(idx)
#define DEBUG_LOG_INC(idx)

#endif


//#define	_DEBUG_PIN_	34		// PIN_PB02
#ifdef _DEBUG_PIN_
uint8_t bios_GpioOutput( int32_t pin, int32_t stat );
#define	_DEBUG_PIN_SET(val)	bios_GpioOutput( _DEBUG_PIN_, val )
#else
#define	_DEBUG_PIN_SET(val)
#endif

#endif		/* _DEBUG_LOG_H_ */
