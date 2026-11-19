# Clock

Shows a large clock. On boards with a real-time clock chip (Core2, CoreS3, StackChan, StickC Plus 1.1 and Plus2) the time survives power-off. Other boards keep time while powered and remember the last known time across reboots.

Press **Select** while the clock is showing to open its menu:

| Entry | What it does |
|---|---|
| **Timer** | Countdown timer with an alarm |
| **Back to Clock** | Return to the clock face |
| **Exit** | Back to the main menu |

## Setting the time

**Config → System Config → Clock** offers:

- **Via NTP Set Timezone**: pick your time zone and sync over the internet (needs Wi-Fi), for example UTC+5:30 for India.
- **Set Time Manually**: set the time by hand.

`automaticTimeUpdateViaNTP` in [`viper.conf`](../using/configuration.md) re-syncs whenever Wi-Fi connects. Once the time is set, the status bar shows the clock instead of `VIPER <version>`.
