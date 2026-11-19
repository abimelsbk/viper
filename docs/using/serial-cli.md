# Serial command line

Everything important in Viper can also be driven from a computer over USB serial. This is useful for automation, for headless boards without a screen, and for scripting tests.

## Connect

- Baud rate **115200**, 8-N-1, newline line endings.
- Any serial terminal works: the Arduino IDE serial monitor, PuTTY, `screen`, or `python -m serial.tools.miniterm COM5 115200`.
- The web interface has the same console under **Serial Cmd**.
- On boards with both, **Config → Dev Mode** lets you choose whether the CLI uses USB or a UART pin pair.

Type `help` to list every command on your build. Commands with sub-commands print their own usage when typed without arguments.

## Command reference

Aliases are shown with commas, for example `rf,subghz`. A part in `/` shows optional letters: `br/ight/ness` accepts `br`, `bright` or `brightness`.

### System

| Command | What it does |
|---|---|
| `help`, `?` | List commands |
| `info`, `device_info`, `!` | Board, firmware version, chip info |
| `free` | Free heap and PSRAM |
| `uptime` | Time since boot |
| `date` | Show the clock |
| `clock` | Open the clock app |
| `reboot` / `poweroff` / `sleep` (or `power reboot\|off\|sleep`) | Power control |
| `set/tings` | Show or change a `viper.conf` value |
| `i2c` | Quick scan of the I²C bus |
| `factory_reset` | Reset all settings |

### Screen & UI

| Command | What it does |
|---|---|
| `nav <command> [duration]` (`navigate`, `navigation`) | Press a control: `prev`, `next`, `sel`, `esc`, `up`, `down` |
| `options [n]` (`option`) / `optionsJSON` | List the current menu's options, or run option *n*; JSON form for tools |
| `screen br/ight/ness <0-100>` | Backlight level |
| `screen color rgb <r> <g> <b>` / `screen color hex <rrggbb>` | Set the UI color |
| `loader <cmd> [appname]` | List or open main-menu apps by name |

### Storage

| Command | What it does |
|---|---|
| `ls`, `dir` · `cat`, `type` · `rm`, `del` · `md`, `mkdir` · `rmdir` | Familiar file commands |
| `storage list\|read\|write\|remove\|rename\|copy\|mkdir\|rmdir\|stat\|md5\|crc32\|free` | The same and more |
| `storage ymodem` | Receive a file over YMODEM |
| `md5`, `crc32` | Checksums of a file |

### Radios & I/O

| Command | What it does |
|---|---|
| `ir rx` · `ir tx <protocol> <address> <command>` · `ir tx_raw …` · `ir tx_from_file <path>` · `ir tx_from_buffer` | Receive and send infrared |
| `rf,subghz rx` · `rf tx …` · `rf txp <name>` · `rf scan` · `rf tx_from_file <path>` · `rf tx_from_buffer` · `rf selftest` | Receive and send Sub-GHz, test the radio |
| `rfid,nfc read\|write\|clone\|erase\|info\|save\|loadfile\|reset\|autotest …` | RFID/NFC operations and module self-test |
| `bu,badusb run_from_file <path>` · `badusb run_from_buffer` | Run a keystroke payload |
| `gpio …` | Read and write pins |
| `led r/ed\|g/reen\|b/lue\|rgb\|hex\|br/ight/ness\|effect\|off` | RGB LED control |
| `tone`, `beep` · `play`, `music_player` · `tts`, `say <text>` | Buzzer tones, audio playback, text-to-speech |
| `wifi …` · `arp` · `listen` · `sniffer` | Wi-Fi: connect/status, host scan, TCP listener, packet sniffer |
| `webui` | Start or stop the web interface |

### Wire tools

All of these start with `wire`:

| Command | What it does |
|---|---|
| `wire uart begin\|send\|read\|end\|status` | Talk to a device on the UART pins |
| `wire i2c scan\|read\|write` | I²C bus |
| `wire spi begin\|xfer\|flashid\|end` | SPI bus, including reading a flash chip ID |
| `wire jtag idcode` | Detect JTAG chains |
| `wire cont test <pin_a> [pin_b]` | Continuity between two pins (or `pin_a` to GND) |

### Crypto & scripts

| Command | What it does |
|---|---|
| `crypto encrypt_to_file` · `crypto decrypt_from_file` · `crypto type_from_file` (shortcuts `encrypt` / `decrypt`) | Password-protected files: create, read, or type the secret as keystrokes |
| `js <path>` (also `run`, `interpreter`) · `js run_from_file <path>` · `js run_from_buffer <size>` · `js exit` | Run or stop a JavaScript app |

## Example session

```text
> info
> storage list /ViperIR
> ir tx_from_file /ViperIR/tv_power.ir
> js /scripts/hello.js
> nav next
> nav sel
```
