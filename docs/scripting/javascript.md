# JavaScript scripting

Viper has a built-in JavaScript engine (based on MicroQuickJS), so you can write your own tools, games and automations without recompiling the firmware.

## Run a script

- **Scripts** menu: lists `.js` files in `/scripts`, `/ViperScripts` and `/ViperJS`. Pick one with **Load…** to run any file.
- **File browser:** select a `.js` file → **JS Script Run**.
- **Serial / web UI:** `js /scripts/hello.js`.
- **At boot:** **Config → System Config → Startup App** → JS interpreter, then pick the file.

Leave a script by handling Back in your own code (see below), or with `js exit` over serial.

## Hello, Viper

```js
// /scripts/hello.js
var display  = require('display');
var keyboard = require('keyboard');
var dialog   = require('dialog');

var green = display.color(57, 255, 20);       // Venom green
display.fill(display.color(0, 0, 0));
display.setTextColor(green);
display.setTextSize(2);
display.drawString('Hello, Viper!', 10, 20);
display.drawString('SEL: beep  BACK: exit', 10, 50);

while (true) {
  if (keyboard.getSelPress()) {
    require('audio').tone(880, 120);
    dialog.message('Ssssss…');
  }
  if (keyboard.getEscPress()) break;          // Back leaves the script
  delay(20);                                  // keep the CPU cool
}
```

## The basics

- **Modules:** load them with `require('<name>')`. They also exist as globals (`display`, `dialog`…), so `require` is optional, but it keeps scripts readable.
- **Globals:** `delay(ms)`, `now()`, `random(min, max)`, `setTimeout` / `setInterval` / `clearTimeout` / `clearInterval`, `parse_int`, `to_string`, `to_hex_string`, `to_lower_case`, `to_upper_case`, `atob` / `btoa`, `exit()`, `console.log()` (prints to serial), `__dirpath` (the script's own folder).
- **Theme colors:** `VIPER_PRICOLOR`, `VIPER_SECCOLOR`, `VIPER_BGCOLOR` and `VIPER_VERSION`. Scripts written for Bruce can keep using `BRUCE_*` names and `device.getBruceVersion()`.
- **Language:** a compact ES5-style JavaScript. Use `var`/`function` and plain loops. Not every modern feature is there; for example optional chaining (`?.`) isn't supported.

## Modules

### `display`: drawing

| Function | Notes |
|---|---|
| `color(r, g, b)` | Make a color value |
| `fill(color)` | Clear the screen |
| `width()`, `height()`, `getRotation()` | Screen size and orientation |
| `setTextColor(c)`, `setTextSize(n)`, `setTextAlign(h, v)`, `setCursor(x, y)` | Text style. `h` = `'left'`/`'center'`/`'right'`, `v` = `'top'`/`'middle'`/`'bottom'` |
| `drawString(text, x, y)`, `drawText(text, x, y)`, `print()`, `println()` | Draw text |
| `drawPixel`, `drawLine`, `drawWideLine`, `drawFastHLine`, `drawFastVLine` | Lines |
| `drawRect`, `drawFillRect`, `drawFillRectGradient`, `drawRoundRect`, `drawFillRoundRect` | Rectangles |
| `drawCircle`, `drawFillCircle`, `drawTriangle`, `drawFillTriangle`, `drawArc` | Shapes |
| `drawBitmap`, `drawXBitmap`, `drawJpg(path, x, y)`, `drawGif`, `gifOpen` | Images |
| `createSprite(w, h)` | Off-screen buffer for flicker-free animation. Has the same drawing calls plus `pushSprite()` / `deleteSprite()` |
| `getBrightness()`, `setBrightness(n)`, `restoreBrightness()` | Backlight |

### `keyboard`: input

| Function | Notes |
|---|---|
| `getPrevPress()`, `getNextPress()`, `getSelPress()`, `getEscPress()`, `getAnyPress()` | `true` once per press. Pass `true` to read "is held" instead |
| `setLongPress(bool)` | Allow held buttons to repeat |
| `getKeysPressed()` | Raw keys on keyboard boards (Cardputer, T-Deck) |
| `keyboard(default, maxLen, title)`, `numKeyboard(…)`, `hexKeyboard(…)` | Ask the user for text, a number or hex |

### `dialog`: ready-made UI

`message(text)`, `info`, `success`, `warning`, `error`, `choice([[label, value], …])` returns the chosen value, `prompt(title)`, `pickFile(startPath)`, `viewFile(path)`, `viewText(text)`, `createTextViewer(text)` (scrollable viewer object), `drawStatusBar()`.

### `menu`

`show(…)`, `showMainBorder()`, `showMainBorderWithTitle(title)`, `printTitle(t)`, `printSubtitle(t)`, `displayMessage(t)`: Viper-styled screens and lists.

### `storage`: files

`read(path)`, `write(path, data[, mode])`, `readdir(path)`, `rename(from, to)`, `remove(path)`, `mkdir(path)`, `rmdir(path)`, `spaceSDCard()`, `spaceLittleFS()`.

### `device`

`getName()`, `getBoard()`, `getModel()`, `getBatteryCharge()`, `getBatteryDetailed()`, `getFreeHeapSize()`, `getEEPROMSize()`.

### `audio` and `notification`

- `audio.tone(freqHz, ms[, nonBlocking])`, `audio.playFile(path)`
- `notification.blink(ms)`: flash the LED or screen

### `led` (RGB LED boards)

`setColor(r, g, b)`, `setBrightness(n)`, `blink(…)`, `off()`.

### `wifi`

`connected()`, `connect(ssid, password)`, `connectDialog()`, `scan()`, `disconnect()`, `httpFetch(url, options)`, `getMACAddress()`, `getIPAddress()`.

### `ble`

`scan()`, `advertise(…)`, `stopAdvertise()`.

### `ir`

`read(timeoutSec)`, `readRaw(timeoutSec)`, `transmit(protocol, address, command)`, `transmitFile(path)`.

### `subghz` (CC1101)

`setFrequency(MHz)`, `read(timeoutSec)`, `readRaw(timeoutSec)`, `transmit(…)`, `transmitFile(path)`, `txSetup` / `txPulses` / `txEnd` for custom pulse trains, and `protoInfo` / `protoDecode` / `protoEncode` / `protoTransmit` for the ProtoPirate decoders.

### `rfid`

`read()`, `readUID()`, `write(…)`, `save(path)`, `load(path)`, `clear()`, `addMifareKey(key)`, plus `srixRead` / `srixWrite` / `srixWriteBlock` / `srixSave` / `srixLoad` / `srixClear` for SRIX4K tags.

### `badusb` (native USB boards)

`setup()`, `print(text)`, `println(text)`, `press(key)`, `hold(key)`, `release(key)`, `releaseAll()`, `pressRaw(code)`, `runFile(path)`.

### `mic`

`recordWav(path, seconds)`, `captureSamples(…)`.

### Hardware buses

- `gpio`: `pinMode`, `digitalRead`, `digitalWrite`, `analogRead`, `analogWrite`, `touchRead`, `dacWrite`, `ledcAttach` / `ledcWrite` / `ledcWriteTone` / `ledcFade` / `ledcChangeFrequency` / `ledcDetach`, `pins()`
- `serial`: `print`, `println`, `readln`, `write`, `cmd(command)` to run a [serial command](../using/serial-cli.md) from JS
- `uart`: `begin(baud, rx, tx)`, `write`, `read`, `available`, `flush`, `end`
- `i2c`: `begin(sda, scl)`, `scan()`, `write`, `read`, `writeRead`
- `nrf24`: `begin`, `isConnected`, `setChannel`, `send`, `receive`

### `runtime`

`toBackground()`, `toForeground()`, `isForeground()`: keep a script running in the background while you use the rest of Viper (see `sd_files/interpreter/timer_background.js`).

## Examples and the App Store

- The repo's `sd_files/interpreter/` has games (snake, space shooter, dino), tools (IR/RF helpers, a calculator, crypto prices) and `Example1.js`, which shows button handling.
- **Config → Install App Store** downloads a JS app store with many community apps.

Argument details can differ slightly between modules. When in doubt, read the native implementation in `src/modules/bjs_interpreter/<module>_js.cpp`. For deeper explanations of the shared API, the [Bruce interpreter docs](https://wiki.bruce.computer/interpreter/) are a good reference.
