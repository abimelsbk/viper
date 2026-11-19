# FM

Needs an **Si4713** FM transmitter module (for example the Adafruit Si4713 breakout) on the I²C pins.

> 🧪 Broadcasting on FM without a licence is illegal in most countries beyond very low power. Keep tests short, low power, and on a frequency nobody is using.

| Entry | What it does |
|---|---|
| **Brdcast std** | Broadcast audio on a standard FM frequency (87.5–108 MHz) that you pick |
| **Brdcast rsvd** | Same, but lets you pick from the extended/reserved range some regions use (down to 76 MHz) |
| **Brdcast stop** | Stop transmitting |
| **FM Spectrum** | Scan the band and show signal levels, or **Auto** scan for stations |
| **Hijack TA** | Send an RDS Traffic Announcement flag, which makes car radios with TA enabled switch to your broadcast. Test only with your own radio |
