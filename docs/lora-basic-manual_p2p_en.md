<h1>
LoRa-BASIC for LRA1<br/>
Reference Manual<br/>
P2P
</h1>

English edition translated from the [Japanese original](lora-basic-manual_p2p_jp.md). Command, function, and system-variable names are case-insensitive. This manual uses the spelling emitted by List and registered in the source code.

# Overview

LoRa-BASIC provides commands, variables and functions for transmitting and receiving LoRa packets.  
This manual covers private communication using LoRa-P2P mode, referred to below as P2P mode.  

For LoRaWAN mode, see ***[LoRa-BASIC for LRA1 Reference Manual: LoRaWAN](lora-basic-manual-wan_en.md)***.  
For basic operation, see ***[LoRa-BASIC for LRA1 Reference Manual: Basics](lora-basic-manual_basics_en.md)***.

In this manual, “device” means the LRA1 module.

> **Regional and Regulatory Notice**  
> The radio functions described in this manual are intended for use in Japan. Operation outside Japan, and compliance with radio regulations or certification requirements in other countries or regions, are not supported.

---

# P2P Mode

The factory-default LoRa communication mode is **P2P mode**.  
In P2P mode, devices communicate directly with each other.  
They cannot interoperate with other vendors' LoRa devices or with LoRaWAN mode.  

Use *Wan_Mode* to switch between P2P and LoRaWAN communication.  
See *Wan_Mode* in ***[LoRa-BASIC for LRA1 Reference Manual: LoRaWAN](lora-basic-manual-wan_en.md)***.  

---

# LoRa Variables

LoRa variables configure and monitor LoRa communication.

Use them like ordinary variables, except that some are read-only.

The LoRa variables below support reading and assignment as indicated.

Values must remain within each variable's minimum and maximum limits.  

<table style="width:97%;">
<colgroup>
<col style="width: 9%" />
<col style="width: 26%" />
<col style="width: 47%" />
<col style="width: 13%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Variable</th>
<th style="text-align: center;">Description</th>
<th style="text-align: center;">Range</th>
<th>Default</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">Modem</td>
<td>Modem selection</td>
<td>0 : FSK , 1 : LoRa</td>
<td>1</td>
</tr>
<tr>
<td style="text-align: center;">Pwr</td>
<td>Transmit power</td>
<td>-4～13 (dBm)</td>
<td>13</td>
</tr>
<tr>
<td style="text-align: center;">Sf</td>
<td>Spreading factor</td>
<td>7～12</td>
<td>10</td>
</tr>
<tr>
<td style="text-align: center;">Bw</td>
<td>Bandwidth</td>
<td>6 : 62.5kHz , 7 : 125kHz , 8 : 250kHz , 9 : 500kHz</td>
<td>7</td>
</tr>
<tr>
<td style="text-align: center;">Cr</td>
<td>Coding rate</td>
<td>1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8</td>
<td>1</td>
</tr>
<tr>
<td style="text-align: center;">Ch</td>
<td>Channel</td>
<td>24～61</td>
<td>36</td>
</tr>
<tr>
<td style="text-align: center;">Frq</td>
<td>Transmit/receive frequency</td>
<td>920600–928000 (kHz) [read-only]</td>
<td>923000</td>
</tr>
<tr>
<td style="text-align: center;">Gid</td>
<td>Group ID</td>
<td>0～65535</td>
<td>0</td>
</tr>
<tr>
<td style="text-align: center;">Own</td>
<td>Local device ID</td>
<td>0～65535</td>
<td>1</td>
</tr>
<tr>
<td style="text-align: center;">Dst</td>
<td>Destination ID</td>
<td>0～65535</td>
<td>0</td>
</tr>
<tr>
<td style="text-align: center;">Rssi</td>
<td>Rssi</td>
<td>-137 to 0 [read-only]</td>
<td>0</td>
</tr>
<tr>
<td style="text-align: center;">Stat</td>
<td>LoRa status</td>
<td>See Stat for details</td>
<td>0</td>
</tr>
<tr>
<td style="text-align: center;">Ctrl</td>
<td>Control</td>
<td><p>Individual bits control operation.</p>
<p>See below for details.</p></td>
<td>$0000</td>
</tr>
<tr>
<td style="text-align: center;">AesKey</td>
<td>Shared encryption key</td>
<td>128-bit data (32 hexadecimal characters)</td>
<td>All zeros</td>
</tr>
</tbody>
</table>

Ssave saves some variables to Flash memory; Sload restores them.  
These values are also restored automatically from Flash at startup.

Default resets values to their defaults.

## Modem

Selects the modulation scheme.  
Range: 0: FSK / 1: LoRa.  
Sf, Cr and Bw settings have no effect in FSK mode.

## Pwr

Sets the transmit power.  
Range: -4 to 13.  
Transmission uses the specified power in dBm.  
Actual power may be lower if the supply voltage is below the nominal 3.3 V.

## Sf

Sets the spreading factor.  
Range: 7 to 12.  
Corresponds to LoRa SF7 through SF12.  
With Bw=9 (500 kHz), only SF10 and above are allowed.

## Bw

Sets the bandwidth.  
Range: 6 to 9.  
6 : 62.5kHz , 7 : 125kHz , 8 : 250kHz , 9 : 500kHz  
FSK uses 500 kHz regardless of Bw.  
Bw=9 (500 kHz) is available only with SF10 or above.

## Cr

Sets the coding rate.  
Range: 1 to 4.  
1 : 4/5 , 2=4/6 , 3: 4/7 , 4: 4/8  
Applies only to LoRa transmission; FSK does not use Cr.  
LoRa reception uses the Cr information in the received packet header, not this variable.

## Ch

Sets the transmit/receive channel.

Range: 24 to 61.  
Transmission and reception use this channel.  
Setting Ch also updates Frq.
For Bw=125 kHz or less, channels 24–38 are LowBand and 39–61 are HighBand.  
For Bw=250 kHz or greater, channels 24–37 are LowBand and 38–60 are HighBand. Ch=61 is unavailable.

## Frq

Reads the transmit/receive frequency.  
Range: 920600 to 928000.  
Returns the frequency in kHz corresponding to Ch.  
This variable is read-only.

## Gid

Sets the group ID.  
Range: 0 to 65535.  
Devices with the same group ID (Gid) can communicate.  
Frames from other group IDs are not received.  
Send with a string, or assigning a string to Txd, automatically copies this value to Gid in the transmit buffer.

## Own

Sets the local device ID.  
Range: 0 to 65535.  
A frame is received when its destination ID (Dst) matches the receiver's local ID (Own). Frames addressed to 65535 are received regardless of Own.  
Transmitted frames also include the local ID (Own), allowing the receiver to identify the sender.  
Send with a string, or assigning a string to Txd, automatically copies this value to Own in the transmit buffer.

## Dst

Sets the destination ID.  
Range: 0 to 65535.  
Sets Dst in transmitted frames; a receiver accepts a frame when Dst matches its Own.  
Destination ID 65535 is broadcast: receivers accept the frame regardless of their local IDs.  
Send with a string, or assigning a string to Txd, automatically copies this value to Dst in the transmit buffer.

## Rssi

Reads the Rssi of a received packet.  
Range: -137 to 0.  
Returns the Rssi of the last received frame in dBm.  
This variable is read-only.

## Stat

Reads transmit/receive status and LoRa errors.  
Range: 0 and above.  
Returns errors from LoRa commands and transmission/reception results.  
The variable is read-only except that assigning 0 clears it. No other assignment is allowed.  
The following table lists Stat values and their messages.

| Stat | Error message | Description |
|:--:|----|----|
| 0 |  | No error |
| 4 | no_free_ch | Carrier-sense error (channel busy) |
| 5 | invalid_data_length | Invalid transmit data length |
| 8 | timeout | Receive timeout |
| 9 | CRC_Error | Receive CRC error (only when enabled by Ctrl) |
| 10 |  | Data packet received |
| 12 | error | Other error (such as a recoverable hardware or internal error) |
| Other values |  | Not applicable to P2P mode |

Rows with no error message are not errors and do not display an error message.

After a timed Recv, check Stat to distinguish packet reception from timeout or another error. During background reception, a BASIC program can monitor Stat for a received packet. No new packet is received in the background while Stat is nonzero. Forwarding remains active even when Stat is nonzero.

> Stat is also used in LoRaWAN mode.  
> Some values are shared by both modes; others are mode-specific.

## TxLen

Reads the maximum transmit payload size.  
Range: 0 to 242.  
Returns the maximum payload length in bytes.  
This value depends on the LoRa settings
(Sf, Bw, Cr and Ch).  
See the LRA1 Hardware Manual for details.

## AesKey

Sets or reads the shared encryption key.  
Range: a 16-byte string.
When AesKey is set, the packet data is encrypted on transmission and decrypted on reception.  
Encryption uses a shared 128-bit AES key.  
Assign a 128-bit (16-byte) string to AesKey; hexadecimal input is recommended.  
Only the data is encrypted; header fields such as Own, Dst and Gid remain unencrypted. However, the header participates in encryption, so different headers produce different ciphertext even with the same AesKey and data.  
Both ends must use the same AesKey. Different keys prevent correct decryption.  
An empty string or an all-zero AesKey disables encryption. The default is all zeros.  
When AesKey is disabled, Print output and its string value are empty.  
The “#?” command displays the final 4 bytes of AesKey as "xxxxxxxx"
so use Print to inspect the full key. In LoRaWAN mode, "\#?" does not display AesKey.  
Ssave and Sload save and restore AesKey.  
AesKey is also used by "Pload &" and "Psave &".

Example:
```
>AesKey=$"0123456789abcdef00000000deadbeef"
OK
>? AesKey
0123456789abcdef00000000deadbeef
OK
>AesKey=""
OK
>? AesKey
00000000000000000000000000000000
OK
```

## Sens

Reads the channel Rssi.  
Range: -178 to 0.  
Reading this variable briefly receives on the channel and returns its Rssi.  
Ch selects the receive channel.  
Sens is read-only.  
Reading Sens does not affect Rssi.

Available only in P2P mode; using it in WAN mode causes an error.

```
 >? Sens
 
 -110  
 OK
```

---

# LoRa Transmit and Receive Buffers

LoRa transmission and reception use separate buffers.

Their layout is shown below.

<table style="width:84%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 11%" />
<col style="width: 28%" />
<col style="width: 33%" />
</colgroup>
<thead>
<tr>
<th rowspan="2" style="text-align: center;">Offset</th>
<th rowspan="2">Name</th>
<th colspan="2">Description</th>
</tr>
<tr>
<th>Txd (transmit buffer)</th>
<th>Rxd (receive buffer)</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0</td>
<td rowspan="2">Gid</td>
<td colspan="2" rowspan="2">Group ID</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
</tr>
<tr>
<td style="text-align: center;">2</td>
<td rowspan="2">Own</td>
<td rowspan="2">Local device ID</td>
<td rowspan="2">Source ID</td>
</tr>
<tr>
<td style="text-align: center;">3</td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td rowspan="2">Dst</td>
<td rowspan="2">Destination ID</td>
<td rowspan="2">Local device ID</td>
</tr>
<tr>
<td style="text-align: center;">5</td>
</tr>
<tr>
<td style="text-align: center;">6</td>
<td>Ctrl</td>
<td>Not accessible</td>
<td>Bits 0–2 = sequence number<br />
3～5 bit = Hop<br />
Bits 6–7 = reserved</td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td>Len</td>
<td colspan="2">Data length (0–242)</td>
</tr>
<tr>
<td style="text-align: center;">8<br />
～<br />
249</td>
<td>Data</td>
<td>Transmit data</td>
<td>Received data</td>
</tr>
</tbody>
</table>

WAN mode uses a different buffer layout.  
The transmit and receive buffers are independent.  
Read and write the transmit buffer using Txd/Txdw/Txdl/Txdm.  
Read the receive buffer using Rxd/Rxdw/Rxdl/Rxdm; assignment is not allowed.  
The 2-byte fields (Gid, Own, Dst) are little-endian and can be accessed using Txdw and Rxdw
variables.  
A received packet overwrites the receive buffer. Transmission does not clear the transmit buffer.  
For broadcast, Dst is 65535 (\$FFFF).

## Txd, Txdw, Txdl, Txdm

Reads or writes the transmit buffer.

Syntax: Txd(expression) or Txd

With parentheses, reads or writes one byte (8 bits) at the transmit-buffer offset specified by the expression.  
For an assigned value greater than 255, only the low 8 bits are used.  
Square brackets “[index]” may replace parentheses “(index)”.

Example:
```
 Txd(7)=10 ： 送信長(Length)に10を設定します。  
 A=Txd(10) ： 送信Dataの10byte目を参照します。
```

Example notes: The annotations set Length to 10 and describe reading the second transmit-data byte with Txd(10).

Without an index, reads or writes the transmit buffer as a string.  
Assigning a string stores it in the transmit-data area and automatically sets Gid,
Own, Dst and Len.  
In Print or Lprint, displays Data from the transmit buffer as a string.

```
>Txd=”1234”
OK
>Send
OK  
>Print Txd  
1234  
OK  
>A=1:B=2  
OK  
>Txd=””:Txd(7)=2:Txd(8)=A:Txd(9)=B:Send  
OK
```

Note:  
When not assigning a string, Gid/Own/Dst/Len are not set automatically. Set the corresponding transmit-buffer header fields before Send.

Executing “Txd=Null” initializes the other fields while setting Len=0.

Syntax: Txdw(expression), Txdl(expression), Txdm(expression)

At the specified transmit-buffer byte offset, Txdw reads or writes a Word (16 bits), Txdl a Long
Word (32 bits), and Txdm a Middle
Word (24 bits). Values are little-endian, with no alignment requirement.  
Square brackets “[index]” may replace parentheses “(index)”.

Example:
```
 Txdw(4)=\$1234      ： 宛先ID(Dst)に$1234を設定します。  
 Txdl(8)=\$12345678  ： Dataの先頭からの4バイト分に\$12345678を設定します。
```

Example notes: The first line sets destination ID (Dst) to $1234. The second sets the first 4 bytes of Data to $12345678.

Send transmits the transmit buffer.

If Send has a string argument, the string is assigned to the transmit buffer as with Txd before transmission.

## Rxd, Rxdw, Rxdl, Rxdm

Reads the receive buffer.

Syntax: Rxd(expression) or Rxd  

With parentheses, reads one byte (8 bits) at the transmit-buffer offset specified by the expression.  
Square brackets “[index]” may replace parentheses “(index)”.

Unlike Txd, this variable is read-only.

Example:  
```
 A=Rxd(9) ： 受信Dataの2byte目を参照します。
```

Example notes: The example reads the second received-data byte.

Without an index, reads the receive buffer as a string.  
It can be used in Print and Lprint.  
The string length is given by Len in the receive buffer.

```
>Recv 0
OK
>Print Stat
10
OK  
>Print Rxd  
1234  
OK  
>Print Rxd(7),Rxd(8)  
4 49  
OK
```

Syntax: Rxdw(expression), Rxdl(expression), Rxdm(expression)

At the specified transmit-buffer offset, Rxdw reads a Word (16 bits), Rxdl a Long
Word (32 bits), and Rxdm a Middle
Word (24 bits). Values are little-endian, with no alignment requirement.  
Square brackets “[index]” may replace parentheses “(index)”.  
  
Example:  
```
　A=Rxdw(2)　： Aに送信元ID(Own)を取得します。
　A=Rxdｌ(8) ： AにDataの先頭から4バイト分（32bit）を取得します。
```

Example notes: The first line reads the source ID (Own) into A. The second reads the first 4 bytes (32 bits) of Data into A.

## Using \# to Access the Transmit and Receive Buffers

"#" can be used as an alias for Txd and Rxd.  
Assignment accesses Txd; reading accesses Rxd.  
` "#L(0)=1" `
It cannot be used for Rxdl, Rxdw, Rxdm, Txdw, Txdl or Txdm as shown above.  
  
Example: the left and right operations below are equivalent.
```
　#(0)=1   ←→ Txd(0)=1  
　#=”1234” ←→ Txd=”1234”  
　A=#(7)   ←→ A=Rxd(7)  
　Print #  ←→ Print Rxd
```

---

# LoRa-P2P Commands

## \#?

Syntax: #?

Displays all configuration values together.

Example
```
>#?
Sn=100386
DevId=$"70b3d559e0ffffff"
Wan_Mode=0[P2P]
Modem=1[LoRa]
Pwr=13(dBm)
Ch=36
Sf=10
Bw=7[125kHz]
Cr=1[4/5]
Gid=0
Own=0
Dst=1
AesKey=Disable
Ctrl=$00000000
Echo=1[On]
Xtal32k=0:[int]
Auto=""
OK
```

\>

## Default

Syntax: Default \[expression1\]

Resets settings to their defaults.  
The value of \[expression1\] selects the behavior below.

<table style="width:93%;">
<colgroup>
<col style="width: 22%" />
<col style="width: 70%" />
</colgroup>
<thead>
<tr>
<th>Argument [expression1]</th>
<th>Behavior</th>
</tr>
</thead>
<tbody>
<tr>
<td>Omitted<br />
or any value other than -1</td>
<td>Reset values are not saved to Flash.<br />
In P2P mode, WAN settings are unchanged.<br />
In WAN mode, P2P settings are unchanged.<br />
The BASIC program is unchanged. Protected mode remains active.</td>
</tr>
<tr>
<td>-1</td>
<td>Clears the BASIC program and erases it from Flash.<br />
Disables protected mode.<br />
Resets both P2P and WAN settings regardless of the current mode. Also initializes settings not reset by ordinary Default, and clears activation to return to the initial state.<br />
Saves the default settings to Flash; Ssave is unnecessary.<br />
Restart after executing this command.</td>
</tr>
</tbody>
</table>

See Pass for protected mode.

## Ssave

Syntax: Ssave

Saves settings to Flash memory.  
Saved settings are loaded automatically at system startup.  
Both P2P and LoRaWAN
settings are saved regardless of the current mode.  
Sload and system startup load the settings saved in Flash.

## Sload

Syntax: Sload  
Loads settings from Flash memory.  
Restores both P2P and LoRaWAN settings regardless of the current mode.  
Loads settings previously saved with Ssave.

## Send

Syntax: Send \[string\]  
Transmits a LoRa packet.  
If a string is specified, it is stored in Data in the transmit buffer and transmitted. Numbers and expressions are converted to strings as in Print before storage and transmission.  
This also automatically sets Gid, Own, Dst and LEN in the buffer.  
The string can specify BASE64 or percent encoding.  
See String Encoding for details.  
Without a string argument, the buffer is transmitted unchanged; Gid, Own, Dst and LEN are not set automatically.  
Executing Send ends any background reception started by Recv.  
Configure the LoRa radio parameters Sf, Cr, Bw, Ch and Pwr before Send.

Transmit data length error

| Depending on Sf, Cr, Bw and Ch, transmitting Data of length Len may exceed the legally permitted transmission time. In that case, no packet is sent and “\*invalid_data_length” is reported. |
|----|

Channel contention error

| If Ch is occupied by another transmitter, including non-LoRa or other vendors' equipment, transmission waits for the channel to become free. If the wait reaches 500 msec, no packet is sent and “\*no_free_ch” is reported. |
|----|

## Recv

Syntax: Recv \[expression1\] \[, expression2\]

Starts LoRa packet reception.  
Configure Sf, Cr, Bw, Ch and Pwr before Recv.  
A packet that meets all the following conditions is stored in the receive buffer.

<table style="width:93%;">
<colgroup>
<col style="width: 93%" />
</colgroup>
<thead>
<tr>
<th>• Transmitter and receiver have matching Sf, Cr and Bw settings.<br />
• The packet's Gid (the sender's Gid) matches Gid.<br />
• The packet's Dst (the sender's Dst) matches Own or is 65535 (broadcast).</th>
</tr>
</thead>
<tbody>
</tbody>
</table>

Expression1 selects the following behavior.

<table style="width:93%;">
<colgroup>
<col style="width: 9%" />
<col style="width: 83%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Expression1</th>
<th style="text-align: center;">Behavior</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">０</td>
<td>0 starts reception and returns immediately.<br />
Reception continues in the background.<br />
Use RxStop to stop background reception.<br />
Check Stat for background receive status. A received packet sets Stat=10; read its data through Rxd. Errors set Stat to a value other than 10.<br />
Packets are received only while Stat=0. After handling a packet or error, clear Stat to allow reception of the next packet.</td>
</tr>
<tr>
<td style="text-align: center;">＞０</td>
<td><p>A positive argument specifies the timeout in msec.<br />
The command returns on packet reception or timeout; Stop is unnecessary.<br />
Neither received packets nor errors such as timeouts are displayed.<br />
Check Stat after the command returns for errors and receive status.</p>
<p>When Stat=10, read the received data through Rxd.</p></td>
</tr>
<tr>
<td style="text-align: center;">Omitted<br />
or<br />
-１</td>
<td>If omitted or -1, reception continues without returning until a break character/signal is received.<br />
Received packets are displayed as “@Rssi, source ID,
received Data”.<br />
Ctrl can change the displayed information.</td>
</tr>
<tr>
<td style="text-align: center;">＜-１</td>
<td>For a negative argument below -1, its absolute value specifies the timeout in msec.<br />
Behavior is the same as for a positive argument, but received data is displayed. Errors are displayed when the command returns.</td>
</tr>
</tbody>
</table>

Expression2 selects the received-data format and error display. It applies only when expression1 is negative.  
Expression1 may be omitted when specifying only expression2; no comma is required in that case.

<table style="width:93%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 57%" />
<col style="width: 25%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Expression2</th>
<th style="text-align: center;">Display format</th>
<th style="text-align: center;">Error display</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">Omitted<br />
or<br />
～</td>
<td><p>Displays received data unchanged.</p>
<p>All bytes, including control codes and other nonprintable characters, are output directly to the UART.</p></td>
<td
rowspan="6">Repeat the format character twice to suppress error messages when the command ends.<br />
<br />
Example:<br />
Recv -1000,$$</td>
</tr>
<tr>
<td style="text-align: center;">＆</td>
<td>Displays received packet data encoded as BASE64.</td>
</tr>
<tr>
<td style="text-align: center;">％</td>
<td>Displays received packet data with percent encoding.</td>
</tr>
<tr>
<td style="text-align: center;">＄</td>
<td>Displays received packet data encoded as hexadecimal.</td>
</tr>
<tr>
<td style="text-align: center;">！</td>
<td>Displays received data unchanged, enclosed by STX($02)/ETX($03).</td>
</tr>
<tr>
<td style="text-align: center;">＊</td>
<td>Does not display received data.<br />
(Same as a positive expression1.)</td>
</tr>
</tbody>
</table>

See String Encoding for encoding details.  
A newline follows the displayed received data.  
Notes:  
Background reception continues even if the BASIC program ends, including termination due to an error.

## RxStop

Syntax: RxStop  
Stops reception.

Use this to stop reception started by Recv with argument 0.  
Even after a BASIC program error, Recv reception continues until RxStop is executed.

## Comm

Syntax: Comm \[expression\]  
Starts a simple bidirectional communication mode.  
Received packets are displayed. Entering a newline transmits the input string as a packet.

A break signal ends Comm, regardless of BrkCh.  
A High-to-Low transition on PA06 enters Sleep; a Low-to-High transition wakes the device and resumes operation.

<table style="width:93%;">
<colgroup>
<col style="width: 11%" />
<col style="width: 82%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Expression</th>
<th style="text-align: center;">Behavior</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">None</td>
<td>Displays received packet data unchanged.<br />
Transmits the input string unchanged, excluding the newline.</td>
</tr>
<tr>
<td style="text-align: center;">＆</td>
<td>Displays received data encoded as BASE64.<br />
Decodes input from BASE64 before transmission.</td>
</tr>
<tr>
<td style="text-align: center;">％</td>
<td>Displays received data with percent encoding.<br />
Percent-decodes input before transmission.</td>
</tr>
<tr>
<td style="text-align: center;">＄</td>
<td>Displays received data encoded as hexadecimal.<br />
Decodes hexadecimal input before transmission.</td>
</tr>
<tr>
<td style="text-align: center;">！</td>
<td>Displays received data enclosed by STX($02)/ETX($03).<br />
Transmits input enclosed by STX($02)/ETX($03), excluding STX/ETX themselves.</td>
</tr>
<tr>
<td style="text-align: center;">＊</td>
<td>Does not display received packet data.<br />
Transmits the input string unchanged, excluding the newline.</td>
</tr>
</tbody>
</table>

## TxWave

Syntax: TxWave \[expression\]  
Transmits only a preamble.  
The required argument specifies a preamble duration from 5 to 3999
msec.  
A duration of 4000 msec or more, or 400 msec or more on a HighBand Ch, causes
an “\*invalid_length” error.  
Uses Ch, Sf, Bw and Pwr. Sf and Bw determine how many preamble symbols fit in the requested duration.  
If the specified duration corresponds to fewer than 12 preamble bits, 12 symbols are transmitted regardless of that duration. Ordinary data packets sent by Send and similar commands automatically begin with a 14-symbol preamble.  
As with Send, carrier sensing runs before preamble transmission.  
Applies only when Modem selects LoRa. In FSK mode, the command returns without action.  
The receiver can detect the preamble with Cad.

#### What is a preamble?

A preamble is a fixed bit pattern sent before data to notify and synchronize the receiver. In LoRa, one information bit after LoRa modulation is called a symbol.

## Cad

Syntax: Cad \[expression1\] \[, expression2\]  

Waits for a preamble. Cad means Channel Activity Detection.  
Expression1:
Specifies the interval in msec. If omitted or 0 or less, reception is continuous.  
A specified interval enables intermittent reception. The receive and preamble-detection time depends on Sf and Bw and is approximately
(2^Sf +32) / Bw seconds (8.45 msec for Sf=10, Bw=125 kHz).  
If the requested timeout is shorter than the detection time, the actual timeout is longer.  
Expression2:
Specifies the timeout in msec. If omitted or 0, no timeout applies.  
When the timeout expires, the command displays “\*timeout”
and returns. With a negative timeout, its absolute value is used and
"\*timeout"
is not displayed. On timeout, “Stat=8”
is set.  
  
Reception uses Ch, Sf and Bw; the transmitter must use matching settings.  
On preamble detection, the command returns with Stat=0.  
Detection continues until timeout or a break character/signal.

Applies only when Modem selects LoRa. In FSK mode, the command returns without action.  
Cad ends background reception started by Recv.  
  
With matching Ch, Sf and Bw, Cad detects preambles from other vendors' LoRa radios as well as packets and preambles from this device. It does not detect non-LoRa preambles.  
  
Intermittent Cad consumes less power than continuous Recv. Pair Cad on the receiver with TxWave on the transmitter to announce an upcoming packet. A preamble longer than the Cad interval improves detection. Cad itself cannot receive data packets.

---

# LoRa Functions

## IsFree

Syntax: IsFree(expression)  

Senses whether Ch is free.  
Returns 1 if free or 0 if busy.  
The expression specifies the maximum wait for a free channel in msec.  
Sensing uses intervals of 7 msec for Low-Ch and 125 usec for High-Ch.
If the argument is shorter than one sensing interval, performs one sensing operation. A negative argument causes an error.  
A break character/signal cannot interrupt the BASIC program while this function runs.  
Also available in WAN mode, but uses the P2P Ch setting.

## LoRa Errors

A LoRa error during a LoRa command displays an error message and ends the command.  
Continuous reception inside Recv is an exception: an error message does not end the command.  
A LoRa error ends the command but BASIC program execution continues.

Stat contains the error code.  
See Stat for details.

---

# P2P Forwarding

LoRa P2P communication supports packet forwarding.  
Enable forwarding using the corresponding Ctrl bit (Ctrl=\$40000).

Forwarding is disabled by default.

Forwarding uses Hop and FwdConf, described below.

## Hop Variable

Hop sets the maximum forwarding count. Its default is 0.  
Each packet has a forwarding counter incremented at every retransmission. A packet is forwarded only if Hop exceeds its forwarding counter. Thus Hop=0 effectively disables forwarding even with Ctrl=\$40000.  
The maximum Hop is 7, allowing up to 7 repeaters.  
Hop can be read and set even when forwarding is disabled in Ctrl.  
When forwarding is enabled, Hop appears in the “#?”
settings display.  
Ssave/Sload save and restore Hop; Default initializes it.

## FwdConf Variable

By default, forwarding uses the same Sf, Bw, Cr and Ch as reception. FwdConf allows different transmit parameters for forwarding.  
Each 8-bit field of the 32-bit FwdConf holds one parameter.

| Bits | Parameter | Range |
|--------|--------------|----------|
| 0～7   | Sf           | 6～12    |
| 8～15  | Bw           | 6～9     |
| 16～23 | Cr           | 1～4     |
| 24～31 | Ch           | 24～61   |

Example: “FwdConf=\$1B01070C” \<-- Ch=27, Cr=1, Bw=7, Sf=12

Out-of-range values do not cause an error but are not applied to forwarded transmissions.  
FwdConf also accepts these special values.

FwdConf = -1 :
Always transmit using the current receive parameters (default).

FwdConf = 0 :
Capture the current communication parameters. Reading returns the captured values, not 0.

Ssave/Sload save and restore FwdConf; Default initializes it.

Setting individual bit fields is cumbersome and error-prone. Instead, configure the communication parameters (Sf/Bw/Cr/Bw) normally, then use
"FwdConf=0"
to capture them in FwdConf before restoring the original communication parameters.  
(Executing "FwdConf=0"
immediately followed by Sload also restores FwdConf along with the communication parameters.  
After setting the current communication parameters, use “A=FwdConf:Sload:FwdConf=A:Ssave”
to avoid this.)

## Forwarding Behavior

### Starting forwarding

Forwarding operates when Ctrl enables it, Hop is nonzero, and P2P Recv starts reception. It operates only during reception started by Recv.  
Forwarding produces no text output, but the transmit LED flashes when a packet is forwarded.

### Background reception and forwarding

Recv with timeout 0 enables background reception and forwarding. Forwarding continues while BASIC runs or REPL waits for input. A background forwarding transmission temporarily pauses BASIC execution or REPL input processing.

### Packet sources and destinations

Forwarding relays all received packets with the same Gid except those addressed to Own. Individual destinations cannot be selectively forwarded or blocked. Broadcast packets (Dst=65535), or packets received with Own=65535 (receive all), are both delivered locally and forwarded.  
At the destination, a forwarded packet otherwise behaves like an original packet. Hop in the receive buffer's Ctrl field identifies it as forwarded.

### Duplicate forwarding and interference

A history of transmitted and received packets is retained for approximately 30 seconds. Duplicate packets are not forwarded again, suppressing loops. Send increments the packet sequence number, so a new Send is distinguishable even with identical payload data.

A randomized delay before forwarding reduces interference between repeaters with overlapping coverage. A later repeater may cancel transmission because of carrier sensing.

### Compatibility

Firmware versions without forwarding, or devices with forwarding disabled, cannot receive forwarded packets; they receive only original transmissions. Their transmitted packets can nevertheless be forwarded by forwarding-enabled devices.

P2P mode does not assign distinct repeater, child or parent roles.  
A receiving device forwards whenever forwarding is enabled and Hop is nonzero.

### Channel selection (important)

LowBand channels CH24–CH37 (up to CH36 for Bw=500 kHz) are recommended.  
HighBand channels CH38 and above cannot transmit continuously under Japanese radio certification requirements.  
HighBand requires an idle period 10 times the transmission duration. LoRa-BASIC implements this behavior.

```
>Ctrl=\$40000  
>Own=65535  
>Hop=1  
>FwdConf=-1
>Auto="Recv\$"  
>Ssave  
>Reset    <---- 再起動すると全受信パケットを16進数で表示しながら転送します。
```

Example notes: After restart, all received packets are displayed in hexadecimal and forwarded.

---

# String Encoding and Decoding


## Encoding in LoRa Commands

LoRa commands can encode displayed receive data and decode transmit input.

### Recv encoding

Syntax: Recv \[expression\]

For Recv,
specifying $, % or & as the argument encodes the received data for display.  
See Recv for details.

```
>Recv %  <----- Percent encode  
@-50,10, a123  
>Recv &  <----- Base64 encode  
@-50,10, YTAxMg==  
>Recv $  <----- Hexdecimal encode  
@-50,10,61303132
```

### Comm encoding

Syntax: Comm \[expression\]

Specifying $, % or & as the Comm argument decodes input strings before transmission.  
Received data is encoded for display.  
  
Comm & \<----- Base64 encode/decode

Comm % \<----- Percent encode/decode  
Comm \$ \<----- Hexdecimal encode/decode

See Comm for details.
