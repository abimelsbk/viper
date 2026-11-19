# Legal & safety

## Use it responsibly

Viper is made for **learning, research and authorized security testing**. Many of its features can disrupt or access other people's devices and networks. In most countries it is illegal to:

- disconnect, jam or interfere with Wi-Fi, Bluetooth, radio or cellular signals you don't own
- capture, crack or use credentials, handshakes or card data that aren't yours
- transmit outside the frequencies and power levels allowed in your region, or broadcast on FM without a licence
- clone access cards, car keys or payment cards you aren't authorized to copy
- run keystroke-injection (BadUSB) payloads on computers you don't own

Before testing anything that isn't yours, get **written permission** from the owner that defines what you may test. If you're unsure whether something is allowed where you live, assume it isn't. You alone are responsible for how you use Viper.

## Electrical safety

- ESP32 pins are **3.3 V**. Don't connect them to 5 V or mains-powered circuits.
- Disconnect power before wiring modules.
- Lithium batteries in sticks and cores can be damaged by short circuits. Handle them with care.

## License

Viper is free software, licensed under the **GNU Affero General Public License v3.0** (see [`LICENSE`](../LICENSE)).

- You may use, study, change and share it.
- If you distribute Viper, modified or not, or run a modified version as a network service, you must offer its **complete corresponding source code** under the same license.
- It comes with **no warranty**.

## Credits

Viper is a modified version of **[Bruce firmware](https://github.com/pr3y/Bruce)** by pr3y, bmorcelli, IncursioHack and the Bruce contributors. Bruce includes work from many other open-source projects, listed in [`THIRD_PARTY.md`](../THIRD_PARTY.md) with their licenses.

This documentation was written for Viper. The [Bruce wiki](https://wiki.bruce.computer) was used only as a reference.
