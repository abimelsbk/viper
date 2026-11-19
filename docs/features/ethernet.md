# Ethernet

Wired network tools using a **W5500** SPI Ethernet module (for example the M5 LAN unit, or a W5500 breakout wired to the SPI pins under **Config → System Config → Advanced → Set Device pins → W5500 Pins**).

When the menu opens, Viper brings up the link and gets an address by DHCP.

| Entry | What it does |
|---|---|
| **Scan Hosts** | ARP-scan the wired LAN and list devices, then run per-host checks |
| **DHCP Starvation** 🧪 | Request every free DHCP lease to test whether the network's DHCP server and switch protections hold up |
| **MAC Flooding** 🧪 | Send frames from many fake MAC addresses to test a switch's CAM-table limits and port security |

> 🧪 These can disrupt a whole network. Run them only on a lab network or with written permission from the network owner.
