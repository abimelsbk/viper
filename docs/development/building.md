# Building from source

## Requirements

- **Python 3.10+**
- **Git**
- **PlatformIO Core:** `python -m pip install platformio`
- About **3 GB** of disk for the ESP32 toolchains, downloaded on the first build

## Get the code

```powershell
git clone --recursive https://github.com/abimelsbk/viper.git
cd viper
```

If you cloned without `--recursive`, fetch the libraries in `lib/` with:

```powershell
git submodule update --init --recursive
```

## Build and flash

```powershell
python -m platformio run -e m5stack-cores3            # build
python -m platformio run -e m5stack-cores3 -t upload  # build + flash over USB
```

Replace `m5stack-cores3` with your target from [Supported devices](../hardware/devices.md). The first build takes 10–20 minutes because it downloads toolchains. Later builds are much faster.

**Output:** `build.py` writes a single flashable image, **`Viper-<target>.bin`**, to the project root. Flash it at offset `0x0` (see [Install](../getting-started/install.md)).

## Windows notes

- **Use PowerShell or cmd, not Git Bash.** ESP-IDF's tool installer refuses to run inside MSYS/MinGW shells such as Git Bash and fails with `MSys/Mingw is not supported`.
- **Conflicts with an older PlatformIO setup.** If you already have the classic `espressif32` platform installed, its Arduino framework package can clash with the pioarduino platform Viper uses (error: `argument should be a str … not 'NoneType'` in `arduino.py`). Give Viper its own PlatformIO folder:

  ```powershell
  $env:PLATFORMIO_CORE_DIR = "$env:USERPROFILE\.platformio-viper"
  python -m platformio run -e m5stack-cores3
  ```

- **"No such file" errors for `WString.h`, `Arduino.h` and similar on the first build** usually mean the framework was still being unpacked. Run the build again.
- **No host C compiler** is needed unless you change `src/modules/bjs_interpreter/mqjs_stdlib.c`. The generated `mqjs_stdlib.h` is checked in. To regenerate it, install a GCC (for example MSYS2 `mingw32`) or build in Docker.

## Building every target

```powershell
python -m platformio run
```

builds all environments, which takes a long time. The `docker/` folder has a CI-style container that does the same thing on Linux.

## Project layout

```
boards/        one folder per device: .ini build settings + interface.cpp (buttons, power, display)
include/       shared headers and compile-time defaults
lib/           bundled and submodule libraries
src/core/      UI, menus, settings, display, serial CLI, web interface
src/core/viper_ui.cpp   Viper boot intro, logo and navigation chrome
src/modules/   features: wifi, ble, rf, rfid, ir, fm, gps, lora, nrf24, others, bjs_interpreter (JS)
embedded_resources/web_interface/   web UI (HTML/CSS/JS), embedded at build time
sd_files/      optional content for the SD card
website/       Viper website and web flasher
docs/          this documentation
```

## Adding a new board

Follow `boards/README.md`. Copy `boards/_New-Device-Model/` and fill in the template; every flag in it is documented.

## License reminder

Viper is AGPL-3.0. If you distribute a modified build, you must also offer its complete source code under the same license.
