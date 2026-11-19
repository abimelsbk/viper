# Web interface

Viper has a built-in web app for managing files and controlling the device from a phone or computer.

## Start it

**Files → WebUI**, then choose how to connect:

| Option | When to use |
|---|---|
| **my Network** | Viper joins a saved Wi-Fi network (it asks for one if none is saved). Open the address it shows from any device on that network. |
| **AP mode** | Viper creates its own network, **`ViperNet`** with password **`vipernet`** by default. Join it, then open the shown address. |

The device screen then shows:

```
Url: http://viper.local     ← works on most computers (mDNS)
IP:  192.168.x.x            ← use this if viper.local doesn't resolve
Usr: admin
Pwd: viper
```

Open **WebUI** again to get **Stop WebUI** or to bring the info screen back.

> **Change the default login.** Go to **Config → System Config → Advanced → Network Creds** or edit `webUI` in [`viper.conf`](configuration.md). Anyone on the same network can reach the page.

## What you can do

- **Browse storage:** switch between the SD card and internal flash (LittleFS).
- **Upload / download** files and whole folders, create folders, rename and delete.
- **Edit text files** in the browser: scripts, BadUSB payloads, IR/RF files, `viper.conf`.
- **Run files:** send an IR or Sub-GHz file, run a BadUSB payload or a JavaScript app directly from the file list.
- **Serial Cmd:** type any [serial command](serial-cli.md) and see the output in the browser.
- **Navigator:** a live mirror of the device screen with on-screen Prev / Select / Next / Back buttons, so you can drive Viper remotely.
- **Settings:** change the web login and Wi-Fi credentials.

## Customise its colors

Upload a `/theme.css` file. See [Themes → Web interface colors](themes.md#web-interface-colors).
