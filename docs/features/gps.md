# GPS

Needs a serial GPS module (u-blox NEO-6M/8M, M5 GPS unit, ATGM336H and similar) on the UART pins under **GPS → Config → GPS Pins**. The default baud rate is 9600; change it under **Config → Baudrate** if your module differs. Some boards (for example the T-Deck with a GPS add-on) have it wired already.

A GPS icon appears in the status bar once the module has a fix.

## Wardriving

**GPS → Wardriving** logs what's around you together with your position:

| Mode | Logs |
|---|---|
| **Scan WiFi Networks** | Access points: SSID, BSSID, channel, encryption, signal |
| **Scan BLE Devices** | Bluetooth devices |
| **Scan Both** | Both at once |

Logs are saved as **WiGLE-compatible CSV** in `/ViperWardriving/`. You can upload them to:

- **WiGLE** (wigle.net): set your API token as `wigleBasicToken` in [`viper.conf`](../using/configuration.md)
- **WDGoWars** (wdgwars.pl): set `wdgwarsApiKey`

Wardriving only listens to public broadcasts. It doesn't connect to anything. Still, check the local rules on collecting location data.

## GPS Tracker

Records your route as a **GPX** track in `/ViperGPS/`. Open it in Google Earth, GPX viewers or mapping apps.
