/**
 * \file
 *
 * \brief Linker script for running in internal FLASH on the SAMR35J17B
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */


OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

/* Memory Spaces Definitions */
MEMORY
{
  rom      (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00020000
  ram      (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00004000
  lpram    (rwx) : ORIGIN = 0x30000000, LENGTH = 0x00002000
}

/* The stack size used by the application. NOTE: you need to adjust according to your application. */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : DEFINED(__stack_size__) ? __stack_size__ : 0x1000;

/* Section Definitions */
SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors .vectors.*))
        KEEP(*(.cfmconfig))     /* Configuration words */
        *(.text .text.* .gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7)
        *(.rodata .rodata* .gnu.linkonce.r.*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)

        /* Support C constructors, and C destructors in both user code
           and the C library. This also provides support for C++ code. */
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

        . = ALIGN(4);
        _efixed = .;            /* End of text section */
    } > rom

    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > rom
    PROVIDE_HIDDEN (__exidx_end = .);

    . = ALIGN(4);
    _etext = .;

    .relocate : AT (_etext)
    {
        . = ALIGN(4);
        _srelocate = .;
        *(.ramfunc .ramfunc.*);
        *(.data .data.*);
        . = ALIGN(4);
        _erelocate = .;
    } > ram

    /* LPRAM */
    .lpram (NOLOAD):
    {
        . = ALIGN(8);
        _slpram = .;
        *(.lpram .lpram.*);
        . = ALIGN(8);
        _elpram = .;
    } > lpram

    /* .bss section which is used for uninitialized data */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = . ;
        _szero = .;
        *(EXCLUDE_FILE (*LoRaMac.o *Region*.o) .bss)
        *(EXCLUDE_FILE (*LoRaMac.o *Region*.o) .bss.*)
        *(EXCLUDE_FILE (*LoRaMac.o *Region*.o) COMMON)
        *(.maczero .maczero.*)
        . = ALIGN(4);
        _ebss = . ;
        _ezero = .;
    } > ram

    .mac_bss (NOLOAD) :
    {
        . = ALIGN(4);
        _smac_bss = .;
        *(.bss .bss.*)
        *(COMMON)

        . = ALIGN(2);
        _mac_bss_version = .;
        . = . + 2;

        . = ALIGN(4);
        _mac_bss_crc = .;
        . = . + 4;
        _emac_bss = .;

        . = ALIGN(4);
    } > ram

    MAC_BSS_SIZE = _emac_bss - _smac_bss;
    /* .mac_backup section must start from 0x20001170 or higher.
       Below that is used by the bootloader. */
    .mac_backup (NOLOAD) :
    {
        . = ALIGN(4);
        _smac_backup = .;
        . = . + MAC_BSS_SIZE;
        _emac_backup = .;
        . = ALIGN(4);
    } > ram

    .var_bss (NOLOAD) :
    {
        . = ALIGN(4);
	_svar_bss = .;
        *(.var_bss .var_bss.*)
	. = ALIGN(4);
	_evar_bss = .;
    } > ram

    .debug_bss (NOLOAD) :
    {
        . = ALIGN(4);
		_sdebug_bss = .;
        *(.debug_bss .debug_bss.*)
		. = ALIGN(4);
		_edebug_bss = .;
    } > ram

    /* stack section */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = .;
    } > ram

    .text :
    {
        . = ALIGN(4);
        _mac_bss_size = .;
        LONG(MAC_BSS_SIZE);
        . = ALIGN(4);
    } > rom

    . = ALIGN(4);
    _end = . ;
}
