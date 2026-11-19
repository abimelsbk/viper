# Files & Connect

## Files

| Entry | What it does |
|---|---|
| **SD Card** | Browse the SD card (shown only when one is mounted) |
| **LittleFS** | Browse the internal flash storage |
| **WebUI** | Start the [web interface](../using/web-ui.md) for browser file management |
| **Connect** | Device-to-device transfer (see below) |
| **Mass Storage** | Expose the SD card to a computer as a USB drive (ESP32-S3 boards with native USB) |

### The file browser

Select a file to see what you can do with it. The type-specific actions appear first:

| File type | Extra actions |
|---|---|
| `.sub` | **Subghz Tx**: transmit it |
| `.ir` | **IR Tx SpamAll** (send every button) or **IR Choose cmd** |
| `.js` / `.bjs` | **JS Script Run** |
| `.txt` | **BadUSB Run** or **USB HID Type** (boards with native USB) |
| `.enc` | **Decrypt+Show** or **Decrypt+Type**: password-protected secrets |
| `.csv` | **Wigle Upload / Up All**, **WDG Upload / Up All**: send wardriving logs |
| `.jpg` `.png` `.gif` `.bmp` | **View Image** |

Every file also has **View File**, **File Info**, **Rename**, **Copy**, **Delete**, **New Folder**, and **Copy->LittleFS / Copy->SD** to move between storages. After **Copy**, open a folder and choose **Paste**.

## Connect

Viper-to-Viper communication over **ESP-NOW**, a direct peer-to-peer link between ESP32s that doesn't need a router or Wi-Fi network.

| Entry | What it does |
|---|---|
| **Send File** | Pick a file and send it to a nearby Viper that is in **Recv File** |
| **Recv File** | Wait for a file and save it |
| **Send Cmds** | Type [serial commands](../using/serial-cli.md) and run them on another Viper |
| **Recv Cmds** | Accept and run commands sent from another Viper |

Both devices need to be in range (tens of metres indoors) and on the matching screen.
