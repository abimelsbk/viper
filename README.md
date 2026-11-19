<p align="center"><img src="media/viper/viper-logo.svg" width="140" alt="Viper logo"></p>

# Viper

**Viper** is an open-source ESP32 firmware for authorized offensive-security work and hardware research:
Wi-Fi, BLE, Sub-GHz (CC1101), RFID/NFC, IR, NRF24, BadUSB, GPS/wardriving, a web interface and a JavaScript runtime,
wrapped in a new "venom" interface.


## Documentation

Full guides are in [`docs/`](docs/README.md): [install](docs/getting-started/install.md), [controls](docs/using/navigation.md), [every feature](docs/README.md#features-main-menu), [JavaScript scripting](docs/scripting/javascript.md), [serial commands](docs/using/serial-cli.md) and [building](docs/development/building.md).

## What's new in Viper

- **New identity**: Viper name, serpent-in-hexagon logo, venom-green palette (`#39FF14`) as the default theme.
- **Animated boot intro**: the scan grid fades in, the hexagon traces itself, the serpent slithers in and fills with venom, then `VIPER` types out and the tongue flicks. It is drawn with primitives, so it scales to every screen. Any key skips it, and `boot.jpg`/`boot.gif` on SD still override it.
- **New navigation style**: fang chevrons and reticle corners on the main carousel, a position track with an `03/14` counter, venom "pill" selection in lists, a slim scroll rail, and corner-bracket frames instead of a rounded border.
- **Web UI reskin**: venom palette, grid backdrop, angled buttons, logo in the header and login page.
- **Website**: `website/` is a static landing page plus a USB **web flasher** (ESP Web Tools).
- **Smooth migration from Bruce**: `/bruce.conf` and `/brucePins.conf` are adopted on first boot, saved `Bruce ... File` captures still load, and JS scripts can still use `BRUCE_*` globals and `getBruceVersion()`.

## Supported devices

Every M5Stack target in the tree builds:

| Device | PlatformIO env |
|---|---|
| CoreS3 / CoreS3 SE | `m5stack-cores3` |
| Cardputer / Cardputer ADV | `m5stack-cardputer` |
| StickS3 | `m5stack-sticks3` |
| StickC Plus2 | `m5stack-cplus2` |
| StickC Plus 1.1 | `m5stack-cplus1_1` |
| Core2 | `m5stack-core2` |
| Core (4MB / 16MB) | `m5stack-core4mb` / `m5stack-core16mb` |
| AtomS3 | `m5stack-atoms3` |
| DinMeter | `m5stack-dinmeter` |
| StackChan | `m5stack-stackchan` |
| Tab5 | `m5stack-tab5` |

The tree also has targets for LilyGO (T-Embed, T-Deck, T-Display, T-Watch, T-LoRa Pager), CYD boards, ESP32-WROOM with ILI9341 or ST7789
(`esp-wroom32-ili9341`, `esp-wroom32-st7789`), and generic ESP32-S3 / C5 boards. Wiring is in `boards/<board>/connections.md`.

## Install

**Web flasher**: host `website/` (any static host with HTTPS), put the built `Viper-<env>.bin` files in `website/firmware/`, then open `flasher.html` in Chrome or Edge.

**esptool**:

```sh
esptool.py --port COM5 write_flash 0x0 Viper-m5stack-cores3.bin
```

## Build

```sh
git submodule update --init --recursive
pip install platformio
pio run -e m5stack-cores3            # build
pio run -e m5stack-cores3 -t upload  # build + flash
```

`build.py` writes the merged image `Viper-<env>.bin` to the project root.

## SD card layout

Viper reads and writes `/ViperRF`, `/ViperIR`, `/ViperRFID`, `/ViperPCAP`, `/ViperJS`, `/ViperGPS` and similar folders.
If you are coming from Bruce, rename your `/Bruce*` folders to `/Viper*`.

## Legal

Viper is for education and **authorized** security testing only. Only use it on networks, radios and devices you own or have
written permission to test. You are responsible for complying with local laws.

## Credits

- Original project: **Bruce firmware**, https://github.com/pr3y/Bruce, by pr3y, bmorcelli, IncursioHack and all Bruce contributors.
- Third-party libraries and their licenses: see [THIRD_PARTY.md](THIRD_PARTY.md).
- License: GNU **AGPL-3.0**. If you distribute Viper binaries or run it as a network service, you must also offer the
  corresponding source code, including your modifications.
