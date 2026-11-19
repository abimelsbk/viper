# ESP32 DevKit V1 / WROOM (no screen)

Run Viper on a plain **ESP32 DevKit V1** (ESP32-WROOM-32 module) with nothing attached. There's no display and no buttons: Viper starts its own Wi-Fi access point and you control it from a phone or laptop browser.

Build target: **`esp32-wroom-headless`** → `Viper-esp32-wroom-headless.bin`

## Flash it

```powershell
python -m esptool --port COM5 write_flash 0x0 Viper-esp32-wroom-headless.bin
```

Find your port in Device Manager → Ports. It's listed as "CP210x" or "CH340" (the board's USB-to-serial chip). If no port appears, install that chip's driver. If flashing doesn't start, hold **BOOT** while it says "Connecting…". You can also use the website's **Web Flasher** and choose "ESP32 DevKit V1 / WROOM".

## Use it

1. Power the board (any USB charger or power bank works).
2. On your phone or laptop, join Wi-Fi **`ViperNet`**, password **`vipernet`**.
3. Open **http://172.0.0.1** (or http://viper.local) and log in as **`admin` / `viper`**.
4. Use the web interface:
   - **Navigator:** a live copy of the Viper screen with Prev / Select / Next / Back buttons. Every menu and app works from here.
   - **Serial Cmd:** type [serial commands](../using/serial-cli.md).
   - **File manager:** upload IR/RF files, scripts and payloads, and download captures.

The same commands also work over the USB cable at 115200 baud. At boot, the serial console shows the Viper banner and the AP details.

> **Change the defaults:** Config → System Config → Advanced → **Network Creds** sets the web login and the AP name and password.

### Things to know

- **Wi-Fi tools take over the radio.** The ESP32 has one Wi-Fi radio. Scans, deauth tests, sniffing, Evil Portal and similar features switch it away from `ViperNet`, so your browser disconnects while they run. Rejoin `ViperNet` afterwards, or drive those features over the USB serial console.
- **Storage:** about 190 KB of internal storage. Add a microSD module for captures, IR/RF libraries and scripts.
- **No clock chip:** the time resets on power loss. Connect Viper to the internet once (Wi-Fi menu) to sync it over NTP.
- **Viper always starts the web interface at boot,** even if you change the startup app, so a screenless board can't lock you out.

## Wiring external modules

Defaults (all changeable in **Config → System Config → Advanced → Set Device pins**):

| Module | Pins (DevKit V1 labels) |
|---|---|
| **CC1101** (Sub-GHz) | SCK **D18**, MISO **D19**, MOSI **D23**, CS **D15**, GDO0 **D2** |
| **nRF24L01+** | SCK **D18**, MISO **D19**, MOSI **D23**, CS **D15**, CE **D4** |
| **microSD module** | SCK **D18**, MISO **D19**, MOSI **D23**, CS **D5** |
| **PN532 / RFID2 / Si4713** (I²C) | SDA **D21**, SCL **D22** |
| **GPS / CH9329** (UART) | Module TX → **RX2** (16), module RX → **TX2** (17) |
| **IR LED** (via transistor) / **IR receiver** | TX **D26** / RX **D27** |
| **RF433 TX / RX** | TX **D25** / RX **D33** |

The SPI modules share SCK/MISO/MOSI, and each needs its own CS. Power modules from **3V3** (not VIN), and connect every **GND**.

## What works on this board

| Feature | Status |
|---|---|
| Wi-Fi tools (scan, attacks for testing, Evil Portal, sniffer, host scan, Responder, WireGuard, wardriving) | ✅ (radio caveat above) |
| BLE tools, BLE Keyboard, **Bad BLE** | ✅ |
| Sub-GHz, NRF24, RFID/NFC, IR, FM, LoRa, GPS, Ethernet (W5500) | ✅ with the external module |
| Wire tools: UART terminal, I²C, SPI, JTAG, continuity | ✅ |
| JavaScript apps | ✅ (drawn in the Navigator; fast games are choppy) |
| ESP-NOW Connect, QR codes (shown in the Navigator) | ✅ |
| **USB BadUSB, USB Keyboard, Mass Storage, U2F** | ❌ The classic ESP32 has no native USB. The USB-C port goes to a USB-to-serial chip. Use **Bad BLE** or a **CH9329** adapter on RX2/TX2 |
| Microphone, speaker | ❌ No hardware on the board |
| WiFite, SDR, 5 GHz | ❌ They need ESP32-S3 / C5 chips |

## Want a screen?

Viper also builds for this same module with an SPI display and five buttons:

| Display | Build target | Wiring guide |
|---|---|---|
| 2.4" / 2.8" **ILI9341** 240×320 | `esp-wroom32-ili9341` | `boards/esp-wroom32-ili9341/connections.md` |
| **ST7789** 240×320 | `esp-wroom32-st7789` | `boards/esp-wroom32-st7789/connections.md` |

With a screen you get the full on-device UI (boot intro, carousel and so on), and the web interface still works too. Those builds put the SD card's CS on **D12**. If your board won't boot with the SD module attached, move that CS to another free pin under Set Device pins.
