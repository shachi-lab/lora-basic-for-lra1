/*!
 * \file      sys_fifo.h
 *
 * \brief     FIFO buffer implementation
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
#include "sys_fifo.h"

static uint16_t FifoNext( Fifo_t *fifo, uint16_t index )
{
    return ( ++index < fifo->Size ) ? index : 0;
//  return ( index + 1 ) % fifo->Size;
}

void FifoInit( Fifo_t *fifo, uint8_t *buffer, uint16_t size )
{
    fifo->Begin = 0;
    fifo->End = 0;
    fifo->Data = buffer;
    fifo->Size = size;
}

void FifoPush( Fifo_t *fifo, uint8_t data )
{
    fifo->End = FifoNext( fifo, fifo->End );
    fifo->Data[fifo->End] = data;
}

uint8_t FifoPop( Fifo_t *fifo )
{
    uint8_t data = fifo->Data[FifoNext( fifo, fifo->Begin )];

    fifo->Begin = FifoNext( fifo, fifo->Begin );
    return data;
}

void FifoFlush( Fifo_t *fifo )
{
    fifo->Begin = 0;
    fifo->End = 0;
}

bool IsFifoEmpty( Fifo_t *fifo )
{
    return ( fifo->Begin == fifo->End );
}

bool IsFifoFull( Fifo_t *fifo )
{
    return ( FifoNext( fifo, fifo->End ) == fifo->Begin );
}

uint16_t FiFoLen( Fifo_t *fifo )
{
	uint16_t len = fifo->End - fifo->Begin;
	if ( len > fifo->Size ) len += fifo->Size;
	return len;
}
