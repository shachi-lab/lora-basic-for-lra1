/*!
 * \file      uart-board.c
 *
 * \brief     Target board UART driver implementation
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
#include "board-config.h"
#include "utilities.h"
#include "sys_gpio.h"
#include "sys_delay.h"
#include "sys_fifo.h"
#include "sys_spi.h"
#include "sys_timer.h"
#include "sys_uart.h"
#include "uart-board.h"

#define UART_CALLBACK_ENABLE		1
#define UART_INSTANCE_MAX			2
#define UART_RX_BUFF_SIZE			128
#define UART_TX_BUFF_SIZE			128

Uart_t Uart_obj[UART_INSTANCE_MAX];

const sercom_settings_t usart_board_settings[UART_INSTANCE_MAX] =
{
	{ SERCOM0, USART_RX_1_TX_0_XCK_1, {PINMUX_PA04D_SERCOM0_PAD0, PINMUX_PA05D_SERCOM0_PAD1, PINMUX_UNUSED, PINMUX_UNUSED}, {UART1_TX}, {UART1_RX} },
	{ SERCOM3, USART_RX_1_TX_0_XCK_1, {PINMUX_PA22C_SERCOM3_PAD0, PINMUX_PA23C_SERCOM3_PAD1, PINMUX_UNUSED, PINMUX_UNUSED}, {UART2_TX}, {UART2_RX} },
};
static struct usart_module usart_instance[UART_INSTANCE_MAX];
static uint8_t UartRxBuff[UART_INSTANCE_MAX][UART_RX_BUFF_SIZE];
static uint8_t UartTxBuff[UART_INSTANCE_MAX][UART_TX_BUFF_SIZE];

#if UART_CALLBACK_ENABLE
static Uart_t *uart_module_to_obj(struct usart_module *const usart_module);
static void usart_read_callback(struct usart_module *const usart_module);
static void usart_write_callback(struct usart_module *const usart_module);
static void usart_break_callback(struct usart_module *const usart_module);
#endif

/*!
 * \brief Initializes the UART object and MCU peripheral
 *
 * \param [IN] obj  UART object
 * \param [IN] uartId UART ID
 */
void UartMcuInit( Uart_t *obj, UartId_t uartId )
{
	if( obj->IsInitialized != false )	return;
	obj->IsInitialized = true;
    obj->UartId = uartId;
	obj->Instance = &usart_instance[obj->UartId];
	GpioMcuSetObj( &(obj->Rx), usart_board_settings[uartId].rx_pin );
	GpioMcuSetObj( &(obj->Tx), usart_board_settings[uartId].tx_pin );
	UartMcuFlush( obj );
	if( obj->Baudrate == 0 ) obj->Baudrate = UART_BAUDRATE_DEFAULT;
	UartMcuConfig( obj, obj->Baudrate );
}

/*!
 * \brief Configures the UART object and MCU peripheral
 *
 * \remark UartInit function must be called first.
 *
 * \param [IN] obj          UART object
 * \param [IN] baudrate     UART baudrate
 * \                        stop bits     (baudrate bit-2)
 * \                        packet parity (baudrate bit-0,1)
 */
void UartMcuConfig( Uart_t *obj, uint32_t baudrate )
{
	struct usart_config config_usart;
	enum status_code res;

	obj->Baudrate = baudrate;
	if( obj->IsInitialized == false )	return;

	sercom_settings_t const *uart_set_p = &usart_board_settings[obj->UartId];

	usart_get_config_defaults( &config_usart );
	obj->Options = baudrate % 100;
	config_usart.baudrate = baudrate - obj->Options;
	config_usart.mux_setting = uart_set_p->muxsettimg;
	config_usart.pinmux_pad0 = uart_set_p->pinmux_pad[0];
	config_usart.pinmux_pad1 = uart_set_p->pinmux_pad[1];
	config_usart.pinmux_pad2 = uart_set_p->pinmux_pad[2];
	config_usart.pinmux_pad3 = uart_set_p->pinmux_pad[3];
	config_usart.lin_slave_enable = true;

	uint32_t parity = obj->Options & UART_BAUD_OPT_PARITY_BITS;
	if( parity == UART_BAUD_OPT_PARITY_ODD )	config_usart.parity = USART_PARITY_ODD;
	if( parity == UART_BAUD_OPT_PARITY_EVEN )	config_usart.parity = USART_PARITY_EVEN;
	if( obj->Options & UART_BAUD_OPT_STOP_2 )	config_usart.stopbits = USART_STOPBITS_2;
	if( obj->Options & UART_BAUD_OPT_SIZE_7 )	config_usart.character_size = USART_CHARACTER_SIZE_7BIT;

	if( obj->Instance->hw != NULL )
	{
		usart_disable(obj->Instance);
		DelayMs(1);
	}
	while ( 1 )
	{
		res = usart_init( obj->Instance, uart_set_p->hw, &config_usart );
		if( res == STATUS_OK )	break;
		if( res == STATUS_ERR_BAUDRATE_UNAVAILABLE )
		{
			config_usart.baudrate = UART_BAUDRATE_DEFAULT;
		}
	}
	obj->IsGpioTxInit = true;
	obj->IsGpioRxInit = true;
	if( obj->Options & UART_BAUD_OPT_RX_PULL_DOWN ) GpioMcuSetPull( &obj->Rx , PIN_PULL_DOWN );

#if UART_CALLBACK_ENABLE
	usart_register_callback( obj->Instance, usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED );
	usart_register_callback( obj->Instance, usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED );
	usart_register_callback( obj->Instance, usart_break_callback, USART_CALLBACK_ERROR );
	usart_enable_callback( obj->Instance, USART_CALLBACK_BUFFER_TRANSMITTED );
	usart_enable_callback( obj->Instance, USART_CALLBACK_BUFFER_RECEIVED );
	usart_enable_callback( obj->Instance, USART_CALLBACK_ERROR );
	_usart_read_buffer( obj->Instance, &obj->RxData, 1 );
#endif
	usart_enable( obj->Instance );
}

/*
 *
 */
static void UartMcuDeinitSetPull( Gpio_t *gpio_obj, uint8_t flag, PinTypes type )
{
	if( !GpioIsMuxEn( gpio_obj ) ) return; 
	if ( flag )
	{
		GpioInit( gpio_obj, gpio_obj->pin, PIN_INPUT, type , 0 );
	} else
	{
		GpioInit( gpio_obj, gpio_obj->pin, PIN_ANALOGIC, PIN_NO_PULL , 0 );
	}
}

/*!
 * \brief DeInitializes the UART object and MCU pins
 *
 * \param [IN] obj  UART object
 */
void UartMcuDeInit( Uart_t *obj )
{
	if( obj->IsInitialized == false )	return;

//	struct usart_module *usartp = &usart_instance[obj->UartId];
    obj->IsInitialized = false;
	obj->IsSendData = false;
	usart_disable(obj->Instance);
	BoardSercomDisableMclk( obj->Instance->hw );

#if UART_CALLBACK_ENABLE
	usart_register_callback( obj->Instance, NULL, USART_CALLBACK_BUFFER_TRANSMITTED );
	usart_register_callback( obj->Instance, NULL, USART_CALLBACK_BUFFER_RECEIVED );
	usart_register_callback( obj->Instance, NULL, USART_CALLBACK_ERROR );
	usart_disable_callback( obj->Instance, USART_CALLBACK_BUFFER_TRANSMITTED );
	usart_disable_callback( obj->Instance, USART_CALLBACK_BUFFER_RECEIVED );
	usart_disable_callback( obj->Instance, USART_CALLBACK_ERROR );
#endif
	UartMcuDeinitSetPull( &obj->Rx, obj->Options & UART_BAUD_OPT_RX_PULL_DOWN, PIN_PULL_DOWN );
	UartMcuDeinitSetPull( &obj->Tx, obj->Options & UART_BAUD_OPT_TX_PULL_UP  , PIN_PULL_UP );
}

/*!
 * \brief Sends a character to the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Character to be sent
 * \retval status    [0: OK, 1: Busy]
 */
uint8_t UartMcuPutChar( Uart_t *obj, uint8_t data )
{
	if( obj->IsInitialized == false ) return 0;
#if UART_CALLBACK_ENABLE
	uint8_t res = 0;
//	struct usart_module *usartp = &usart_instance[obj->UartId];

	CRITICAL_SECTION_BEGIN();
	res = IsFifoFull( &obj->TxFifo );
	if( res == 0 )
	{
		FifoPush( &obj->TxFifo, data );
		if( obj->Instance->remaining_tx_buffer_length == 0 )
		{
			obj->TxData = FifoPop( &obj->TxFifo );
			_usart_write_buffer( obj->Instance, &obj->TxData, 1 );
			obj->IsSendData = true;
		}
	}
	CRITICAL_SECTION_END();
	return res;
#else
	uint16_t txd = data;
	
	enum status_code res = usart_write_wait( obj->Instance, txd );
	return res;
#endif
}

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj   UART object
 * \param [IN] data  Received character
 * \retval status    [0: OK, 1: Busy]
 */
uint8_t UartMcuGetChar( Uart_t *obj, uint8_t *data )
{
	if( obj->IsInitialized == false ) return 0;
#if UART_CALLBACK_ENABLE
	CRITICAL_SECTION_BEGIN();
	uint8_t res = IsFifoEmpty( &obj->RxFifo );
	if( res == 0 )
	{
		*data = FifoPop( &obj->RxFifo );
	}
	CRITICAL_SECTION_END();
	return res;
#else
	uint16_t rxd;

	enum status_code res = usart_read_wait( obj->Instance, &rxd );
	obj->IsBreak = (res == STATUS_ERR_BAD_FORMAT) ? IS_BREAK_BREAK : IS_BREAK_NONE;
	*data = (uint8_t)rxd;
	return res;
#endif
}

/*!
 * \brief Sends a buffer to the UART
 *
 * \param [IN] obj    UART object
 * \param [IN] buffer Buffer to be sent
 * \param [IN] size   Buffer size
 * \retval status     [0: OK, 1: Busy]
 */
uint8_t UartMcuPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size )
{
	if( obj->IsInitialized == false ) return 0;
#if UART_CALLBACK_ENABLE
	while( size )
	{
		while( UartMcuPutChar( obj, *buffer ) != 0 );
		buffer++;
		size--;
	}
	return 0;
#else
	enum status_code res = usart_write_buffer_wait( obj->Instance, buffer, size );
	return res;
#endif
}

/*!
 * \brief Gets a character from the UART
 *
 * \param [IN] obj          UART object
 * \param [IN] buffer       Received buffer
 * \param [IN] size         Number of bytes to be received
 * \param [OUT] nbReadBytes Number of bytes really read
 * \retval status           [0: OK, 1: Busy]
 */
uint8_t UartMcuGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size, uint16_t *nbReadBytes )
{
	if( obj->IsInitialized == false ) return 0;
#if UART_CALLBACK_ENABLE
	while( size )
	{
		while( UartMcuGetChar( obj, buffer ) != 0 );
		buffer++;
		size--;
	}
	return 0;
#else
	enum status_code res = usart_read_buffer_wait( obj->Instance, buffer, size );
	return res;
#endif
}

/*
 *
 */
void UartMcuEnable( Uart_t *obj )
{
	if( obj->IsInitialized == false ) return;
	usart_enable(obj->Instance);	
}

/*
 *
 */
void UartMcuDisable( Uart_t *obj )
{
	if( obj->IsInitialized == false ) return;
	usart_disable(obj->Instance);	
}

/*
 *
 */
uint8_t UartMcuIsBreak( Uart_t *obj )
{
	if( obj->IsInitialized == false ) return IS_BREAK_NONE;
	uint8_t brk = obj->IsBreak;
	if( brk )	obj->IsBreak = IS_BREAK_NONE;
	return brk;
}

/*
 *
 */
void UartMcuFlush( Uart_t *obj )
{
	if( obj->IsInitialized == false ) return;
#if UART_CALLBACK_ENABLE
	FifoInit( &obj->RxFifo, UartRxBuff[obj->UartId], UART_RX_BUFF_SIZE );
	FifoInit( &obj->TxFifo, UartTxBuff[obj->UartId], UART_TX_BUFF_SIZE );
#endif
}

/*
 *
 */
void UartMcuWaitTxEmpty( Uart_t *obj )
{
	if( obj->IsInitialized == false ) return;
#if UART_CALLBACK_ENABLE
	while( IsFifoEmpty( &obj->TxFifo ) == 0 );
#endif
	SercomUsart *const usart_hw = &(obj->Instance->hw->USART);
	while( !(usart_hw->INTFLAG.bit.DRE) );
	if( !obj->IsSendData )	return;
	while( !(usart_hw->INTFLAG.bit.TXC) );	
}

/*
 *
 */
uint16_t UartMcuTxBuffLen( Uart_t *obj )
{
	if( obj->IsInitialized == false ) return 0;
	uint16_t len = 0;
#if UART_CALLBACK_ENABLE
	len = FiFoLen( &obj->TxFifo );
#endif
	SercomUsart *const usart_hw = &(obj->Instance->hw->USART);
	if ( !usart_hw->INTFLAG.bit.DRE ||
		(obj->IsSendData && !usart_hw->INTFLAG.bit.TXC) ) len++;
	return len;
}

/*
 *
 */
uint16_t UartMcuRxBuffLen( Uart_t *obj )
{
	if( obj->IsInitialized == false ) return 0;
	uint16_t len = 0;
#if UART_CALLBACK_ENABLE
	len = FiFoLen( &obj->RxFifo );
#endif
	return len;
}


#if UART_CALLBACK_ENABLE
/*
 *
 */
static Uart_t *uart_module_to_obj( struct usart_module *const usart_module )
{
	if( usart_module == &usart_instance[UART_1] )	return &Uart1;
	if( usart_module == &usart_instance[UART_2] )	return &Uart2;
	return NULL;
}

/*
 *
 */
static void usart_read_callback(struct usart_module *const usart_module)
{
	Uart_t *obj = uart_module_to_obj( usart_module );
	if( obj == NULL )	return;
	if( IsFifoFull( &obj->RxFifo ) == 0 )
	{
		FifoPush( &obj->RxFifo, obj->RxData );
	}
	_usart_read_buffer( usart_module, &obj->RxData, 1 );
	if( obj->BreakChar && obj->BreakChar == obj->RxData )
	{
		obj->IsBreak = IS_BREAK_CHAR;
	}
}

/*
 *
 */
static void usart_write_callback(struct usart_module *const usart_module)
{
	Uart_t *obj = uart_module_to_obj( usart_module );
	if( obj == NULL )	return;
	if( IsFifoEmpty( &obj->TxFifo ) == 0 )
	{
		obj->TxData = FifoPop( &obj->TxFifo );
		_usart_write_buffer( usart_module, &obj->TxData, 1 );		
	}
}

/*
 *
 */
static void usart_break_callback(struct usart_module *const usart_module)
{
	Uart_t *obj = uart_module_to_obj( usart_module );
	if( obj == NULL )	return;
	if( usart_module->rx_status == STATUS_ERR_BAD_FORMAT )
	{
		if( !obj->BrkDisable )	obj->IsBreak = IS_BREAK_BREAK;
	}
}

#endif
