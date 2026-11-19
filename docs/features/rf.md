# RF (Sub-GHz)

Work with 300–928 MHz remotes and sensors: garage doors, weather stations, doorbells and similar.

## Hardware

You need a radio module, wired to the pins set under **RF → Config → RF Module**:

| Module | Notes |
|---|---|
| **CC1101** (recommended) | Transmit and receive on 300–348, 387–464 and 779–928 MHz, with adjustable frequency and modulation. Share the SPI bus with the SD card, or use separate pins |
| **Simple 433 MHz TX/RX pair** (e.g. M5 RF433T/R units) | Fixed 433.92 MHz, on/off keying only |

On CoreS3 the simple modules plug into **Port A** (red Grove, GPIO 1/2). Choose **M5 RF433T/R** or **Grove W/Y** under **RF TX Pin** / **RF RX Pin**. See [Supported devices](../hardware/devices.md) for wiring.

> 🧪 Transmitting is regulated. Stay within the bands and power allowed where you live, and only replay signals from devices you own.

## Entries

| Entry | What it does |
|---|---|
| **Scan/copy** | Listen for a signal, show frequency and protocol, and save or replay it |
| **ProtoPirate** | Protocol decoder and encoder for many common remote types. Captures go to `/ViperRF/ProtoPirate/` |
| **Record RAW** | Record the raw pulse timings of any signal to a `.sub` file |
| **Custom SubGhz** | Pick and transmit a `.sub` file from the SD card. Flipper Zero files work |
| **Spectrum** / **RSSI Spectrum** | Live view of activity across the band |
| **SquareWave Spec** | Visualize the on/off pulses of a received signal |
| **Spectogram** | Waterfall view of signal activity over time |
| **Listen** | Turn received on/off keying into sound on devices with a speaker, so you can "hear" remotes |
| **Bruteforce** 🧪 | Cycle through codes for simple fixed-code systems. For testing your own gear's weakness |
| **Jammer** 🧪 | Transmit noise on a frequency. Illegal outside a shielded lab or Faraday enclosure in most countries |
| **Config** | RF module type, TX/RX pins, default frequency |

## Files

- Saved signals: `/ViperRF/*.sub` (Flipper Sub-GHz format).
- Files saved by Bruce (`Filetype: Bruce SubGhz File`) still load.
- From the computer: `rf tx_from_file /ViperRF/door.sub` over [serial](../using/serial-cli.md).
