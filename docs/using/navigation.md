# Navigation & controls

Every Viper screen understands the same five actions:

| Action | Meaning |
|---|---|
| **Prev** | Move to the previous item |
| **Next** | Move to the next item |
| **Select** | Open or confirm |
| **Back** | Leave the current screen or menu |
| **Any key** | Wake the screen, skip the boot intro |

How you trigger them depends on your device.

## Controls by device

### Touchscreen devices (CoreS3 / CoreS3 SE, Core2, StackChan, Tab5, CYD…)

The screen is split into a 3 × 3 grid of touch zones:

```
┌─────────┬─────────┬─────────┐
│         │   Up    │  Back   │
│         ├─────────┼─────────┤
│  Prev   │ Select  │         │
│         ├─────────┤  Next   │
│         │  Down   │         │
└─────────┴─────────┴─────────┘
```

- On boards with a touch footer (Core2), a **PREV · SEL · NEXT** bar is drawn under the screen. Tap its thirds.
- In lists, the `[ x ]` in the top-right corner is Back.
- On CoreS3 the physical **power button** also works as Select (`Ok`).

### Three-button sticks (StickC Plus 1.1, StickC Plus2)

| Button | Action |
|---|---|
| Front **M5** button | Select |
| Upper side button | Prev (hold for 3 s to power off) |
| Lower side button | Next |
| Prev + Next together | Back |

### M5Stack Core (A / B / C buttons)

| Button | Action |
|---|---|
| A (left) | Prev |
| B (middle) | Select |
| C (right) | Next |
| A + C together | Back |

### Cardputer / Cardputer ADV (keyboard)

| Key | Action |
|---|---|
| `;` | Prev / up |
| `.` | Next / down |
| `,` / `/` | Previous / next page in long lists |
| `Enter` or the shoulder **G0** button | Select |
| `` ` `` or `Backspace` | Back |

Text entry uses the physical keyboard directly.

### Single-button devices (AtomS3)

| Press | Action |
|---|---|
| Short press | Next |
| Double press | Prev |
| Hold about ½ s | Select |
| Hold about 1¼ s | Back |

### Rotary encoder (DinMeter)

Turn to move Prev/Next, press to Select. Back is a long press of the knob or the side button.

## The main menu

Viper's main menu is a **carousel**: one app at a time, big icon in the middle.

```
┌ VIPER      ⌂  ▣      87% ┐
╲━━━━━━━━━━━■━━━━━━━━━━━━━╱   ← position track
                     03/14    ← item counter
 ◀  ┌──            ──┐   ▶     ← fang chevrons
    │      icon      │
    └──            ──┘        ← reticle corners
        ▐ WIFI ▌              ← name pill
```

- **Status bar:** the clock (or `VIPER <version>` if the clock isn't set) on the left, active-service icons (SD, GPS, Wi-Fi, Web UI, BLE, WireGuard) in the middle, and the battery on the right.
- **Position track:** the bright segment on the divider shows where you are in the list, and `03/14` shows the exact index.
- Prefer seeing everything at once? Switch **Config → Display & UI → Menu Layout** to **Grid**.
- Hide apps you never use with **Config → System Config → Hide/Show Apps**.

## Lists and submenus

Inside an app, options are shown as a vertical list:

- The **selected** entry sits in a solid green pill.
- Neighbouring entries are shown dimmer above and below it.
- A thin **scroll rail** on the right edge shows your position in long lists.
- Greyed-out entries are unavailable on your hardware or in the current state.

## Keyboard on screen

When Viper needs text (Wi-Fi password, file name…) and your device has no physical keyboard, an on-screen keyboard appears. Move with Prev/Next, Select to type a key, and use the on-screen `OK` / `DEL` / `BACK` keys to finish.

## Screen dimming

After the dim time (**Config → Display & UI → Dim Time**) the backlight lowers, and later the screen turns off. The first key press only wakes the screen and does not trigger an action.
