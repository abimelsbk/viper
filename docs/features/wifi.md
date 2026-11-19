# WiFi

The ESP32's built-in 2.4 GHz radio powers this menu. ESP32-C5 boards add 5 GHz.

> Everything marked 🧪 transmits or interferes with other networks. Use it **only on networks you own or are authorized to test**. Deauthentication and jamming are illegal against third-party networks in most countries.

## Connection

| Entry | What it does |
|---|---|
| **Connect to Wifi** | Scan and join a network. Passwords are remembered in `viper.conf` |
| **Start WiFi AP** | Turn Viper into an access point (`ViperNet` by default) |
| **Turn Off WiFi** | Stop all Wi-Fi activity and save power |
| **AP info** | Details of the network you're connected to: IP, gateway, channel, signal |

## Wifi Atks 🧪

Pick a target network from the scan list (or choose a mode that needs none):

| Entry | What it does |
|---|---|
| **Information** | Show the target's BSSID, channel, encryption and signal |
| **Target Atks → Deauth / Deauth Flood / Enhanced Deauth / Station Deauth (Single) / Deauth All Clients / Deauth Target List** | Send 802.11 deauthentication frames to test how clients and the AP handle disconnects |
| **Capture Handshake** | Wait for (or provoke) a WPA 4-way handshake and save it to `/ViperPCAP/handshakes/` for offline password auditing |
| **Clone Portal / Deauth+Clone / Deauth+Clone+Verify** | Copy the target's name into a captive-portal test, optionally checking any password entered against the real network |
| **Beacon SPAM** | Broadcast many fake network names: random, from a list, funny presets, or your own custom SSIDs |
| **Karma Attack** | Answer devices' probe requests for remembered networks, to show how they auto-join |

## Evil Portal 🧪

Run a captive portal to test whether users enter credentials into a fake login page.

- Pick a page: a built-in template, or **Custom Html** to choose any `.html` file from the SD card or LittleFS. Examples are in the repo's `sd_files/portals/`.
- Anything typed into the page appears live on screen and is saved to `/ViperEvilCreds/`.
- Settings under **WiFi → Config**: **Add / Remove Evil Wifi** (saved portal SSIDs), **Evil Wifi Settings** (gateway IP, endpoints), and **Password Mode** (store the full password or only a hint).

## Network tools (need a Wi-Fi connection)

| Entry | What it does |
|---|---|
| **Scan Hosts** | ARP-scan the local network and list devices, with per-host port scan and options |
| **NetCut** 🧪 | ARP-based tool to cut a chosen device off the LAN (test networks only) |
| **Listen TCP / Client TCP** | Simple TCP listener and client, like a mini netcat |
| **SOCKS4 Proxy** | Run a SOCKS4 proxy on the device |
| **TelNET / SSH** | Terminal clients that use the on-screen or physical keyboard |
| **Wireguard** | Join a WireGuard VPN using a config from the SD card. A lock icon appears in the status bar |
| **Responder** 🧪 | LLMNR/NBT-NS poisoning that captures NTLM hashes into `/ViperResponder/` for authorized internal tests |

## Analysis

| Entry | What it does |
|---|---|
| **Sniffer** | Capture raw 802.11 frames to PCAP files in `/ViperSniffer/`, viewable in Wireshark |
| **Channel Analyzer** | Live channel use and signal levels |
| **Dual Band Analyzer** | 2.4 GHz and 5 GHz view on boards that support both |
| **Jam Detect** | Hops channels quickly and alerts when it sees deauth floods or jamming nearby. It's the defensive counterpart to the attacks above |
| **SDR** (Cardputer ADV builds only) | Experimental raw I/Q access to the Wi-Fi radio. Recordings go to `/ViperIQ/`. Other boards are built with `VIPER_NO_SDR_JAMMER` to save memory |

## Other apps

| Entry | What it does |
|---|---|
| **Vipergotchi** | Companion mode for Pwnagotchi devices: shows and interacts with nearby units |
| **WiFite** (ESP32-S3 boards) | Automated WPS audit of your own router (Pixie-Dust style checks) |
| **WiFi Pass Recovery** | Try a word list against **your own** network to recover a forgotten password |

## Config

| Entry | What it does |
|---|---|
| **Change MAC** | Set a custom MAC address (saved as `wifiMAC`) |
| **Add / Remove Evil Wifi**, **Evil Wifi Settings**, **Set Gateway IP**, **Password Mode** | Evil Portal options |
| **Rename /creds**, **Rename /ssid**, **Allow /creds access**, **Allow /ssid access**, **Display endpoints** | Change or lock down the portal's helper URLs |

Further reading: the [Bruce wiki's WiFi page](https://wiki.bruce.computer/features/wifi/) covers the shared features in more depth.
