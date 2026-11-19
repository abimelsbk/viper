# Configuration file (`viper.conf`)

Viper keeps its settings in a JSON file called **`/viper.conf`**. It lives on the SD card when one is inserted, otherwise in internal storage (LittleFS). Changes made in the menus are saved there automatically. You only need to edit the file by hand for bulk changes or to recover from a bad setting.

Pin assignments for external modules are stored separately, in **`/viperPins.conf`** (see [Config menu → Set Device pins](../features/config-menu.md)).

> **Upgrading from Bruce?** If `/viper.conf` doesn't exist but `/bruce.conf` does, Viper renames it on first boot. `brucePins.conf` is handled the same way.

## Editing safely

1. Copy the file to your computer, using the [web interface](web-ui.md) or a card reader.
2. Edit it with any text editor and keep it valid JSON.
3. Put it back and reboot.

If Viper can't parse the file it falls back to defaults. **Config → System Config → Advanced → Factory Reset** also resets everything.

## Keys

### Display & sound

| Key | Default | Meaning |
|---|---|---|
| `priColor` / `secColor` / `bgColor` | `3FE2` / `1D41` / `0` | UI colors (RGB565 hex) |
| `colorInverted` | depends on board | Invert panel colors (some displays need it) |
| `bright` | `100` | Backlight 0–100 % |
| `dimmerSet` | `60` | Seconds before dimming. `0` = never |
| `mainMenuStyle` | `0` | `0` = carousel, `1` = grid |
| `themeFile` / `themeOnSd` | empty | Active theme JSON and where it is stored |
| `soundEnabled` / `soundVolume` | `1` / `100` | Beeps and speaker volume |
| `disabledMenus` | `[]` | App names hidden from the main menu |
| `instantBoot` | `0` | `1` skips the boot intro entirely |
| `keyboardLang` | `QWERTY` | On-screen keyboard layout: `QWERTY`, `AZERTY` or `QWERTZ` |

### RGB LED (boards with an LED)

| Key | Meaning |
|---|---|
| `ledColor`, `ledBright`, `ledEffect`, `ledEffectSpeed`, `ledEffectDirection`, `ledBlinkEnabled` | LED color (hex RGB), brightness, effect and its speed/direction, blink on key press |

### Time

| Key | Default | Meaning |
|---|---|---|
| `tmz` | `0` | Time-zone offset from UTC in hours (e.g. `5.5` for India) |
| `dst` | `false` | Daylight-saving on/off |
| `automaticTimeUpdateViaNTP` | `true` | Sync time from the internet when Wi-Fi connects |

### Wi-Fi & web

| Key | Default | Meaning |
|---|---|---|
| `wifi` | `{}` | Saved networks: `{"SSID": "password"}` |
| `wifiAtStartup` | `0` | Auto-connect to a saved network at boot |
| `wifiAp` | `ViperNet` / `vipernet` | Name and password of Viper's own access point |
| `webUI` | `admin` / `viper` | Web interface login. **Change this.** |
| `webUISessions` | | Remembered browser sessions (managed automatically) |
| `wifiMAC` | empty | Custom MAC address (empty = factory MAC) |
| `TerminalLog` | `true` | Mirror the screen log to the web terminal |
| `evilWifiNames`, `evilWifiEndpoints`, `evilWifiPasswordMode` | | Captive-portal testing settings (see [WiFi](../features/wifi.md)) |

### BadUSB / BLE keyboard

| Key | Default | Meaning |
|---|---|---|
| `badUSBBLEKeyboardLayout` | `0` | Keyboard layout used when typing over USB/BLE |
| `badUSBBLEKeyDelay` | `10` | Delay between keystrokes (ms) |
| `badUSBBLEShowOutput` | `true` | Show each line on screen while a script runs |

### Apps & misc

| Key | Meaning |
|---|---|
| `startupApp` | App to open automatically after boot |
| `startupAppJSInterpreterFile` | JS file to run when the startup app is the interpreter |
| `qrCodes` | Your saved QR codes, shown under **Others → QRCodes** |
| `wigleBasicToken` | WiGLE API token for wardriving uploads |
| `wdgwarsApiKey` | API key for the wdgwars wardriving service |
| `devMode` | Shows the developer options under **Config → Dev Mode** |
