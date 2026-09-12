# LoRa-BASIC Command, Variable, and Function Index

[日本語](lora-basic-command-reference_jp.md)

This index lists the commands, keywords, variables, constants, and functions registered in the LoRa-BASIC for LRA1 source code. See the linked reference manuals for complete syntax, arguments, ranges, operating conditions, and behavior.

Names follow the spelling registered in the source code and emitted by List. Names are case-insensitive on input. Some items depend on the build configuration, operating mode, or configuration level.

## BASIC Statements, Commands, and Keywords

These statements, commands, and syntax keywords control programs, I/O, peripherals, P2P communication, and maintenance operations.

| Name | Syntax | Overview | Details |
|---|---|---|---|
| `Print` | `Print [expression]`, `$Print [expression]`, `%Print [expression]`, `&Print [expression]`, or `?[expression]` | Outputs data in plain, hexadecimal, percent-encoded, or Base64 form. `?` is an abbreviation for `Print`. | [Basics](lora-basic-manual_basics_en.md) |
| `Input` | `Input [variable]` | Waits for serial input and stores the entered number in the variable. | [Basics](lora-basic-manual_basics_en.md) |
| `Goto` | `Goto [label]` | Jumps to the specified line. | [Basics](lora-basic-manual_basics_en.md) |
| `Gosub` | `Gosub [label]` | Calls the subroutine at the specified line. | [Basics](lora-basic-manual_basics_en.md) |
| `Return` | `Return` | Returns from a subroutine. | [Basics](lora-basic-manual_basics_en.md) |
| `For` | `For [variable=expression1] To [expression2] Step [expression3] … Next` | Starts a For–Next loop over the specified range. | [Basics](lora-basic-manual_basics_en.md) |
| `Next` | `Next` | Continues with the next iteration of a For loop. | [Basics](lora-basic-manual_basics_en.md) |
| `Do` | `Do … Loop` or `Do … Loop While [expression]` | Starts an unconditional Do–Loop or one that repeats while its trailing condition is true. | [Basics](lora-basic-manual_basics_en.md) |
| `Loop` | `Loop` or `Loop While [expression]` | Ends a Do loop. With While, it repeats while the condition is true. | [Basics](lora-basic-manual_basics_en.md) |
| `While` | `While [expression] … Loop` | Continues a loop while the expression is true. | [Basics](lora-basic-manual_basics_en.md) |
| `If` | `If [expression] Then … ElseIf [expression] Then … Else … EndIf` | Starts conditional execution. | [Basics](lora-basic-manual_basics_en.md) |
| `Run` | `Run [label]` | Executes the program in the program area. | [Basics](lora-basic-manual_basics_en.md) |
| `Resume` | `Resume` | Resumes execution interrupted by Stop or a serial break character/signal. | [Basics](lora-basic-manual_basics_en.md) |
| `Stop` | `Stop` | Interrupts program execution. | [Basics](lora-basic-manual_basics_en.md) |
| `End` | `End` | Ends program execution and returns to REPL input. | [Basics](lora-basic-manual_basics_en.md) |
| `New` | `New` | Clears the program and variables. Causes an error when program entry is disabled. | [Basics](lora-basic-manual_basics_en.md) |
| `List` | `List [[label1], [label2]]` | Displays the program range from label1 through label2 in line-number order. | [Basics](lora-basic-manual_basics_en.md) |
| `Renum` | `Renum [expression]` | Renumbers program lines. Causes an error when program entry is disabled. | [Basics](lora-basic-manual_basics_en.md) |
| `Edit` | `Edit [expression]` | Enables or disables program entry. | [Basics](lora-basic-manual_basics_en.md) |
| `Delete` | `Delete [[label1], [label2]]` | Deletes program lines. Causes an error when program entry is disabled. | [Basics](lora-basic-manual_basics_en.md) |
| `Pload` | `Pload [![label]]` | Loads the saved Flash program into the program area. | [Basics](lora-basic-manual_basics_en.md) |
| `Psave` | `Psave [] [expression]` | Saves the RAM program to Flash. Causes an error when program entry is disabled. | [Basics](lora-basic-manual_basics_en.md) |
| `Ver` | `Ver` | Displays the software version. | [Basics](lora-basic-manual_basics_en.md) |
| `Delay` | `Delay time` | Pauses for the specified time in 1/1000-second units, then resumes. | [Basics](lora-basic-manual_basics_en.md) |
| `Pause` | `Pause` | Waits for one character from the serial port. | [Basics](lora-basic-manual_basics_en.md) |
| `Exit` | `Exit` | Exits a For–Next or Do/While–Loop and continues after Next or Loop (Loop While). | [Basics](lora-basic-manual_basics_en.md) |
| `Continue` | `Continue` | Skips the remaining body of a For–Next or Do/While–Loop iteration and continues the loop. | [Basics](lora-basic-manual_basics_en.md) |
| `Randomize` | `Randomize [expression]` | Sets the random seed. | [Basics](lora-basic-manual_basics_en.md) |
| `Data` | `Data [expression1], [expression2], …` | Defines values for Read. Expressions, including function calls, are allowed. | [Basics](lora-basic-manual_basics_en.md) |
| `Read` | `Read [variable]` | Reads the numeric result of a Data expression into the variable. | [Basics](lora-basic-manual_basics_en.md) |
| `Restore` | `Restore [label]` | Changes the Data position used by Read. | [Basics](lora-basic-manual_basics_en.md) |
| `Reset` | `Reset` | Performs a system reset. | [Basics](lora-basic-manual_basics_en.md) |
| `Sleep` | `Sleep [time] [,mode]` | Waits in a low-power mode. | [Basics](lora-basic-manual_basics_en.md) |
| `Deep` | `Deep [time] [,mode]` | Waits in ultra-low-power mode (Deep Sleep). | [Basics](lora-basic-manual_basics_en.md) |
| `Outp` | `Outp [expression1], [expression2]` | Sets GPIO states individually or in a group. | [Basics](lora-basic-manual_basics_en.md) |
| `Swap` | `Swap [variable1], [variable2]` | Exchanges the two variable values. | [Basics](lora-basic-manual_basics_en.md) |
| `Else` | `Else` | Specifies the branch used when preceding conditions are false. | [Basics](lora-basic-manual_basics_en.md) |
| `ElseIf` | `ElseIf expression Then` | Specifies an additional branch condition. | [Basics](lora-basic-manual_basics_en.md) |
| `EndIf` | `EndIf` | Marks the end of an If block. | [Basics](lora-basic-manual_basics_en.md) |
| `Lclr` | `Lclr` | Clears the LCD display. | [Basics](lora-basic-manual_basics_en.md) |
| `Lprint` | `Lprint [arguments]` | Displays a string on the LCD. | [Basics](lora-basic-manual_basics_en.md) |
| `#?` | `#?` | Displays all configuration values together. | [P2P](lora-basic-manual_p2p_en.md) |
| `Nop` | `Nop` | Continues without performing an operation. | [Basics](lora-basic-manual_basics_en.md) |
| `Default` | `Default [expression1]` | Resets settings to their defaults. | [P2P](lora-basic-manual_p2p_en.md) |
| `Sload` | `Sload` | Loads settings from Flash memory. | [P2P](lora-basic-manual_p2p_en.md) |
| `Ssave` | `Ssave` | Saves settings to Flash memory. | [P2P](lora-basic-manual_p2p_en.md) |
| `Send` | `Send [string]` | Transmits a LoRa packet. | [P2P](lora-basic-manual_p2p_en.md) |
| `Recv` | `Recv [expression1] [, expression2]` | Starts LoRa packet reception. | [P2P](lora-basic-manual_p2p_en.md) |
| `RxStop` | `RxStop` | Stops reception. | [P2P](lora-basic-manual_p2p_en.md) |
| `Comm` | `Comm [expression]` | Starts a simple bidirectional communication mode. | [P2P](lora-basic-manual_p2p_en.md) |
| `Txif` | `Txif [string]` | Transmits packets periodically. | [Maintenance](lora-basic-manual-maintenance_en.md) |
| `TxWave` | `TxWave [expression]` | Transmits a continuous RF signal. | [Maintenance](lora-basic-manual-maintenance_en.md) |
| `Sens` | `Sens [expression]` | Measures and displays Rssi periodically (every 200 msec). | [Maintenance](lora-basic-manual-maintenance_en.md) |
| `Bme` | `Bme [argument1[,argument2[,variable3]]]` | Measures temperature, humidity, and pressure using the evaluation board's BME280. | [Basics](lora-basic-manual_basics_en.md) |
| `I2cR` | `I2cR [expression1], [expression2], [expression3]` | Reads from an I2C device. | [Basics](lora-basic-manual_basics_en.md) |
| `I2cW` | `I2cW [expression1], [expression2], [expression3]` | Writes to an I2C device. | [Basics](lora-basic-manual_basics_en.md) |
| `Aload` | `Aload` | Loads array variables @(0–255) previously saved with Asave. | [Basics](lora-basic-manual_basics_en.md) |
| `Asave` | `Asave` | Saves all array variables @(0–255) to Flash, overwriting the stored array. | [Basics](lora-basic-manual_basics_en.md) |
| `Utrans` | `Utrans [expression]` | Uses UART2 in transparent mode. | [Basics](lora-basic-manual_basics_en.md) |
| `Pwm` | `Pwm [expression1], [expression2], [expression3]` | Outputs Pwm on a port. | [Basics](lora-basic-manual_basics_en.md) |
| `Ugps` | `Ugps [expression1], [expression2]` | Reads time, latitude, and longitude from a GPS device connected to UART2. | [Basics](lora-basic-manual_basics_en.md) |
| `Datetime` | `Datetime(expression)` | In a string expression, the GPS Datetime command converts serial seconds to text. | [Basics](lora-basic-manual_basics_en.md) |
| `Ext_cmd` | `Ext_cmd` | Invokes an extension command included at build time. | [Basics](lora-basic-manual_basics_en.md) |
| `Uprint` | `Uprint [expression]` | Outputs the expression result to UART2. | [Basics](lora-basic-manual_basics_en.md) |
| `Then` | `Then` | Follows the condition in If or ElseIf. | [Basics](lora-basic-manual_basics_en.md) |
| `To` | `To end-value` | Specifies the final value of a For loop. | [Basics](lora-basic-manual_basics_en.md) |
| `Step` | `Step increment` | Specifies the increment of a For loop. | [Basics](lora-basic-manual_basics_en.md) |
| `Distance` | `Distance [expression1]` | Calculates the distance and bearing between two GPS positions. | [Basics](lora-basic-manual_basics_en.md) |
| `Catch` | `Catch [label]` | Defines an error handler. | [Basics](lora-basic-manual_basics_en.md) |
| `Cad` | `Cad [expression1] [, expression2]` | Waits for a preamble. Cad means Channel Activity Detection. | [P2P](lora-basic-manual_p2p_en.md) |
| `MemDump` | `MemDump` | Displays memory contents for maintenance. | [Maintenance](lora-basic-manual-maintenance_en.md) |

## Encoded String Notation

A prefix on a string or `Print` allows binary data to be entered or output in the required representation.

| Format | String input | Print output |
|---|---|---|
| Plain | `"..."` | `Print [expression]` |
| Hexadecimal | `$"..."` | `$Print [expression]` |
| Percent-encoded | `%"..."` | `%Print [expression]` |
| Base64 | `&"..."` | `&Print [expression]` |

See [Basics](lora-basic-manual_basics_en.md) for conversion rules and examples.

## Variables and Constants

These identifiers access system state, settings, communication parameters, and transmit or receive buffers. Some are read-only or write-only.

| Name | Syntax | Overview | Details |
|---|---|---|---|
| `@` | `@[index]` or `@(index)` | Numeric array whose element is selected by index. | [Basics](lora-basic-manual_basics_en.md) |
| `^` | `^` | The only string variable in LoRa-BASIC. | [Basics](lora-basic-manual_basics_en.md) |
| `Null` | `Null` | A fixed value that evaluates to 0 in a numeric context and an empty string in a string context. | [Basics](lora-basic-manual_basics_en.md) |
| `Tick` | `Tick` | A counter incremented every 1/1000 second after system startup. | [Basics](lora-basic-manual_basics_en.md) |
| `Clock` | `Clock` | A counter incremented every second after system startup. | [Basics](lora-basic-manual_basics_en.md) |
| `Inkey` | `Inkey, Inkey(expression)` | Returns the ASCII code of a character from the serial UART input buffer. | [Basics](lora-basic-manual_basics_en.md) |
| `Auto` | `Auto` | Sets the BASIC command string executed at startup. | [Basics](lora-basic-manual_basics_en.md) |
| `False` | `False` | Fixed value representing false (0). | [Basics](lora-basic-manual_basics_en.md) |
| `True` | `True` | Fixed value representing true (1). | [Basics](lora-basic-manual_basics_en.md) |
| `Baud` | `Baud` | Sets the serial baud rate. | [Basics](lora-basic-manual_basics_en.md) |
| `Echo` | `Echo` | Controls echo of serial input characters. | [Basics](lora-basic-manual_basics_en.md) |
| `Lpos` | `Lpos` | Sets the text display position on the evaluation board's LCD. | [Basics](lora-basic-manual_basics_en.md) |
| `Lcont` | `Lcont` | Sets the contrast of the evaluation board's LCD. | [Basics](lora-basic-manual_basics_en.md) |
| `Modem` | `Modem` | Selects the modulation scheme. | [P2P](lora-basic-manual_p2p_en.md) |
| `Pwr` | `Pwr` | Sets the transmit power. | [P2P](lora-basic-manual_p2p_en.md) |
| `Sf` | `Sf` | Sets the spreading factor. | [P2P](lora-basic-manual_p2p_en.md) |
| `Bw` | `Bw` | Sets the bandwidth. | [P2P](lora-basic-manual_p2p_en.md) |
| `Cr` | `Cr` | Sets the coding rate. | [P2P](lora-basic-manual_p2p_en.md) |
| `Ch` | `Ch` | Sets the transmit/receive channel. | [P2P](lora-basic-manual_p2p_en.md) |
| `Frq` | `Frq` | Reads the transmit/receive frequency. | [P2P](lora-basic-manual_p2p_en.md) |
| `Gid` | `Gid` | Sets the group ID. | [P2P](lora-basic-manual_p2p_en.md) |
| `Own` | `Own` | Sets the local device ID. | [P2P](lora-basic-manual_p2p_en.md) |
| `Dst` | `Dst` | Sets the destination ID. | [P2P](lora-basic-manual_p2p_en.md) |
| `Snr` | `Snr` | Reads the Snr value of the received packet. | [P2P](lora-basic-manual_p2p_en.md) |
| `Rssi` | `Rssi` | Reads the Rssi of a received packet. | [P2P](lora-basic-manual_p2p_en.md) |
| `DevId` | `DevId` | Reads the device-specific EUI-64 identifier. | [P2P](lora-basic-manual_p2p_en.md) |
| `Stat` | `Stat` | Reads transmit/receive status and LoRa errors. | [P2P](lora-basic-manual_p2p_en.md) |
| `Period` | `Period` | Txif transmission interval. | [Maintenance](lora-basic-manual-maintenance_en.md) |
| `Ctrl` | `Ctrl` | Sets control options. | [Basics](lora-basic-manual_basics_en.md) |
| `Txd` | `Txd(expression) or Txd` | Reads or writes the transmit buffer. | [P2P](lora-basic-manual_p2p_en.md) |
| `Rxd` | `Rxd(expression) or Rxd` | Reads the receive buffer. | [P2P](lora-basic-manual_p2p_en.md) |
| `Txdw` | `Txdw(offset)` | Reads or writes the transmit buffer in 16-bit units. | [P2P](lora-basic-manual_p2p_en.md) |
| `Rxdw` | `Rxdw(offset)` | Reads the receive buffer in 16-bit units. | [P2P](lora-basic-manual_p2p_en.md) |
| `Txdl` | `Txdl(offset)` | Reads or writes the transmit buffer in 32-bit units. | [P2P](lora-basic-manual_p2p_en.md) |
| `Rxdl` | `Rxdl(offset)` | Reads the receive buffer in 32-bit units. | [P2P](lora-basic-manual_p2p_en.md) |
| `Pass` | `Pass` | Sets the configuration level. | [Basics](lora-basic-manual_basics_en.md) |
| `BrkCh` | `BrkCh` | Selects the ASCII break character and break-signal behavior used to interrupt execution. | [Basics](lora-basic-manual_basics_en.md) |
| `Level` | `Level` | Carrier-sense threshold. | [Maintenance](lora-basic-manual-maintenance_en.md) |
| `I2cD` | `I2cD(expression) or I2cD` | Reading and writing the I2C buffer | [Basics](lora-basic-manual_basics_en.md) |
| `Sn` | `Sn` | Serial number. | [Maintenance](lora-basic-manual-maintenance_en.md) |
| `Drift` | `Drift` | Frequency offset. | [Maintenance](lora-basic-manual-maintenance_en.md) |
| `MpuId` | `MpuId` | Reads the unique MCU identifier. | [Basics](lora-basic-manual_basics_en.md) |
| `Xtal32k` | `Xtal32k` | Selects one of the following 32.768 kHz clocks. | [Basics](lora-basic-manual_basics_en.md) |
| `Ubaud` | `Ubaud` | Gets or sets the UART2 baud rate. | [Basics](lora-basic-manual_basics_en.md) |
| `Uinkey` | `Uinkey or Uinkey(expression)` | Reads one character from UART2. | [Basics](lora-basic-manual_basics_en.md) |
| `AesKey` | `AesKey` | Sets or reads the shared encryption key. | [P2P](lora-basic-manual_p2p_en.md) |
| `Error` | `Error` | Reads an error code or throws an error. | [Basics](lora-basic-manual_basics_en.md) |
| `Gets` | `Gets, Gets(expression)` | Reads one line from the serial UART input buffer. | [Basics](lora-basic-manual_basics_en.md) |
| `Ugets` | `Ugets or Ugets(expression)` | Reads a string from UART2. | [Basics](lora-basic-manual_basics_en.md) |
| `TxLen` | `TxLen` | Reads the maximum transmit payload size. | [P2P](lora-basic-manual_p2p_en.md) |
| `Hop` | `Hop` | Hop sets the maximum forwarding count. Its default is 0. | [P2P](lora-basic-manual_p2p_en.md) |
| `FwdConf` | `FwdConf` | By default, forwarding uses the same Sf, Bw, Cr and Ch as reception. FwdConf allows different transmit parameters for forwarding. | [P2P](lora-basic-manual_p2p_en.md) |
| `Cause` | `Cause` | A 32-bit value indicating the cause of system startup, as listed below. | [Basics](lora-basic-manual_basics_en.md) |
| `SpiBaud` | `SpiBaud` | Sets the SPI baud rate and enables SPI on the GPIO pins. | [Basics](lora-basic-manual_basics_en.md) |
| `Bup1` | `Bup1` | Bup1 and Bup2 are two independent variables whose values are retained across resets. | [Basics](lora-basic-manual_basics_en.md) |
| `Bup2` | `Bup2` | Reads or writes a value in the backup area. | [Basics](lora-basic-manual_basics_en.md) |
| `Milisec` | `Milisec` | Returns the millisecond component (0–999) captured by the most recent read of Clock. | [Basics](lora-basic-manual_basics_en.md) |
| `Txdm` | `Txdm(offset)` | Reads or writes the transmit buffer in 24-bit units. | [P2P](lora-basic-manual_p2p_en.md) |
| `Rxdm` | `Rxdm(offset)` | Reads the receive buffer in 24-bit units. | [P2P](lora-basic-manual_p2p_en.md) |
| `ProtId` | `ProtId` | Reads or writes the P2P packet protocol ID. | [P2P](lora-basic-manual_p2p_en.md) |
| `SyncWord` | `SyncWord` | Reads or writes the P2P Sync Word. | [P2P](lora-basic-manual_p2p_en.md) |
| `Debug` | `Debug` | Holds the bit settings for debug functions. | [Maintenance](lora-basic-manual-maintenance_en.md) |

## Functions

These functions calculate, convert, read, or process values and buffers within expressions, then return a result.

| Name | Syntax | Overview | Details |
|---|---|---|---|
| `Rnd` | `Rnd(expression)` | For argument 0, returns a random value from 0 to 2,147,483,647 ($7FFF FFFF). | [Basics](lora-basic-manual_basics_en.md) |
| `Abs` | `Abs(expression)` | Returns the absolute value. | [Basics](lora-basic-manual_basics_en.md) |
| `Chr` | `Chr(expression)` | Inserts the low 8 bits directly into the string. | [Basics](lora-basic-manual_basics_en.md) |
| `WChr` | `WChr(expression)` | Inserts the low 16 bits in little-endian order into the string. | [Basics](lora-basic-manual_basics_en.md) |
| `Form` | `Form(format-string, expression)` | Formats the numeric expression as a string. | [Basics](lora-basic-manual_basics_en.md) |
| `Inp` | `Inp(expression)` | Reads a GPIO port state. | [Basics](lora-basic-manual_basics_en.md) |
| `Adc` | `Adc(expression)` | Reads an Adc conversion result. | [Basics](lora-basic-manual_basics_en.md) |
| `Int8` | `Int8(expression)` | Uses only the low 8 bits of the expression, treating all higher bits as 0, and returns the value interpreted as a signed 8-bit integer. | [Basics](lora-basic-manual_basics_en.md) |
| `Int16` | `Int16(expression)` | Uses only the low 16 bits of the expression, treating all higher bits as 0, and returns the value interpreted as a signed 16-bit integer. | [Basics](lora-basic-manual_basics_en.md) |
| `IsFree` | `IsFree(expression)` | Senses whether Ch is free. | [P2P](lora-basic-manual_p2p_en.md) |
| `Atan2` | `Atan2(expression1, expression2)` | Returns the arctangent of expression2/expression1. | [Basics](lora-basic-manual_basics_en.md) |
| `Sqrt` | `Sqrt(expression)` | Returns the square root of expression1. A negative argument returns 2,147,483,647 ($7FFF FFFF) | [Basics](lora-basic-manual_basics_en.md) |
| `Pow` | `Pow(expression1, expression2)` | Raises expression1 to expression2. If expression2 is 0 or less, returns 1. | [Basics](lora-basic-manual_basics_en.md) |
| `SpiTRx` | `SpiTRx(expression1[,expression2])` | Outputs expression1 through SPI and returns the value received through SPI. | [Basics](lora-basic-manual_basics_en.md) |
| `Bswap16` | `Bswap16(expression)` | Swaps the high and low bytes within each 16-bit half of a 32-bit value. | [Basics](lora-basic-manual_basics_en.md) |
| `Bswap32` | `Bswap32(expression)` | Reverses the byte order of a 32-bit value. | [Basics](lora-basic-manual_basics_en.md) |
| `TimeVal` | `TimeVal(expression)` | Converts date (year, month, day) and time (hour, minute, second) array values to serial seconds. | [Basics](lora-basic-manual_basics_en.md) |
| `LChr` | `LChr(expression)` | Inserts the low 32 bits in little-endian order into the string. | [Basics](lora-basic-manual_basics_en.md) |
| `MChr` | `MChr(expression)` | Inserts the low 24 bits in little-endian order into the string. | [Basics](lora-basic-manual_basics_en.md) |
| `Int24` | `Int24(expression)` | Uses only the low 24 bits of the expression, treating all higher bits as 0, and returns the value interpreted as a signed 24-bit integer. | [Basics](lora-basic-manual_basics_en.md) |
| `Bcc` | `Bcc(string)` | Calculates the string's longitudinal parity. | [Basics](lora-basic-manual_basics_en.md) |
| `Sum` | `Sum(string)` | Calculates the string's sum. | [Basics](lora-basic-manual_basics_en.md) |
| `ToInt` | `ToInt(string [, [offset] [, [length]]])` | Converts a selected part of a string to a number. | [Basics](lora-basic-manual_basics_en.md) |

## LoRaWAN Commands

These commands perform LoRaWAN activation, transmission, reception, saving, and channel operations. The `Wan_` prefix can be abbreviated as `#`.

| Name | Syntax | Overview | Details |
|---|---|---|---|
| `Wan_Join` | `Wan_Join` | Activates (joins) the device. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_UnJoin` | `Wan_UnJoin` | Deactivates (unjoins) the device. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_LinkChk` | `Wan_LinkChk` | Requests a link-check MAC command. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Tx` | `Wan_Tx [encode]` | Transmits uplink data. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Rx` | `Wan_Rx [timeout , ] [encode]` | Receives downlink data. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Save` | `Wan_Save` | Saves settings and join state. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_ChList` | `Wan_ChList` | Displays the channel list. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_RxStop` | `Wan_RxStop` | Stops downlink reception. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_ChAdd` | `Wan_ChAdd chid, Freq, dr-min, dr-max` | Adds a channel to the channel list. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_ChRemove` | `Wan_ChRemove chid` | Removes a channel from the list. | [LoRaWAN](lora-basic-manual-wan_en.md) |

## LoRaWAN Variables

These variables access LoRaWAN settings, keys, counters, and communication state. Write access and persistence vary by item.

| Name | Syntax | Overview | Details |
|---|---|---|---|
| `Wan_Mode` | `Wan_Mode` | Sets the LoRa-BASIC communication mode. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Class` | `Wan_Class` | Sets or reads the LoRaWAN class. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_TxPwr` | `Wan_TxPwr` | Sets or reads transmit power. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_FPort` | `Wan_FPort` | Reads or sets the transmit port. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Cnf` | `Wan_Cnf` | Reads or sets transmission confirmation. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Adr` | `Wan_Adr` | Reads or sets ADR (Adaptive Data Rate). | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Dr` | `Wan_Dr` | Reads or sets the data rate. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Trial` | `Wan_Trial` | Reads or sets the number of transmission attempts. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_DevEui` | `Wan_DevEui` | Reads or sets DevEui (Device EUI). | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_JoinEui` | `Wan_JoinEui` | Reads or sets JoinEui (AppEui). | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_AppKey` | `Wan_AppKey` | Reads or sets AppKey (Application Key). | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_DevAddr` | `Wan_DevAddr` | Reads or sets DevAddr (Device Address). | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_NetID` | `Wan_NetID` | Reads or writes the LoRaWAN NetID. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_AppSkey` | `Wan_AppSkey` | Reads or sets AppSkey (Application Session Key). | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_NwkSkey` | `Wan_NwkSkey` | Reads or sets NwkSkey (Network Session Key). | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_JoinStat` | `Wan_JoinStat` | Reads the activation state. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_UpCnt` | `Wan_UpCnt` | Sets or reads the uplink counter. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_DownCnt` | `Wan_DownCnt` | Sets or reads the downlink counter. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Length` | `Wan_Length` | Reads the available transmit byte count at the current data rate. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_JoinMode` | `Wan_JoinMode` | Sets or reads the activation method. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_ChMask` | `Wan_ChMask` | Reads or sets the channel mask. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_Pending` | `Wan_Pending` | Reads the pending downlink state. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_RecvDelay1` | `Wan_RecvDelay1` | Sets or reads RECEIVE_DELAY1. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_RecvDelay2` | `Wan_RecvDelay2` | Sets or reads RECEIVE_DELAY2. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_JoinDelay1` | `Wan_JoinDelay1` | Sets or reads JOIN_ACCEPT_DELAY1. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_JoinDelay2` | `Wan_JoinDelay2` | Sets or reads JOIN_ACCEPT_DELAY2. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_TimeReq` | `Wan_TimeReq` | Sets or reads the DEVICE_TIME_REQ MAC command state. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_DutyTime` | `Wan_DutyTime` | Reads the transmission restriction time. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_DwellStat` | `Wan_DwellStat` | Reads the dwell state. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_LastTxCh` | `Wan_LastTxCh` | Reads the last transmit channel. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_MacCmd` | `Wan_MacCmd` | Reads the retained LoRaWAN MAC command as a hexadecimal string. | [LoRaWAN](lora-basic-manual-wan_en.md) |
| `Wan_BatLevel` | `Wan_BatLevel` | Sets or reads the battery level reported by LRA1 to the LoRaWAN network. This value is not obtained from the server. | [LoRaWAN](lora-basic-manual-wan_en.md) |

## Aliases and Abbreviations

These forms shorten input or preserve compatibility with names used by earlier firmware versions.

| Form | Meaning |
|---|---|
| `?` | Abbreviation for `Print` |
| `#Join`, `#Tx`, etc. | Abbreviated forms of `Wan_Join`, `Wan_Tx`, and other names with the `Wan_` prefix |
| `Wan_AppEui` | Former name of `Wan_JoinEui` used through firmware Ver1.07.f |

`#?` is a separate command for accessing the LoRa transmit and receive buffers; it is not an abbreviation for `Print`.
