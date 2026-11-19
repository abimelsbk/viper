# First boot

## The intro

When Viper starts you'll see the boot intro, which takes about 3.5 seconds:

1. A dim scan grid fades in.
2. The hexagon frame traces itself edge by edge.
3. The serpent's shadow slithers in from tail to head, then fills with bright "venom".
4. `VIPER` types out letter by letter while the tongue flicks.
5. The tagline and firmware version appear.

**Press any key (or tap the screen) to skip it.** If you put a `boot.jpg` or `boot.gif` in the root of the SD card or internal storage, Viper shows that instead. A theme can set its own boot image too (see [Themes](../using/themes.md)).

Devices with a buzzer give a short beep. Devices with a speaker play `boot.wav` from the SD card or internal storage if it's there.

## Then the main menu

After the intro you land on the main menu carousel. See [Navigation & controls](../using/navigation.md) for how to move around.

## Prepare an SD card (recommended)

Most features work without an SD card, but captures, logs, scripts and IR/RF libraries are much more useful with one.

1. Format a microSD card as **FAT32**. Cards of 32 GB or smaller are the most reliable.
2. Optionally copy the contents of the repo's `sd_files/` folder to the card. It has IR remote libraries, sample RF files, BadUSB payloads and example JavaScript apps.
3. Insert the card and reboot. An SD icon appears in the status bar when it is mounted.

Viper creates its working folders (`/ViperRF`, `/ViperIR` and so on) as it needs them. See [SD card layout](../using/sd-card.md).

## Moving over from Bruce

Viper is built from Bruce, so most of your existing setup carries over:

| What | What Viper does |
|---|---|
| `/bruce.conf` settings | Automatically renamed to `/viper.conf` on first boot and loaded |
| `/brucePins.conf` pin setup | Automatically renamed to `/viperPins.conf` |
| Saved Sub-GHz and encrypted-password files with a `Bruce ...` header | Still open normally |
| JavaScript apps using `BRUCE_PRICOLOR`, `getBruceVersion()` and similar | Still work (aliases are kept) |
| SD folders such as `/BruceRF`, `/BruceIR` | **Rename them by hand** to `/ViperRF`, `/ViperIR` and so on |

## Default look

Viper starts in its **Venom** palette: green `#39FF14` on black. You can change it under **Config → Display & UI → UI Color**.
