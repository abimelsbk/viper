# FAQ

**My device shows a black screen after flashing.**
Make sure you flashed the build for your exact board, at offset `0x0`. Try `esptool erase_flash`, then flash again.

**The web flasher doesn't list a serial port.**
Use Chrome or Edge on a desktop, try a different (data) USB cable, and install the CH9102/CP210x driver for older StickC boards. Holding the boot button while plugging in forces download mode.

**How do I skip the boot animation?**
Press any key. To skip it permanently, turn on **Config → System Config → InstaBoot**. To replace it, put `boot.jpg` or `boot.gif` on the SD card.

**How do I go back / exit an app?**
It depends on your device. See [Navigation & controls](using/navigation.md). On touchscreens tap the top-right area. On sticks press both side buttons together.

**I used Bruce. Will my stuff work?**
Mostly, yes. Settings (`bruce.conf`) and pin setup migrate automatically, old capture files open, and Bruce JS apps run. Rename SD folders like `/BruceRF` to `/ViperRF`. See [First boot](getting-started/first-boot.md#moving-over-from-bruce).

**My SD card isn't detected.**
Format it as FAT32 (32 GB or less is safest), and check the SD pins under **Config → … → Set Device pins** if you added an external slot.

**`viper.local` doesn't open.**
Some networks and Android phones don't support mDNS. Use the IP address shown on the device instead.

**What's the web interface password?**
`admin` / `viper` until you change it under **Config → System Config → Advanced → Network Creds**. Do change it.

**Why is a feature greyed out or missing?**
It needs hardware your board doesn't have (for example BadUSB needs native USB, RF needs a CC1101), or the feature isn't included in that board's build to save memory.

**Can I write my own apps?**
Yes, in JavaScript. See [JavaScript scripting](scripting/javascript.md).

**Is Viper legal?**
The firmware is a tool. Whether using it is legal depends on what you point it at. Read [Legal & safety](legal.md).
