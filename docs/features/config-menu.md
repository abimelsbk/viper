# Config menu

Everything here is saved to [`viper.conf`](../using/configuration.md) (and pin settings to `viperPins.conf`) as soon as you change it.

## Display & UI

| Entry | What it does |
|---|---|
| **Brightness** | Backlight level |
| **Dim Time** | How long until the screen dims: 10 s, 20 s, 30 s, 60 s or Disabled |
| **Orientation** | Rotate the screen in 90° steps |
| **Menu Layout** | Carousel or Grid main menu ([Navigation](../using/navigation.md)) |
| **UI Color** | Palette presets (**Venom** is the default) or custom colors |
| **UI Theme** | Load a theme folder from the SD card ([Themes](../using/themes.md)) |
| **Calibrate Touch** | Re-align the touchscreen (boards with a resistive touchscreen, such as CYD) |

## LED Config (boards with an RGB LED)

**LED Color**, **LED Effect** (static, breathe, rainbow…) and **LED Brightness**.

## Audio Config

**Sound Volume** for the speaker or buzzer, and sound on/off.

## System Config

| Entry | What it does |
|---|---|
| **InstaBoot: ON/OFF** | Skip the boot intro entirely |
| **WiFi Startup: ON/OFF** | Connect to a saved Wi-Fi network at boot |
| **Startup App** | Open an app (or a specific JS script) automatically after boot |
| **Hide/Show Apps** | Choose which apps appear in the main menu |
| **Clock** | Set time via NTP or manually ([Clock](clock.md)) |
| **Keyboard Language** | On-screen keyboard layout: QWERTY, AZERTY or QWERTZ |
| **Advanced** | Opens the submenu below |

**Advanced:**

| Entry | What it does |
|---|---|
| **Set Device pins** | Pins for external modules: CC1101, NRF24, LoRa, ST25R3916, W5500, SD card, I²C, UART, GPS, BadUSB (CH9329), speaker, buzzer, mic. Includes **I2C Finder** |
| **Toggle BLE API** | Enable the Bluetooth control service for companion apps |
| **BadUSB/BLE** | Keyboard layout and keystroke delay for BadUSB and Bad BLE |
| **BLE name** | Name Viper advertises over Bluetooth |
| **Network Creds** | Change the web-interface login and Viper's AP name and password |
| **Factory Reset** | Erase all settings and start fresh |

## Power

**Deep Sleep**, **Sleep** (screen off, wake on key), **Restart**, **Power Off**.

## Install App Store

Downloads the community **App Store** JavaScript app to `/ViperJS/`. Run it from **Scripts** to browse and install more apps over Wi-Fi. It uses the Bruce community app store server, and the apps are compatible.

## Dev Mode

Developer options, shown once dev mode is enabled: choose whether the serial console uses **USB** or a **UART** pin pair, and turn dev mode off again.

## About

Device model, firmware version, chip, flash and memory info.
