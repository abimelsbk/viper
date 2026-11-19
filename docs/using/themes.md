# Themes & look

## Quick color change

**Config → Display & UI → UI Color** offers ready palettes. **Venom** (green on black) is Viper's default. You can also enter custom primary and secondary colors.

Colors are stored as 16-bit **RGB565** hex values, which is how the display hardware works. To convert a normal `#RRGGBB` color:

```
RGB565 = ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3)
#39FF14  →  0x3FE2   (Venom primary)
#1FA80F  →  0x1D41   (Venom secondary)
```

## Menu layout

**Config → Display & UI → Menu Layout** switches between:

- **Carousel**: one big icon at a time (default)
- **Grid**: all apps as tiles on one page

## Theme files

A theme is a folder with a JSON file plus optional images. It can replace menu icons, colors, the border, the LED setup and the boot image and sound. Put the folder on the SD card (for example `/Themes/MyTheme/`), then open **Config → Display & UI → UI Theme** and pick its `.json` file. Image paths in the JSON are relative to that folder.

Example `mytheme.json`:

```json
{
  "priColor": "3FE2",
  "secColor": "1D41",
  "bgColor":  "0000",
  "border": 1,
  "label": 1,
  "gifDuration": 0,

  "wifi": "wifi.png",
  "ble": "ble.png",
  "rf": "rf.gif",
  "rfid": "rfid.png",
  "ir": "ir.png",
  "files": "files.png",
  "config": "config.png",

  "boot_img": "boot.gif",
  "boot_sound": "boot.wav",

  "ledColor": "00FF00",
  "ledBright": 50
}
```

| Key | Meaning |
|---|---|
| `priColor` / `secColor` / `bgColor` | RGB565 hex colors |
| `border` | `1` draws the corner-bracket frame and divider, `0` hides them |
| `label` | `1` draws the app name under theme images |
| `gifDuration` | How long (ms) animated menu icons play |
| `wifi`, `ble`, `ethernet`, `rf`, `rfid`, `fm`, `ir`, `files`, `gps`, `nrf`, `interpreter`, `others`, `clock`, `connect`, `config`, `lora`, `wire` | Image for that app's carousel tile, relative to the theme folder. PNG, JPG or GIF |
| `boot_img` | Image or GIF shown instead of the Viper intro |
| `boot_sound` | WAV played at boot (devices with a speaker) |
| `ledColor`, `ledBright`, `ledEffect`, `ledEffectSpeed`, `ledEffectDirection` | RGB LED setup on boards that have one |

**Tips**
- Size icons to fit your screen's icon area. On a 320 × 240 screen around 200 × 200 px works well.
- Any key you leave out falls back to Viper's built-in drawing.

## Custom boot image (no theme needed)

Put one of these in the root of the SD card or internal storage:

- `boot.jpg`: still image, shown for about 3 seconds
- `boot.gif`: animation

Delete the file to get the Viper serpent intro back.

## Web interface colors

The browser interface reads an optional `theme.css` override. Copy `theme.css.example` from the repo's `embedded_resources/web_interface/` folder, change the variables, and upload it as `/theme.css` to the device:

```css
:root {
  --color: #39ff14;       /* accent */
  --sec-color: #1fa80f;   /* secondary accent */
  --background: #070b07;  /* page background */
}
```
