<h1>
LoRa-BASIC for LRA1<br/>
Reference Manual<br/>
Maintenance Level
</h1>

English edition translated from the [Japanese original](lora-basic-manual-maintenance_jp.md). Command, function, and system-variable names are case-insensitive. This manual uses the spelling emitted by List and registered in the source code.

# Overview

LoRa-BASIC provides commands and variables for hardware calibration during device manufacturing.  
To prevent unintended operation, these commands and settings require the maintenance configuration level.  

This manual describes the commands and writable variables available at maintenance level.

---

# Configuration Levels

LoRa-BASIC provides the following configuration levels.  

### Normal level (Level 0)

The device enters normal level after power-on or a system reset.  
At normal level, administrator- and maintenance-level commands and variable settings are unavailable.

### Administrator level (Level 1)

This level protects user parameters against unintended changes.  
See the relevant reference manual for the variables that can be set.

### Maintenance level (Level 2)

This level is intended for factory calibration and hardware adjustment.  
Use by end users is not recommended.  
Maintenance level includes administrator level: all administrator operations are available.  


To change the configuration level, assign a specific value to the [*Pass* variable](#pass).  
See the *Pass* variable for details.

---

# Commands

## Txif

Syntax: Txif \[string\]

Transmits packets periodically.  
The argument is the same as for Send.  
Set the transmission interval with Period.

## TxWave

Syntax: TxWave \[expression\]

Transmits a continuous RF signal.  
An argument of 10 or greater selects the normal command behavior: transmit a preamble for the specified duration.  
At maintenance level (Level 2), an argument of 4 or less is also allowed.  
The argument selects the following transmission pattern.

<table style="width:70%;">
<colgroup>
<col style="width: 13%" />
<col style="width: 18%" />
<col style="width: 37%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">Argument</th>
<th>Signal</th>
<th>Transmission pattern</th>
</tr>
</thead>
<tbody>
<tr>
<td style="text-align: center;">&lt; 0</td>
<td>Burst (PN9)</td>
<td>Continuous transmission</td>
</tr>
<tr>
<td style="text-align: center;">0</td>
<td rowspan="5">Unmodulated</td>
<td>Continuous transmission</td>
</tr>
<tr>
<td style="text-align: center;">1</td>
<td>Transmit for 4 seconds, pause for 100 msec</td>
</tr>
<tr>
<td style="text-align: center;">2</td>
<td>Transmit for 4 seconds, pause for 5 msec</td>
</tr>
<tr>
<td style="text-align: center;">3</td>
<td>Transmit for 400 msec, pause for 4 msec</td>
</tr>
<tr>
<td style="text-align: center;">4</td>
<td>Transmit for 400 msec, pause for 125 usec</td>
</tr>
<tr>
<td style="text-align: center;">5 or greater</td>
<td>Preamble transmission</td>
<td>Specified duration (available in normal mode)</td>
</tr>
</tbody>
</table>

## Sens

Syntax: Sens \[expression\]  

Measures and displays Rssi periodically (every 200 msec).  
Expression = 0: peak; = 1: average.  
If omitted, the argument defaults to 0.  
Reception uses LoRa or FSK mode as selected by Modem.  
(Normal carrier sensing always uses FSK, regardless of Modem.)

The bandwidth follows Bw.

---

# Variables

The following variables can be set only at maintenance level, except for *Pass*.  
They can also be read at normal level.

## Pass

Sets the configuration level.  
Range: unrestricted.  
- 999999: enter administrator level (Level 1).  
- 123456: enter maintenance level (Level 2).  

Any other value selects normal level.  

Reading the variable returns:  
- Normal level = 0  
- Administrator level = 1  
- Maintenance level = 2  

```
 >? Pass
 0
 OK
 >Pass=999999
 OK
 >? Pass
 1
 OK
```

## Level

Carrier-sense threshold.  
Range: -127 to 0.  
Sets the carrier-sense threshold.

The unit is dBm; the default is -76 dBm.  
Ssave does not save this value. Reset or power-on restores the default.

## Drift

Frequency offset.  
Range: -200 to 200.  
Sets the frequency offset in units of 100 Hz. Note that Frq uses a different unit.  
The RF chip (SX1276) PLL setting resolution is 32MHz/2^19
(61 Hz).  
The value is saved to Flash memory when assigned.

## Sn

Serial number.  
Range: 100000 to 999999.  
The serial number of this module.  
The value is saved to Flash memory when assigned.

## Period

Txif transmission interval.  
Range: 0 and above.  
Sets the Txif transmission interval in msec.  
The interval excludes transmission time: it runs from the end of one transmission to the start of the next.  
Ssave saves the value; it is loaded automatically at system startup.

## Frq

Transmit/receive frequency.

Range: 920600 to 928000.  
Sets the transmit/receive frequency.  
The unit is 1 kHz.  
Setting Ch automatically sets Frq to the frequency corresponding to that channel.  
Ssave saves the value; it is loaded automatically at system startup.
