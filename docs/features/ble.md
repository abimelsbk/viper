# BLE (Bluetooth Low Energy)

Uses the ESP32's built-in Bluetooth radio. No extra hardware is needed.

> Entries marked 🧪 broadcast or connect to other people's devices. Only use them with devices you own or have permission to test.

| Entry | What it does |
|---|---|
| **BLE Scan** | List nearby BLE devices with name, address, signal strength and advertised services |
| **BLE Sniffer** | Watch advertising traffic live and log it for analysis |
| **iBeacon** | Advertise an Apple-format iBeacon with your own UUID, major and minor values |
| **BLE Keyboard** | Viper becomes a Bluetooth keyboard. Type from the device keyboard or the web UI into a paired computer or phone |
| **Bad BLE** 🧪 | Run a BadUSB-style keystroke script over a Bluetooth keyboard connection (scripts in Ducky format; see [Others → BadUSB](others.md#badusb--hid)) |
| **Media Cmds** | Bluetooth media remote: play/pause, next, volume, and so on |
| **Presenter mode** | Slide clicker: next and previous slide over Bluetooth |
| **BLE Spam** 🧪 | Broadcast pairing pop-up advertisements (Apple, Android Fast Pair, Samsung, Windows Swift Pair) to test how phones react to unsolicited pairing prompts |
| **BLE Suite** 🧪 | A Bluetooth security-testing toolkit: scans with service detection (audio, HID, Fast Pair) and runs protocol tests against your own devices |
| **Ninebot** | Bluetooth tool for Ninebot/Segway scooters you own |
| **Disconnect** | Drop the current BLE connection and stop advertising |

## Name and layout

- **Config → System Config → BLE name** changes the name Viper advertises as a keyboard.
- **Config → System Config → BadUSB/BLE** sets the keyboard layout and keystroke delay used by Bad BLE and BLE Keyboard.

## BLE API

**Config → System Config → Toggle BLE API** exposes a Bluetooth service that companion apps can use to control Viper. Leave it off unless you need it.
