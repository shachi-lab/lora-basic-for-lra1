# LoRa-BASIC for LRA1 Firmware

[日本語](README_jp.md) | [English](README.md)

**LoRa-BASIC is firmware that lets you program the LRA1 LoRa module in BASIC, including direct control of both LoRa private (P2P) and LoRaWAN communication.**

You can send and receive LoRa P2P packets, join a LoRaWAN network, exchange LoRaWAN data, control peripherals, and run application logic entirely from BASIC without an external MCU.

Commands can be tested interactively from the REPL, or combined into line-numbered BASIC programs and executed with the `RUN` command.

“Send data over LoRaWAN in just a few lines of BASIC.”

> **Note on the source code**  
> This code has grown through many years of feature additions and maintenance, so some parts are patchwork and not especially clean.  
> It is published as a working implementation and reference, so please keep that background in mind when reading the source.

## LRA1 and Development

LRA1 is a LoRa communication module designed, manufactured and sold by i2-electronics. See the [i2-electronics product information](https://i2-ele.co.jp/LoRa.html) for hardware details.

This firmware was **designed and implemented by Shachi-lab** for LRA1. It continues the development of the firmware supplied with LRA1, with ongoing development and releases under the Shachi-lab name.

The BASIC interpreter is based on nanoBASIC, which was also developed by Shachi-lab, and has been extended and maintained for LoRa-BASIC. The project uses third-party code, including Microchip ASF and Semtech LoRaMac-node, under its original licenses.

## Regional and Regulatory Notice

LoRa-BASIC for LRA1 is designed for use in Japan.  
The LoRaWAN mode supports the AS923-1 regional parameters, and the P2P mode uses frequencies and channel settings intended for the Japanese 920 MHz band.

The LRA1 module is certified for operation under Japan's Radio Act. 
The firmware published in this repository is intended to operate within the conditions covered by that certification.

However, modifying the source code in ways that affect radio operation, such as transmit frequency, transmit power, transmission duration, or carrier-sense behavior, may cause the device to no longer comply with the conditions of its certification.

If modified firmware is used on actual hardware, the user is responsible for confirming compliance with all applicable laws, regulations, and certification requirements.

Operation outside Japan, and compliance with radio regulations or certification requirements in other countries or regions, are not supported.　

## Features

- **Wireless communication from BASIC**: LoRa private (P2P) and LoRaWAN support.
- **Processing and communication without an external MCU**: Run simple tasks entirely on the module using BASIC.
- **Peripheral access**: Use UART, I2C and SPI from BASIC.
- **Try commands interactively in the REPL**: Enter BASIC statements and commands directly from a serial terminal and see the results immediately.
- **Create and run BASIC programs**: Enter line-numbered BASIC programs from the REPL and execute them with the `RUN` command.

## Manuals

Four reference manuals are provided for different areas of operation. The Japanese editions are the source documents, with corresponding English editions also available.

For a concise index of commands, variables, and functions, see the [English index](docs/lora-basic-command-reference.md) or the [Japanese index](docs/lora-basic-command-reference_jp.md).

| Manual | Japanese | English | Contents |
|---|---|---|---|
| Basics | [日本語](docs/lora-basic-manual_basics_jp.md) | [English](docs/lora-basic-manual_basics_en.md) | Connections, operating modes, BASIC syntax, core commands, input/output, system variables, UART2, I2C and SPI |
| P2P | [日本語](docs/lora-basic-manual_p2p_jp.md) | [English](docs/lora-basic-manual_p2p_en.md) | LoRa P2P settings, transmission and reception, communication buffers, carrier sensing and forwarding |
| LoRaWAN | [日本語](docs/lora-basic-manual-wan_jp.md) | [English](docs/lora-basic-manual-wan_en.md) | LoRaWAN activation, OTAA and ABP, uplinks, downlinks, commands and variables |
| Maintenance | [日本語](docs/lora-basic-manual-maintenance_jp.md) | [English](docs/lora-basic-manual-maintenance_en.md) | Maintenance-level commands and variables used for manufacturing adjustment and testing |

Refer to the relevant manual for usage, connections, communication settings, commands and syntax.   
The examples below illustrate what you can do.

### REPL Mode Example: Measure VDD Voltage and Send It over P2P

Measure VDD voltage, inspect the reading, divide the value by 100 and send it over P2P. Try each command without writing a complete program.

```basic
>a=adc(27)
OK
>?a
3332
OK
>b=a/100
OK
>send b
OK
```

### BASIC Program Example: Send Evaluation Board Data over LoRaWAN

This example uses the LRA1 evaluation board to read temperature, humidity, atmospheric pressure and voltage, then transmit them over LoRaWAN. The BASIC program handles sensor readings, payload preparation, display output and wireless transmission.

```basic
100 Do
110 If !Wan_JoinStat Then Wan_Join EndIf
120 If Wan_JoinStat Then
130 Bme T,H,P
140 V=Adc(27)
150 Txd[7]=8
160 Txdw[8]=T:Txdw[10]=H:Txdw[12]=P:Txdw[14]=V
170 Lpos=0:Lprint 0(T/10,2)"."T%10" "0(H/10,2)"%"
180 Lpos=64:Lprint 0(P/10,4)"*"V/10
190 Wan_Tx
200 EndIf
210 Sleep 600
220 Loop
```

Error handling is omitted from this example for clarity.

## Version Scope

This repository covers firmware source code and binaries (.bin) for **Shachi-lab v1.28.a and later**.

**Versions earlier than v1.28.a are outside its scope.** For earlier versions, use the binaries distributed on the [i2-electronics website](https://i2-ele.co.jp/LoRa.html). See [CHANGELOG.md](CHANGELOG.md) for change history.

## Firmware Download

Released firmware binaries (`.bin`) are available from [GitHub Releases](https://github.com/shachi-lab/lora-basic-for-lra1/releases). Select the required version and download its attached binary file.

## Firmware Updates

Use the **[Python updater published separately by Shachi-lab](https://github.com/shachi-lab/lra1_tool)**. Refer to its README for setup and usage instructions.

| Updater | Shachi-lab binaries | Binaries distributed by i2-electronics |
|---|---|---|
| Python updater published by Shachi-lab | Supported | Supported |
| Updaters distributed by i2-electronics, including Windows and Python versions | Not supported; an error occurs | Supported |

**Updaters distributed by i2-electronics cannot install Shachi-lab firmware.** The Python updater published by Shachi-lab can also write binaries distributed by i2-electronics.

The bootloader **will be published in a separate repository at a later date**. It is compatible with the bootloader programmed into LRA1 modules at the factory.

## Building the Firmware and Source Layout

**Microchip Studio 7** is required to build the firmware itself from source. Open `LoRa_Basic_SAMR35/LoRa_Basic_SAMR35.atsln`. Microchip Studio is not needed to use the built-in BASIC interpreter.

The main project contents are listed below. Paths are relative to `LoRa_Basic_SAMR35/`.

| Project path | Contents |
|---|---|
| `LoRa_Basic_SAMR35.cproj` | Microchip Studio project settings |
| `src/apps/` | BASIC interpreter, LoRaWAN/P2P control and other applications |
| `src/boards/` | Board-specific code for LRA1/SAMR35 |
| `src/mac/`, `src/radio/` | LoRaMAC and radio drivers |
| `src/system/`, `src/peripherals/`, `src/config/` | System code, peripheral support and configuration |
| `src/ASF/` | Microchip ASF and bundled third-party components |
| `LoRa_Basic_SAMR35.ld` | Linker script |

The repository root contains this README, change history and license documents. The updater and bootloader are maintained separately from this firmware repository.

## License and Disclaimer

Original portions owned by Shachi-lab are provided under the **[MIT License](LICENSE.txt)**. Copyright remains with Shachi-lab.

Use, modification, commercial use, redistribution and sale are permitted. Retain the copyright notice and the full license, including its disclaimer. Third-party code retains its original license terms; see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

The software is provided free of charge and without warranty. The authors disclaim liability as stated in the MIT License and have no obligation to provide support. Distributors may decide whether to offer their own support. Users must check applicable laws and certification conditions, including Japanese radio certification, when using, modifying or integrating the software into products.

## Paid Support and Development

Support for this software and related firmware development are available **for a fee by separate arrangement**. Contact [Shachi-lab](https://shachi-lab.com/). Permission to use the code does not include support from its author.

Please do not contact i2-electronics about the Shachi-lab firmware in this repository. Direct hardware inquiries to the hardware vendor.

---

Copyright (c) 2019–2026 Shachi-lab
