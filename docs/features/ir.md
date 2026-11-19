# IR (Infrared)

Control TVs, air conditioners and anything else with an IR remote.

## Hardware

- Many devices have an **IR LED built in**: StickC Plus 1.1, StickC Plus2, StickS3, Cardputer, AtomS3 and others.
- **CoreS3, Core2 and Core** need an external IR unit, such as the M5 IR unit on a Grove port.
- To **receive** (learn) you need an IR receiver, such as the M5 IR unit's receiver.

Pick pins under **IR → Config → Ir TX Pin / Ir RX Pin**. On CoreS3 choose **M5 IR Mod** or **Grove W/Y**.

## Entries

| Entry | What it does |
|---|---|
| **TV-B-Gone** | Send power-off codes for hundreds of TV brands, one after another. Choose Region NA or Region EU |
| **Custom IR** | Pick an `.ir` file from the SD card and send any button in it |
| **IR Read** | Learn buttons from a real remote and save them to `/ViperIR/` |
| **IR Jammer** | Flood IR to block remotes nearby (your own equipment) |
| **Config** | TX/RX pins and how many times each code repeats (**Ir TX Repeats**) |

## Files

- `.ir` files use the Flipper Zero IR format, so community libraries work as they are.
- The repo's `sd_files/infrared/` has a large collection of TV codes.
- From the computer: `ir tx_from_file /ViperIR/tv.ir`, or send a single code with `ir tx NEC 0x04 0x08`.
