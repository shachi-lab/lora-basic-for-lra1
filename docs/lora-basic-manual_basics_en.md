<h1>
LoRa-BASIC for LRA1<br/>
Reference Manual<br/>
Basics
</h1>

English edition translated from the [Japanese original](lora-basic-manual_basics_jp.md). Command, function, and system-variable names are case-insensitive. This manual uses the spelling emitted by List and registered in the source code.

# Overview

The LRA1 module includes LoRa-BASIC, a simplified subset of BASIC.  

LoRa-BASIC is derived from nanoBASIC and extended to control LoRa radio and device-specific functions.  

Its syntax and commands resemble conventional BASIC but differ in some specifications.

This manual describes LoRa-BASIC syntax, basic commands and device-specific functions.  

For LoRa-P2P mode, see ***[LoRa-BASIC for LRA1 Reference Manual: P2P](lora-basic-manual_p2p_en.md)***.
For LoRaWAN mode, see ***[LoRa-BASIC for LRA1 Reference Manual: LoRaWAN](lora-basic-manual-wan_en.md)***.

In this manual, “device” means the LRA1 module.

> **Regional and Regulatory Notice**  
> The radio functions described in this manual are intended for use in Japan. Operation outside Japan, and compliance with radio regulations or certification requirements in other countries or regions, are not supported.

---

# Connections

## Connecting to a PC

Connect the device UART to a PC serial port and control it through text in a serial terminal such as Tera Term. A UART-to-USB adapter may be required.  
The default serial speed is 115,200 bps with no parity.  
This manual uses Tera Term in its examples.  
The following Tera Term settings are recommended.

<table style="width:81%;">
<colgroup>
<col style="width: 29%" />
<col style="width: 22%" />
<col style="width: 29%" />
</colgroup>
<thead>
<tr>
<th>Settings dialog</th>
<th>Item</th>
<th>Value</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="7">Serial port setup and connection</td>
<td>Port</td>
<td>As assigned by the PC</td>
</tr>
<tr>
<td>Speed</td>
<td>115200</td>
</tr>
<tr>
<td>Data</td>
<td>8 bit</td>
</tr>
<tr>
<td>Parity</td>
<td>none</td>
</tr>
<tr>
<td>Stop bits</td>
<td>1 bit</td>
</tr>
<tr>
<td>Flow control</td>
<td>none</td>
</tr>
<tr>
<td>Transmit delay</td>
<td>0 ms/character, 10 ms/line</td>
</tr>
<tr>
<td rowspan="4">Terminal setup</td>
<td>Newline</td>
<td>Receive: CR; transmit: CR</td>
</tr>
<tr>
<td>Local echo</td>
<td>Unchecked</td>
</tr>
<tr>
<td>Kanji (receive)</td>
<td>UTF-8</td>
</tr>
<tr>
<td>Kanji (transmit)</td>
<td>UTF-8</td>
</tr>
</tbody>
</table>

## Connecting to an External MCU

An external MCU controls the device over the serial port in the same way as Tera Term. For LoRa transmission/reception only, configure Recv or Comm to run at startup so that the MCU exchanges only transmit and receive data. See the commands for details.

## Connecting External Peripherals

LoRa-BASIC implements control of the evaluation board's BME280 and LCD peripherals.

Other peripherals are not assumed for these functions.

## External Pin Control

LoRa-BASIC commands can control the device's external pins.  
The table below lists the available pin functions.

<table style="width:85%;">
<colgroup>
<col style="width: 6%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 14%" />
</colgroup>
<thead>
<tr>
<th rowspan="2" style="text-align: center;">Pin number</th>
<th rowspan="2" style="text-align: center;">Name</th>
<th colspan="6" style="text-align: center;">Command functions</th>
<th rowspan="2" style="text-align: center;">SYSTEM</th>
</tr>
<tr>
<th style="text-align: center;">GPIO</th>
<th style="text-align: center;">Adc</th>
<th style="text-align: center;">Pwm</th>
<th style="text-align: center;">UART2</th>
<th style="text-align: center;">I2C</th>
<th style="text-align: center;">SPI</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">1</td>
<td style="text-align: center;">PB02</td>
<td style="text-align: center;">34</td>
<td style="text-align: center;">10</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">MISO</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">2</td>
<td style="text-align: center;">PA05</td>
<td style="text-align: center;">5</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">UART-RX</td>
</tr>
<tr>
<td style="text-align: center;">3</td>
<td style="text-align: center;">PA04</td>
<td style="text-align: center;">4</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">UART-TX</td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td style="text-align: center;">PB03</td>
<td style="text-align: center;">35</td>
<td style="text-align: center;">11</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">5</td>
<td style="text-align: center;">PA27</td>
<td style="text-align: center;">27</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">6</td>
<td style="text-align: center;">PB22</td>
<td style="text-align: center;">54</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">MOSI</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td style="text-align: center;">PA06</td>
<td style="text-align: center;">6</td>
<td style="text-align: center;">6</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">WAKEUP</td>
</tr>
<tr>
<td style="text-align: center;">8</td>
<td style="text-align: center;">PA07</td>
<td style="text-align: center;">7</td>
<td style="text-align: center;">7</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">9</td>
<td style="text-align: center;">PA08</td>
<td style="text-align: center;">8</td>
<td style="text-align: center;">16</td>
<td style="text-align: center;">8 [0]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">10</td>
<td rowspan="2" style="text-align: center;">VDD</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">11</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">12</td>
<td rowspan="2" style="text-align: center;">GND</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">13</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">14</td>
<td style="text-align: center;">PA09</td>
<td style="text-align: center;">9</td>
<td style="text-align: center;">17</td>
<td style="text-align: center;">9 [0]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">15</td>
<td style="text-align: center;">PA13</td>
<td style="text-align: center;">13</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">16</td>
<td style="text-align: center;">PA16</td>
<td style="text-align: center;">16</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">SDA</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">17</td>
<td style="text-align: center;">PA17</td>
<td style="text-align: center;">17</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">SCL</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">18</td>
<td style="text-align: center;">PA18</td>
<td style="text-align: center;">18</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">18 [0]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">LED-TX</td>
</tr>
<tr>
<td style="text-align: center;">19</td>
<td style="text-align: center;">PA28</td>
<td style="text-align: center;">28</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">20</td>
<td style="text-align: center;">PA23</td>
<td style="text-align: center;">23</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">RX</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">21</td>
<td style="text-align: center;">PB23</td>
<td style="text-align: center;">55</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">SCLK</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">22</td>
<td style="text-align: center;">GND</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">23</td>
<td style="text-align: center;">PA15</td>
<td style="text-align: center;">15</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">PA14</td>
<td style="text-align: center;">14</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">ACTIVE</td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">PA19</td>
<td style="text-align: center;">19</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">19 [0]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">LED-RX</td>
</tr>
<tr>
<td style="text-align: center;">26</td>
<td style="text-align: center;">PA22</td>
<td style="text-align: center;">22</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">TX</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">27</td>
<td style="text-align: center;">PA25</td>
<td style="text-align: center;">25</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">25 [1]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">28</td>
<td style="text-align: center;">PA24</td>
<td style="text-align: center;">24</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">24 [1]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">29</td>
<td style="text-align: center;">PA31</td>
<td style="text-align: center;">31</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">30</td>
<td style="text-align: center;">PA30</td>
<td style="text-align: center;">30</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">31</td>
<td style="text-align: center;">PA01</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">1 [2]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">XOUT32</td>
</tr>
<tr>
<td style="text-align: center;">32</td>
<td style="text-align: center;">PA00</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">0 [2]</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">XIN32</td>
</tr>
<tr>
<td style="text-align: center;">33</td>
<td style="text-align: center;">Reset</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">34</td>
<td style="text-align: center;">GND</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
</tbody>
</table>

Numbers in the table are command/function arguments. Pwm [n] identifies the group.  
See the individual commands for details.

# User Interface

## Line Input

The “>” prompt appears when command input is available.

A line consists of serial input up to CR.

Commands and variable names are case-insensitive.

Comments and strings retain case exactly as entered.

A line can contain up to 254 characters. Additional characters are ignored.

### Line editing

The following editing operations are available with Tera Term's default key settings.

<table style="width:93%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 65%" />
<col style="width: 13%" />
</colgroup>
<thead>
<tr>
<th>Input</th>
<th>Behavior</th>
<th>Code</th>
</tr>
</thead>
<tbody>
<tr>
<td>Character</td>
<td>At the end of the line, append the character.<br />
Within the line, insert it immediately before the cursor.</td>
<td>$20～<br />
UTF-8</td>
</tr>
<tr>
<td>「Backspace」</td>
<td>Delete the character before the cursor</td>
<td>$08</td>
</tr>
<tr>
<td>「Delete」</td>
<td>Delete the character at the cursor</td>
<td>$7f</td>
</tr>
<tr>
<td>「Tab」</td>
<td>Same as a space</td>
<td>$09</td>
</tr>
<tr>
<td>「←」</td>
<td>Move left</td>
<td>$1b,[,D</td>
</tr>
<tr>
<td>「→」</td>
<td>Move right</td>
<td>$1b,[,C</td>
</tr>
<tr>
<td>「↑」</td>
<td>Previous history entry</td>
<td>$1b,[,A</td>
</tr>
<tr>
<td>「↓」</td>
<td>Next history entry; if none, the last program line displayed by List</td>
<td>$1b,[,B</td>
</tr>
<tr>
<td>「Home」</td>
<td>Move to the start of the line</td>
<td>$1b,[,~,1</td>
</tr>
<tr>
<td>「End」</td>
<td>Move to the end of the line</td>
<td>$1b,[,~,4</td>
</tr>
<tr>
<td>「Enter」</td>
<td>Complete input (CR)</td>
<td>$0d</td>
</tr>
<tr>
<td>Ctrl-C</td>
<td>Cancel input; character code is configurable</td>
<td>$03</td>
</tr>
</tbody>
</table>

> *Notes*  
> • Character codes \$1F and below cannot be entered as text.  
> • Canceled input and empty lines are not added to history.  
> • History holds up to 10 entries and 256 characters in total.  
> • Cursor movement across display lines is unsupported.
> Editing does not work correctly when input wraps across multiple lines.  
> • Set the terminal type to VT100.
> In Tera Term: [Setup] → [Terminal] → [Terminal ID].  
> • Use UTF-8 character encoding.
> In Tera Term: [Setup] → [Terminal] → [Kanji receive/transmit].  
> • Recalling history discards the current input; it cannot be recovered.

## Interpreter

LoRa-BASIC converts input to intermediate bytecode, then executes commands in sequence or stores the bytecode in the program area.

Bytecode generally requires fewer bytes than the input command text.

The program area holds **8192 bytes** of bytecode in RAM. Reset or power loss clears it, but it can be saved to and loaded from internal Flash memory.

## Operating Modes

The interface has REPL mode and RUN mode.

***REPL mode***

Executes serial input interactively, one command line at a time.

***RUN mode***

Executes the BASIC program in the RAM program area sequentially.

The device starts in REPL mode.

Auto can select a command to run automatically at startup.

## Protected Mode

Protected mode prevents displaying the BASIC program externally.  
It restricts Edit, List and Psave.
Enable protected mode using any of these methods.  
(1) Assign -1 to Pass.  
(2) Save to Flash with Psave \*.  
(3) Load BASE64 code containing a protection-enabled Pass key with Pload &.  

> Notes  
> Methods (1) and (2) require Ssave for protection to persist after reset.  
> Only for method (3), Psave also executes Ssave.

Use “Default -1” to disable protected mode saved by Ssave.  
See the variables and commands above for details.

# Language Specification

## Variables

### Variable types

General-purpose variables:
26 single-letter variables A–Z, case-insensitive.

Array variable: one one-dimensional array @(index), with indices 0–255.  
The index may be an expression. Parentheses “(index)”
may be replaced by square brackets “[index]”.

Special variables: system variables, LoRa variables and other special-purpose variables.
See the individual variable descriptions.

String variables: ^ (caret) and . (dot).  
General-purpose and array variables are signed 32-bit integers.  
All variables are global; there are no local variables.

### Variable initialization

On reset—power-on, hardware, brownout, watchdog, return from Deep or Reset—some special variables are restored from Flash.  
These resets, New and Run initialize general-purpose and array variables to 0 and strings to empty strings of length 0. Bup1 and Bup2 are initialized only on power-on reset.

## Numeric Literals

Only signed 32-bit integer literals are supported.

A minus sign is allowed; a unary plus sign is not.

Numbers are decimal by default. Prefix a value with $ for hexadecimal.

## String Literals

Enclose string literals in double quotes.
UTF-8 is supported.

Strings can be used in string expressions and commands such as Print.

A decoding prefix (%, &, $) before the opening quote decodes the string.

## Expressions and Operators

### Expressions

Expressions combine numeric constants and the operators below.

Arithmetic overflow does not cause an error. Division by zero causes "Division by 0".  
Numbers and strings may be mixed with some restrictions. From the first string onward, the expression is treated as a string expression.

### Arithmetic operators

The following arithmetic operators are available.

| Operator | Operation | Example |
|:------:|:----:|---------------------------------------------------------------|
| ＋ | Addition | A=B+C |
| － | Subtraction | A=B-C |
| ＊ | Multiplication | A=B\*C |
| ＊＊ | Exponentiation | A=B\*\*C — syntactic sugar for Pow(B,C) |
| ／ | Division | A=B/C — C99 behavior; truncate toward zero |
| ／／ | Logarithm | A=B//C — log <sub>C</sub> B = log B/log C; approximate calculation |
| ％ | Remainder | A=B%C — C99 behavior; result has the dividend's sign |

### Unary operators

The following unary operators are available.

| Operator | Operation | Example |
|:------:|:---------------:|----------------|
| － | Negation | A=-B, A=-(B+C) |
| ～ | Bitwise complement | A=~B, A=~(B+C) |
| ！ | Logical NOT | A=!B, A=!(B+C) |

The following operators can appear only as standalone operations immediately after a general-purpose or array variable, Ctrl, Bup1 or Bup2.  
They cannot be used with string, LoRa or system variables, or inside expressions.

| Operator | Operation | Example | Equivalent |
|:------:|:--------------:|--------|-------|
| ＋＋ | Increment | A++ | A=A+1 |
| －－ | Decrement | A-- | A=A-1 |

### Logical operators

The following logical operators are available.

| Operator | Operation | Example |
|:------:|:------------:|-------------------------------|
| ＆＆ | Logical AND | If A=B && A=C Then C=A&&B |
| ｜｜ | Logical OR | If A=B \|\| A=C Then C=A\|\|C |

### Bitwise operators

The following bitwise operators are available.

| Operator | Operation | Example |
|:--:|:--:|----|
| ＆ | Bitwise AND | A=B&C |
| ｜ | Bitwise OR | A=B\|C |
| ^ | Bitwise XOR | A=B^C — distinct from the ^ (caret) string variable |

### Comparison operators

The following comparison operators are available.

| Operator | Operation | Example |
|:--------:|:------:|------------------------------------------------------------|
| =, = = | Equal | If A=B Then A=B==C — distinct from assignment = |
| \<\>, != | Not equal | If A\<\>B Then A=B!=C |
| \< | Less than | If A\<B Then A=B\<C |
| \<= | Less than or equal | If A\<=B Then A=B\<=C |
| \> | Greater than | If A\>C Then A=B\>C |
| \>= | Greater than or equal | If A\>=B Then A=B\>=C |

Comparisons return 1 for true and 0 for false.

### Shift operators

The following shift operators are available.

| Operator | Operation | Example |
|:--:|:--:|----|
| \>\> | Arithmetic right shift | A=B\>\>C — C99 behavior; preserves the MSB sign bit |
| \<\< | Logical left shift | A=B\<\<C — C99 behavior; shifts the MSB sign bit as well |

### Parentheses

( ): parentheses.  
Parenthesized operations take priority; this is distinct from string-to-number conversion.  
Example: A=((B+C)/D)\>(E+1)

### String expressions

Adjacent strings/string variables, or strings joined with a semicolon, concatenate into one string. In Print, commas separate strings with TAB
(ASCII 0x09). A trailing semicolon in Print or Lprint suppresses the newline.

String expressions cannot use parentheses or numeric operators; parentheses cause numeric interpretation.

### Operator precedence

Operators have the following precedence, highest first.

Parentheses > unary > multiplication/division/remainder/exponentiation/logarithm > addition/subtraction >
comparison/shift  
> bitwise/logical. Equal-precedence operations are evaluated left to right
in sequence.

## Assignment

Write “variable =
expression”: a variable name, the assignment operator =, and an expression.

The expression result is assigned to the variable. Unlike conventional BASIC,
there is no LET command.  
The target can be a general-purpose variable (A–Z), array (@), LoRa variable, system variable or string variable.  
LoRa, system and string variables support simple assignment (=) only. General-purpose variables, arrays, Ctrl, Bup1 and Bup2 also support the compound assignments below.

<table style="width:89%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 19%" />
<col style="width: 28%" />
<col style="width: 28%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Operator</th>
<th style="text-align: center;">Operation</th>
<th>Example</th>
<th>Equivalent</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">＝</td>
<td style="text-align: center;">Simple assignment</td>
<td colspan="2">A=B 　
Available for all writable variables.</td>
</tr>
<tr>
<td style="text-align: center;">＋＝</td>
<td style="text-align: center;">Add and assign</td>
<td>A+=B</td>
<td>A=A+B</td>
</tr>
<tr>
<td style="text-align: center;">－＝</td>
<td style="text-align: center;">Subtract and assign</td>
<td>A-=B</td>
<td>A=A-B</td>
</tr>
<tr>
<td style="text-align: center;">＊＝</td>
<td style="text-align: center;">Multiply and assign</td>
<td>A*=B</td>
<td>A=A*B</td>
</tr>
<tr>
<td style="text-align: center;">／＝</td>
<td style="text-align: center;">Divide and assign</td>
<td>A/=B</td>
<td>A=A/B</td>
</tr>
<tr>
<td style="text-align: center;">％＝</td>
<td style="text-align: center;">Remainder and assign</td>
<td>A%=B</td>
<td>A=A%B</td>
</tr>
<tr>
<td style="text-align: center;">｜＝</td>
<td style="text-align: center;">Bitwise OR and assign</td>
<td>A|=B</td>
<td>A=A|B</td>
</tr>
<tr>
<td style="text-align: center;">＆＝</td>
<td style="text-align: center;">Bitwise AND and assign</td>
<td>A&amp;=B</td>
<td>A=A&amp;B</td>
</tr>
<tr>
<td style="text-align: center;">＾＝</td>
<td style="text-align: center;">Bitwise XOR and assign</td>
<td>A^=B</td>
<td>A=A^B</td>
</tr>
<tr>
<td style="text-align: center;">&gt;&gt;＝</td>
<td style="text-align: center;">Arithmetic right shift and assign</td>
<td>A&gt;&gt;=B</td>
<td>A=A&gt;&gt;B</td>
</tr>
<tr>
<td style="text-align: center;">&lt;&lt;＝</td>
<td style="text-align: center;">Logical left shift and assign</td>
<td>A&lt;&lt;=B</td>
<td>A=A&lt;&lt;B</td>
</tr>
</tbody>
</table>

For simple assignment only, the left side may be omitted. The expression is evaluated and its result discarded.

## String and Number Conversion

Strings and numbers can be converted in either direction.

Conversion is automatic according to the expression context.

### Number to string

#### Decimal strings

When assigning to a string variable or passing a string argument such as to Print, a numeric expression result is automatically converted to decimal text.  
Example:
```
 Print 123+123 --> 246  
 ^="@"(1234+\$1234)/10"#" --> ^="@589#"  
```
To specify the width without Form, use 0(expression1,expression2)
to convert expression1 to decimal with the width given by expression2. If expression2 is omitted or 0, the minimum width is used. Positive widths pad with spaces; negative widths pad with zeros.  
Use Form for additional formatting options. See [8.3.5](#form)
[Form](#form).  
Example:
```
 Print 0(123+123) --> 246　:　「Print 123+123 と等価」  
 ^="@"0(5678+123,6)"#"  --> ^="@ 5801#"  
 ^="@"0(5678+123,-6)"#" --> ^="@005801#"
```

Example notes: The first line is equivalent to Print 123+123.

#### Hexadecimal strings

Instead of Form, use \$(expression1,expression2)
to convert expression1 to hexadecimal with the width given by expression2. If omitted or 0, the minimum width is used. Digits A–F are uppercase; use Form for lowercase or other formatting. Positive widths pad with spaces; negative widths pad with zeros.  
Example:
```
 Print \$(123+123) --> F6　:　「Print Form("X",123+123) と等価」  
 ^="@"\$(5678+123,6)"#"  --> ^="@ 16A9#"  
 ^="@"\$(5678+123,-6)"#" --> ^="@0016A9#"
```

Example notes: The first line is equivalent to Print Form("X",123+123).

### String to number

Assigning a string to a non-string variable or numeric command argument, or enclosing a string expression in parentheses, converts it to decimal. A leading $ selects hexadecimal. A nonnumeric string becomes 0; conversion stops at the first nonnumeric character.  
A string expression at the end of an expression does not require parentheses and does not cause an error.

Example:
```
 A=("12";"3";"456")\*100  --> A=1235600  
 B=\$123+"\$Beef0000"     --> B=-1091632861  
```

ToInt converts a string to a number starting at an arbitrary position.

## Multiple Statements

Separate commands with a colon to place multiple statements on a line.

A line is limited to 254 characters, or 254 bytes when converted to bytecode. Exceeding the limit causes an error.

## Line Numbers

Every program line requires a leading line number.  
Line numbers range from 0 to 65535.

Execution follows line-number order.

Input program lines are sorted by line number.

## Labels

Program
branches such as Goto and Gosub can use label strings instead of line numbers.

A label begins with an underscore (_).

A label is valid only immediately after a line number. Labels elsewhere on a line are ignored as targets but do not cause an error.

If a label occurs more than once, the first occurrence in program order is used.

All program lines require numbers; add labels only where needed, such as branch targets.  
Labels can contain uppercase/lowercase letters, digits, underscores and dots. They are case-sensitive and must end with a colon or the end of the line.  
A label in a numeric expression returns its line number, or 0 if not found.

## Comments

A single quote (') begins a comment extending to the end of the line.  
UTF-8 is allowed in comments.  
Comments do not affect execution.

## Interrupting Execution

A serial ***break character or break signal*** interrupts the program or command.  
Resume can continue an interrupted program from the interruption point.

Programs interrupted by errors cannot be resumed.  
  
> #### Break character and break signal  
> BrkCh=0 enables only the break signal. A positive value enables both its ASCII character and the signal. A negative value enables only the character given by its absolute value. The default is 0x03 (Ctrl-C). See BrkCh.  
> LoRa-BASIC recognizes Low lasting at least 10 bit times at the current baud rate as a break signal. The evaluation board's USB-UART chip produces approximately 1 second of Low.  
> This manual uses ***break character/signal*** for these mechanisms.

## Nesting

Gosub–Return, For–Next and Do/While–Loop share a 16-level stack. Deeper nesting causes a stack overflow error.

# BASIC Commands

## Branch Commands

### Goto

Syntax: Goto \[label\]

Jumps to the specified line.

The target may be a line number or label string.

A line-number target may be a literal or an expression containing variables.

### Gosub

Syntax: Gosub \[label\]

Calls the subroutine at the specified line.

Target syntax is the same as Goto.

Return resumes at the command after Gosub.

### Return

Syntax: Return

Returns from a subroutine.

## Conditions and Loops

### If～Then～ElseIf～Then～Else～EndIf

Syntax: If \[expression\] Then … ElseIf \[expression\] Then … Else … EndIf

If the expression is nonzero (true), executes the commands after Then.

If it is 0 (false), tests ElseIf, or executes Else if there is no ElseIf.

ElseIf and Else are optional. Then and EndIf are required.

A line number or label immediately after Then or Else behaves as Goto
to that target.

### For～To～Step～Next

Syntax: For \[variable=expression1\] To \[expression2\] Step \[expression3\] … Next

Assigns expression1 to the variable, then loops through Next until the variable reaches expression2.

Each Next adds expression3 to the variable.

Step expression3 is optional and defaults to 1.  
Only general-purpose variables, array variables, Bup1 and Bup2 are allowed as the loop variable; other variables and expressions are not.

### Do～Loop

Syntax: Do … Loop

Repeats the commands between Do and Loop.

### Do～Loop While

Syntax: Do … Loop While \[expression\]

Executes the body before testing the expression, and repeats Do–Loop
While the result is nonzero (true).

### While～Loop

Syntax: While \[expression\] … Loop

Repeats the body while the expression is nonzero (true).

### Exit

Syntax: Exit

Exits For–Next or Do/While–Loop and continues immediately after Next or Loop (Loop
While).

In nested loops, affects only the innermost loop.

### Continue

Syntax: Continue

Skips the remaining body of a For–Next or Do/While–Loop iteration and continues the loop.

In nested loops, affects only the innermost loop.

## Input/Output Commands

### Print、？

Syntax: Print \[expression\] or ?\[expression\]

Outputs numbers and strings. ? is an alias for Print.  
Accepts one or more expressions or strings. Numeric results are converted to strings.  
Separate multiple items with commas or semicolons.  
A comma inserts a tab; a semicolon concatenates output.  
A trailing semicolon suppresses the newline.  
Expressions may include strings, Chr, Form and similar functions.  
A single quote (') in a Print expression emits the clear-screen escape sequence.
Example: Print '.  
See String Expressions for details.

### Input

Syntax: Input \[variable\]

Waits for serial input and stores the entered number in the variable.

A leading $ selects hexadecimal input.

A $ suffix on the variable stores the ASCII code of the first input character instead.  
Only general-purpose variables, array variables, Bup1 and Bup2 are allowed; other variables and expressions are not.

### Outp

Syntax: Outp \[expression1\], \[expression2\]

Sets GPIO states individually or in a group.  
**Setting one port**  
Expression1 specifies the port number. Only the ports below are available.

<table style="width:39%;">
<colgroup>
<col style="width: 10%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">Port number</th>
<th rowspan="2" style="text-align: center;">Port name</th>
<th rowspan="2" style="text-align: center;">Pin</th>
</tr>
<tr>
<th style="text-align: center;">Decimal</th>
<th style="text-align: center;">Hexadecimal</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0</td>
<td style="text-align: center;">$00</td>
<td style="text-align: center;">PA00</td>
<td style="text-align: center;">Pin.32</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
<td style="text-align: center;">$01</td>
<td style="text-align: center;">PA01</td>
<td style="text-align: center;">Pin.31</td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td style="text-align: center;">$04</td>
<td style="text-align: center;">PA04</td>
<td style="text-align: center;">Pin.3</td>
</tr>
<tr>
<td style="text-align: center;">5</td>
<td style="text-align: center;">$05</td>
<td style="text-align: center;">PA05</td>
<td style="text-align: center;">Pin.2</td>
</tr>
<tr>
<td style="text-align: center;">6</td>
<td style="text-align: center;">$06</td>
<td style="text-align: center;">PA06</td>
<td style="text-align: center;">Pin.7</td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td style="text-align: center;">$07</td>
<td style="text-align: center;">PA07</td>
<td style="text-align: center;">Pin.8</td>
</tr>
<tr>
<td style="text-align: center;">8</td>
<td style="text-align: center;">$08</td>
<td style="text-align: center;">PA08</td>
<td style="text-align: center;">Pin.9</td>
</tr>
<tr>
<td style="text-align: center;">9</td>
<td style="text-align: center;">$09</td>
<td style="text-align: center;">PA09</td>
<td style="text-align: center;">Pin.14</td>
</tr>
<tr>
<td style="text-align: center;">13</td>
<td style="text-align: center;">$0D</td>
<td style="text-align: center;">PA13</td>
<td style="text-align: center;">Pin.15</td>
</tr>
<tr>
<td style="text-align: center;">14</td>
<td style="text-align: center;">$0E</td>
<td style="text-align: center;">PA14</td>
<td style="text-align: center;">Pin.24</td>
</tr>
<tr>
<td style="text-align: center;">15</td>
<td style="text-align: center;">$0F</td>
<td style="text-align: center;">PA15</td>
<td style="text-align: center;">Pin.23</td>
</tr>
<tr>
<td style="text-align: center;">16</td>
<td style="text-align: center;">$10</td>
<td style="text-align: center;">PA16</td>
<td style="text-align: center;">Pin.16</td>
</tr>
<tr>
<td style="text-align: center;">17</td>
<td style="text-align: center;">$11</td>
<td style="text-align: center;">PA17</td>
<td style="text-align: center;">Pin.17</td>
</tr>
</tbody>
</table>

<table style="width:39%;">
<colgroup>
<col style="width: 10%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">Port number</th>
<th rowspan="2" style="text-align: center;">Port name</th>
<th rowspan="2" style="text-align: center;">Pin</th>
</tr>
<tr>
<th style="text-align: center;">Decimal</th>
<th style="text-align: center;">Hexadecimal</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">18</td>
<td style="text-align: center;">$12</td>
<td style="text-align: center;">PA18</td>
<td style="text-align: center;">Pin.18</td>
</tr>
<tr>
<td style="text-align: center;">19</td>
<td style="text-align: center;">$13</td>
<td style="text-align: center;">PA19</td>
<td style="text-align: center;">Pin.25</td>
</tr>
<tr>
<td style="text-align: center;">22</td>
<td style="text-align: center;">$16</td>
<td style="text-align: center;">PA22</td>
<td style="text-align: center;">Pin.26</td>
</tr>
<tr>
<td style="text-align: center;">23</td>
<td style="text-align: center;">$17</td>
<td style="text-align: center;">PA23</td>
<td style="text-align: center;">Pin.20</td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">$18</td>
<td style="text-align: center;">PA24</td>
<td style="text-align: center;">Pin.28</td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">$19</td>
<td style="text-align: center;">PA25</td>
<td style="text-align: center;">Pin.27</td>
</tr>
<tr>
<td style="text-align: center;">27</td>
<td style="text-align: center;">$1B</td>
<td style="text-align: center;">PA27</td>
<td style="text-align: center;">Pin.5</td>
</tr>
<tr>
<td style="text-align: center;">28</td>
<td style="text-align: center;">$1C</td>
<td style="text-align: center;">PA28</td>
<td style="text-align: center;">Pin.19</td>
</tr>
<tr>
<td style="text-align: center;">30</td>
<td style="text-align: center;">$1E</td>
<td style="text-align: center;">PA30</td>
<td style="text-align: center;">Pin.29</td>
</tr>
<tr>
<td style="text-align: center;">31</td>
<td style="text-align: center;">$1F</td>
<td style="text-align: center;">PA31</td>
<td style="text-align: center;">Pin.30</td>
</tr>
<tr>
<td style="text-align: center;">34</td>
<td style="text-align: center;">$22</td>
<td style="text-align: center;">PB02</td>
<td style="text-align: center;">Pin.1</td>
</tr>
<tr>
<td style="text-align: center;">35</td>
<td style="text-align: center;">$23</td>
<td style="text-align: center;">PB03</td>
<td style="text-align: center;">Pin.4</td>
</tr>
<tr>
<td style="text-align: center;">54</td>
<td style="text-align: center;">$36</td>
<td style="text-align: center;">PB22</td>
<td style="text-align: center;">Pin.6</td>
</tr>
<tr>
<td style="text-align: center;">55</td>
<td style="text-align: center;">$37</td>
<td style="text-align: center;">PB23</td>
<td style="text-align: center;">Pin.21</td>
</tr>
</tbody>
</table>

Expression2 specifies the port state (Level).

| Level | Port state |
|-----------|-----------------------|
| \> 0 | Output High |
| 0 | Output Low |
| -1, \< -3 | High impedance |
| -2 | Input with pull-up |
| -3 | Input with pull-down |

**Setting multiple ports**  
Set the MSB (\$80000000) of expression1 to 1
to set multiple ports together.  
Set each desired port's bit to 1 as listed below.

<table style="width:98%;">
<colgroup>
<col style="width: 7%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
<col style="width: 5%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Bit</th>
<th style="text-align: center;">31</th>
<th style="text-align: center;">30</th>
<th style="text-align: center;">29</th>
<th style="text-align: center;">28</th>
<th style="text-align: center;">27</th>
<th style="text-align: center;">26</th>
<th style="text-align: center;">25</th>
<th style="text-align: center;">24</th>
<th style="text-align: center;">23</th>
<th style="text-align: center;">22</th>
<th style="text-align: center;">21</th>
<th style="text-align: center;">20</th>
<th style="text-align: center;">19</th>
<th style="text-align: center;">18</th>
<th style="text-align: center;">17</th>
<th style="text-align: center;">16</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">Port</td>
<td rowspan="2">‘1’</td>
<td colspan="4" style="text-align: center;"></td>
<td colspan="4" style="text-align: center;">PB</td>
<td colspan="7" style="text-align: center;">PA</td>
</tr>
<tr>
<td style="text-align: center;">x</td>
<td style="text-align: center;">x</td>
<td style="text-align: center;">x</td>
<td style="text-align: center;">x</td>
<td style="text-align: center;">23</td>
<td style="text-align: center;">22</td>
<td style="text-align: center;">03</td>
<td style="text-align: center;">02</td>
<td style="text-align: center;">31</td>
<td style="text-align: center;">30</td>
<td style="text-align: center;">28</td>
<td style="text-align: center;">27</td>
<td style="text-align: center;">25</td>
<td style="text-align: center;">24</td>
<td style="text-align: center;">23</td>
</tr>
<tr>
<td style="text-align: center;">Bit</td>
<td style="text-align: center;">15</td>
<td style="text-align: center;">14</td>
<td style="text-align: center;">13</td>
<td style="text-align: center;">12</td>
<td style="text-align: center;">11</td>
<td style="text-align: center;">10</td>
<td style="text-align: center;">9</td>
<td style="text-align: center;">8</td>
<td style="text-align: center;">7</td>
<td style="text-align: center;">6</td>
<td style="text-align: center;">5</td>
<td style="text-align: center;">4</td>
<td style="text-align: center;">3</td>
<td style="text-align: center;">2</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">0</td>
</tr>
<tr>
<td rowspan="2" style="text-align: center;">Port</td>
<td colspan="16" style="text-align: center;">PA</td>
</tr>
<tr>
<td style="text-align: center;">22</td>
<td style="text-align: center;">19</td>
<td style="text-align: center;">18</td>
<td style="text-align: center;">17</td>
<td style="text-align: center;">16</td>
<td style="text-align: center;">15</td>
<td style="text-align: center;">14</td>
<td style="text-align: center;">13</td>
<td style="text-align: center;">09</td>
<td style="text-align: center;">08</td>
<td style="text-align: center;">07</td>
<td style="text-align: center;">06</td>
<td style="text-align: center;">05</td>
<td style="text-align: center;">04</td>
<td style="text-align: center;">01</td>
<td style="text-align: center;">00</td>
</tr>
</tbody>
</table>

Port-state values are the same as for individual setting.

> Note:  
> In [4.4](#external-pin-control)
> [External Pin Control](#external-pin-control), ports shared with LED/UART/UART2/I2C/SPI/XTAL32 and other functions are overridden by Outp even while those functions are active. Check the hardware configuration before using these ports.  
> Avoid using system ports such as PA04, PA05, PA06 and PA14 where possible.  
> In particular, Outp on PA04 or PA05 disables UART operation.  
> PA18 and PA19 change during LoRa transmission/reception regardless of Outp. Ctrl can disable this behavior.

Example:
```
 >Outp 8,1  
 OK  
 >Outp 9,0  
 OK  
 >Outp 9,-1  
 OK  
 >Outp \$80000060,1  
 OK
```

### Pwm

Syntax: Pwm \[expression1\], \[expression2\], \[expression3\]

Outputs Pwm on a port.

Expression1 specifies the output port; only the listed ports are allowed.  
This argument is required.

Expression2 specifies period. Omitting it or specifying 0 or less stops Pwm.  
Frequency (Hz) = 2MHz/(period + 1)

Expression3 specifies the High duration (width). Specifying width also requires period.  
Duty (%) = width / (period + 1)  
If width is omitted or negative, width = (period + 1)/2 (Duty=50%).

<table style="width:92%;">
<colgroup>
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 8%" />
<col style="width: 9%" />
<col style="width: 20%" />
<col style="width: 23%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">Port number</th>
<th rowspan="2" style="text-align: center;">Port name</th>
<th rowspan="2" style="text-align: center;">Pin</th>
<th rowspan="2" style="text-align: center;">Group</th>
<th rowspan="2">period/width<br />
range</th>
<th rowspan="2">Notes</th>
</tr>
<tr>
<th style="text-align: center;">Decimal</th>
<th style="text-align: center;">Hexadecimal</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">8</td>
<td style="text-align: center;">$08</td>
<td style="text-align: center;">PA08</td>
<td style="text-align: center;">Pin.9</td>
<td rowspan="4" style="text-align: center;">0</td>
<td rowspan="6"><p>0～16777215</p>
<p>(24bit ： $00FFFFFF)</p></td>
<td rowspan="2"></td>
</tr>
<tr>
<td style="text-align: center;">9</td>
<td style="text-align: center;">$09</td>
<td style="text-align: center;">PA09</td>
<td style="text-align: center;">Pin.14</td>
</tr>
<tr>
<td style="text-align: center;">18</td>
<td style="text-align: center;">$12</td>
<td style="text-align: center;">PA18</td>
<td style="text-align: center;">Pin.18</td>
<td>Shared with transmit LED</td>
</tr>
<tr>
<td style="text-align: center;">19</td>
<td style="text-align: center;">$13</td>
<td style="text-align: center;">PA19</td>
<td style="text-align: center;">Pin.25</td>
<td>Shared with receive LED</td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">$18</td>
<td style="text-align: center;">PA24</td>
<td style="text-align: center;">Pin.28</td>
<td rowspan="2" style="text-align: center;">1</td>
<td rowspan="2"></td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">$19</td>
<td style="text-align: center;">PA25</td>
<td style="text-align: center;">Pin.27</td>
</tr>
<tr>
<td style="text-align: center;">0</td>
<td style="text-align: center;">$00</td>
<td style="text-align: center;">PA00</td>
<td style="text-align: center;">Pin.32</td>
<td rowspan="2" style="text-align: center;">2</td>
<td rowspan="2"><p>0～65535</p>
<p>(16bit ： $0000FFFF)</p></td>
<td rowspan="2">Shared with external XTAL32k pins</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
<td style="text-align: center;">$01</td>
<td style="text-align: center;">PA01</td>
<td style="text-align: center;">Pin.31</td>
</tr>
</tbody>
</table>

Ports in a group share period. The most recently assigned period applies to every port in that group.  
Allowed period and width ranges depend on the port. Out-of-range values cause no error but do not produce correct Pwm.  
After Pwm stops, the port remains in GPIO output mode.

> Notes:  
> - Outp or Inp on an active Pwm port stops Pwm and takes control.
> To resume Pwm, first stop it with Pwm, then configure Pwm output again.
> A Pwm command without first stopping Pwm does not update the port.  
> - PA18 and PA19 are shared with the transmit/receive LEDs.  
> LED activation during traffic stops Pwm on these ports.  
> Ctrl can disable the transmit/receive LEDs.  
> - PA00 and PA01 are shared with the external XTAL32k.  
> Do not use Pwm on these ports when an XTAL32k crystal is connected.

Example
```
 >Pwm 8,5000  
 OK  
 >Pwm 8  
 OK  
 >Pwm 18,10000,1000
```

---

## Execution Control Commands

### End

Syntax: End

Ends program execution and returns to REPL input.

Execution cannot be resumed with Resume.  
An endless-loop program need not end with End. In a main-program/subroutine layout, End at the end of the main program is required to prevent falling through into a following subroutine.

### Stop

Syntax: Stop

Interrupts program execution.  
Behaves like a break character/signal.

Resume continues at the command after Stop.

### Resume

Syntax: Resume

Resumes execution interrupted by Stop or a serial break character/signal.

Execution interrupted by an error cannot be resumed.  
Interrupted REPL execution cannot be resumed.

### Pause

Syntax: Pause

Waits for one character from the serial port.

A break character/signal interrupts the command.  
During program execution, this also interrupts the BASIC program; Resume can continue it.

### Delay

Syntax: Delay time

time ：
Pauses for the specified time in 1/1000-second units, then resumes.  
time accepts 0–2,147,483,647
(31 bits=\$7FFFFFFF). Negative or out-of-range values cause an error.

A serial break character/signal interrupts Delay.  
During program execution, this also interrupts the BASIC program; Resume can continue it.

### Sleep

Syntax: Sleep \[time\] \[,mode\]

Waits in a low-power mode.

time selects the following behavior.

<table style="width:95%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 27%" />
<col style="width: 55%" />
</colgroup>
<thead>
<tr>
<th>time</th>
<th>Timed wakeup</th>
<th>PA06 wakeup condition</th>
</tr>
</thead>
<tbody>
<tr>
<td>&gt; ０</td>
<td rowspan="2">After the absolute value of time, in seconds</td>
<td>mode=0 or omitted: PA06 disabled; mode≠0: selected by mode</td>
</tr>
<tr>
<td>&lt; ０</td>
<td rowspan="2">Selected by mode</td>
</tr>
<tr>
<td>= ０<br />
omitted</td>
<td>No time limit (indefinite)</td>
</tr>
</tbody>
</table>

The serial port is disabled during Sleep, so a break character/signal does not wake the device.  
When PA06 wakeup is enabled, Sleep forces PA06 to input regardless of Inp or Outp. Do not use PA06 for another purpose in that case. Sleep also forces PA14 to output High. Ctrl can change PA14 polarity or disable this control.  
time accepts -2,097,151 to 2,097,151. Other values cause no error but may behave incorrectly.  
  
mode specifies the PA06 wakeup condition and defaults to 0.

<table style="width:93%;">
<colgroup>
<col style="width: 12%" />
<col style="width: 16%" />
<col style="width: 43%" />
<col style="width: 20%" />
</colgroup>
<thead>
<tr>
<th>mode</th>
<th>Wakeup condition</th>
<th>Description</th>
<th>Pin pull-up/down</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">０</td>
<td rowspan="2">Follows Ctrl $200</td>
<td>Ctrl $200=0: rising edge</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>Ctrl $200=1: falling edge</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>１</td>
<td>Rising edge</td>
<td>Low-to-High transition</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>２</td>
<td>Falling edge</td>
<td>High-to-Low transition</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>３</td>
<td>Both edges</td>
<td>Low-to-High and High-to-Low transitions</td>
<td>None</td>
</tr>
<tr>
<td>４</td>
<td>High level</td>
<td>Detect High</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>５</td>
<td>Low level</td>
<td>Detect Low</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>Other</td>
<td colspan="3">Error</td>
</tr>
</tbody>
</table>

For mode 4 or 5 (High/Low level), Sleep returns immediately if PA06 is already at the wakeup level.  
  
Port states during Sleep are listed below.

<table style="width:93%;">
<colgroup>
<col style="width: 12%" />
<col style="width: 13%" />
<col style="width: 20%" />
<col style="width: 45%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Port name</th>
<th>Name</th>
<th>State during Sleep</th>
<th>Notes</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">PA06</td>
<td>WAKEUP</td>
<td>Input (weak pull-down)</td>
<td>Only when selected as a wakeup source.<br />
Weak pull-up when Ctrl selects a rising edge.</td>
</tr>
<tr>
<td style="text-align: center;">PA14</td>
<td>ACTIVE</td>
<td>Output High</td>
<td>Ctrl can select output Low or disable control</td>
</tr>
<tr>
<td style="text-align: center;">PA04</td>
<td>UART-TX</td>
<td>Hi-Z ※1</td>
<td rowspan="2">Retains state after Inp/Outp<br />
Returns to UART function on wakeup.</td>
</tr>
<tr>
<td style="text-align: center;">PA05</td>
<td>UART-RX</td>
<td>Hi-Z ※2</td>
</tr>
<tr>
<td style="text-align: center;">PA22</td>
<td>UART2-TX</td>
<td>Hi-Z ※１</td>
<td rowspan="2">Only when used as UART2.<br />
Otherwise, or after Inp/Outp, retains state.</td>
</tr>
<tr>
<td style="text-align: center;">PA23</td>
<td>UART2-RX</td>
<td>Hi-Z ※２</td>
</tr>
<tr>
<td style="text-align: center;">PA16</td>
<td>I2C-SDA</td>
<td>Hi-Z</td>
<td rowspan="2">Only when used as I2C.<br />
Otherwise retains state.</td>
</tr>
<tr>
<td style="text-align: center;">PA17</td>
<td>I2C-SCL</td>
<td>Hi-Z</td>
</tr>
<tr>
<td style="text-align: center;">Other</td>
<td></td>
<td>State retained</td>
<td></td>
</tr>
</tbody>
</table>

> Note 1: weak pull-up if Baud/Ubaud enables transmit-port pull-up.  
> Note 2: weak pull-down if Baud/Ubaud enables receive-port pull-down.  
   
Sleep uses the following clocks.

| Xtal32k | Ctrl | Clock | Accuracy | Power |
|-------------|------------|--------------------|----------------|----------|
| 0 | \$00000000 | Internal low-power 32 kHz | -3.0 to +3.9% | Ultra-low |
| 0 | \$00000040 | Internal 32 kHz | -0.6 to +0.8% | Low |
| 1 | N/A | External 32 kHz | Depends on device | Low |

External 32 kHz is available only with an Xtal32k connected to the external pins.  
  
> Note:  
> Low-power operation in Sleep/Deep is sensitive to external electrical noise. Depending on the environment, the device may fail to wake or exhibit other problems.  
> To reduce external interference, do not leave unused ports Hi-Z. Use Outp/Inp to select output or pull-up/down, or connect them to GND or VDD in hardware.

At startup, ports are Hi-Z except those used by the system.

### Deep

Syntax: Deep \[time\] \[,mode\]

Waits in ultra-low-power mode (Deep Sleep).  
time selects the following behavior.

<table style="width:95%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 27%" />
<col style="width: 55%" />
</colgroup>
<thead>
<tr>
<th>time</th>
<th>Timed wakeup</th>
<th>PA06 wakeup condition</th>
</tr>
</thead>
<tbody>
<tr>
<td>&gt; ０</td>
<td rowspan="2">After the absolute value of time, in seconds</td>
<td>mode=0 or omitted: PA06 disabled; mode≠0: selected by mode</td>
</tr>
<tr>
<td>&lt; ０</td>
<td rowspan="2">Selected by mode</td>
</tr>
<tr>
<td>= ０<br />
omitted</td>
<td>No time limit (indefinite)</td>
</tr>
</tbody>
</table>

mode selects the PA06 wakeup condition and defaults to 0.

<table style="width:93%;">
<colgroup>
<col style="width: 12%" />
<col style="width: 16%" />
<col style="width: 43%" />
<col style="width: 20%" />
</colgroup>
<thead>
<tr>
<th>mode</th>
<th>Wakeup condition</th>
<th>Description</th>
<th>Pin pull-up/down</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">０、３</td>
<td rowspan="2">Follows Ctrl $200</td>
<td>Ctrl $200=0: detect High level</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>Ctrl $200=1: detect Low level</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>１、４</td>
<td>High level</td>
<td>Detect High</td>
<td>Pull-Down</td>
</tr>
<tr>
<td>２、５</td>
<td>Low level</td>
<td>Detect Low</td>
<td>Pull-Up</td>
</tr>
<tr>
<td>Other</td>
<td colspan="3">Error</td>
</tr>
</tbody>
</table>

Differences from Sleep:  
- Deep Sleep reduces current below Sleep.
- Wakeup performs a system reset.  
- RAM contents are erased, not retained.  
- PA06 detects a level; edge triggering is unavailable.  
- If PA06 is already at the wakeup level, Deep returns immediately through a system reset.  
- The internal 32K clock is unavailable; external 32K or internal low-power 32K can be used.  
 
Other behavior is the same as Sleep.  
   
> #### Power consumption  
> Sleep resumes operation immediately. Deep restarts through reset and waits approximately 6 seconds before BASIC autostart, consuming additional energy.  
> Depending on the sleep duration, Sleep may therefore use less total energy.  
>
> The autostart wait draws approximately IDLE current, giving this break-even calculation:  
> {0.7 mA (IDLE current) × 6 sec} / {1.8 uA (Sleep current) − 0.6 uA (Deep current)}  
> 　＝ 4,200 uAsec / 1.2uA ＝ 3,500 sec ＝ 58.3 min ≒ 1 hour  
>
> Deep is more efficient for approximately 1 hour or longer; Sleep is more efficient for less than 1 hour.
> This estimate excludes peripheral circuitry and port states.  
   
Deep uses the following clocks.

| Xtal32k | Ctrl | Clock | Accuracy | Power |
|-------------|----------|--------------------|----------------|----------|
| 0 | N/A | Internal low-power 32 kHz | -3.0 to +3.9% | Ultra-low |
| 1 | N/A | External 32 kHz | Depends on device | Low |

> External 32 kHz is available only with Xtal32k connected to the external pins.

### Reset

Syntax: Reset

Performs a system reset.

### Catch

Syntax: Catch \[label\]

Defines an error handler.  
An error during BASIC program execution transfers control to the specified label.  
The target may be a label string or a line number, including a numeric expression.  
A target of 0 cancels the handler.  
Catch is disabled on entry to the handler. An error within it therefore terminates the program. Execute Catch again to reinstall handling, for example before returning to normal processing.  
Error gives the error code; Error# gives the error line. See Error Messages.

Ignored in REPL mode.  
Break characters/signals are also caught. If the handler does not stop for a Break error, the program cannot be stopped manually.

Example:
```
 >>10 Catch 60
 >>20 Do
 >>30 Print "Val=";:Input A
 >>40 @\[256/A\]=A
 >>50 Loop
 >>60 If Error=3 Then Print "Over !":Goto 50 EndIf
 >>70 Error#=Error
 >>run
 Val=3
 Val=2
 Val=1
 Over !
 Val=0
 Division by 0 error in 40  
 OK
```

## Data Commands

### Data

Syntax: Data \[expression1\], \[expression2\], …

Defines values for Read. Expressions, including function calls, are allowed.

Specify one or more comma-separated expressions. Consecutive or trailing commas imply 0.  
Data may appear anywhere in the program, independently of Read's location.  
See the Restore example.

### Read

Syntax: Read \[variable\]

Reads the numeric result of a Data expression into the variable.  
Only general-purpose variables, arrays, Bup1 and Bup2 are allowed; other special variables are not.

Successive Read operations consume Data in program order from the beginning.

Reading past the end of Data causes an error.  
An error in the Data expression being read reports the Read command's line number.  
See the Restore example.

### Restore

Syntax: Restore \[label\]

Changes the Data position used by Read.  
With no target or target 0, selects the first Data in the program.  
With a specified label,
selects the nearest Data following that label or line number.

Example
```
 >>10 Data A+1,3,I+J
 >>20 For J=1 To 2
 >>30 For I=1 To 3:Read A:Print A,:Next
 >>40 Restore
 >>50 Next
 >>Run
 1 3 4 5 3 5 OK
```

### Aload

Syntax: Aload

Loads array variables @(0–255) previously saved with Asave.  
Overwrites all array variables.

### Asave

Syntax: Asave

Saves all array variables @(0–255) to Flash, overwriting the stored array.  
Use Aload to restore them.  
Saved values survive reset and power loss.

## Other Commands

### Randomize

Syntax: Randomize \[expression\]

Sets the random seed.

The expression seeds Rnd().
The default argument is 0.

The seed also incorporates the device ID, so the same argument produces device-specific seeds. Startup behaves as Randomize
0.

### Swap

Syntax: Swap \[variable1\], \[variable2\]

Exchanges the two variable values.  
Only general-purpose variables, arrays, Bup1 and Bup2 are allowed; other variables and expressions are not.

### Ver

Syntax: Ver

Displays the software version.

## Program Management Commands

### Edit

Syntax: Edit \[expression\]

Enables or disables program entry.

An omitted or zero argument disables program entry.

A nonzero argument enables program entry.

Program entry is disabled at startup.  
In protected mode selected by Pass, Edit fails with “Permission error”.

#### Program entry disabled

The REPL prompt is “\>”.

Entering a line beginning with a number causes an error instead of writing a program line. New, Renum, Delete and Psave also cause errors.

This prevents accidental numeric input from altering or erasing the program.

#### Program entry enabled

The REPL prompt is “\>\>”.

A line beginning with a number is written as a program line.

Indentation and unnecessary spaces are ignored. Case matters only in comments, labels and strings. After a program line is stored, the next prompt appears without “OK”.

Each line is converted to bytecode. Wait for the prompt before sending the next line. Syntax or conversion errors display a message and leave that line unwritten.

Program lines are stored in line-number order.

An existing line with the same number is replaced. Entering only a line number deletes that line.

### New

Syntax: New

Clears the program and variables. Causes an error when program entry is disabled.

Deletes the entire RAM program and initializes variables to 0.  
Special variables such as LoRa settings are retained.  

### Renum

Syntax: Renum \[expression\]

Renumbers program lines. Causes an error when program entry is disabled.

Starts at the specified number and increments by 10. The optional argument defaults to 10.

Only line numbers change; Goto and Gosub targets remain unchanged.

### Delete

Syntax: Delete \[\[label1\], \[label2\]\]

Deletes program lines. Causes an error when program entry is disabled.

Deletes the range from label1 through label2.  
A single label1 without a comma selects only that line.  
A comma after label1 makes it the start of a range.  
Omitting label1 before the comma selects the program's beginning.  
label2 selects the final line of the range.  
Omitting label2 selects the program's end.

Numeric range endpoints need not correspond to existing program lines.  
Both labels cannot be omitted.

### Pload

#### Loading from Flash

Syntax: Pload \[!\[label\]\]

Loads the saved Flash program into the program area.  
Overwrites the RAM program.  
If no program is stored in Flash, “Program not exist
error” occurs.

“Pload！”
loads from Flash and then runs the program. A target label can be specified as with Run.

#### Restoring from a BASE64 string

Syntax: Pload& \[+\]

“Pload&” restores from BASE64 text produced by “Psave
&”, rather than from Flash.

“Pload &” waits for input. Enter the successive lines produced by “Psave
&”.  
The leading single quote in Psave& output may be removed.  
Enter consecutive newlines to finish input and restore the program, replacing the current program.  
Pass key and AesKey must match those used for Psave&. Mismatched keys prevent restoration.  
After restoring a program with a nonzero Pass key, Psave is allowed regardless of protected mode or program-entry state.  
“Pload&+” appends the restored program to the current program. It does not sort line numbers or resolve duplicate numbers.  
During program execution, only append mode is allowed.

#### Downloading over LoRa

Syntax: Pload% \[+\] \[expression1\]

“Pload%” downloads a program over LoRa from another device running “Psave%”.  
It waits for LoRa reception. Execute Psave% on the other device. Download progress is displayed; completion ends the command.  
Expression1 specifies a timeout in seconds. If omitted, there is no timeout. The download must finish within the timeout; too short a timeout prevents completion.  
If AesKey is enabled, transfer is encrypted and must use the sender's matching AesKey. Different keys prevent communication and cause an error.  
“Pload%+” appends the downloaded program without sorting line numbers or resolving duplicate numbers.  
During program execution, only append mode is allowed.  
Notes:  
Unavailable in LoRaWAN mode; causes “Invalid mode error”.  
Both ends must have matching Ch and Gid, and the receiver's Own must match the sender's Dst.  
Sf＝７、Bw＝125KHz、Cr=4/5
are fixed. Ordinary Recv cannot perform this transfer.  
Radio conditions may interrupt or stop a download. Before timeout, the sender can restart it by executing Psave% again.  
Radio certification limits prevent continuous transmission and may lengthen intervals between blocks, especially on higher channels.

### Psave

#### Saving to Flash

Syntax: Psave \[*\] \[expression\]

Saves the RAM program to Flash. Causes an error when program entry is disabled.

Flash holds only one program.

The saved program survives power loss and reset.  
An omitted or zero argument saves the program. If RAM contains no program, “Program
not exist error” occurs.  
A negative argument erases the saved Flash program.  
An argument of 1 or greater causes “Parameter error”.

“Psave*”
enables protected mode after saving to Flash.
”Psave:Pass=-1”
performs the equivalent operation; execute Ssave to retain protection.  
If Pload& restored a program with a nonzero Pass key, Psave is allowed regardless of protection or program-entry state. Only with a negative Pass key does Psave also execute Ssave.

#### Saving as BASE64 text

Syntax: Psave& \[\[label1\], \[label2\]\]

“Psave&” outputs the program as multiple BASE64-encoded lines instead of saving to Flash. Leading single quotes may be removed.  
label1 and label2 select the output range by line number or label, using List syntax.  
Encryption incorporates random data, so repeated Psave& operations on the same program produce different strings.  
Encryption also uses the Pass key and, if enabled, AesKey. Matching keys are required to restore with Pload&.  
“Psave &”
can run regardless of whether program entry is disabled.

#### Uploading over LoRa

Syntax: Psave% \[\[label1\], \[label2\]\]

“Psave%” uploads a program over LoRa to another device running “Pload%”.  
label1 and label2 select the upload range by line number or label, using List syntax.  
Start Pload% on the destination before Psave% starts transmission.  
Upload progress is displayed. The command ends when upload completes.  
Each program block waits for a receiver response before the next block is sent. Missing responses trigger retries; an error response displays an error and ends the command.  
“Psave%”
can run regardless of whether program entry is disabled.

See Pload% for additional notes.  
  
With Pass protected mode enabled, “Permission
error” occurs and Psave cannot execute.

### Run

Syntax: Run \[label\]

Executes the program in the program area.  

Without a target, starts at the beginning.

With a target, starts at the specified line number or label.

Initializes variables to 0 first, except special variables such as LoRa settings. General-purpose and array variables are also retained if Ctrl has \$40 set.

Execution enters RUN mode.  
When execution ends, the device returns to REPL mode.


### List

Syntax: List \[\[label1\], \[label2\]\]

Displays the program range from label1 through label2 in line-number order.  
A single label1 without a comma selects only that line.  
A comma after label1 makes it the start of a range.  
Omitting label1 before the comma selects the program's beginning.  
label2 selects the final line of the range.  
Omitting label2 selects the program's end.

Numeric range endpoints need not correspond to existing program lines.  
If both are omitted, displays the entire program.

Example:
```
 List        : 全プログラム  
 List 10     : 行番号10のみ  
 List 10,    : 行番号10からプログラムの最後まで  
 List ,20    : プログラムの先頭から行番号20まで  
 List 10,20  : 行番号10から行番号20までを表示
```

Example notes: The five forms display the entire program; line 10 only; line 10 through the end; the start through line 20; and lines 10 through 20, respectively.
With Pass protected mode enabled, List causes “Permission error”.

---

# BASIC Functions

## General Functions

General functions mainly perform numeric calculations.

### Rnd

Syntax: Rnd(expression)  
For argument 0, returns a random value from 0 to 2,147,483,647 (\$7FFF FFFF).  
Otherwise returns a nonnegative random value not exceeding the argument's absolute value.  
Example:
```
 A=Rnd(B) ← 0≦A≦\|B\| の乱数
```

Example notes: A is a random value in the range 0 ≤ A ≤ |B|.

### Abs

Syntax: Abs(expression)

Returns the absolute value.  
Example:
```
 A=Abs(B) ← A=\|B\|
```

### Atan2

Syntax: Atan2(expression1, expression2)

Returns the arctangent of expression2/expression1.  
The angle is between −π and +π radians, returned as an integer scaled by 1000000.  
Divide by 17453 (1000000π/180)
to convert to degrees.  
If argument1 is 0, returns 0 without “Division by 0 error”
being raised.  
Unlike the usual arctangent range −π/2 to +π/2, this function resolves the angle correctly when expression1 is negative.  
Example: A=Atan2(B,C) ← A=tan<sup>-1</sup>(C/B)

### Sqrt

Syntax: Sqrt(expression)

Returns the square root of expression1. A negative argument returns 2,147,483,647 (\$7FFF FFFF)
as the result.  
Example:
```
 A=Sqrt(B) ← A=√B
```

### Pow

Syntax: Pow(expression1, expression2)

Raises expression1 to expression2. If expression2 is 0 or less, returns 1.  
The ** operator performs the same exponentiation.  
Example:
```
 A=Pow(B,C) または A=B\*\*C ← A=B<sup>C</sup>
```

Example notes: Either expression calculates B raised to the power C.

### Int8

Syntax: Int8(expression)

Uses only the low 8 bits of the expression, treating all higher bits as 0, and returns the value interpreted as a signed 8-bit integer.  
Low 8-bit values 0–127 are unchanged; 128–255 return -128 to -1.

### Int16

Syntax: Int16(expression)

Uses only the low 16 bits of the expression, treating all higher bits as 0, and returns the value interpreted as a signed 16-bit integer.  
Low 16-bit values 0–32,767 are unchanged; 32,768–65,535 return -32,768 to -1.

### Int24

Syntax: Int24(expression)

Uses only the low 24 bits of the expression, treating all higher bits as 0, and returns the value interpreted as a signed 24-bit integer.  
Low 24-bit values 0–8,388,607 are unchanged; 8,388,608–16,777,215 return -8,388,608 to -1.

### Bswap16

Syntax: Bswap16(expression)

Swaps the high and low bytes within each 16-bit half of a 32-bit value.  
Useful for endian conversion.  
Example:
```
 A=Bswap16(\$12345678) ← A=\$34127856 となります
```

Example notes: The annotation gives A=$34127856 as the result, as in the Japanese source.

### Bswap32

Syntax: Bswap32(expression)

Reverses the byte order of a 32-bit value.  
Useful for endian conversion.  
Example:
```
 A=Bswap32(\$12345678) ← A=\$78563412 となります
```

Example notes: The result is A=$78563412.

---

## Input Functions

Input functions read digital or analog values from device GPIO ports.

### Inp

Syntax: Inp(expression)

Reads a GPIO port state.  
Specify a port number from the table; other ports are unavailable.  
Returns 0 for Low or 1 for High.

<table style="width:39%;">
<colgroup>
<col style="width: 8%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 11%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">Port number</th>
<th rowspan="2" style="text-align: center;">Port name</th>
<th rowspan="2" style="text-align: center;">Pin</th>
</tr>
<tr>
<th style="text-align: center;">Decimal</th>
<th style="text-align: center;">Hexadecimal</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0</td>
<td style="text-align: center;">$00</td>
<td style="text-align: center;">PA00</td>
<td style="text-align: center;">Pin.32</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
<td style="text-align: center;">$01</td>
<td style="text-align: center;">PA01</td>
<td style="text-align: center;">Pin.31</td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td style="text-align: center;">$04</td>
<td style="text-align: center;">PA04</td>
<td style="text-align: center;">Pin.3</td>
</tr>
<tr>
<td style="text-align: center;">5</td>
<td style="text-align: center;">$05</td>
<td style="text-align: center;">PA05</td>
<td style="text-align: center;">Pin.2</td>
</tr>
<tr>
<td style="text-align: center;">6</td>
<td style="text-align: center;">$06</td>
<td style="text-align: center;">PA06</td>
<td style="text-align: center;">Pin.7</td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td style="text-align: center;">$07</td>
<td style="text-align: center;">PA07</td>
<td style="text-align: center;">Pin.8</td>
</tr>
<tr>
<td style="text-align: center;">8</td>
<td style="text-align: center;">$08</td>
<td style="text-align: center;">PA08</td>
<td style="text-align: center;">Pin.9</td>
</tr>
<tr>
<td style="text-align: center;">9</td>
<td style="text-align: center;">$09</td>
<td style="text-align: center;">PA09</td>
<td style="text-align: center;">Pin.14</td>
</tr>
<tr>
<td style="text-align: center;">13</td>
<td style="text-align: center;">$0D</td>
<td style="text-align: center;">PA13</td>
<td style="text-align: center;">Pin.15</td>
</tr>
<tr>
<td style="text-align: center;">14</td>
<td style="text-align: center;">$0E</td>
<td style="text-align: center;">PA14</td>
<td style="text-align: center;">Pin.24</td>
</tr>
<tr>
<td style="text-align: center;">15</td>
<td style="text-align: center;">$0F</td>
<td style="text-align: center;">PA15</td>
<td style="text-align: center;">Pin.23</td>
</tr>
<tr>
<td style="text-align: center;">16</td>
<td style="text-align: center;">$10</td>
<td style="text-align: center;">PA16</td>
<td style="text-align: center;">Pin.16</td>
</tr>
<tr>
<td style="text-align: center;">17</td>
<td style="text-align: center;">$11</td>
<td style="text-align: center;">PA17</td>
<td style="text-align: center;">Pin.17</td>
</tr>
</tbody>
</table>

<table style="width:39%;">
<colgroup>
<col style="width: 8%" />
<col style="width: 9%" />
<col style="width: 9%" />
<col style="width: 11%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">Port number</th>
<th rowspan="2" style="text-align: center;">Port name</th>
<th rowspan="2" style="text-align: center;">Pin</th>
</tr>
<tr>
<th style="text-align: center;">Decimal</th>
<th style="text-align: center;">Hexadecimal</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">18</td>
<td style="text-align: center;">$12</td>
<td style="text-align: center;">PA18</td>
<td style="text-align: center;">Pin.18</td>
</tr>
<tr>
<td style="text-align: center;">19</td>
<td style="text-align: center;">$13</td>
<td style="text-align: center;">PA19</td>
<td style="text-align: center;">Pin.25</td>
</tr>
<tr>
<td style="text-align: center;">22</td>
<td style="text-align: center;">$16</td>
<td style="text-align: center;">PA22</td>
<td style="text-align: center;">Pin.26</td>
</tr>
<tr>
<td style="text-align: center;">23</td>
<td style="text-align: center;">$17</td>
<td style="text-align: center;">PA23</td>
<td style="text-align: center;">Pin.20</td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">$18</td>
<td style="text-align: center;">PA24</td>
<td style="text-align: center;">Pin.28</td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">$19</td>
<td style="text-align: center;">PA25</td>
<td style="text-align: center;">Pin.27</td>
</tr>
<tr>
<td style="text-align: center;">27</td>
<td style="text-align: center;">$1B</td>
<td style="text-align: center;">PA27</td>
<td style="text-align: center;">Pin.5</td>
</tr>
<tr>
<td style="text-align: center;">28</td>
<td style="text-align: center;">$1C</td>
<td style="text-align: center;">PA28</td>
<td style="text-align: center;">Pin.19</td>
</tr>
<tr>
<td style="text-align: center;">30</td>
<td style="text-align: center;">$1E</td>
<td style="text-align: center;">PA30</td>
<td style="text-align: center;">Pin.29</td>
</tr>
<tr>
<td style="text-align: center;">31</td>
<td style="text-align: center;">$1F</td>
<td style="text-align: center;">PA31</td>
<td style="text-align: center;">Pin.30</td>
</tr>
<tr>
<td style="text-align: center;">34</td>
<td style="text-align: center;">$22</td>
<td style="text-align: center;">PB02</td>
<td style="text-align: center;">Pin.1</td>
</tr>
<tr>
<td style="text-align: center;">35</td>
<td style="text-align: center;">$23</td>
<td style="text-align: center;">PB03</td>
<td style="text-align: center;">Pin.4</td>
</tr>
<tr>
<td style="text-align: center;">54</td>
<td style="text-align: center;">$36</td>
<td style="text-align: center;">PB22</td>
<td style="text-align: center;">Pin.6</td>
</tr>
<tr>
<td style="text-align: center;">55</td>
<td style="text-align: center;">$37</td>
<td style="text-align: center;">PB23</td>
<td style="text-align: center;">Pin.21</td>
</tr>
</tbody>
</table>

Add the following value to the port number to select pull-up/down.

<table style="width:67%;">
<colgroup>
<col style="width: 44%" />
<col style="width: 11%" />
<col style="width: 11%" />
</colgroup>
<thead>
<tr>
<th rowspan="2">Port state</th>
<th colspan="2" style="text-align: center;">Value to add</th>
</tr>
<tr>
<th style="text-align: center;">Hexadecimal</th>
<th style="text-align: center;">Decimal</th>
</tr>
</thead>
<tbody>
<tr>
<td>High impedance (no pull-up/down)</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">0</td>
</tr>
<tr>
<td>Pull-down</td>
<td style="text-align: center;">$100</td>
<td style="text-align: center;">256</td>
</tr>
<tr>
<td>Pull-up</td>
<td style="text-align: center;">$200</td>
<td style="text-align: center;">512</td>
</tr>
</tbody>
</table>

> Note:  
> Inp overrides ports shared with LED/UART/UART2/I2C/SPI/XTAL32 and other functions in [4.4 External Pin Control](#external-pin-control), even while those functions are active. Check the hardware configuration.  
> Avoid using system ports such as PA04, PA05, PA06 and PA14 where possible.  
> In particular, Inp on PA04 or PA05 disables UART control.  
> PA18 and PA19 become outputs during LoRa traffic; Ctrl can disable this.

### Adc

Syntax: Adc(expression)

Reads an Adc conversion result.

Specify an Adc number from the table. Other values cause an error.

<table style="width:91%;">
<colgroup>
<col style="width: 7%" />
<col style="width: 6%" />
<col style="width: 8%" />
<col style="width: 13%" />
<col style="width: 16%" />
<col style="width: 19%" />
<col style="width: 18%" />
</colgroup>
<thead>
<tr>
<th colspan="2" style="text-align: center;">Adc number</th>
<th rowspan="2">Name</th>
<th colspan="2">Input source</th>
<th rowspan="2">Return value</th>
<th rowspan="2">Reference</th>
</tr>
<tr>
<th style="text-align: center;">Decimal</th>
<th style="text-align: center;">Hexadecimal</th>
<th>GPIO</th>
<th>Pin</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">6</td>
<td style="text-align: center;">$06</td>
<td>AN6</td>
<td>PA06</td>
<td>Pin.7</td>
<td rowspan="8"><p>0～4095</p>
<p>Returns the raw Adc conversion result.</p></td>
<td rowspan="8">VDD</td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td style="text-align: center;">$07</td>
<td>AN7</td>
<td>PA07</td>
<td>Pin.8</td>
</tr>
<tr>
<td style="text-align: center;">10</td>
<td style="text-align: center;">$0A</td>
<td>AN10</td>
<td>PB02</td>
<td>Pin.1</td>
</tr>
<tr>
<td style="text-align: center;">11</td>
<td style="text-align: center;">$0B</td>
<td>AN11</td>
<td>PB03</td>
<td>Pin.4</td>
</tr>
<tr>
<td style="text-align: center;">16</td>
<td style="text-align: center;">$10</td>
<td>AN16</td>
<td>PA08</td>
<td>Pin.9</td>
</tr>
<tr>
<td style="text-align: center;">17</td>
<td style="text-align: center;">$11</td>
<td>AN17</td>
<td>PA09</td>
<td>Pin.14</td>
</tr>
<tr>
<td style="text-align: center;">24</td>
<td style="text-align: center;">$18</td>
<td>AN24</td>
<td colspan="2">CPU internal temperature sensor reading</td>
</tr>
<tr>
<td style="text-align: center;">25</td>
<td style="text-align: center;">$19</td>
<td>AN25</td>
<td colspan="2">Internal reference voltage (1.00 V)</td>
</tr>
<tr>
<td style="text-align: center;">26</td>
<td style="text-align: center;">$1A</td>
<td>AN26</td>
<td colspan="2">CPU core voltage (typical: 0.9–1.2 V)</td>
<td rowspan="2">Measured value in 1 mV units</td>
<td rowspan="2">Internal reference<br />
(1.00V)</td>
</tr>
<tr>
<td style="text-align: center;">27</td>
<td style="text-align: center;">$1B</td>
<td>AN27</td>
<td colspan="2">VDD voltage</td>
</tr>
<tr>
<td style="text-align: center;">28</td>
<td style="text-align: center;">$1C</td>
<td>AN28</td>
<td colspan="2">Converted CPU internal temperature</td>
<td>Temperature in 0.1°C units</td>
<td></td>
</tr>
</tbody>
</table>

> Notes:  
> Inp port numbers and Adc numbers differ even for the same GPIO pin.  
  
#### GPIO pins (AN6–17)
For a GPIO input source, Adc configures the port as an analog input.  
Calculate its voltage as follows.  
Port voltage [mV] = Adc(n) \* Adc(27) / 4095, where n=6,7,10,11,16,17.

#### CPU internal temperature sensor (AN24,28)
AN24 varies between devices even at the same CPU temperature.  
It measures internal chip temperature, not ambient temperature.  
AN28 is calculated from AN24 and AN27 using MPU-specific calibration values programmed by the chip manufacturer. It also differs from ambient temperature.  
If necessary, correct errors using actual temperature measurements in the operating environment.  
The internal temperature sensor has the following measurement errors, from the MPU datasheet.

| Condition | Minimum | Maximum | Unit |
|----------|--------|--------|------|
| T \< 0℃  | -12    | 14     | ℃    |
| T \> 0℃  | -8     | 9      | ℃    |
| T \> 30℃ | -5     | 5      | ℃    |

### Inkey

Syntax: Inkey, Inkey(expression)

Returns the ASCII code of a character from the serial UART input buffer.  
The expression specifies a timeout in msec.  
If omitted or 0, returns immediately without waiting.

A negative argument waits indefinitely for input.  
Returns -1 if the buffer is empty at timeout.  
Previously buffered input, such as characters entered while BASIC runs, is read in sequence.

Reading a character removes it from the buffer.  
No echo is generated.

---

## String Functions

String functions operate on strings.  
LoRa-BASIC uses integer numeric values and provides limited string support.  
Strings are available with specific commands and variables.  
Print, Lprint, Send, and variables such as TDX and Rxd accept strings as well as numbers.

Some variables are primarily string-based.  

### Chr

Syntax: Chr(expression)

Inserts the low 8 bits directly into the string.  
Use it to emit a specific character code in Print or similar commands.

### WChr

Syntax: WChr(expression)

Inserts the low 16 bits in little-endian order into the string.

Available in Print and string construction.

Useful for WORD-sized (2-byte) binary transmit data.

### LChr

Syntax: LChr(expression)

Inserts the low 32 bits in little-endian order into the string.  
Available in Print and string construction.

Useful for LONG
WORD-sized (4-byte) binary transmit data.

### MChr

Syntax: MChr(expression)

Inserts the low 24 bits in little-endian order into the string.  
Available in Print and string construction.

Useful for MIDDLE
WORD-sized (3-byte) binary transmit data.

### Form

Syntax: Form(format-string, expression)

Formats the numeric expression as a string.

The format string accepts the following specifiers.

D: decimal (default).  
X: hexadecimal (0–9, A–F).  
x: hexadecimal (0–9, a–f).  
0: leading-zero padding; otherwise pad with spaces.  
1–9: output width; otherwise variable width.  
-: add one leading position for the sign.  
+: show a plus sign for positive numbers.  
With no specifier, use variable-width decimal.  
  
Shorthand alternatives are 0(expression1,expression2) for decimal and $(expression1,expression2) for hexadecimal. See [6.6.1](#number-to-string) [Number to String](#number-to-string).

Example:
```  
 Print Form("",-1234)      --> "-1234" を表示  
 Print Form("X04", 1234)   --> "04D2" を表示  
 Lprint Form("-05", 1234)  --> "01234" をLCD表示  
 Send Form("-5",-1234)     --> "- 1234" をLoRaで送信  
 Txd=Form("-+5",1234)      --> "+ 1234" をLoRa送信バッファに設定
```

Example notes: The examples display "-1234" and "04D2", display "01234" on the LCD, send "- 1234" over LoRa, and store "+ 1234" in the LoRa transmit buffer, respectively.

### Gets

Syntax: Gets, Gets(expression)

Reads one line from the serial UART input buffer.  
CR or LF terminates the line; the completed line is returned as a string.  
Control codes \$00–\$1F, including CR/LF, are excluded.  
The expression specifies a timeout in msec.  
If the argument is omitted or 0, waits indefinitely for a complete line.  
If the line is incomplete at timeout, returns an empty string even if some characters were entered.  
The maximum length is 255 bytes; additional input is discarded.  
UART input is echoed according to Echo.  
   
#### ***Framed input***
Gets! selects framed input and waits for data enclosed by STX(02) and ETX(03)
without including those delimiters in the returned string.  
Only in Gets! framed mode, a break character cannot interrupt the program. A break signal remains effective.

### Datetime

Syntax: Datetime(expression)  
In a string expression, the GPS Datetime command converts serial seconds to text.  
The format is “YY/MM/DD hh:mm/ss”.

Obtain serial seconds from TimeVal or Clock.

Example:
 ```
 >^=Datetime(116858096):? “20“^  
 2023/09/14 12:34:56
```

> For command usage rather than string usage, see Datetime under GPS Commands.

### Bcc

Syntax: Bcc(string)  
Calculates the string's longitudinal parity.  
Returns the XOR of all 8-bit characters.

Example:
```
>? $(Bcc("ABCDEFG"))  
40
```

### Sum

Syntax: Sum(string)  
Calculates the string's sum.  
Returns the sum of all 8-bit characters.

Example:
 ```
 > ? $(Sum("ABCDEFG"))  
 1DC
```

### ToInt

Syntax: ToInt(string \[, \[offset\] \[, \[length\]\]\])  
Converts a selected part of a string to a number.  
offset selects the starting position; omitted or 0 means the beginning.  
length selects the character count; omitted or 0 means through the end.  
Negative offset or length causes an error.  
A leading $ in the selected text means hexadecimal; otherwise decimal. An offset beyond the string or nonnumeric text returns 0. Conversion stops before the first nonnumeric character. Decimal text may begin with - or +.

Example:
```
 >? ToInt("ABC-1234",3,4)  
 -123
```

## Date and Time Functions

These functions process dates and times.

### TimeVal

Syntax: TimeVal(expression)  
Converts date (year, month, day) and time (hour, minute, second) array values to serial seconds.  
The expression is the first array index (0–249) containing the date/time.  
Serial seconds count from 2020/1/1, 00:00:00.  
Array field order follows Datetime.  
Input outside Datetime's converted-value ranges does not produce a correct conversion.

# String Variables

The general-purpose string variables are ^ (caret) and .
(dot).  
They behave differently.  
Assigning a numeric variable to a string variable converts the number to text.  
Assigning a string to a numeric variable converts it to a number. In a mixed expression, the portion from the first string onward is treated as a string expression; the preceding portion is numeric. Nonnumeric text becomes 0.  
Parenthesizing a string expression converts it to numeric form, allowing it within a numeric expression.

Example:
```
 A=100+”1234”;”567”;^    <-- OK  
 A=”1234”+100            <-- Error  
 A=(“1234”;”567”)+100+^  <-- OK
```

## ^ (Caret) String Variable

Stores a temporary string.  
Can be assigned and read using string expressions as in Print.  
It is distinct from numeric XOR. Use semicolons when explicit separation is needed.
Example: Print A;^;B.  
The caret string also has the following special functions.

#### Decoding the ^ string

Prefix ^ (caret)
with $, % or & to decode the string.  
Example: Print \$^

#### Character codes in ^

Parentheses after ^, as with an array,
read the character code at the specified position.  
Position 0 is the first character. A position beyond the current length returns -1.  

Example:
```
 A=^[1]  
```
A character code can also be assigned at a position. Assignment at or beyond the current length causes an error.  

Example:
```
 ^[1]=A
```

#### Length of ^

Omitting the position in the character-code access syntax
reads the length of ^.  

Example:
```
> L=^[]
```

The length can also be assigned. Extending it fills the added portion with spaces.

Example:
```
 ^[]=10  
```

A negative length deletes that many characters from the beginning.

Example:
```
 ^[]=-3
```

Example:
```
 >B=10203  
 OK  
 >^=B“XYZ“  
 OK  
 >Print ^[1],^[8]  
 48 -1  
 OK  
 >Print ^  
 10203XYZ  
 OK  
 >^=Form(“X“,^[1]+32)  
 OK  
 >^=^^  
 OK  
 >Print ^  
 5050  
 OK  
 >A=B+^“40“  
 OK  
 >Print A  
 515243  
 OK  
 >^=A  
 OK  
 >^=“\[“$Gets“]“  
 123  
 OK  
 >Txd=^  
 OK  
 >Send  
 OK  
 >Print Txd  
 [QRC123]  
 OK
```

## . (Dot) String Variable

Refers directly to the last-used string, not a copy. Within a string expression, the content generated so far remains valid, but the length becomes that of the last-used string and the reference position returns to its beginning.  
After Print with a newline, referring to the string through .
also includes CR/LF.

```
 >Print “XYZ”  
 XYZ  
 OK  
 >Print “A”.;
 AYZ  
 OK  
 >^=.”123”Chr(40)  
 OK  
 >Print .;  
 AYZ  
 123(OK  
 >#=.  
 OK  
 >Print Txd(7)  
 9  
 OK  
 >Send  
 OK
```

## Other String Variables

Transmit/receive buffers Txd, Rxd and #
act as strings when accessed without an index; assignment is allowed only for Txd.  
Auto is also a string variable.

# System Variables and Constants

BASIC reserves certain variables and constants for the system.  
Except for string values, they can be used like ordinary variables/numbers in expressions and command arguments.

## System Constants

System constants do not change during operation, though some differ between devices.
They are read-only and cannot be assigned.  
**System constants**

<table style="width:93%;">
<colgroup>
<col style="width: 15%" />
<col style="width: 18%" />
<col style="width: 59%" />
</colgroup>
<thead>
<tr>
<th>Constant</th>
<th>Description</th>
<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
<td>Null</td>
<td>Null</td>
<td>Numeric 0, or an empty string of length 0 in string contexts</td>
</tr>
<tr>
<td>False</td>
<td>False</td>
<td>Equivalent to 0</td>
</tr>
<tr>
<td>True</td>
<td>True</td>
<td>Equivalent to 1</td>
</tr>
<tr>
<td>Sn</td>
<td>Serial number</td>
<td>Device-specific serial number.<br />
Matches the number on the device label.</td>
</tr>
<tr>
<td>DevId</td>
<td>Device-specific ID string</td>
<td>An IEEE EUI-64 generated from Sn, unique to the device.<br />
The upper 36-bit OUI-36 is “70-B3-D5-59-E”, officially assigned by IEEE to the hardware manufacturer.<br />
In Print and other string contexts, returns 16 hexadecimal characters. In numeric contexts such as assignment, returns the low 20 bits of DevId, which differ from Sn.</td>
</tr>
</tbody>
</table>

## System Variables

Each system variable has a specific function.  
Read, write, save and restore capabilities vary by variable.  
**System variables**

<table style="width:96%;">
<colgroup>
<col style="width: 18%" />
<col style="width: 33%" />
<col style="width: 15%" />
<col style="width: 16%" />
<col style="width: 12%" />
</colgroup>
<thead>
<tr>
<th>Variable</th>
<th>Description</th>
<th style="text-align: center;">Read/write<br />
(✕ means read-only)</th>
<th style="text-align: center;">Save/restore<br />
(Ssave/Sload)</th>
<th style="text-align: center;">Default</th>
</tr>
</thead>
<tbody>
<tr>
<td>Echo</td>
<td>Input echo</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">1</td>
</tr>
<tr>
<td>Auto</td>
<td>Autostart string</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">Empty string</td>
</tr>
<tr>
<td>Baud</td>
<td>Baud rate</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">115,200</td>
</tr>
<tr>
<td>BrkCh</td>
<td>Break character/signal</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">3 (Ctrl-C)</td>
</tr>
<tr>
<td>Xtal32k</td>
<td>Low-frequency clock selection</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>Hop</td>
<td>Forwarding count</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">0</td>
</tr>
<tr>
<td>FwdConf</td>
<td>Forwarding parameters</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">-1</td>
</tr>
<tr>
<td>Tick</td>
<td>System counter</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>Clock</td>
<td>RTC clock</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>Milisec</td>
<td>RTC milliseconds</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>Cause</td>
<td>Startup cause</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>_ (underscore)</td>
<td>Current line number</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>Inkey</td>
<td>Input character</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>Gets</td>
<td>Line input</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>BUP１、２</td>
<td>Backup variables</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>Ctrl</td>
<td>Control variable</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">0</td>
</tr>
<tr>
<td>Error</td>
<td>Error code</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>Pass</td>
<td>Configuration level</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">△</td>
<td style="text-align: center;">０</td>
</tr>
</tbody>
</table>

Individual variables are described below.

### Echo

Controls echo of serial input characters.

- = 0: echo disabled.  
- ≠ 0: echo enabled.  

Unrecognized control characters are not echoed even when echo is enabled.

### Auto

Sets the BASIC command string executed at startup.  
Multiple statements are allowed as in normal command input. Maximum length is 63 characters.  
Auto can be read as a string in Print and similar commands.

When Auto is set, the system normally waits 5 seconds after startup before executing it. Auto execution can be canceled with BREAK during this delay.  
When `$04000000` is set in *Ctrl*, the delay is skipped and Auto is executed immediately only when the system restarts after wakeup from Deep. This avoids wasting power during the 5-second startup delay in low-power Deep operation. The conventional 5-second delay still applies after power-on and resets other than wakeup from Deep.

### Baud

Sets the serial baud rate.

The following values are supported.  
> 300, 600, 1200, 1800, 2400, 3600, 4800, 7200, 9600, 14400, 19200,
> 28800, 38400, 57600, 76800, 115200  

A negative value stops the serial port.  
Example: “Baud=-1”.  
Port states follow those during Sleep.

Add the following values to configure parity, character bits, stop bits and receive-port state.

<table style="width:85%;">
<colgroup>
<col style="width: 18%" />
<col style="width: 12%" />
<col style="width: 13%" />
<col style="width: 13%" />
<col style="width: 26%" />
</colgroup>
<thead>
<tr>
<th rowspan="2">Item</th>
<th colspan="2">Value to add</th>
<th rowspan="2">Description</th>
<th rowspan="2">Notes</th>
</tr>
<tr>
<th>Decimal</th>
<th>Hexadecimal</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="3">Parity</td>
<td>０、３</td>
<td>＄００、＄０３</td>
<td>No parity</td>
<td rowspan="3"></td>
</tr>
<tr>
<td>１</td>
<td>＄０１</td>
<td>Odd parity</td>
</tr>
<tr>
<td>２</td>
<td>＄０２</td>
<td>Even parity</td>
</tr>
<tr>
<td rowspan="2">Stop bits</td>
<td>０</td>
<td>＄００</td>
<td>1 bit</td>
<td rowspan="2"></td>
</tr>
<tr>
<td>４</td>
<td>＄０４</td>
<td>2 bits</td>
</tr>
<tr>
<td rowspan="2">Character bits</td>
<td>０</td>
<td>＄００</td>
<td>8 bits</td>
<td rowspan="2"></td>
</tr>
<tr>
<td>８</td>
<td>＄０８</td>
<td>7 bits</td>
</tr>
<tr>
<td rowspan="2">Receive port</td>
<td>０</td>
<td>＄００</td>
<td>Hi-Z</td>
<td rowspan="2">Ctrl settings also apply,<br />
including during Sleep/Deep.</td>
</tr>
<tr>
<td>１６</td>
<td>＄１０</td>
<td>Pull-Down</td>
</tr>
<tr>
<td rowspan="2">Transmit port</td>
<td>０</td>
<td>＄００</td>
<td>Hi-Z</td>
<td rowspan="2">State during Sleep/Deep</td>
</tr>
<tr>
<td>３２</td>
<td>＄２０</td>
<td>Pull-Up</td>
</tr>
</tbody>
</table>

Example:
```
 ”Baud=326” （300bps, 7bit, parity-even）」
```

Baud takes effect immediately; “OK” is output at the new baud rate.  
”Baud=9600：Ssave”
as a multiple-statement command is recommended.  

> Notes:  
> Resetting Baud with Default does not immediately change the port speed. Save with Ssave and restart to apply it.  
> A receive-port setting of 0 does not override Ctrl's receive pull-down setting.  
> The transmit-port setting specifies its state during Sleep/Deep.  
> Outp settings on transmit/receive ports persist until Sleep/Deep ends.

### BrkCh

Selects the ASCII break character and break-signal behavior used to interrupt execution.

Range: -255 to 255.

<table style="width:50%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 23%" />
<col style="width: 12%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">BrkCh</th>
<th>Break character</th>
<th style="text-align: left;">Break signal</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">＝０</td>
<td>Disabled</td>
<td rowspan="2" style="text-align: left;">Enabled</td>
</tr>
<tr>
<td style="text-align: center;">＞０</td>
<td rowspan="2" style="text-align: left;">ASCII code of the absolute value</td>
</tr>
<tr>
<td style="text-align: center;">＜０</td>
<td style="text-align: left;">Disabled</td>
</tr>
</tbody>
</table>

Note:

PA05 (UART-RX) has no internal pull-up. Without an external pull-up, a disconnected or unstable input may be recognized as an unintended break signal and interrupt BASIC. Set BrkCh negative to disable the signal, or pull up the receive port in hardware or through Baud.

### Xtal32k

Selects one of the following 32.768 kHz clocks.  
=0: Use the internal RC oscillator (factory setting).  
=1: Use an external crystal.

Other values are ignored.  
Default does not reset this setting unless its argument is -1.  
Set this to 1 (external) only when a 32.768kHz crystal is connected to the external pins. Setting 1 without a crystal causes an error and may hang the device, depending on the hardware state.  
For details, see the ***[LoRa-BASIC for LRA1 Reference Manual: LoRaWAN](lora-basic-manual-wan_en.md)***.  
For crystal connections, see the LRA1 Hardware Manual.

### Hop

Gets or sets the number of forwarding attempts when forwarding is enabled.

Valid values are 0–7.  
For details, see [14.8 P2P Forwarding](lora-basic-manual_p2p_en.md#p2p-forwarding).

### FwdConf

Gets or sets the transmission parameters used when forwarding is enabled.  
For details, see [14.8 P2P Forwarding](lora-basic-manual_p2p_en.md#p2p-forwarding).

### Tick

A counter incremented every 1/1000 second after system startup.  
It is cleared to 0 on reset, power-on, or wakeup from Deep.  
Tick continues counting during Sleep.  
The value can be read and written.

### Clock

A counter incremented every second after system startup.  
It is cleared to 0 on power-on. It continues counting across resets and wakeup from Deep.  
Clock continues counting during Deep.  
Writing Clock sets Milisec to 0. Milisec cannot be assigned an arbitrary value.  
LoRaWAN time acquisition using Wan_TimeReq
automatically sets Clock. Only in this case is Milisec set to the millisecond component of the acquired time.  
For details, see the ***[LoRa-BASIC for LRA1 Reference Manual: LoRaWAN](lora-basic-manual-wan_en.md)***.

### Milisec

Returns the millisecond component (0–999) captured by the most recent read of Clock.  
Read Clock before reading Milisec.  
This value is read-only, except when set by LoRaWAN Wan_TimeReq.  
Like Clock, it continues counting across resets and wakeup from Deep.

> Notes  
>Repeated reads of Clock do not produce consecutive Milisec values.  
>The same value is returned for approximately 4msec, while Clock continues counting.

### Cause

A 32-bit value indicating the cause of system startup, as listed below.  
This value is read-only.

The lower 16 bits indicate the reset cause.

| Lower 16 bits | Reset cause |
|--------------|-----------------------------------------------------|
| \$----0001   | Power On Reset                                      |
| \$----0002 | Core voltage brownout reset (1.2V or lower) |
| \$----0004 | Supply voltage brownout reset (1.6V or lower) |
| \$----0010 | External reset signal (Reset pin) |
| \$----0020 | Watchdog reset |
| \$----0040 | Reset command (including inactivity reset after a BASIC error) |
| \$----0180 | Deep WAKEUP pin signal |
| \$----0280 | Deep timeout |

The upper 16 bits indicate the cause of wakeup from Sleep.

| Upper 16 bits | Sleep wakeup cause |
|--------------|----------------------|
| \$0001---- | Sleep WAKEUP pin signal |
| \$0002---- | Sleep timeout |

### _ (underscore)

When used alone:  
Returns the current line number, or 0 in REPL mode.  
It can also be used as a branch target for commands such as Goto.  
This value is read-only.  
  
When used as a label:  
An underscore followed by text is treated as a normal label and returns that label's line number.  
Returns 0 if the label does not exist in the program.  
This value is read-only.

### Inkey

Reads a character from the serial port (UART) input buffer.  
Equivalent to calling Inkey with a timeout of 0.  
Inkey ⇔ Inkey(0)  
See Inkey under input functions.

### Gets

Reads a string from the serial port (UART) input buffer.  
Equivalent to calling the Gets string function with a timeout of 0.  
Gets ⇔ Gets(0)  
See Gets under string functions.

### Bup1、Bup2

Bup1 and Bup2 are two independent variables whose values are retained across resets.  
Bup1 and Bup2 are not initialized by a reset; they retain their values from immediately before the reset.  
Their values become undefined when power is removed, so they are cleared to 0 on a power-on reset.

These signed 32-bit variables can be used like general-purpose variables.  
They support compound assignment and unary increment and decrement.  
They can be specified as variables in For, Read, Input, Swap, and Bme commands.

### Ctrl

Sets control options.

Range: unrestricted (32 bits).  
Controls system behavior and transmit/receive commands.  
The functions and their bit assignments are listed below. LoRaWAN-related entries are shown in gray.

<table style="width:95%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 80%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;"><strong>Value (hexadecimal)</strong></th>
<th><strong>Function</strong></th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0000 0001</td>
<td>Suppress Rssi in Recv output</td>
</tr>
<tr>
<td style="text-align: center;">0000 0002</td>
<td>Suppress source ID in Recv output</td>
</tr>
<tr>
<td style="text-align: center;">0000 0004</td>
<td>Suppress the leading @ in Recv output</td>
</tr>
<tr>
<td style="text-align: center;">0000 0008</td>
<td>Display CRC errors in Recv</td>
</tr>
<tr>
<td style="text-align: center;">0000 0010</td>
<td>Suppress version and other startup output</td>
</tr>
<tr>
<td style="text-align: center;">0000 0020</td>
<td>Disable LED control (PA18, PA19)</td>
</tr>
<tr>
<td style="text-align: center;">0000 0040</td>
<td>Use the internal 32K clock for Sleep instead of the internal low-power 32K clock</td>
</tr>
<tr>
<td style="text-align: center;">0000 0080</td>
<td>Reset after 180 seconds of inactivity following a BASIC execution error</td>
</tr>
<tr>
<td style="text-align: center;">0000 0100</td>
<td>Comm transmits only; no reception</td>
</tr>
<tr>
<td style="text-align: center;">0000 0200</td>
<td>Invert WAKEUP (PA06) polarity; default wakes on High</td>
</tr>
<tr>
<td style="text-align: center;">0000 0400</td>
<td>Invert ACTIVE (PA14) polarity; default is active Low</td>
</tr>
<tr>
<td style="text-align: center;">0000 0800</td>
<td>Pull down Rxd (PA05); default is Hi-Z</td>
</tr>
<tr>
<td style="text-align: center;">0000 1000</td>
<td>Disable ACTIVE (PA14) control; Outp and Inp remain available</td>
</tr>
<tr>
<td style="text-align: center;">00002000</td>
<td>Swap LED(PA18) and LED(PA19)</td>
</tr>
<tr>
<td style="text-align: center;">0000 4000</td>
<td>Set LED(PA18) High during transmit BUSY; default is High during transmission</td>
</tr>
<tr>
<td style="text-align: center;">0000 8000</td>
<td>Do not use the external XTAL32k for timers; use it only for Deep/Sleep timeouts and Clock.</td>
</tr>
<tr>
<td style="text-align: center;">0001 0000</td>
<td>Output the RF 1MHz signal on PB23 (only while RF is active).</td>
</tr>
<tr>
<td style="text-align: center;">0002 0000</td>
<td>Output the internal 1 kHz timer on PA22</td>
</tr>
<tr>
<td style="text-align: center;">0004 0000</td>
<td>Enable forwarding (P2P only)</td>
</tr>
<tr>
<td style="text-align: center;">0008 0000</td>
<td>Extend forwarding: when disabled, receive forwarded packets only; when enabled, allow duplicate reception</td>
</tr>
<tr>
<td style="text-align: center;">0010 0000</td>
<td>Save the join state to Flash when sending or receiving MAC commands, if Flash saving is enabled.</td>
</tr>
<tr>
<td style="text-align: center;">0020 0000</td>
<td rowspan="2">LoRaWAN join-state saving (bitwise OR of 2 bits)<br />
0=save all / 2=join information only in Flash / 4=RAM only / 6=do not save</td>
</tr>
<tr>
<td style="text-align: center;">0040 0000</td>
</tr>
<tr>
<td style="text-align: center;">0080 0000</td>
<td>Fix the LoRaWAN transmit-frame UpCounter; effective after reset while unjoined</td>
</tr>
<tr>
<td style="text-align: center;">0100 0000</td>
<td>Set LoRaWAN Up/Down Dwell
to unrestricted. Takes effect after a reset while unjoined.</td>
</tr>
<tr>
<td style="text-align: center;">0200 0000</td>
<td>Use the private LoRaWAN sync word; effective after reset</td>
</tr>
<tr>
<td style="text-align: center;">0400 0000</td>
<td>After a restart caused by wakeup from Deep, skip the startup delay and execute Auto immediately to reduce power consumption</td>
</tr>
<tr>
<td style="text-align: center;">0800 0000</td>
<td rowspan="5">Unused; set to 0</td>
</tr>
<tr>
<td style="text-align: center;">1000 0000</td>
</tr>
<tr>
<td style="text-align: center;">2000 0000</td>
</tr>
<tr>
<td style="text-align: center;">4000 0000</td>
</tr>
<tr>
<td style="text-align: center;">8000 0000</td>
</tr>
</tbody>
</table>

To enable multiple functions, combine bits using bitwise OR.  
The table lists hexadecimal values, but Ctrl can also be assigned decimal values like other variables.  
For LoRaWAN-related bits, see the ***[LoRa-BASIC for LRA1 Reference Manual: LoRaWAN](lora-basic-manual-wan_en.md)***.

Although Ctrl is a special variable, it supports compound assignment.  
  
Note:
Always set unused bits to 0. Behavior is not guaranteed if they are nonzero.

### Error

Reads an error code or throws an error.

Read: Returns the most recent error code.  
Write: Throws an error with the specified code.  
For details, see [11.3 Error Variable](#error-variable).

### Pass

Sets the configuration level.  
The values written and read are listed below.

<table style="width:97%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 11%" />
<col style="width: 16%" />
<col style="width: 58%" />
</colgroup>
<thead>
<tr>
<th>Value</th>
<th>Read value</th>
<th>Level</th>
<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
<td>-1</td>
<td>-1</td>
<td>Protected mode</td>
<td><p>List, Edit, and Psave return an error.</p>
<p>Once Pass is set to -1, it cannot be changed.<br />
Protected mode can be saved with Ssave. Otherwise, it is cleared on restart.<br />
To clear protected mode saved by Ssave, use Default. See the Default command for details.</p></td>
</tr>
<tr>
<td>999999</td>
<td>1</td>
<td>Administrator level</td>
<td>Allows reading and writing LoRaWAN variables that are inaccessible at the normal level.</td>
</tr>
<tr>
<td>Other</td>
<td>0</td>
<td>Normal level</td>
<td>Some LoRaWAN variables cannot be read or written.</td>
</tr>
</tbody>
</table>

For LoRaWAN variables accessible at the administrator level, see the ***[LoRa-BASIC for LRA1 Reference Manual: LoRaWAN](lora-basic-manual-wan_en.md)***.  
The level selected by Pass is not saved by Ssave. A restart restores the normal level.

```
 >Pass=999999  
 OK  
 >? Pass  
 1  
 OK  
 >Pass=-1  
 OK  
 >List  
 Permission error  
 OK
```

### Pass. (Pass followed by a dot)

Sets the Pass key (encryption key) used by Psave&/Pload& for BASE64 encoding and decoding.  
Pass. is write-only and is a different variable from Pass.  
The BASE64 program string output by Psave& uses the following settings.

<table style="width:60%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 23%" />
<col style="width: 22%" />
</colgroup>
<thead>
<tr>
<th>Pass.</th>
<th>Encryption key</th>
<th>Protection on restore</th>
</tr>
</thead>
<tbody>
<tr>
<td>0</td>
<td>AesKey</td>
<td rowspan="2">Keep current state</td>
</tr>
<tr>
<td>Positive number</td>
<td rowspan="2">AesKey and Pass key</td>
</tr>
<tr>
<td>Negative number</td>
<td>Enforce</td>
</tr>
</tbody>
</table>

AesKey is used only when it is enabled.

Loading a BASE64 string with enforced protection using Pload& enables protected mode, as if Pass=-1 had been executed. Negative and positive Pass keys are treated as different keys.

```
 >Pass.=-1234  
 OK  
 >Psave &  
 '2eGfB3lHjOpFFpW3VDdNy+9UaPtJV74SVD1gE+oi6Gt++0ogboY5RQVetLdk0rZ4
 'BKXxp09z1HJnvbWoEVJx6y0jlM8lyFN/1gLh  
 OK  
 >Pload &  
 
 [ここにBASE64を貼り付け]  
 
 OK  
 >List   
 Permission error  
 OK
```

Example notes: Paste the BASE64 text at the placeholder in the example.

# Errors


## Error Messages

When an error occurs, execution stops and one of the following messages is displayed.

In RUN mode, the line number where the error occurred is also displayed.

<table style="width:96%;">
<colgroup>
<col style="width: 26%" />
<col style="width: 6%" />
<col style="width: 63%" />
</colgroup>
<thead>
<tr>
<th>Error message</th>
<th style="text-align: center;">Code</th>
<th>Description</th>
</tr>
</thead>
<tbody>
<tr>
<td>OK</td>
<td style="text-align: center;">0</td>
<td>No error</td>
</tr>
<tr>
<td>Syntax error</td>
<td style="text-align: center;">1</td>
<td>Syntax error: an unrecognized command or incorrect syntax.</td>
</tr>
<tr>
<td>Division by 0 error</td>
<td style="text-align: center;">2</td>
<td>Division by zero.</td>
</tr>
<tr>
<td>Array index over error</td>
<td style="text-align: center;">3</td>
<td>An array index is negative or exceeds 255.<br />
Also occurs when a specified range exceeds the buffer bounds.</td>
</tr>
<tr>
<td>Parameter error</td>
<td style="text-align: center;">4</td>
<td>A required command parameter is missing,<br />
or a parameter value is out of range.</td>
</tr>
<tr>
<td>Stack overflow error</td>
<td style="text-align: center;">5</td>
<td>Stack overflow.<br />
Gosub, For, Do, or While nesting exceeds 16 levels.</td>
</tr>
<tr>
<td>Can't resume error</td>
<td style="text-align: center;">6</td>
<td>The program cannot be continued with Resume.<br />
Execution cannot be resumed after an error stops the program.</td>
</tr>
<tr>
<td>Label not found error</td>
<td style="text-align: center;">7</td>
<td>The target label number specified by Goto or Gosub was not found.</td>
</tr>
<tr>
<td>Unless from run-mode error</td>
<td style="text-align: center;">8</td>
<td>The command cannot run in RUN mode.<br />
Pload, New, Renum, and Resume cannot be executed from a program.</td>
</tr>
<tr>
<td>Program area overflow error</td>
<td style="text-align: center;">9</td>
<td>BASIC program memory capacity exceeded.</td>
</tr>
<tr>
<td>Loop nothing error</td>
<td style="text-align: center;">10</td>
<td>No Loop corresponding to Do/While.</td>
</tr>
<tr>
<td>Endif not found error</td>
<td style="text-align: center;">11</td>
<td>No EndIf corresponding to If.</td>
</tr>
<tr>
<td>Device access error</td>
<td style="text-align: center;">12</td>
<td>I2C device access error (LCD, BME280, etc.).</td>
</tr>
<tr>
<td>Edit mode error</td>
<td style="text-align: center;">13</td>
<td>Not in program editing mode.</td>
</tr>
<tr>
<td>Invalid mode error</td>
<td style="text-align: center;">14</td>
<td>The operation is unavailable in the current mode (P2P or WAN).</td>
</tr>
<tr>
<td>Permission error</td>
<td style="text-align: center;">15</td>
<td>The configuration level is not the administrator level.</td>
</tr>
<tr>
<td>Overflow error</td>
<td style="text-align: center;">16</td>
<td>String buffer overflow.</td>
</tr>
<tr>
<td>Unexpected Next error</td>
<td style="text-align: center;">17</td>
<td>No For corresponding to Next.</td>
</tr>
<tr>
<td>Unexpected Return error</td>
<td style="text-align: center;">18</td>
<td>No Gosub corresponding to Return.</td>
</tr>
<tr>
<td>Unexpected Loop error</td>
<td style="text-align: center;">19</td>
<td>No Do/While corresponding to Loop.</td>
</tr>
<tr>
<td>Unexpected Exit error</td>
<td style="text-align: center;">20</td>
<td>No For or Do/While corresponding to Exit.</td>
</tr>
<tr>
<td>Unexpected Continue error</td>
<td style="text-align: center;">21</td>
<td>No For or Do/While corresponding to Continue.</td>
</tr>
<tr>
<td>Unexpected Read error</td>
<td style="text-align: center;">22</td>
<td>No Data for Read, or the end of Data has been reached.</td>
</tr>
<tr>
<td>Flash write error</td>
<td style="text-align: center;">23</td>
<td>An error occurred while writing to Flash memory.</td>
</tr>
<tr>
<td>Program not exist error</td>
<td style="text-align: center;">24</td>
<td>No program exists.</td>
</tr>
<tr>
<td>Break</td>
<td style="text-align: center;">255</td>
<td>Interrupted by a break character or signal.</td>
</tr>
</tbody>
</table>

## Error Handling

Normally, a BASIC command error displays a message and stops execution. Use Catch to define a user error handler, transfer control to it when an error occurs, and continue the BASIC program after handling the error.  
See the Catch command for details.

## Error Variable

Use Error to read error codes or throw errors.  
In a user error handler, read Error to select the appropriate action for the error.  
Error can also be read or written at any time outside an error handler.

### Reading Error

Reading Error returns the most recent error code.  
Reading Error# (Error with # appended) returns the line number of the most recent error.  
Error# is 0 for an error in REPL mode.

### Writing Error

Writing an error code to Error throws that error.  
Writing 0 clears the stored error code without throwing an error.  
Writing an error code to Error# (Error with # appended) throws that error at the line number of the most recent error. An arbitrary line number cannot be specified.  
  
An error code without a defined message is also accepted; its numeric value is used as the error message.  
  
In an error handler, for errors that the handler does not handle,
execute Error#=Error to stop the program.

# LCD Control

　  
Use the following commands and variables to display text on the evaluation board's LCD.

The LCD must be connected via I2C.

## LCD Commands

### Lclr

Syntax: Lclr

Clears the LCD display.

### Lprint

Syntax: Lprint \[arguments\]

Displays a string on the LCD.  
Arguments follow Print syntax, but newlines and tabs are ignored.  
Output does not wrap when it exceeds the width of a line.

## LCD Variables

### Lpos

Range: 0–7, 64–71  
Sets the text display position on the evaluation board's LCD.

Positions and their values are shown below.

| Line 1 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|:-----:|-----|-----|-----|-----|-----|-----|-----|-----|
| Line 2 | 64 | 65 | 66 | 67 | 68 | 69 | 70 | 71 |

An out-of-range value does not cause an error.  
Reading Lpos returns the current display position. It increments by 1 for each character displayed.

### Lcont

Range: 1–63  
Sets the contrast of the evaluation board's LCD.  
1: Light ←→ 63: Dark

The default is 25.  
Values of 1 or less are set to 1; values of 63 or more are set to 63.  
Reading returns the current setting.  
Contrast depends on the supply voltage. Use approximately (4.5-Vdd)x20.  
Example: Lcont=(4500-Adc(27))/50

# BME280 Control

Use the following command to read temperature, humidity, and pressure from the evaluation board's BME280.

The BME280 must be connected via I2C.

## BME280 Command

### Bme

Syntax: Bme \[argument1\[,argument2\[,variable3\]\]\]

Measures temperature, humidity, and pressure using the evaluation board's BME280.  
With no arguments, displays temperature, humidity, and pressure in that order.  
With arguments, stores the measurements in the specified variables without displaying them.  
The first variable receives temperature, the second humidity, and the third pressure.  
Only general-purpose variables, array variables, Bup1, and Bup2 can be specified. Other variables and expressions are not accepted.  
Displayed or stored measurements use the following units:

Temperature: 0.1°C  
Humidity: 0.1%  
Pressure: 0.1hPa  

Example:
```
 >Bme  
 226 391 10223  
 OK  
 >Bme A,B,C  
 OK  
 >Print A,B,C  
 226 391 10223  
 OK
```

---

# String Encoding and Decoding

Strings can be encoded and decoded using hexadecimal, BASE64, or percent encoding.

## Encoding with Print

Prefix Print with & to output a BASE64-encoded string.  
Prefix Print with % to output a percent-encoded string.  
Prefix Print with $ to output a hexadecimal-encoded string.

```
>&Print "1234"   <---- Base64 decode  
MTIzNA0KOK  
>%Print "1234"   <---- Percent decode  
1234%0d%0aOK  
>\$Print "1234"  <---- Hexdecimal decode  
313233340d0aOK
```

## Encoded Strings

Specify a string as follows to treat it as an encoded string.

```
&"xxxxxxxxxxxx"  <---- Base64 decode
```

```
>Txd=&”MTIzNDU=”  
OK  
>Print Txd  
12345  
OK  
>&Print Txd  
MTIzNDUNCg==OK　　　　<--- 改行コードもエンコードされて、コマンド終了の”OK”が続いて表示  
>Send  
OK  
>Txd=\$”61313233”  
OK  
>Print Txd  
a123  
OK
```

Example notes: The newline is encoded too. The command completion message "OK" appears immediately after the encoded output.

---

# UART2 Control

In addition to the main serial port, PA22 and PA23 can be used as an additional serial port as shown below.  
LoRa-BASIC calls this serial interface on PA22 and PA23 UART2.

| Port | Pin | Name | Description |
|----------|--------|----------|--------------|
| PA22 | Pin.26 | UART2-TX | Output from the device |
| PA23 | Pin.20 | UART2-RX | Input to the device |

UART2 is enabled on the first UART2 command or variable input after a system reset, power-on, or wakeup from Sleep/Deep. Until then, the UART2 ports (PA22,
PA23) are not enabled.  
UART2 uses the baud rate specified by Ubaud, which defaults to 9600.  
  
This chapter describes UART2 commands and variables, including GPS commands available when a GPS device is connected to UART2.

## UART2 Commands

### Utrans

Syntax: Utrans \[expression\]  
Uses UART2 in transparent mode.  
UART2 uses the baud rate specified by Ubaud, which defaults to 9600.  
Bridges the main UART and UART2: characters received on UART are sent to UART2, and characters received on UART2 are sent to UART.  
A break character or signal on the main UART ends the command.  
The optional expression specifies an inactivity timeout in msec. The command ends if neither UART receives input within that period. Omitting it or specifying 0 disables the timeout; a negative value causes an error. Each input on either UART restarts the timeout.  
The two UARTs may use different baud rates, but take care to avoid buffer overflow.  
PA22 and PA23 are enabled for UART2 only while this command runs. Both return to Hi-Z when it ends.

### Uprint

Syntax: Uprint \[expression\]  
Outputs the expression result to UART2.  
Otherwise, it works like Print.  
Like Print, Uprint can be prefixed with a decoding specifier.

## UART2 Variables

The following variables control UART2.

UART2 variable list

<table style="width:96%;">
<colgroup>
<col style="width: 18%" />
<col style="width: 33%" />
<col style="width: 15%" />
<col style="width: 16%" />
<col style="width: 12%" />
</colgroup>
<thead>
<tr>
<th>Variable</th>
<th>Description</th>
<th style="text-align: center;">Read/write<br />
(✕ means read-only)</th>
<th style="text-align: center;">Save/restore<br />
(Ssave/Sload)</th>
<th style="text-align: center;">Default</th>
</tr>
</thead>
<tbody>
<tr>
<td>Ubaud</td>
<td>UART2 baud rate</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">9600</td>
</tr>
<tr>
<td>Uinkey</td>
<td>UART2 character input</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
<tr>
<td>Ugets</td>
<td>UART2 string input</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">✕</td>
<td style="text-align: center;">None</td>
</tr>
</tbody>
</table>

### Ubaud

Gets or sets the UART2 baud rate.  
The setting takes effect when Uprint, Uinkey, Ugets, Ugps, or Utrans
is executed.  
Accepted values are the same as for Baud. See Baud for details.

### Uinkey

Syntax: Uinkey or Uinkey(expression)

Reads one character from UART2.  
Returns its ASCII code, or -1 if no input is available.  
Otherwise, it works like the Inkey variable.  
With parentheses, a timeout can be specified as with Inkey.

### Ugets

Syntax: Ugets or Ugets(expression)

Reads a string from UART2.  
Otherwise, it works like the Gets variable.  
With parentheses, Ugets acts as a function accepting a timeout, like Gets.  
Input is not echoed.  
Ugets! reads input using the message format. A break character or signal on UART can still interrupt the program.

---
# GPS

Connect a GPS device to UART2 to use Ugps. Datetime handles the time information obtained from GPS.  
GPS devices that output standard NMEA data over a serial interface can be used, although compatibility with every such device is not guaranteed.

## Ugps

Syntax: Ugps \[expression1\], \[expression2\]  
Reads time, latitude, and longitude from a GPS device connected to UART2.

Expression1 specifies the timeout in msec. Omitting it or specifying 0 disables the timeout.  
Expression2 specifies the HDOP threshold multiplied by 10. The default is 0.

UART2 uses the baud rate specified by Ubaud, which defaults to 9600.  
GPS values are stored in array elements 0–4 as follows.

| Destination | Value |
|--------|--------------------------------------------------------|
| @(0) | Serial seconds: elapsed seconds since 2020/1/1, 00:00:00 UTC |
| @(1) | Latitude in degrees × 1000000; negative for south |
| @(2) | Longitude in degrees × 1000000; negative for west |
| @(3) | Altitude in meters × 10 |
| @(4) | Horizontal dilution of precision (HDOP) × 10 |

Each valid GPS fix is stored in @(0–4). The command ends when its HDOP is at or below the threshold in expression2,
or immediately after a valid fix if expression2 is 0 or omitted. Otherwise, it continues acquiring fixes.  
A timeout or a break character or signal on the main UART also ends the command.  
On timeout or break, @(0–4) contains the most recent valid fix, if any. If no valid fix was obtained, all five elements are set to 0.  
This command enables UART2 on PA22 and PA23. Both pins return to Hi-Z when it ends.  
Use a GPS device that outputs standard NMEA data. Some models or configurations may not be supported.

```
例： Ugps 10000, 14 ← タイムアウ=10秒、HDOP閾値=1.4  
@(0) : 19743865 ←　20/08/16 12:24:25  
@(1) : 35710950 ←　北緯35度42.6570分 (35.710950度)  
@(2) : 139707341 ←　東経139度42.4405分 (139.707341度)  
@(3) : 161 ←　16.1M  
@(4) : 15 ←　HDOP 1.5
```

Example notes: Ugps 10000, 14 specifies a 10-second timeout and an HDOP threshold of 1.4. The illustrated latitude is 35°42.6570′ N (35.710950°), and longitude is 139°42.4405′ E (139.707341°).

> #### HDOP  
> DOP (dilution of precision) includes PDOP, HDOP, and VDOP. Only HDOP is used here.  
> HDOP describes the dilution of horizontal position accuracy. Lower values indicate better accuracy; satellites spread evenly across the sky generally produce lower HDOP. A value of 1.0 or less is generally considered highly accurate.

> #### Time to Acquire a Fix  
> A cold start or a low HDOP threshold (high accuracy) can increase acquisition time.  
> This depends strongly on the installation environment and retained ephemeris and almanac data.  
> Adjust the HDOP threshold and timeout to suit the conditions.

> #### Serial Seconds  
> Serial seconds obtained from GPS are in UTC (Coordinated Universal Time).  
> JST (Japan Standard Time) = UTC + 32400 seconds

## Datetime

Syntax: Datetime \[expression1\], \[expression2\]  
Converts serial seconds to a date (year, month, day) and time (hour, minute, second).  
Expression1 specifies serial seconds, the elapsed seconds since 2020/1/1,
00:00:00.  
Expression2 specifies the starting array index (0–249) for the results.

The results occupy six consecutive array elements starting at expression2.

| Destination | Result |
|----------|--------------------------|
| @(+0) | Year (20–88), last two digits |
| @(+1) | Month (1–12) |
| @(+2) | Day (1–31) |
| @(+3) | Hour (0–23) |
| @(+4) | Minute (0–59) |
| @(+5) | Second (0–59) |

The numbers in parentheses in the destination column are offsets from argument2.  
The valid serial-second range is 0 (20/01/01 00:00:00) to 2147483647 (88/19 03:14:05)
inclusive.  
Other values do not cause an error, but produce invalid results.  
Use TimeVal to convert a date and time to serial seconds.

Example:
```
 Datetime @(0)+32400,4   ←  UTCからJST(+9h)の日時に変換して@(4～9)に設定
 ```

Example notes: Converts UTC to JST (+9 hours) and stores the date and time in @(4–9).
In a string expression, Datetime acts as a function that converts serial seconds to a string.


## Distance

Syntax: Distance \[expression1\]

Calculates the distance and bearing between two GPS positions.

Expression1 specifies the starting array index (0–250) used by this command.  
The default is 0.

Place the latitude and longitude of two positions in the array. The results are stored in the following elements.

The command uses six consecutive array elements starting at expression1.

<table style="width:85%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 18%" />
<col style="width: 52%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Array element</th>
<th colspan="2">Description</th>
</tr>
</thead>
<tbody>
<tr>
<td>＠(＋０)</td>
<td rowspan="2">Position 1</td>
<td>Latitude in degrees × 1000000; negative for south</td>
</tr>
<tr>
<td>＠(＋１)</td>
<td>Longitude in degrees × 1000000; negative for west</td>
</tr>
<tr>
<td>＠(＋２)</td>
<td rowspan="2">Position 2</td>
<td>Latitude in degrees × 1000000; negative for south</td>
</tr>
<tr>
<td>＠(＋３)</td>
<td>Longitude in degrees × 1000000; negative for west</td>
</tr>
<tr>
<td>＠(＋４)</td>
<td rowspan="2">Results</td>
<td>Distance to position 2 in meters</td>
</tr>
<tr>
<td>＠(＋５)</td>
<td>Bearing to position 2 in degrees × 1000000; true north is 0</td>
</tr>
</tbody>
</table>

```
>>10 @(1)=35654400
>>20 @(2)=139744770
>>30 @(3)=21422500
>>40 @(4)=39826100
>>50 Distance 1
>>60 Print @(5),@(6)
>>run
9491280　　-66947015
OK
```

---

# I2C Control

The following commands and variables control the device's I2C port.  
The evaluation board connects its LCD and BME280 to this port, with dedicated commands for each. The commands and variables in this chapter allow communication with other I2C devices.

## I2C Commands

### I2cR

Syntax: I2cR \[expression1\], \[expression2\], \[expression3\]

Reads from an I2C device.  
Expression1 is the required I2C device address, 0–127
(\$7F).  
Expression2 specifies the register (memory address), -1–255.
The default is -1.

A value of -1 performs only the read operation, without specifying a register.

Expression3 specifies the number of bytes to read, 0–32. The default is 1.  
Received data is stored in the I2C buffer, which is accessible through the I2cD array variable. See I2C Buffer.

### I2cW

Syntax: I2cW \[expression1\], \[expression2\], \[expression3\]

Writes to an I2C device.  
Expression1 is the required I2C device address, 0–127 (\$7F).  
Expression2 specifies the register (memory address), -1–255.
The default is -1.

A value of -1 performs only the write operation, without specifying a register.

Expression3 specifies the number of bytes to write, 0–32. The default is 1.  
Data is written from the I2C buffer, which is accessible through the I2cD array variable. See I2C Buffer.

## I2C Buffer

The I2C commands I2cR and
I2cW use a dedicated buffer for sending and receiving data.  
Access this buffer through the I2cD array variable.  
The 32-byte buffer is shared by transmit and receive operations.

### I2cD

Reading and writing the I2C buffer

Syntax: I2cD(expression) or I2cD

The expression specifies a buffer offset, 0–31.  
An index of -1 accesses the baud rate setting. See I2C Baud Rate.  
Offsets are in bytes from the start of the buffer. Values read or written are unsigned 8-bit values.  
If a value exceeds 8 bits (1 byte), only its lower 8 bits are stored.  
Without an index in parentheses, a string can be assigned to the buffer. It cannot be read as a string.  
A string containing character codes can also be assigned, as shown below.

```
>I2cR \$76,\$D0  
OK  
>? Form("x",I2cD(0))  
60  
OK  
>Lclr  
OK  
>I2cD(0)=\$24
OK
>I2cW \$3E,\$80,1
OK
>I2cD=\$"C041C042C0434044"
OK
>I2cW \$3E,-1,8
OK
```

## I2C Baud Rate

Use I2cD with an index of -1 to read or set the I2C baud rate.

The setting range is 0–400kbps.

Values of 400 or more select the maximum, 400kbps.  
A value of 0 selects the default, 100kbps.  
Choose a baud rate supported by the connected device. An incompatible rate may cause timeouts or other I2C access errors.  
The startup baud rate is 100kbps. This setting is not saved to Flash.

```
>? I2cD(-1)  
100  
OK  
>Lclr  
OK  
>I2D(-1)=400  
OK  
>Lprint ”1234”  
OK
```

---

# SPI Control

The following function and variable configure specific pins as an SPI port.  
The device operates only as an SPI master; slave operation is not supported.  
The following ports are available for SPI.

| SPI signal | Port | Direction | Description |
|---------|--------|------|----------------------------------|
| MISO | PB02 | Input | Data from the connected device |
| MOSI | PB22 | Output | Data to the connected device |
| SCLK | PB23 | Output | Clock to the connected device |

The SS (Slave
Select) signal must be controlled separately by the user with Outp or another port command.  
Assign a suitable pin to SS.  
SpiBaud enables SPI and specifies its transfer rate and mode.  
Using Outp or Inp on an active SPI pin disables SPI for that pin. Other SPI pins remain enabled.

## SPI Variable

### SpiBaud

Sets the SPI baud rate and enables SPI on the GPIO pins.  
Set SpiBaud to start SPI operation before using SpiTRx.  
The baud rate range is 1000–8000000Hz.  
The lower 3 bits specify SPI mode and bit order and are ignored as part of the baud rate.  
Setting 0 stops SPI operation and releases the GPIO pins.  
The lower 2 bits of SpiBaud (bits
0 and 1) specify the SPI mode, as shown below.

<table style="width:94%;">
<colgroup>
<col style="width: 8%" />
<col style="width: 8%" />
<col style="width: 7%" />
<col style="width: 7%" />
<col style="width: 13%" />
<col style="width: 48%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Lower<br />
2 bits</th>
<th style="text-align: center;">SPI<br />
mode</th>
<th style="text-align: center;">CPOL</th>
<th style="text-align: center;">CPHA</th>
<th style="text-align: center;">Idle<br />
clock polarity</th>
<th
style="text-align: center;">Clock phase for data sampling and shifting</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">00</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">Low</td>
<td style="text-align: center;">Sample on the rising edge, shift on the falling
edge</td>
</tr>
<tr>
<td style="text-align: center;">01</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">Low</td>
<td style="text-align: center;">Shift on the rising edge, sample on the falling
edge</td>
</tr>
<tr>
<td style="text-align: center;">10</td>
<td style="text-align: center;">2</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">0</td>
<td style="text-align: center;">High</td>
<td style="text-align: center;">Sample on the falling edge, shift on the rising
edge</td>
</tr>
<tr>
<td style="text-align: center;">11</td>
<td style="text-align: center;">3</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">1</td>
<td style="text-align: center;">High</td>
<td style="text-align: center;">Shift on the falling edge, sample on the rising
edge</td>
</tr>
</tbody>
</table>

Bit
2 of SpiBaud specifies the bit order, as shown below.

| Bit 2 | Bit order |
|:----------:|--------------------------------|
| 0 | Transmit/receive MSB (most significant bit) first |
| 1 | Transmit/receive LSB (least significant bit) first |

SpiBaud is not saved to Flash and must be set after every reset or power-on.  
Sleep and Deep do not automatically stop SPI. Enabled SPI consumes a small amount of power even when idle. To save power, explicitly stop it with SpiBaud=0.  
Reading SpiBaud returns the current setting, or 0 when SPI is disabled.

## SPI Function

### SpiTRx

Syntax: SpiTRx(expression1\[,expression2\])

Outputs expression1 through SPI and returns the value received through SPI.  
Expression2 specifies the number of bytes, 1–4. It defaults to 1; values of 1 or less select 1, and values of 4 or more select 4. A length of 1 uses the lower 8 bits, 2 uses the lower 16 bits, 3 uses the lower 24 bits, and 4 uses all 32 bits.  
Data is transferred in little-endian byte order, least significant byte first.  
Bswap16 and Bswap32 can be used to change byte order.  
Set SpiBaud to enable SPI before calling this function. Calling SpiTRx while SPI is disabled causes a "Device
access error".

```
>Outp 28,1  
Ok  
>SpiBaud=1000000  
Ok  
>Outp 28,0:A=SpiTRx(-1,4):Outp 28,1  
Ok  
>? \$(Bswap32(A))  
12345678  
Ok  
>SpiBaud=0  
Ok
```
