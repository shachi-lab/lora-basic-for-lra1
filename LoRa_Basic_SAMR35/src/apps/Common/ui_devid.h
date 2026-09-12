/*!
 * @file    ui_devid.h
 * @brief   HeaderFile of Device-ID (EUI-64)
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
#ifndef __UI_DEVID_H
#define __UI_DEVID_H

/*
 * Device ID definition
 *
 * This file defines the fixed components used to generate
 * a unique device identifier.
 *
 * The ID is composed of:
 *  - OUI-36 (Assigned to hardware manufacturers by the IEEE)
 *  - Product ID
 *  - Obfuscated serial number
 *
 * Notes:
 *  - The OUI uses a locally administered address space.
 *  - Serial number is XOR-obfuscated to avoid raw exposure.
 */

/*
 * LRA1 hardware manufacturer OUI.
 * DevEUI is derived from the factory-programmed serial number.
 * OUI-36 (36-bit) : 70-B3-D5-59-E
 */
#define	DEVID_OUI36_0		0x70		// 36 bit
#define	DEVID_OUI36_1		0xb3
#define	DEVID_OUI36_2		0xd5
#define	DEVID_OUI36_3		0x59
#define	DEVID_OUI36_4		0xe0

/*
 * Product identifier (8-bit)
 * Used to distinguish product variants under the same OUI.
 */
#define DEVID_PRODUCT        0x01    // Product ID (8bit)

/*
 * Magic number for serial obfuscation
 *
 * Purpose:
 *  - Avoid exposing raw serial numbers
 *  - Provide lightweight, reversible scrambling
 *
 * Bit width:
 *  - Lower 20 bits are used
 */
#define DEVID_MAGIC_NUM      0x000caffe  // 20bit magic value

/*
 * Serial number mask
 *
 * Limits the serial number range to 28 bits.
 * Upper bits must be zero.
 */
#define DEVID_SERIAL_MASK    0x0fffffff  // 28bit mask

/*
 * Obfuscated serial number
 *
 * Raw serial (selial number) is XOR-ed with the magic number
 * to generate the externally visible serial value.
 */
#define DEVID_SN( sn )		(sn ^ DEVID_MAGIC_NUM)

#endif /* __UI_DEVID_H */
