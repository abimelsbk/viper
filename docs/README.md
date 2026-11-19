# Viper documentation

Viper is an open-source ESP32 firmware for authorized security research and hardware hacking. It runs on the M5Stack family and many other ESP32 boards.

> **Use responsibly.** Only test networks, radios, cards and devices that you own or have written permission to assess. See [Legal & safety](legal.md).

## Start here

| | |
|---|---|
| [Install Viper](getting-started/install.md) | Web flasher, esptool, or building from source |
| [First boot](getting-started/first-boot.md) | What you'll see, SD card setup, moving over from Bruce |
| [Supported devices](hardware/devices.md) | Every board target and the modules each one can use |
| [ESP32 DevKit V1 (no screen)](hardware/esp32-headless.md) | Run Viper on a bare ESP32 and control it from your phone |

## Using Viper

| | |
|---|---|
| [Navigation & controls](using/navigation.md) | Buttons, touch zones, the carousel and lists |
| [Themes & look](using/themes.md) | Colors, theme files, custom boot image |
| [Configuration file](using/configuration.md) | Every key in `viper.conf` |
| [SD card layout](using/sd-card.md) | Where Viper reads and writes files |
| [Web interface](using/web-ui.md) | File manager and remote control from a browser |
| [Serial command line](using/serial-cli.md) | Control Viper over USB serial |

## Features (main menu)

| Menu | What's inside |
|---|---|
| [WiFi](features/wifi.md) | Connect/AP, scanners, attacks for testing, sniffer, network tools |
| [BLE](features/ble.md) | Scanner, HID keyboard tricks, beacons, sniffer |
| [Ethernet](features/ethernet.md) | W5500 network tools |
| [RF (Sub-GHz)](features/rf.md) | Record, replay, decode and analyze Sub-GHz signals |
| [RFID / NFC](features/rfid.md) | Read, write, emulate cards and tags |
| [IR](features/ir.md) | TV-B-Gone, learn and replay remotes |
| [FM](features/fm.md) | FM broadcast and spectrum (with an FM module) |
| [NRF24](features/nrf24.md) | 2.4 GHz spectrum and testing tools |
| [LoRa](features/lora.md) | Off-grid text chat |
| [GPS](features/gps.md) | Wardriving and tracking |
| [Wire tools](features/wire-tools.md) | UART, I²C, SPI, JTAG, logic analyzer |
| [Others](features/others.md) | BadUSB, QR codes, microphone, iButton and more |
| [Files & Connect](features/files-connect.md) | File browser, mass storage, device-to-device transfer |
| [Clock](features/clock.md) | Clock and timer |
| [Config](features/config-menu.md) | All settings menus |

## Scripting & development

| | |
|---|---|
| [JavaScript scripting](scripting/javascript.md) | Write your own apps that run from the SD card |
| [Building from source](development/building.md) | PlatformIO setup, targets, troubleshooting on Windows |

## More

- [FAQ](faq.md)
- [Legal & safety](legal.md)

---

Viper is a modified version of [Bruce firmware](https://github.com/pr3y/Bruce) and is licensed under AGPL-3.0.
For deeper background on individual features you can also read the [Bruce wiki](https://wiki.bruce.computer); most features work the same way in Viper.
