# Install Viper

There are three ways to get Viper onto a device. Each of them **erases the existing firmware**.

## Which file do I need?

Every board has its own build, named `Viper-<target>.bin`. Find your target in [Supported devices](../hardware/devices.md). For example:

| Device | File |
|---|---|
| M5Stack CoreS3 / CoreS3 SE | `Viper-m5stack-cores3.bin` |
| M5StickC Plus2 | `Viper-m5stack-cplus2.bin` |
| M5 Cardputer / ADV | `Viper-m5stack-cardputer.bin` |

Each `.bin` is a complete image (bootloader, partitions and app) and is written at address **0x0**.

## Option 1: Web flasher (easiest)

1. Use **Chrome or Edge** on a desktop computer. Firefox and Safari can't talk to USB serial ports.
2. Open the Viper website and go to **Web Flasher**.
3. Plug in the device with a USB cable that carries data, not a charge-only cable.
4. Pick your board, press **Install Viper**, and choose the serial port when the browser asks.
5. Wait for "Installation complete", then press the device's reset button.

**If the port doesn't appear:**
- Try another cable or USB port.
- Hold the boot/G0 button while plugging in to force download mode. On CoreS3, hold the reset button for about 3 seconds until the green LED lights.
- Older StickC boards need the **CH9102** or **CP210x** USB driver on Windows.

## Option 2: esptool (command line)

Install esptool once:

```powershell
python -m pip install esptool
```

Find the port: on Windows, Device Manager → Ports (COM & LPT); on Linux it is usually `/dev/ttyACM0` or `/dev/ttyUSB0`. Then flash:

```powershell
python -m esptool --port COM5 write_flash 0x0 Viper-m5stack-cores3.bin
```

To wipe the flash completely first, which helps if something behaves oddly after an upgrade:

```powershell
python -m esptool --port COM5 erase_flash
```

## Option 3: Build it yourself

See [Building from source](../development/building.md). In short:

```powershell
git clone --recursive https://github.com/abimelsbk/viper.git
cd viper
python -m platformio run -e m5stack-cores3 -t upload
```

## Updating

Flash the new `.bin` the same way. Your settings live in `viper.conf` on the SD card (and a copy in internal flash), so they survive updates unless you erase the flash.

## Coming from M5Launcher

If you manage apps with M5Launcher, you can install `Viper-<target>.bin` from the SD card through Launcher like any other app.
