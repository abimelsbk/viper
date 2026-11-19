# Wire tools

Hardware-hacking helpers for talking to chips and boards over wires. Connect to the target's pins using the board's Grove port or header pins, and **share ground (GND)** with the target.

> ⚠️ Check voltage first. ESP32 pins are **3.3 V only**. Connecting them straight to 5 V signals can damage your device.

| Entry | What it does |
|---|---|
| **UART Terminal** | Serial console to a router, IoT board or another microcontroller. Pick TX/RX pins and baud rate, then type with the keyboard. Sessions are logged in `/Viper/Terminal/` |
| **I2C Bus** | Scan for I²C devices and read or write registers |
| **SPI Bus** | Talk to SPI chips. Read a flash chip's JEDEC ID before dumping it |
| **JTAG Scanner** | Find which pins are JTAG (TCK/TMS/TDI/TDO) and read the chip's IDCODE |
| **Multimeter** | Continuity tester: beeps when two pins are connected |

Each tool has a serial-command version under `wire …` (see [Serial command line](../using/serial-cli.md#wire-tools)).

A logic analyzer is in development but held back from this release.

**Config → System Config → Advanced → I2C Finder** is a quick way to list every I²C device on the default bus.
