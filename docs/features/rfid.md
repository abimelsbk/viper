# RFID / NFC

Read, save, write and emulate cards and tags.

## Choose your reader

Set it under **RFID → Config → RFID Module**:

| Module | Frequency | Notes |
|---|---|---|
| **M5 RFID2** unit | 13.56 MHz | Plugs into the Grove I²C port (Port A) |
| **PN532 on I2C / SPI** (and the G33 / G36 pin presets) | 13.56 MHz | The most capable option: read, write, emulate and SRIX |
| **RC522 on SPI** | 13.56 MHz | Cheap, read/write MIFARE Classic and Ultralight |
| **ST25R3916 SPI / I2C** | 13.56 MHz | Advanced front end, better range, NFC emulation |
| 125 kHz reader (EM4100 style) | 125 kHz | Used by **Read 125kHz** |

## Entries

| Entry | What it does |
|---|---|
| **Read tag** | Detect a card, show its type, UID and data, then save to `/ViperRFID/` or write elsewhere |
| **Read 125kHz** | Read low-frequency proximity tags (EM4100 family) |
| **Scan tags** | Keep scanning and log every tag that passes, into `/ViperRFID/Scans/` |
| **Read EMV** | Read the public data a contactless bank card shares (card brand, masked number, expiry). No secret keys are read |
| **Load file** | Open a saved `.rfid` dump to view, write or emulate it |
| **Write NDEF** | Write a URL, text or Wi-Fi record to an NFC tag |
| **Emulate NDEF** | Make Viper act as an NFC tag that phones can read (PN532 / ST25R3916) |
| **Erase data** | Wipe a writable tag |
| **SRIX Tool** | Read and write SRIX4K tags (appears when a PN532 is on I²C) |
| **Add MIF Key** | Add a MIFARE Classic key to `/ViperRFID/keys.conf` for reading protected sectors |
| **Amiibolink** | Send Amiibo dumps to an Amiibolink emulator over BLE |
| **Chameleon** | Control a ChameleonUltra/Lite over BLE |
| **PN532 BLE** | Control a PN532Killer-style reader over BLE |
| **PN532 UART** | Tools for a PN532Killer connected over UART |

> Only clone, write or emulate cards you own or are authorized to test. Copying access cards or payment data you don't own is illegal.

## Files

Dumps are plain-text `.rfid` files in `/ViperRFID/`, close to the Flipper `.nfc` format. Example tag files are in the repo's `sd_files/nfc/`.
