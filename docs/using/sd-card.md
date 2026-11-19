# SD card layout

Format the card as **FAT32**. Viper creates these folders the first time a feature needs them.

| Folder / file | Used by | Contents |
|---|---|---|
| `/viper.conf` | Config | All settings ([details](configuration.md)) |
| `/viperPins.conf` | Config | External module pin assignments |
| `/boot.jpg`, `/boot.gif`, `/boot.wav` | Boot | Optional custom splash and sound |
| `/theme.css` | Web UI | Optional web interface colors |
| `/ViperRF/` | RF | Recorded and saved Sub-GHz signals (`.sub`) |
| `/ViperRF/ProtoPirate/` | RF → ProtoPirate | Decoded protocol captures |
| `/ViperIQ/` | RF / SDR | Raw I/Q recordings |
| `/ViperIR/` | IR | Learned remotes (`.ir`) |
| `/ViperRFID/` | RFID | Dumps (`.rfid`), `keys.conf` for MIFARE keys |
| `/ViperRFID/Scans/` | RFID | Scan logs and EMV reads |
| `/ViperRFID/SRIX/` | RFID → SRIX Tool | SRIX4K dumps |
| `/ViperIButton/` | Others → iButton | iButton dumps |
| `/ViperPCAP/` | WiFi | Packet captures and `handshakes/` |
| `/ViperSniffer/` | WiFi → Sniffer | Sniffer captures |
| `/ViperEvilCreds/` | WiFi → Evil Portal | Credentials entered in portal tests |
| `/ViperResponder/` | WiFi → Responder | Captured NTLM hashes |
| `/ViperWardriving/` | GPS → Wardriving | WiGLE-format CSV logs |
| `/ViperGPS/` | GPS → Tracker | GPX tracks |
| `/ViperMIC/` | Others → Microphone | WAV recordings |
| `/ViperJS/` | Scripts | Downloaded and bundled JavaScript apps |
| `/scripts/` or `/ViperScripts/` | Scripts | Your own scripts. The Scripts menu looks in `/scripts`, `/ViperScripts` and `/ViperJS` |
| `/Viper/Terminal/` | UART terminal | Session logs |
| `/ViperWebUI/` | Web UI | Web interface working files |

## Starter content

The repository's `sd_files/` folder has ready-to-use content you can copy to the card:

- `infrared/`: IR codes for many TVs and other devices
- `BadUSB and BlueDucky/`: example keystroke payloads
- `interpreter/`: example JavaScript apps (plus `ProtoPirate.js` and `xFlipper.js` in the root)
- `portals/`: example captive-portal pages for Evil Portal tests
- `themes/`: example theme folders
- `nfc/`, `wifi/`, `reverseshell/`, `pwnagotchi/`: sample files for those features

## File formats

Viper reads and writes Flipper-compatible text formats for Sub-GHz (`.sub`), IR (`.ir`) and RFID/NFC (`.rfid`/`.nfc`). Files made on a Flipper Zero usually work as they are.
