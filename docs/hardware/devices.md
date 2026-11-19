# Supported devices

## M5Stack family

| Device | Build target | Chip | Controls | Native USB (BadUSB, Mass Storage) | Real-time clock |
|---|---|---|---|---|---|
| **CoreS3 / CoreS3 SE** | `m5stack-cores3` | ESP32-S3 | Touchscreen | ✅ | ✅ |
| **Cardputer / Cardputer ADV** | `m5stack-cardputer` | ESP32-S3 | Full keyboard | ✅ | — |
| **StickS3** | `m5stack-sticks3` | ESP32-S3 | Buttons | ✅ | — |
| **StickC Plus2** | `m5stack-cplus2` | ESP32 | 3 buttons | ❌ (use CH9329) | ✅ |
| **StickC Plus 1.1** | `m5stack-cplus1_1` | ESP32 | 3 buttons | ❌ (use CH9329) | ✅ |
| **Core2** | `m5stack-core2` | ESP32 | Touchscreen | ❌ (use CH9329) | ✅ |
| **Core (Basic/Gray/Fire), 16 MB** | `m5stack-core16mb` | ESP32 | A/B/C buttons | ❌ | — |
| **Core (Basic), 4 MB** | `m5stack-core4mb` | ESP32 | A/B/C buttons | ❌ | — |
| **AtomS3** | `m5stack-atoms3` | ESP32-S3 | 1 button | ✅ | — |
| **DinMeter** | `m5stack-dinmeter` | ESP32-S3 | Rotary encoder | ✅ | — |
| **StackChan** | `m5stack-stackchan` | ESP32-S3 | Touchscreen | ✅ | ✅ |
| **Tab5** | `m5stack-tab5` | ESP32-P4 | Touchscreen (+ keyboard) | ✅ | — |
| **Arduino Nesso N1** | `arduino-nesso-n1` | ESP32-C6 | Touchscreen | — | — |

Most have a **built-in IR LED** (sticks, Cardputer, AtomS3). The Core series uses an IR unit on a Grove port instead.

## Other boards

The firmware also builds for:

- **LilyGO:** T-Embed and T-Embed CC1101 (Plus), T-Deck / T-Deck Pro, T-Display S3 (and Pro), T-Watch S3 / Ultra, T-LoRa Pager, T-HMI, T4
- **CYD ("Cheap Yellow Display"):** `CYD-2432S028` and others
- **ESP32 DevKit V1 / WROOM, no screen:** `esp32-wroom-headless`. You control it through its own Wi-Fi AP and the web UI. See [ESP32 DevKit V1 (no screen)](esp32-headless.md)
- **Plain ESP32-WROOM** with an SPI display: `esp-wroom32-ili9341`, `esp-wroom32-st7789`
- **ESP32-S3 dev boards:** `esp32s3-ili9341`, `esp32s3-st7789`, `esp32-s3-n16r8-st7789v`, Seeed XIAO ESP32S3
- **ESP32-C5** (5 GHz Wi-Fi) boards, Elecrow panels, Marauder Mini/Touch, Lilka, Phantom, Mako, Smoochiee and RF Reaper PCBs

Each board folder in `boards/` has its build settings, and many have a `connections.md` wiring guide.

## External modules

| Module | Bus | Used by |
|---|---|---|
| **CC1101** | SPI | [RF](../features/rf.md) |
| **nRF24L01+** | SPI | [NRF24](../features/nrf24.md), BLE Suite |
| **PN532 / RC522 / ST25R3916 / M5 RFID2** | I²C or SPI | [RFID](../features/rfid.md) |
| **Si4713** FM transmitter | I²C | [FM](../features/fm.md) |
| **SX1276 / SX1262** LoRa | SPI | [LoRa](../features/lora.md) |
| **W5500** Ethernet | SPI | [Ethernet](../features/ethernet.md) |
| **GPS** (NEO-6M/8M, ATGM336H, M5 GPS unit) | UART | [GPS](../features/gps.md) |
| **IR LED / receiver** (M5 IR unit) | GPIO | [IR](../features/ir.md) |
| **CH9329** USB-HID adapter | UART | BadUSB on boards without native USB |
| **microSD** | SPI | Storage, if the board lacks a slot |

Set pins under **Config → System Config → Advanced → Set Device pins**. They're saved in `/viperPins.conf`.

### Sharing the SPI bus

On sticks and Cardputer, the CC1101 / NRF24 often share the SPI bus with the SD card. Give each module its own **CS** pin. The repo's `media/connections/` folder has wiring photos for common combinations, for example a CC1101 with an SD card on the StickC.

### CoreS3 / CoreS3 SE ports

| Port | Pins | Typical use in Viper |
|---|---|---|
| **Port A** (red) | GPIO 1 / 2 | I²C units (RFID2, PN532 over I²C, Si4713). Or, as plain GPIO, the IR unit and RF433 TX/RX units: choose **Grove W** / **Grove Y** (or **M5 IR Mod** / **M5 RF433T/R**) in the IR and RF pin menus |
| **Port B** (black) / **Port C** (blue) | GPIO / UART | GPS, CH9329 and other UART devices. Set the pins under **Set Device pins** |

The IR and RF pin menus on CoreS3 also let you pick **any GPIO**, so other wiring works too. SPI modules (CC1101, NRF24, W5500) connect through the M-Bus header or an M5 module base. Set their pins to match your wiring.

> ⚠️ All ESP32 GPIOs are **3.3 V**. Don't feed 5 V signals into them.
