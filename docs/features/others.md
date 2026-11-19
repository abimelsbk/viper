# Others

## QRCodes

Show a QR code on screen.

- **Saved codes:** includes Viper's own network (`ViperNet`), links to the Viper docs and repo, and any codes you add.
- **PIX:** build a Brazilian PIX payment QR.
- **Custom:** type any text or URL, then **Display** it or **Save&Display** it to keep it in `viper.conf`. **Remove** deletes saved ones.

## BadUSB & HID

Needs a board with **native USB**: ESP32-S3 boards such as CoreS3, Cardputer, StickS3, AtomS3 and T-Embed. Classic ESP32 boards (StickC Plus 1.1, Plus2, Core, Core2) can use an external **CH9329** USB-HID adapter instead.

| Entry | What it does |
|---|---|
| **BadUSB** 🧪 | Plug Viper into a computer and it types a keystroke script. Scripts use **Ducky Script** (`STRING`, `DELAY`, `GUI r`, `ENTER`, …) and are chosen from the SD card. Examples are in `sd_files/BadUSB and BlueDucky/` |
| **USB Keyboard** | Use Viper as a regular USB keyboard (most useful on Cardputer) |
| **USB Clicker** | Use Viper as a USB presentation clicker or mouse button |
| **USB U2F** | Viper acts as a basic U2F/FIDO security key, for learning how hardware 2FA works |

The same scripts can run over Bluetooth with **BLE → Bad BLE**. Set the keyboard layout and key delay under **Config → System Config → BadUSB/BLE**.

> 🧪 Only run BadUSB scripts on computers you own or are authorized to test. A script can do anything the logged-in user can.

## Microphone

On boards with a microphone (StickC, Cardputer, CoreS3, Core2…):

- **Spectrum:** live audio frequency display
- **Record:** save WAV recordings to `/ViperMIC/`

## iButton

Read, save and write 1-Wire iButton keys (Dallas DS1990 style) through a 1-Wire probe on a GPIO. Dumps go to `/ViperIButton/`.

## Megalodon

A small built-in game: steer the shark and eat the fish. 🦈
