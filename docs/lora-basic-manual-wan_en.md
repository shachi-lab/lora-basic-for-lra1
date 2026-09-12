<h1>
LoRa-BASIC for LRA1<br/>
Reference Manual<br/>
LoRaWAN
</h1>

English edition translated from the [Japanese original](lora-basic-manual-wan_jp.md). Command, function, and system-variable names are case-insensitive. This manual uses the spelling emitted by List and registered in the source code.

# Overview

LoRa-BASIC provides LoRaWAN commands, variables and functions for operation as a LoRaWAN end device.
This manual covers LoRaWAN mode.  

For P2P mode, see ***[LoRa-BASIC for LRA1 Reference Manual: P2P](lora-basic-manual_p2p_en.md)***.  
For basic operation, see ***[LoRa-BASIC for LRA1 Reference Manual: Basics](lora-basic-manual_basics_en.md)***.

In this manual, “device” means the LRA1 module.

> **Regional and Regulatory Notice**  
> The radio functions described in this manual are intended for use in Japan. Operation outside Japan, and compliance with radio regulations or certification requirements in other countries or regions, are not supported.

## LoRaWAN Specifications

LoRa-BASIC supports the following LoRaWAN specifications.

| Item | Specification |
|:---------------|:-----------------------------------------|
| Version | LoRaWAN Specification Version 1.0.3 |
| Region | AS923-1 ISM Band (Japan), 920 MHz band |
| Classes | Class-A / C (Class-B is not supported) |
| Activation | OTAA / ABP |
| Data rate | DR0–6 (DR7 FSK is not supported) |
| ADR | Supported, subject to the activated network |
| Confirmation | Both confirmed and unconfirmed |
| Other | Operates within the radio certification limits |

> Configure the LoRaWAN server and gateway for “AS923-1LBT” where possible.  
> LBT means “Listen Before Transmit”: carrier sensing before transmission.

## Operating Modes

LoRa-BASIC supports **P2P mode** and **LoRaWAN mode**.  

P2P provides direct device-to-device communication. LoRaWAN mode operates as a network end device through a LoRaWAN-compatible gateway.  
LoRaWAN mode retains the BASIC features and adds LoRaWAN commands and variables.  

Select either LoRaWAN or P2P mode.  
They cannot operate simultaneously.  

Some P2P commands can be executed in LoRaWAN mode but do not operate correctly.  
To switch modes, change *Wan_Mode*, save with *SSave*, then restart.  
LoRa commands may not work correctly after changing *Wan_Mode* without restarting.

See *Wan_Mode* for details.

# LoRaWAN Commands

The following commands are available in LoRaWAN mode.

| Command | Description | Arguments |
|----|----|----|
| *Wan_Join* | Activate (join) | None |
| *Wan_UnJoin* | Deactivate (unjoin) | None |
| *Wan_Tx* | Transmit uplink data | \[ encode \] |
| *Wan_Rx* | Receive downlink data (Class-C) | \[ timeout , \] \[ encode \] |
| *Wan_RxStop* | Stop downlink reception | None |
| *Wan_LinkChk* | Request a link-check MAC command | None |
| *Wan_ChList* | Display the channel list | None |
| *Wan_ChAdd* | Add a channel to the list | chid, freq, dr-min, dr-max |
| *Wan_ChRemove* | Remove a channel from the list | chid |

See each command for details.  
Command names are case-insensitive; mixed case is allowed.  
These commands cause an error in P2P mode.  
LoRaWAN commands may take time to complete and cannot be interrupted by Ctrl-C, except for *Wan_Rx*.  
Results are reflected in *Stat*; see that variable for details.  
LoRaWAN commands and variables use the “*Wan\_*” prefix, which can be replaced by “*\#*
”.  
Examples: “Wan_Join” → “#Join”, “Wan_Tx” → “#Tx”, “Wan_Cnf=1” → “#Cnf=1”.

## Wan_Join

Activates (joins) the device.  
Syntax: *Wan_Join*  
Arguments: none.  
Activates the device on a LoRaWAN network.  
*Wan_JoinMode* selects the activation method.

| *Wan_JoinMode* | Activation method |
|:--------------:|---------------------------------------|
|       1        | OTAA （Over-The-Air Activation)       |
|       2        | ABP （Activation By Personalization） |

Activation is required before sending data with *Wan_Tx*.  
Read *Wan_JoinStat* to check the current activation state.  
If already activated, *Wan_Join* displays “already_joined”
and keeps the current state. Deactivate first to activate again.  
Use *Wan_UnJoin* to deactivate.  
The activation methods are described below.

### Join - OTAA

OTAA (Over-The-Air Activation) sends a Join-Request to the network.  
Successful activation displays “join_accepted”.
The network supplies DevAddr, NwkSkey and AppSkey
which can be read through the corresponding LoRaWAN variables.  
Before OTAA, set *Wan_DevEui*, *Wan_JoinEui* and *Wan_AppKey*, and register their matching values with the LoRaWAN network server.  
After failed activation, LoRaWAN limits prevent another activation request until the restricted period expires. Executing *Wan_Join*
(OTAA) during the restriction after Join-Request transmission causes “\*dutycycle_restricted” (*Stat* = 7). See *Wan_DutyTime*.  
A network rejecting activation sends no response, resulting in “\*timeout” (*Stat* = 8).  
OTAA sets JOIN_ACCEPT_DELAY and
JOIN_ACCEPT_DELAY2 from *Wan_JoinDelay1* and *Wan_JoinDelay2*. AS923 defaults are 5000 msec and 6000 msec, so activation takes approximately 6 seconds and timeout approximately 7 seconds, with 1000 msec receive windows.  
Activation state is saved to Flash and restored after Deep, Reset or power-on.

### Join - ABP

Activates using ABP (Activation By Personalization).  
No packets are exchanged with the network.  
ABP activation displays *“joined”*.  
Before ABP, set the network-provided DevAddr, NwkSkey and AppSkey in *Wan_DevAddr*, *Wan_NwkSkey* and *Wan_AppSkey*, respectively.  
If *Wan_DevAddr* is 0, ABP activation generates a random value. Depending on the network configuration, *Wan_DevAddr*
must instead be set in advance.  
ABP sets RECEIVE_DELAY1 and
RECEIVE_DELAY2 from *Wan_RecvDelay1* and *Wan_RecvDelay2*. These can also be changed while activated.  
Activation state is saved to Flash and restored after Deep, Reset or power-on.  
  
Limitation:
ABP may not work correctly with some servers or network environments.

```
 >Wan_JoinMode=1  
 OK  
 >Wan_Join  
 *timeout  
 OK  
 >Wan_Join  
 *dutycycle_restricted  
 OK  
 >Wan_Join  
 join_accepted  
 OK  
 >? Wan_JoinStat  
 1  
 OK  
 >Wan_Tx  
 OK
```

### Saving and Restoring Join State

Join parameters are saved automatically to Flash and restored after Deep, reset or power loss.  
The following three operations run automatically by default; user action is normally unnecessary.  
Ctrl bits select the saving behavior to suit the operating environment and Flash endurance.

See the Ctrl bit settings for bit positions.

#### Saving to RAM

By default, join state is saved to both Flash and a RAM buffer. Selecting “RAM only” in Ctrl disables Flash writes but still restores join state after reset from RAM. When both copies exist, RAM takes priority after hardware reset, Reset or watchdog reset. Join state is not backed up in RAM for a restart after Deep.

#### Saving Join Information to Flash

Join information is saved to Flash/RAM when *Wan_Join* or *Wan_UnJoin* finishes.  
Activation is usually infrequent and has little effect on Flash life. By default, traffic also updates and saves Up/DownLinkCounter and Channel. Selecting “join information only in Flash” in Ctrl prevents those writes and automatic Flash saving of network MAC-command parameter changes while joined. RAM is still updated; Ctrl can separately enable automatic saving on MAC command transmission/reception.  
Use *Wan_Save* to save parameters to Flash.

#### Saving Counters to Flash

By default, traffic updates and saves Up/DownLinkCounter and Channel to Flash/RAM.  
A server discards a forwarded frame if its Up/DownLinkCounter is below the server's stored counter, treating it as already received. Keep the saved counters current while activated.  
“Fix UpLinkCounter” can bypass this handling, but requires matching server settings.

#### Flash Write Endurance

The device Flash supports an average of 100,000 erase/write cycles. Counter writes use successive portions of one sector, for an expected approximately 1,500,000 writes—more than one frame every approximately 4 minutes for 10 years.  
(60/4) x 24 x 365 x 10 = 1,314,000  
Actual endurance may be lower depending on temperature, voltage and device variation.

## Wan_UnJoin

Deactivates (unjoins) the device.  
Syntax: *Wan_UnJoin*  
Arguments: none.  
Clears LoRaWAN activation.  
Clears OTAA or ABP activation and displays *“Unjoined”*
as confirmation.  
No network communication occurs; only the device's internal activation state is cleared.  
After OTAA, *Wan_DevAddr*, *Wan_AppSkey* and *Wan_NwkSkey*
are cleared to 0.  
After ABP, these settings are retained.  
Class-C downlink reception is stopped if active.

```
 >? Wan_JoinStat  
 1  
 OK  
 >Wan_Tx  
 OK  
 >Wan_UnJoin  
 Unjoined  
 OK  
 >Wan_Tx  
 *not_joined  
 OK  
 >? Wan_JoinStat  
 0  
 OK
```

## Wan_Tx

Transmits uplink data.  
Syntax: *Wan_Tx* \[encode\]  
encode ：
Specifies the display encoding for received downlink data.

| encode | Received-data display |
|:------:|------------------------------------------------------|
| ％ | Percent-encoded |
| ＆ | BASE64-encoded |
| ＄ | Hexadecimal string |
| ！ | Binary enclosed by STX/ETX |
| ＊ | No display |
| ～ | Raw binary \<default when omitted\> |

Transmits uplink data stored in *Txd*. Transmit data cannot be passed directly as a *Wan_Tx* argument.  
Activation is required; see *Wan_Join*.  
The frame's FPort is set from *Wan_FPort*; see that variable.

Set the length in *Txd(7)*. Assigning a string to *Txd* sets *Txd(7)* to the string length automatically. See *Txd*.  
*Wan_Dr* specifies the data rate. Each DR has a maximum data length; longer data cannot be transmitted. Adjust DR if necessary. See *Wan_Dr* and *Wan_Length*.  
*Wan_Adr* supplies the frame's ADR bit. Actual ADR control depends on the network. See *Wan_Adr*.  
*Wan_Cnf* = 0 (off)
means no acknowledgment and no retransmission control.  
*Wan_Cnf* = 1 (on)
requests acknowledgment and retransmission according to *Wan_Trial*. See *Wan_Trial*.  
After the uplink, the device waits for downlink data according to LoRaWAN V1.0.3. A received downlink is displayed in the following format.

@\<RxRSSI\>,\<RxFPort\>,\<RxData\>  
\<RxData\> uses the specified encode format.  
The receive result also updates the variables below.  
*Stat* = 10、*Rssi* = \<RxRSSI\>、*Rxd(6)* = \<RxFPort\>、*Rxd(7)* = \<RxData\>.Length、*Rxd(8～)* = \<RxData\>

If *Wan_LinkChk* was executed first, a link-check MAC command is included in the uplink. A link-check response from the network is displayed; see *Wan_LinkChk*.  
During Class-C reception, *Wan_Tx* resumes downlink reception after transmission.  
In contrast, P2P *Send* does not resume reception started by *Recv*.

```
 >Txd=$"01020304"  
 OK
 >Wan_FPort=50  
 OK  
 >Wan_Tx $  
 @-75,100,0a0b0c  
 OK  
 >? Stat  
 10  
 OK  
 >Wan_Tx $  
 OK  
 >? Stat  
 0  
 OK
```

## Wan_Rx

Receives downlink data.  
Syntax: *Wan_Rx* \[timeout , \] \[encode\]  
timeout: receive timeout in
1/1000-second units. Default: -1.  
encode: display encoding for received data.
See *Wan_Tx*.  
  
*Wan_Rx* receives Class-C downlink data.  
It is available only in Class-C (*Wan_Class* = 2); other classes cause
“\*incompatible_class”. Without activation, it causes
“\*not_joined”.  
timeout is in 1/1000-second units and selects the following behavior.

| timeout | Behavior |
|:--:|----|
| \> 0 | Returns when the time expires or new downlink data is received. |
| = 0 | Returns immediately. |
| = -1 | Returns when new downlink data is received. \<Default\> |
| \< -1 | Displays every new downlink and does not return. |


If not already receiving, *Wan_Rx* starts reception. It displays any previously received downlink data.
Except with timeout=0, downlink data received during *Wan_Rx* is displayed.
Reception continues in the background after *Wan_Rx* returns. *Wan_RxStop*, *Wan_Join* and *Wan_UnJoin* stop it. If join state is saved during background reception and the device restarts, activation is restored but reception remains stopped, as immediately after activation.  
Ctrl-C or Break during *Wan_Rx* ends the command and stops reception, including background reception. It also interrupts the BASIC program.  
Encoding and downlink display are the same as *Wan_Tx*.  
*Wan_Tx* can transmit during background reception; background reception resumes afterward.  
Check *Stat* for downlink status. After receiving data, display it with *Wan_Rx* or clear the status using “*Stat*
= 0”; otherwise new downlinks are discarded and the receive buffer is not updated.  
  
> Note:  
> Queuing or pushing downlink data does not guarantee immediate server transmission. Timing depends on the LoRaWAN server; consult its specifications.  
> Background reception continues even after a BASIC program ends, including termination due to an error.

```
 >Wan_Rx -1,$  
 @-75,50,010203  
 OK  
 >? Stat  
 10  
 OK  
 >Wan_Rx 0,$  
 OK  
 >? Stat  
 0  
 OK
```

## Wan_RxStop

Stops downlink reception.  
Syntax: *Wan_RxStop*  
Arguments: none.  
Stops Class-C background downlink reception.  
Available only in Class-C (*Wan_Class* = 2); otherwise “\*incompatible_class” occurs.
Without activation, “\*not_joined” occurs.  
If reception is already stopped, this command has no effect.

## Wan_LinkChk

Requests a link-check MAC command.  
Syntax: *Wan_LinkChk*  
Arguments: none.  
Adds LINK_CHECK_REQ to the frame sent by the next *Wan_Tx*.  
The network answers with LINK_CHECK_ANS in a downlink; *Wan_Tx* displays the response.  
The response reports how gateways received this device's uplink.  
The output format and fields are shown below.  
*“#\<DemodMargin\>, \<NbGateways\>”*

| Field | Description |
|:--:|----|
| *\<DemodMargin\>* | Link margin in dB for the last successfully received LINK_CHECK_REQ. |
| *\<NbGateways\>* | Number of gateways that successfully received the last LINK_CHECK_REQ (0–255). |

Nothing is displayed if no downlink or no LINK_CHECK_ANS is received. Downlink payload data, if present, is displayed after the link-check output.  
Executing this command clears the previous LINK_CHECK_ANS information.  
  
Activation is required; otherwise “\*not_joined” occurs.  
*Wan_UnJoin* also clears LINK_CHECK_REQ.  
A pending LINK_CHECK_REQ reduces the available payload length by 1 byte. See *Wan_Length*.  
A reset while joined does not clear LINK_CHECK_REQ.  
  
*Wan_LinkChk* can also be used as a string in *Print* and similar commands. Its text is the same as the *Wan_Tx* link-check output.
The string remains unchanged until a new *Wan_LinkChk* command.  
As a numeric variable, *Wan_LinkChk* reports the current LINK_CHECK_REQ request state.

```
 >Wan_LinkChk  
 OK  
 >? Wan_LinkChk  
   
 OK  
 >? 0+Wan_LinkChk  
 1  
 OK  
 >Wan_Tx $  
 #19,1  
 @-75,50,010203  
 OK  
 >? Wan_LinkChk  
 #19,1  
 OK
```

## Wan_Save

Saves settings and join state.  
Syntax: *Wan_Save*  
Arguments: none.  
Saves LoRaWAN variables to Flash.  
Assignments save automatically by default, so this command is normally unnecessary.  
*Default* does not save reset values automatically; save them using this command.  
It also saves join-state parameters and counters, unless the corresponding Ctrl bits disable that function.

```
 >? Wan_UpCnt  
 5  
 OK  
 >Wan_Save  
 OK  
 >Reset  

 LoRa-BASIC / Shachi-lab
 Ver x.xx.x
 OK
 >? Wan_UpCnt
 5  
 OK
```

## Wan_ChList

Displays the channel list.  
Syntax: *Wan_ChList*  
Arguments: none.  
Lists uplink channel frequencies.  
Includes default channels and those assigned by the LoRa server during join.  
Channels masked by *Wan_ChMask* are marked with “*”.  
The available DR range is also shown for each channel.

```
 >Wan_ChList  
 1:923200000
 2:923400000  
 OK  
 >Wan_Join  
 join_accepted  
 OK  
 >Wan_ChList  
 1:923200000 ,DR0-5
 2:923400000 ,DR0-5
 3:922200000 ,DR0-5
 4:922400000 ,DR0-5
 5:922600000 ,DR0-5
 6:922800000 ,DR0-5
 7:923000000 ,DR0-5  
 OK
```

## Wan_ChAdd

Adds a channel to the channel list.  
Syntax: *Wan_ChAdd chid, Freq, dr-min, dr-max*

Adds a channel for ABP.  
Specify chid=3–15, Freq in Hz, and the minimum/maximum DR in dr-min/dr-max. Values must match the LoRaWAN server settings.  
chid=1,2 are default LoRaWAN channels and cannot be changed.  
Executing this command with OTAA does not cause an error, but operation is not guaranteed.  
This setting is not saved.

## Wan_ChRemove

Removes a channel from the list.  
Syntax: *Wan_ChRemove chid*

For ABP, removes a channel previously added by *Wan_ChAdd*.  
Specify the added channel's chid, from 3 to 15.  
Default LoRaWAN channels chid=1,2 cannot be removed.  
Executing this command with OTAA does not cause an error, but operation is not guaranteed.  
This setting is not saved.

# LoRaWAN Variables

Use the variables below like ordinary variables, reading or assigning them as permitted.  
Some variables are read-only.

Assignments must remain within each variable's minimum and maximum values.

| Variable | Description | Range | Read | Write | Save | Default |
|----|----|----|:--:|:--:|:--:|----|
| *Wan_Mode* | LoRa mode | 0: P2P, 1: LoRaWAN | 〇 | 〇 | △ | \- |
| *Wan_Class* | LoRaWAN class | 0: Class-A, 2: Class-C | 〇 | 〇 | 〇 | \- |
| *Wan_JoinMode* | Activation method | 1: OTAA, 2: ABP | 〇 | 〇 | 〇 | \- |
| *Wan_TxPwr* | Transmit power | 0–7 | 〇 | 〇 | △ | 0 (13dBm) |
| *Wan_Dr* | Data rate | 0–6 | 〇 | 〇 | △ | 2 (DR2) |
| *Wan_FPort* | Transmit port | 1–223 | 〇 | 〇 | △ | 1 |
| *Wan_Cnf* | Transmission confirmation | 0, 1 | 〇 | 〇 | △ | 0 (off) |
| *Wan_Adr* | ADR | 0, 1 | 〇 | 〇 | △ | 0 (off) |
| *Wan_Trial* | Transmission attempts | 1–8 | 〇 | 〇 | △ | 1 |
| *Wan_Length* | Payload bytes | 0–242 (depends on *Wan_Dr*) | 〇 | ✕ | ✕ | \- |
| *Wan_JoinStat* | Join state | 0, 1, 2 | 〇 | ✕ | ✕ | \- |
| *Wan_UpCnt* | Uplink counter | 32-bit integer | 〇 | 〇 | 〇 | \- |
| *Wan_DownCnt* | Downlink counter | 32-bit integer | 〇 | 〇 | 〇 | \- |
| *Wan_DevAddr* | DevAddr | 32-bit integer | 〇 | ABP | 〇 | \- |
| *Wan_DevEui* | DevEui | 8 bytes (hexadecimal string) | 〇 | △ | 〇 | \- |
| *Wan_JoinEui* | JoinEui | 8 bytes (hexadecimal string) | OTAA | OTAA | 〇 | \- |
| *Wan_AppKey* | AppKey | 16 bytes (hexadecimal string) | OTAA | OTAA | 〇 | \- |
| *Wan_AppSkey* | AppSkey | 16 bytes (hexadecimal string) | △ | ABP | 〇 | \- |
| *Wan_NwkSkey* | NwkSkey | 16 bytes (hexadecimal string) | △ | ABP | 〇 | \- |
| *Wan_ChMask* | Channel mask | 16-bit integer | 〇 | 〇 | △ | 0 |
| *Wan_Pending* | MAC pending state | 0, 1 | 〇 | ✕ | ✕ | \- |
| *Wan_JoinDelay1* | Join response delay 1 | 500–9999 | 〇 | △ | ◎ | 5000 |
| *Wan_JoinDelay2* | Join response delay 2 | 500–9999 | 〇 | △ | ◎ | 6000 |
| *Wan_RecvDelay1* | Receive response delay 1 | 500–9999 | 〇 | △ | ◎ | 1000 |
| *Wan_RecvDelay2* | Receive response delay 2 | 500–9999 | 〇 | △ | ◎ | 2000 |
| *Wan_TimeReq* | TimeReq request | 0–3 | 〇 | 〇 | ✕ | 0 |
| *Wan_DutyTime* | Transmission restriction time | 16-bit integer | 〇 | ✕ | ✕ | 0 |
| *Wan_LastTxCh* | Last transmit channel | 1–16 | 〇 | ✕ | ✕ | 1 |
| *Wan_DwellStat* | Dwell state | 0–3 | 〇 | ✕ | ✕ | 3 |
| *Wan_BatLevel* | Battery level | 0–255 | 〇 | 〇 | ✕ | 0 |

> Read/write: 〇 allowed; ✕ unavailable; △ administrator level; OTAA means OTAA & △; ABP means ABP & △.  
> Save: 〇 automatic; ◎ automatic (*Default* + *Wan_Save* also available); △ save with *SSave*; ✕ cannot be saved.  

See each variable for details.  
Some variables are saved with *SSave*, others with *Wan_Save*.  
Activation-related variables are automatically saved to Flash when changed.  
Saved values are automatically loaded at system startup.

#### Default command  
*Default* resets LoRaWAN variables to their default values.  
Activation-related variables and variables without defaults are unaffected.  
In LoRaWAN mode, P2P-only variables are not reset.  
In P2P mode, LoRaWAN-only variables are not reset.  
These exceptions do not apply to Default with argument -1.

## Wan_Mode

Sets the LoRa-BASIC communication mode.  
Range: 0, 1 (no default).  
Assign this variable to change modes.

| *Wan_Mode* | Communication mode |
|:----------:|---------------|
| 0 | P2P mode |
| 1 | LoRaWAN mode |

Save *Wan_Mode* to Flash with *SSave*, not *Wan_Save*.  
After changing *Wan_Mode*, execute *SSave* and restart the system.  
LoRa commands (P2P/LoRaWAN) are not guaranteed to work correctly if executed after a mode change without restarting.  
Changing modes does not alter the BASIC program, general-purpose variables or array variables.  
*Default* does not affect *Wan_Mode*, except with argument -1.  
This manual describes LoRaWAN mode (*Wan_Mode* = 1).  
For P2P (*Wan_Mode* = 0), see ***[LoRa-BASIC for LRA1 Reference Manual: P2P](lora-basic-manual_p2p_en.md)***.

```
 >? Wan_Mode  
 0  
 OK  
 >Wan_Mode=1  
 OK  
 >SSave  
 OK  
 >Reset
```

## Wan_Class

Sets or reads the LoRaWAN class.  
Range: 0, 2 (default: 0).

| *Wan_Class* | LoRaWAN class |
|:-----------:|----------------------------------|
| 0 | Class-A \<default\> |
| 1 | Class-B (unsupported; cannot be selected) |
|      2      | Class-C                          |

Class-C enables *Wan_Rx* and *Wan_RxStop*.  
Assigning *Wan_Class* while activated causes an error, even when assigning the current class.  
Before changing *Wan_Class*, execute *Wan_UnJoin*
to deactivate.  

> Changes to *Wan_Class* are automatically saved to Flash.

```
 >? Wan_Class  
 0  
 OK  
 >Wan_Class=2  
 OK
```

> Note:  
> Class-C activation with *Wan_Join* does not itself start reception.
> Execute *Wan_Rx* after activation to start reception; see that command.

## Wan_JoinMode

Sets or reads the activation method.  
Range: 1, 2 (default: 1).  
Selects the method used to activate on the LoRaWAN network.  
The following values are available.

| *Wan_JoinMode* | Activation method |
|:--------------:|-------------------------------------------------|
| 1 | OTAA (Over-The-Air Activation) \<default\> |
|       2        | ABP （Activation By Personalization）           |

*Wan_JoinMode* selects the method used by *Wan_Join*.  
The device must be unjoined before assignment. Otherwise “\*denied” occurs, but BASIC execution continues.  
See *Wan_Join* for the activation methods.  
Changes to *Wan_JoinMode* are automatically saved to Flash.

```
 >Wan_JoinMode=1  
 OK  
 >Wan_Join  
 join_accepted  
 OK  
 >? Wan_JoinStat  
 1  
 OK  
 >Wan_Tx  
 OK
```

## Wan_TxPwr

Sets or reads transmit power.  
Range: 0–7 (default: 0).  
*Wan_TxPwr* values are listed below.

| *Wan_TxPwr* | LoRaWAN specification (EIRP) | Output (dBm) |
|:-----------:|:------------------:|:---------:|
|      0      |      MaxEIRP       |    13     |
|      1      |   MaxEIRP - 2dBm   |    11     |
|      2      |   MaxEIRP - 4dBm   |     9     |
|      3      |   MaxEIRP - 6dBm   |     7     |
|      4      |   MaxEIRP - 8dBm   |     5     |
|      5      |  MaxEIRP - 12dBm   |     3     |
|      6      |  MaxEIRP - 14dBm   |     1     |
|      7      |  MaxEIRP - 16dBm   |    -1     |

EIRP: Equivalent Isotropically Radiated Power.  
The P2P *Pwr* setting does not affect LoRaWAN transmit power.  
Actual output may be lower if the device supply is below the nominal 3.3 V.  
*Wan_TxPwr*
must be saved to Flash with *SSave*, not *Wan_Save*.

```
 >Wan_TxPwr=2  
 OK  
 >? Wan_TxPwr  
 2  
 >Wan_Tx  
 OK
```

## Wan_Dr

Reads or sets the data rate.  
Range: 0–6 (default: 2).  
Data-rate values are listed below.

| *Wan_Dr* | Setting (Sf/bandwidth) | Bit rate (bps) | Maximum payload length |
|:--------:|:-------------------:|:---------------:|:-------------:|
|    0     |    SF12 / 125kHz    |       250       |       0       |
|    1     |    SF11 / 125kHz    |       440       |       0       |
|    2     |    SF10 / 125kHz    |       980       |      11       |
|    3     |    SF9 / 125kHz     |      1760       |      53       |
|    4     |    SF8 / 125kHz     |      3125       |      125      |
|    5     |    SF7 / 125kHz     |      5470       |      242      |
|    6     |    SF7 / 250kHz     |      11000      |      242      |

Shaded data rates are restricted by default, although they can be assigned to *Wan_Dr*.  
P2P *Sf*, *Bw* and *Cr* do not affect LoRaWAN mode.  
The actual available payload may be shorter than the maximum. See *Wan_Length*.

With *Wan_Adr* =1, setting *Wan_Dr*
=0 enables dynamic data-rate control through ADR.  
See *Wan_Adr*.  
*Wan_Dr*
must be saved to Flash with *SSave*, not *Wan_Save*.

```
 >Wan_Dr=3  
 OK  
 >? Wan_Dr  
 3  
 >Wan_Tx  
 OK
```

Thus **DR2–5 are available by default** in LoRa-BASIC.
*Wan_Dr* accepts 0–6, but values 0 and 1 use DR2 regardless of the stored value, except when ADR control is active.  
With *Wan_Dr*=6, *Wan_Tx* causes an error (\*no_free_ch)
on execution.  
DR0 and DR1 become available when the network sends TxParamSetupReq with unrestricted dwell time. This depends on network operation and cannot be requested or configured from LoRa-BASIC.  
DR6 likewise requires configuration from the LoRaWAN network.

## Wan_FPort

Reads or sets the transmit port.  
Range: 1–223 (default: 1).  
The FPort of data frames sent by *Wan_Tx*.  
This is an application-specific port number chosen by the user.  
*Wan_FPort*
must be saved to Flash with *SSave*, not *Wan_Save*.

```
 >Wan_FPort=100  
 OK  
 >? Wan_FPort  
 100  
 >Wan_Tx  
 OK
```

## Wan_Cnf

Reads or sets transmission confirmation.  
Range: 0, 1.  
Selects whether *Wan_Tx* requests acknowledgment.

| *Wan_Cnf* | Confirmation |
|:---------:|-------------------------------------------------------|
| 0 | No acknowledgment (Unconfirm) \<default\> |
| 1 | Acknowledgment requested (Confirm) |

With *Wan_Cnf*=1 (Confirm), *Wan_Tx* uses acknowledgment and retransmission control.  
Retransmissions follow *Wan_Trial*; see that variable.  
*Wan_Cnf*
must be saved to Flash with *SSave*, not *Wan_Save*.

```
 >Wan_Cnf=1  
 OK  
 >? Wan_Cnf  
 1  
 >Wan_Tx  
 *timeout  
 OK
```

## Wan_Adr

Reads or sets ADR (Adaptive Data Rate).  
Range: 0, 1.  
*Wan_Tx* sets this ADR value in uplink frames.

| *Wan_Adr* | ADR control |
|:---------:|--------------------------|
| 0 | ADR-off \<default\> |
|     1     | ADR-on                   |

*Wan_Adr* =1 (ADR-on)
notifies the network that ADR is ON. ADR control then follows these conditions.  
The data rate changes dynamically only when *Wan_Adr*=1 and *Wan_Dr*=0.  
With *Wan_Adr*=1 and *Wan_Dr* other than 0, *Wan_Dr*
takes priority.

With *Wan_Adr*=0, ADR is disabled and the data rate follows *Wan_Dr*.  
ADR depends on the network and may not take effect with some network settings.  
Save *Wan_Adr* with *SSave*, not *Wan_Save*.

```
 >Wan_Adr=1  
 OK  
 >? Wan_Adr  
 1  
 >Wan_Tx  
 OK
```

## Wan_Trial

Reads or sets the number of transmission attempts.  
Range: 1–8 (default: 1).  
*Wan_Trial*
specifies the transmission attempts when a frame sent by *Wan_Tx* receives no acknowledgment.
Retransmissions occur only with *Wan_Cnf*=1 (confirm).  
*Wan_Trial*=1 means one initial transmission and no retransmission; timeout errors still occur.  
Data-rate adaptation follows LoRaWAN as shown below.

| Attempt | Data rate |
|-----------|---------------|
| 1 (initial) | DR |
| 2         | DR            |
| 3         | max (DR-1, 0) |
| 4         | max (DR-1, 0) |
| 5         | max (DR-2, 0) |
| 6         | max (DR-2, 0) |
| 7         | max (DR-3, 0) |
| 8         | max (DR-3, 0) |

With *Wan_Trial*=1 or 2, a missing acknowledgment does not reduce DR.  
If reducing DR would make the frame too long for the new DR, retransmission retains the current DR.  
Example:
Starting at DR3 with 11 bytes or less, the fifth transmission would use DR1. However, DR1's maximum is 0 bytes, so it retains DR2 from the fourth attempt.  
*Wan_Trial*
must be saved to Flash with *SSave*, not *Wan_Save*.

```
 >Wan_Trial=3  
 OK  
 >? Wan_Trial  
 3  
 >Wan_Tx  
 OK
```

## Wan_Length

Reads the available transmit byte count at the current data rate.  
Range: 0–242 (default depends on *Wan_Dr*).  
Read-only; assignment is not allowed.  
Returns the number of bytes available for the payload.  
Sending a longer payload with *Wan_Tx* causes \*invalid_data_length.  
See *Wan_Dr* for maximum payload lengths by data rate.  
When dwell limits are enabled (default) and *Wan_Dr* is DR0 or DR1, the returned length corresponds to the default DR2.

```
 >Wan_Dr=2  
 OK  
 >? Wan_Length  
 11  
 OK
```

> Notes
> LoRaWAN MAC commands and responses can occupy part of the payload. Available bytes therefore depend on data rate, ADR and pending network commands/responses.
> *Wan_Length* may be smaller than the maximum payload for the DR selected by *Wan_Dr*.  
> Pending *Wan_LinkChk* and *Wan_TimeReq* requests each consume 1 byte.

## Wan_JoinStat

Reads the activation state.  
Range: 0, 1, 2 (no default).  
The values indicate the following states.

| *Wan_JoinStat* | Activation state |
|:--------------:|----------------------------|
| 0 | Not activated |
| 1 | Activated through OTAA |
| 2 | Activated through ABP |

*Wan_JoinStat* is read-only.

```
 >Wan_Join  
 join_accepted  
 OK  
 >? Wan_JoinStat  
 1  
 OK
```

## Wan_UpCnt

Sets or reads the uplink counter.  
Range: 0–4,294,967,295.  
Sets or reads the uplink counter for the next transmitted frame.  
The end device increments this counter for each uplink. Assignment can synchronize it with the server's stored counter.  
OTAA activation resets the uplink counter to 0.  
By default, LoRaWAN discards a received frame when the device/server counter gap is 16,384 or more. A sender counter lower than the receiver's is treated as a gap of at least 16,384.  
This variable cannot be saved manually. With automatic join-state saving enabled, it is saved during traffic and restored at restart. See Saving and Restoring Join State.

```
 >? Wan_UpCnt  
 5  
 OK  
 >Wan_Tx  
 OK  
 >? Wan_UpCnt  
 6  
 OK  
 >Wan_UpCnt=7  
 OK
```

## Wan_DownCnt

Sets or reads the downlink counter.  
Range: 0–4,294,967,295.  
Sets or reads the counter for the next received downlink frame.  
The network server increments this counter for every downlink. Assignment can synchronize it with the server's stored value.  
Otherwise behaves like *Wan_UpCnt*, with the link direction reversed.

```
 >? Wan_DownCnt  
 5  
 OK  
 >Wan_DownCnt=6  
 OK
```

## Wan_DevAddr

Reads or sets DevAddr (Device Address).  
Range: 32 bits (default: 0).  
Identifies the end device within the network and is shared by the device, network server and application server.  
DevAddr
is assigned by the network on each OTAA join and changes with every JOIN. It is separate from the ABP *Wan_DevAddr* and does not alter that setting.  
  
For ABP, configure the matching address on the network server in advance. If *Wan_DevAddr*=0, ABP activation generates a random address.  
Changes to *Wan_DevAddr* are automatically saved to Flash; the OTAA *Wan_DevAddr* is not saved this way.  
Changing it while activated prevents correct network communication.  
*Wan_DevAddr* can be changed only with *Wan_JoinMode* set to ABP.  
Changing *Wan_DevAddr* requires administrator level.

```
 >Wan_DevAddr=12345  
 OK  
 >? Wan_DevAddr  
 12345  
 OK
```

## Wan_DevEui

Reads or sets DevEui (Device EUI).  
Range: 64 bits (no default).  
Identifies the end device, generally assigned by its manufacturer.

An IEEE-issued EUI-64 is globally unique and differs for each end device.

DevEui is required for OTAA and must be registered with the network.

If *Wan_DevEui* is set to all 64 bits zero and saved, startup sets *Wan_DevEui* to *DevId*.  
(Wan_DevEui="" sets it to zero.)

Can be read as a hexadecimal string in *Print* or other string contexts.  
Accepts encoded strings; hexadecimal encoding is recommended.  
Changes to *Wan_DevEui* are automatically saved to Flash.  
Changing *Wan_DevEui* requires administrator level.

```
 >Wan_DevEui=$"0123456789abcdef"  
 OK  
 >? Wan_DevEui  
 0123456789abcdef  
 OK
```

## Wan_JoinEui (Wan_AppEui)

Reads or sets JoinEui (AppEui).  
Range: 64 bits (no default).  
Identifies the Join Server and is assigned by the LoRaWAN network operator.

An IEEE-issued EUI-64 is globally unique.

JoinEui is required for OTAA and must be registered with the LoRaWAN network.  
It is not used for ABP.  
  
Can be read as a hexadecimal string in *Print* or other string contexts.  
Accepts encoded strings; hexadecimal encoding is recommended.  
Changes to *Wan_JoinEui* are automatically saved to Flash.  
※*Wan_JoinEui*
can be read or changed only with *Wan_JoinMode* set to OTAA.  
Changing *Wan_JoinEui* requires administrator level.  
Firmware Ver1.07.f and earlier use the name *Wan_AppEui*. Only the name differs.

```
 >Wan_JoinEui=$"0123456789abcdef"  
 OK  
 >? Wan_JoinEui  
 0123456789abcdef  
 OK
```

## Wan_AppKey

Reads or sets AppKey (Application Key).  
Range: 128 bits (no default).  
A device-specific AES-128 root key used to derive NwkSkey and
AppSKey.  
AppKey
is required for OTAA and must be registered with the network.  
It is not used for ABP.  
  
Can be read as a hexadecimal string in *Print* or other string contexts.  
Accepts encoded strings; hexadecimal encoding is recommended.  
Changes to *Wan_AppKey* are automatically saved to Flash.  
*Wan_AppKey* can be read or changed only with *Wan_JoinMode* set to OTAA.  
Reading or changing *Wan_AppKey* requires administrator level.

```
 >Wan_AppKey=$"0123456789abcdef0123456789abcdef"  
 OK  
 >? Wan_AppKey  
 0123456789abcdef0123456789abcdef  
 OK
```

## Wan_AppSkey

Reads or sets AppSkey (Application Session Key).  
Range: 128 bits (no default).  
A 128-bit AES key shared by the end device and application server.

Encrypts and decrypts application data/messages to protect the application payload.

OTAA assigns it during activation. It is separate from the ABP *Wan_AppSkey* and does not change the ABP setting.  
For ABP, register matching values on the network server in advance.  
  
Can be read as a hexadecimal string in *Print* or other string contexts.  
Accepts encoded strings; hexadecimal encoding is recommended.  
Changes to *Wan_AppSkey* are automatically saved to Flash.  
Changing it while activated prevents correct network communication.  
*Wan_AppSkey* can be changed only with *Wan_JoinMode* set to ABP.  
Reading or changing *Wan_AppSkey* requires administrator level.

```
 >Wan_AppSkey=$"0123456789abcdef0123456789abcdef"  
 OK  
 >? Wan_AppSkey  
 0123456789abcdef0123456789abcdef  
 OK
```

## Wan_NwkSkey

Reads or sets NwkSkey (Network Session Key).  
Range: 128 bits (no default).  
A 128-bit AES key shared by the end device and network server.  
Provides message integrity and security between the device and network server.  
OTAA assigns it during activation. It is separate from the ABP *Wan_NwkSkey* and does not change the ABP setting.  
For ABP, register matching values on the network server in advance.  
Can be read as a hexadecimal string in *Print* or other string contexts.  
Accepts encoded strings; hexadecimal encoding is recommended.  
Changes to *Wan_NwkSkey* are automatically saved to Flash.  
Changing it while activated prevents correct network communication.  
*Wan_NwkSkey* can be changed only with *Wan_JoinMode* set to ABP.  
Reading or changing *Wan_NwkSkey* requires administrator level.

```
 >Wan_NwkSkey=$"0123456789abcdef0123456789abcdef"  
 OK  
 >? Wan_NwkSkey  
 0123456789abcdef0123456789abcdef  
 OK
```

## Wan_ChMask

Reads or sets the channel mask.  
Range: 16 bits (default: 0).  
Restricts the channels used for uplinks.  
Each bit controls one of 16 channels, starting with Ch1 at the LSB, then Ch2, and so on.  
A bit of 1 disables the channel; 0
enables it.  
By default, *Wan_ChMask*
is 0 (all bits zero), enabling all channels.  
*Wan_ChMask*
must be saved with *SSave*, not *Wan_Save*.  
Ch1 and Ch2 are default LoRaWAN channels; disabling them may prevent correct communication.

```
 >? Wan_ChMask  
 0  
 OK  
 >Wan_ChList  
 1:923200000 ,DR0-5
 2:923400000 ,DR0-5
 3:923600000 ,DR0-5
 4:923800000 ,DR0-5
 5:924000000 ,DR0-5
 6:924200000 ,DR0-5
 7:924400000 ,DR0-5  
 OK
 >Wan_ChMask=4  
 OK  
 >Wan_ChList  
 1:923200000 ,DR0-5
 2:923400000 ,DR0-5
 3:923600000\*,DR0-5
 4:923800000 ,DR0-5
 5:924000000 ,DR0-5
 6:924200000 ,DR0-5
 7:924400000 ,DR0-5  
 OK
```

## Wan_Pending

Reads the pending downlink state.  
Range: 0 or 1.  
Indicates whether the network server has pending downlink data.  
When *Wan_Tx* receives a downlink after an uplink, *Wan_Pending* reports whether the server has further downlinks queued.  
*Wan_Pending* ＝０
means no pending downlink data on the server.  
*Wan_Pending* ≠０
means downlink data is pending. Send an uplink with *Wan_Tx* to retrieve it promptly; an empty uplink payload is sufficient.  
*Wan_Pending* updates only on downlink reception. Server-side queued data is not reflected until a downlink is received.

```
 >Wan_Tx "1234"  
 @95,1,ABC  
 OK  
 >? Wan_Pending  
 1
 OK  
 >Wan_Tx ""
 @96,1,XYZ  
 OK  
 >? Wan_Pending  
 0
 OK  
 >
```

## Wan_JoinDelay1

Sets or reads JOIN_ACCEPT_DELAY1.  
Range: 500–9999 msec.  
Sets or reads the LoRaWAN JOIN_ACCEPT_DELAY1 duration.

The AS923 default is 5000 msec.  
Applied when *Wan_Join* executes in OTAA mode.  
Has no meaning in ABP mode.  
JOIN_ACCEPT_DELAY1 must match the server setting.  
Normally use the default; change it only when necessary.  
Changes to *Wan_JoinDelay1* are automatically saved to Flash, but *Default*
does not save reset values. After Default, use *Wan_Save*.

## Wan_JoinDelay2

Sets or reads JOIN_ACCEPT_DELAY2.  
Range: 500–9999 msec.  
Sets or reads the LoRaWAN JOIN_ACCEPT_DELAY2 duration.  
The AS923 default is 6000 msec.  
Other details are the same as *Wan_JoinDelay1*.

## Wan_RecvDelay1

Sets or reads RECEIVE_DELAY1.  
Range: 500–9999 msec.  
Sets or reads the LoRaWAN RECEIVE_DELAY1 duration.  
The AS923 default is 1000 msec.  
Applied when *Wan_Join* executes in ABP mode.  
For OTAA, the server supplies RECEIVE_DELAY1 in the join response and this variable is updated accordingly. Manual setting is unnecessary for OTAA.  
RECEIVE_DELAY1 must match the server setting.

Changes to *Wan_RecvDelay1* are automatically saved to Flash, but *Default*
does not save reset values. After Default, use *Wan_Save*.

## Wan_RecvDelay2

Sets or reads RECEIVE_DELAY2.  
Range: 500–9999 msec.  
Sets or reads the LoRaWAN RECEIVE_DELAY2 duration.  
The AS923 default is 2000 msec.  
Other details are the same as *Wan_RecvDelay1*.

## Wan_TimeReq

Sets or reads the DEVICE_TIME_REQ MAC command state.  
Range: 0–3.  
Requests DEVICE_TIME_REQ from the LoRaWAN server or clears the result.  
Also reads the request state and response result.  
Read and write values have the following meanings.

<table style="width:85%;">
<colgroup>
<col style="width: 10%" />
<col style="width: 36%" />
<col style="width: 37%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Value</th>
<th style="text-align: center;">Write</th>
<th style="text-align: center;">Read</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0</td>
<td>Clear (only if the current value is not 1)</td>
<td>No DEVICE_TIME_REQ pending</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
<td>Request DEVICE_TIME_REQ</td>
<td>DEVICE_TIME_REQ pending</td>
</tr>
<tr>
<td style="text-align: center;">2</td>
<td rowspan="3">Cannot be assigned</td>
<td>DEVICE_TIME_ANS received (success)</td>
</tr>
<tr>
<td style="text-align: center;">3</td>
<td>DEVICE_TIME_ANS received (error)</td>
</tr>
<tr>
<td style="text-align: center;">Other</td>
<td>None</td>
</tr>
</tbody>
</table>

```
 >Wan_TimeReq=1  
 OK
 >Wan_Tx Null  
 OK  
 >? Wan_TimeReq  
 2  
 OK  
 >? Clock  
 117504004  
 OK
 >? Datetime(Clock+32400)  
 23/09/22 09:00:29  
 OK
```

## Wan_DutyTime

Reads the transmission restriction time.  
Range: 0 and above.  
Read-only; assignment is not allowed.  
Returns the remaining restriction in milliseconds; it counts down while restricted.  
  
LoRaWAN restricts retransmission after sending to manage channel use.  
For AS923, retransmission follows these limits.

| Period | Permitted total transmission time | DutyCycleTime |
|----|----|----|
| First 1 hour after power-on or reset | Total \< 36 seconds | 36s/1h = 1% |
| Next 10 hours | Total \< 36 seconds | 36s/10h = 0.1% |
| Each 24-hour period after the first 11 hours | Total \< 8.7 seconds | 8.7s/24h = 0.01% |

When retrying a Join-Request with *Wan_Join* after an OTAA timeout,
the restriction is (transmission time / DutyCycleTime) − transmission time, as below.

| Join-Request period | Restriction |
|-----------------------------------------|---------------------------------|
| First 1 hour after power-on or reset | 36.6sec (370ms/1% - 370ms) |
| Next 10 hours | 369.6sec (370ms/0.1% - 370ms) |
| Each 24-hour period after the first 11 hours | 3699.6sec (370ms/0.01% - 370ms) |

Join-Request airtime is approximately 370 msec.  
The 6000 msec response wait after a Join-Request reduces the remaining restriction by that amount when *Wan_Join* returns.
These restrictions do not apply to *Wan_Tx* after a successful join. The elapsed-period counters still apply to rejoining after Unjoin.

For *Wan_Tx*, the server can impose DutyCycleTime through a MAC command. The specified restriction applies after transmission and is readable through *Wan_DutyTime*. Servers may adjust it for frequently transmitting devices; unjoining and rejoining may not clear it.

## Wan_LastTxCh

Reads the last transmit channel.  
Range: 1–16.  
Read-only; assignment is not allowed.

Returns the channel used by *Wan_Join* or *Wan_Tx*.  
The number corresponds to *Wan_ChList*.  
It is 1 if the device started unjoined and has not transmitted since startup.  
On *Wan_Tx*, the channel number (the same as *Wan_LastTxCh*) is written to transmit-buffer offset
+4.

## Wan_DwellStat

Reads the dwell state.  
Range: 0–3.  
Read-only; assignment is not allowed.

The state is the bitwise OR of these values.

| Dwell | Restricted | Unrestricted |
|-------|----------|----------|
| Up    | 1        | 0        |
| Down  | 2        | 0        |

Dwell can be configured through *Ctrl*, but OTAA takes its setting from the server.  
Both uplink and downlink are restricted by default.  
  
Restricted dwell:
LoRa packet airtime is limited to 400 msec or less (default).  
Unrestricted dwell:
LoRa packet airtime is allowed up to 4 seconds.  
   
Typical LoRaWAN networks restrict dwell to reduce channel occupancy per device.

## Wan_BatLevel

Sets or reads the battery level reported by the device to the LoRaWAN network. This value is not obtained from the server.  
Range: 0–255 (default: 0).

| Value | Meaning |
|------:|---------|
| 0 | External power source |
| 1 | Minimum battery level |
| 2–253 | Value proportional to the remaining battery level |
| 254 | Fully charged |
| 255 | Battery level cannot be measured |

When the device receives a `DevStatusReq` MAC command, it reports the configured value to the LoRaWAN network in `DevStatusAns`.  
The device does not calculate this value automatically from its supply voltage. Set it from the application according to the measured result.  
This value is not saved and returns to 0 after a system restart.

---

# Transmit and Receive Buffers

LoRaWAN uses transmit and receive buffers.  
The two buffers are independent.

## Buffer Layout

The buffer layout is shown below.

<table style="width:84%;">
<colgroup>
<col style="width: 8%" />
<col style="width: 12%" />
<col style="width: 31%" />
<col style="width: 32%" />
</colgroup>
<thead>
<tr>
<th rowspan="2" style="text-align: center;">Offset</th>
<th rowspan="2">Name</th>
<th colspan="2">Description</th>
</tr>
<tr>
<th><em>Txd</em></th>
<th><em>Rxd</em></th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0</td>
<td rowspan="4">LinkCounter</td>
<td rowspan="4">UpLinkCounter<br />
(Little endian)</td>
<td rowspan="4"><p>DownLinkCounter</p>
<p>(Little endian)</p></td>
</tr>
<tr>
<td style="text-align: center;">1</td>
</tr>
<tr>
<td style="text-align: center;">2</td>
</tr>
<tr>
<td style="text-align: center;">3</td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td>Ch / Rssi</td>
<td>Channel number (1–16)</td>
<td>Rssi</td>
</tr>
<tr>
<td style="text-align: center;">5</td>
<td>DR</td>
<td colspan="2">Data Rate （0～6）</td>
</tr>
<tr>
<td style="text-align: center;">6</td>
<td>FPort</td>
<td colspan="2">FPort (1–223)</td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td>Length</td>
<td colspan="2">Data length (0–242)</td>
</tr>
<tr>
<td style="text-align: center;">8<br />
～<br />
249</td>
<td>Payload</td>
<td>Transmit data</td>
<td>Received data</td>
</tr>
</tbody>
</table>

Except for *Txd* data and length, fields are set by the system after transmission or on reception. Although these fields can be assigned in *Txd*, they do not affect transmission.  
These are the same memory areas used for P2P buffers.  
Basic buffer access is the same as in P2P mode.

## Txd, Txdw, Txdl

Basic usage is the same as P2P mode and is not repeated here.  
Store the uplink payload for *Wan_Tx* in *Txd*.  
Assigning a string to *Txd* automatically sets the data length (Length) in *Txd(7)*.  
The system writes transmission information to *Txd(0)–(6)* when transmission completes. See Buffer Layout.  
Sending with *Wan_Tx* retains the transmit data, so another *Wan_Tx* sends the same data.

```
 >Txd="12345"  
 OK

 >Wan_FPort=10  
 OK  
 >Wan_Tx  
 OK
```

## Rxd, Rxdw, Rxdl

Basic usage is the same as P2P mode and is not repeated here.  
*Rxd* stores the downlink payload and receive information (Rssi, data rate, FPort) received by *Wan_Tx* or, for Class-C, *Wan_Rx*.  

- *Rxd(7)*: received data length (Length).  
- *Rxd(6)*: port number (FPort).  
- *Rxd(5)* ： Data Rate(DR)  
- *Rxd(4)*: Rssi.  
- *Rxd(0)–(3)*: unused.  

See Buffer Layout.

```
 >Wan_Tx $  
 @-80,55,30313233  
 OK
 >? Rxd  
 01234  
 OK  
 >? Rxd(7)  
 4  
 OK  
 >? Rxd(6)  
 55  
 OK
```

As in P2P mode, "#" can replace Txd and Rxd.  
Assignment accesses Txd; reading accesses Rxd.  
It cannot replace Rxdl, Rxdw, Txdw or Txdl as in “#L(0)=1”.  
  
Example: the left and right operations below are equivalent.
```
 　#(0)=1    ↔  Txd(0)=1  
 　#="1234"  ↔  Txd＝"1234"  
 　A=#(7)    ↔  A=Rxd(7)  
 　Print #   ↔  Print Rxd
```

# LoRa Commands Shared with P2P Mode

BASIC control commands and variables are shared by both modes. Some LoRa communication commands and variables are also shared.

P2P variables such as Sf and Bw
remain accessible in LoRaWAN mode but do not affect it. Some P2P commands run without a Mode error, but their results are not guaranteed. Avoid P2P-only commands and variables in LoRaWAN mode.

## \#? (Display Settings)

Syntax: #?  
Displays LoRaWAN and other settings together.  
The display differs from P2P mode, with some shared fields.  
It also varies slightly with *Wan_JoinMode* and configuration level.

```
 >#?  
 Sn=123456
 DevId=$"70b3d559e01d4dbe"
 Wan_Mode=1[Wan]
 Class=0\[A]
 JoinMode=1[OTAA]*
 TxPwr=0[13dBm]
 Dr=2
 FPort=1
 Cnf=0
 Adr=0
 Trial=1
 DevEui=$"0123456789abcdef"
 JoinEui=$"0123456789abcdef"  
 DevAddr=$12345678
 Ctrl=$00000000
 Echo=1[On]
 Auto=""  
 >
```

## SSave

Syntax: SSave

Saves settings to Flash memory.  
Saved settings are loaded automatically at system startup.  
Both P2P and LoRaWAN
settings are saved regardless of the current mode.  
Saved settings are loaded by SLoad and at startup.  
Does not save LoRaWAN activation-related values. See LoRaWAN Variables.

## SLoad

Syntax: SLoad  
Loads settings from Flash memory.  
Both P2P and LoRaWAN
Restores settings for both modes regardless of the current mode.  
Loads settings saved to Flash with SSave.

LoRaWAN activation-related values are unchanged.

## Default

Syntax: Default \[argument\]

If the argument is omitted or is not -1, behavior is as follows.

Resets LoRaWAN variables and other settings to defaults.  
In LoRaWAN mode, P2P-only variables (*Sf*, *Bw*,
etc.) are not reset.  
Shared LoRaWAN/P2P variables and settings are reset.  
Activation-related variables are unchanged. See LoRaWAN Variables.  
*Default* alone does not save to Flash.  
  
The argument selects the following behavior.

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
Resets both P2P and WAN settings regardless of mode, including settings not initialized by ordinary Default. Clears activation and returns to the initial state.<br />
Saves defaults to Flash; SSave is unnecessary.<br />
Restart after executing this command.</td>
</tr>
</tbody>
</table>

For protected mode, see ***Pass*** in ***LoRa-BASIC for LRA1 Reference Manual***.

# LoRa Variables Shared with P2P Mode

## Rssi

Reads receive Rssi.  
Range: -157 to 0 (no default).  
Set when *Wan_Tx* receives a network downlink.  
*Rssi* means Rssi (Received Signal Strength
Indication), in dBm.  
Updated on each downlink reception.  
For downlink payload data, the same value is stored in *Rxd(4)*
in the receive buffer.  
A downlink without payload, such as an acknowledgment, still updates Rssi, but *Rxd(4)*
is not updated.  
Join-Accept responses to *Wan_Join* and similar frames do not update it.  
*Rssi* is read-only and otherwise behaves as in P2P mode.

```
 >Wan_Tx $  
 @-50,10,beef  
 OK  
 >? Rssi  
 -50  
 OK
```

## Snr

Reads receive Snr.  
Range: -128 to 127 (no default).  
Set when *Wan_Tx* receives a network downlink.  
*Snr* is the Signal-to-Noise Ratio in dB.  
Updated on each downlink reception.  
Downlinks without payload, such as acknowledgments, also update Snr.  
Join-Accept responses to *Wan_Join* and similar frames do not update it.

*Snr* is read-only and otherwise behaves as in P2P mode.

```
 >Wan_Tx $  
 @-50,10,beef  
 OK  
 >? Snr  
 8  
 OK
```

## Stat

*Stat* is shared by LoRaWAN and P2P modes.  
In LoRaWAN mode, it reports LoRaWAN command results and errors.  
The table below shows results for the main commands.

<table style="width:93%;">
<colgroup>
<col style="width: 6%" />
<col style="width: 21%" />
<col style="width: 30%" />
<col style="width: 8%" />
<col style="width: 8%" />
<col style="width: 8%" />
<col style="width: 8%" />
</colgroup>
<thead>
<tr>
<th rowspan="2" style="text-align: center;"><em>Stat</em></th>
<th rowspan="2" style="text-align: center;">Meaning</th>
<th rowspan="2" style="text-align: center;">Display</th>
<th colspan="4" style="text-align: center;">Main <em>Wan_</em>
commands</th>
</tr>
<tr>
<th style="text-align: center;"><em>Join</em></th>
<th style="text-align: center;"><em>Tx</em></th>
<th style="text-align: center;"><em>Rx</em></th>
<th style="text-align: center;"><em>LinkChk</em></th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">0</td>
<td>No error</td>
<td></td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
<td>Incompatible class</td>
<td>*incompatible_class</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">2</td>
<td>Invalid parameter</td>
<td>*invalid_parameter</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">3</td>
<td>Denied</td>
<td>*denied</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td>No free channel</td>
<td>*no_free_ch</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">5</td>
<td>Invalid data length</td>
<td>*invalid_data_length</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">6</td>
<td>Not activated</td>
<td>*not_joined</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
</tr>
<tr>
<td style="text-align: center;">7</td>
<td>Duty-cycle restriction active</td>
<td>*dutycycle_restricted</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">8</td>
<td>Timeout</td>
<td>*timeout</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">9</td>
<td>Receive CRC error</td>
<td>*crc_error</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">10</td>
<td>Data received</td>
<td></td>
<td style="text-align: center;"></td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">11</td>
<td>Busy</td>
<td>*busy</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">12</td>
<td>Other error</td>
<td>*error</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;">〇</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
</tbody>
</table>

BASIC execution continues after a LoRaWAN error (*Stat* ≠
0).  
BASIC syntax or parameter errors, however, interrupt execution.  
  
Additional notes:  
2: “\*invalid_parameter” may occur when assigning a LoRaWAN variable.  
3: “\*denied” occurs when assigning *Wan_Class* or *Wan_JoinMode* while activated.  
9: “\*crc_error” occurs only in P2P, not LoRaWAN mode.

```
 >Wan_Join  
 *timeout  
 OK  
 >Wan_Join  
 *dutycycle_restricted  
 OK  
 >? Stat  
 7  
 OK
 >Wan_Tx
 *not_joined
 OK  
 >? Stat  
 6
 >Wan_Join  
 join_accepted  
 OK  
 >Wan_Tx $  
 @-50,10,303132  
 OK  
 >? Stat  
 10  
 OK
```

## Xtal32k

Selects the low-frequency clock.
Range: 0, 1.  
0: internal RC oscillator.  
1: external crystal.  
Other values are ignored.

Selects the external crystal or internal RC oscillator for the 32.768 kHz clock.  
The clock switches immediately upon assignment.  
SSave saves this setting, which takes effect at system startup.  
Select 1 only with an external 32.768 kHz crystal connected. Otherwise an error occurs; depending on hardware conditions the device may hang.  
Default does not initialize Xtal32k.  

> Additional notes
> Ver1.10.a and earlier required an external crystal for stable LoRaWAN operation.  
> Ver1.10.b and later can operate stably without it.  
> An external crystal slightly reduces idle power consumption.  

> For sleep entered by Deep, the internal RC clock reduces standby power.
> See the hardware manual for crystal connections.

```
 >Xtal32k=0  
 OK  
 >? Xtal32k  
 0  
 OK  
 >Xtal32k=1  
 OK  
 >? Xtal32k  
 1  
 OK
```

## Ctrl

Sets control options.

Range: unrestricted (32 bits).  
Controls system behavior and transmit/receive commands.  
Ctrl is shared with P2P mode.  
Bits and functions are listed below; non-LoRaWAN items are gray.

<table style="width:96%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 81%" />
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
<td style="text-align: center;">0000 2000</td>
<td>Swap LED(PA18) and LED(PA19)</td>
</tr>
<tr>
<td style="text-align: center;">0000 4000</td>
<td>Set LED(PA18) High during transmit BUSY; default is High during transmission</td>
</tr>
<tr>
<td style="text-align: center;">0000 8000</td>
<td>Do not use external XTAL32k for timers; use it only for Deep/Sleep timeouts and Clock</td>
</tr>
<tr>
<td style="text-align: center;">0001 0000</td>
<td>Output the RF 1 MHz clock on PB23 during RF operation only</td>
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
<td>Also save join state to Flash on MAC command transmission/reception, when Flash saving is enabled</td>
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
<td>Set default LoRaWAN uplink/downlink
dwell to unrestricted; effective after reset while unjoined</td>
</tr>
<tr>
<td style="text-align: center;">0200 0000</td>
<td>Use the private LoRaWAN sync word; effective after reset</td>
</tr>
<tr>
<td style="text-align: center;">0400 0000</td>
<td>After a restart caused by wakeup from Deep, skip the startup delay and execute Auto immediately to reduce power consumption; other startups retain the conventional 5-second delay</td>
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
The table uses hexadecimal values, but Ctrl also accepts decimal like ordinary variables.  
  
Note:  
Always set unused or internal-use bits to 0. Behavior with nonzero values is not guaranteed.  
Changing “LoRaWAN uplink/downlink
dwell” requires LoRaWAN knowledge and may conflict with AS923 requirements or server settings.

# Configuration Levels

Configuration levels prevent unintended parameter changes.  
Assign a specific value to *Pass* to enter administrator level.  
See individual variables for settings available at that level.  
Configuration levels guard against accidental assignments; they are not intended as a security mechanism.  
Protected mode prevents inspection of the BASIC program.

## Pass

Sets the configuration level.

See *Pass* in ***[LoRa-BASIC for LRA1 Reference Manual: Basics](lora-basic-manual_basics_en.md)***.
