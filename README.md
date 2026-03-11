<div align="center">

# 💡 HueDial

### An ESP8266-powered Philips Hue room controller  
**with a rotary encoder, 240×240 display, and one-press toggle**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-ESP8266-blue.svg)](https://github.com/esp8266/Arduino)
[![Home Assistant](https://img.shields.io/badge/Requires-Home%20Assistant-41BDF5?logo=home-assistant)](https://www.home-assistant.io)
[![Arduino](https://img.shields.io/badge/IDE-Arduino-00979D?logo=arduino)](https://www.arduino.cc)

<br/>

![HueDial Demo](docs/demo.gif)
*← Replace with your own photo/gif*

</div>

---

## ✨ Features

- 🔆 **Scroll through all your Hue rooms** with a buttery smooth rotary encoder
- 🌡️ **Adjust brightness** in 5% steps — changes debounced and sent to HA after 300 ms
- 🔘 **One-press toggle** on/off with the dedicated KEY0 button
- 📺 **Crisp 240×240 colour display** — room list, brightness bar, on/off badge
- 📡 **Auto-discovers** all `light.*` entities from Home Assistant — no manual config list
- 🔄 **Live sync** — polls HA every 5 seconds in the detail view to catch external changes
- 🔒 **Secrets-safe** — `config.h` is in `.gitignore`; only the `.example` file is committed

---

## 🛒 Hardware

| Part | Notes |
|------|-------|
| **ESP8266** (Wemos D1 Mini or NodeMCU) | Any ESP8266 board works |
| **1.54" ST7789 240×240 SPI display** | [Common AliExpress board](https://aliexpress.com) |
| **EC11 rotary encoder** | With built-in push button |
| **Momentary push button** (KEY0) | Any tactile switch |
| Breadboard + jumper wires | |

---

## 🔌 Wiring

```
ESP8266 (D1 Mini)          ST7789 Display
──────────────────         ──────────────
D7 / GPIO13  (MOSI)  ──►  SDA / MOSI
D5 / GPIO14  (SCLK)  ──►  SCL / SCLK
D2 / GPIO4   (CS)    ──►  CS
D1 / GPIO5   (DC)    ──►  DC / A0
D0 / GPIO16  (RST)   ──►  RES / RST
3.3V                 ──►  VCC
GND                  ──►  GND

ESP8266                    EC11 Encoder
──────────────────         ────────────
D6 / GPIO12          ──►  CLK  (A)
D3 / GPIO0           ──►  DT   (B)
D4 / GPIO2           ──►  SW   (push button)
GND                  ──►  GND

ESP8266                    KEY0 Button
──────────────────         ───────────
D8 / GPIO15          ──┐  one side
GND                  ──┘  other side  (uses INPUT_PULLUP)
```

> All GPIO pins are configurable in `config.h`.

---

## 🚀 Setup

### 1. Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) 1.8+ or 2.x
- [Home Assistant](https://www.home-assistant.io) with Philips Hue integration
- A **Long-Lived Access Token** from HA  
  *(HA → Your Profile → Long-Lived Access Tokens → Create Token)*

### 2. Install Libraries

Open **Sketch → Manage Libraries** and install:

| Library | Version |
|---------|---------|
| **TFT_eSPI** by Bodmer | latest |
| **ArduinoJson** by Benoit Blanchon | 6.x |

### 3. Configure TFT_eSPI

Copy `User_Setup.h` (in this repo) to your `TFT_eSPI` library folder, **replacing** the existing one:

```
<Documents>/Arduino/libraries/TFT_eSPI/User_Setup.h
```

### 4. Configure the firmware

```bash
cp config.h.example config.h
```

Edit `config.h`:

```cpp
#define WIFI_SSID        "YourNetworkName"
#define WIFI_PASSWORD    "YourPassword"
#define HA_BASE_URL      "http://192.168.1.100:8123"   // your HA IP
#define HA_TOKEN         "ey..."                        // HA long-lived token
```

### 5. Select board & upload

1. **Tools → Board** → `LOLIN(WEMOS) D1 R2 & mini` (or your board)
2. **Tools → Upload Speed** → `921600`
3. Open `lightController.ino`, click **Upload**

---

## 🎮 Controls

| Input | Room List screen | Room Detail screen |
|-------|------------------|--------------------|
| **Rotary CW/CCW** | Scroll through rooms | Adjust brightness ±5% |
| **Encoder push** | Enter room detail | Go back to room list |
| **KEY0 button** | Refresh room list from HA | Toggle light on/off |

---

## 🗂 Project Structure

```
lightController/
├── lightController.ino     # Main sketch – state machine & WiFi
├── config.h                # ⚠️ Your secrets (gitignored)
├── config.h.example        # Template to copy
├── encoder.h / .cpp        # ISR-driven EC11 + KEY0 handler
├── ha_client.h / .cpp      # Home Assistant REST API client
├── display.h / .cpp        # TFT_eSPI drawing helpers
├── User_Setup.h            # TFT_eSPI config for ST7789 240×240
└── README.md
```

---

## 🏗 How It Works

```
[Boot]
  ├── Init display → show splash
  ├── Connect WiFi
  └── Fetch all light.* entities from HA

[Room List]
  ├── Rotary encoder scrolls the list
  ├── Encoder push → enter detail for selected room
  └── KEY0 → refresh list from HA

[Room Detail]
  ├── Shows name, on/off badge, brightness % and bar
  ├── Rotary encoder adjusts brightness (debounced 300ms before sending to HA)
  ├── KEY0 → toggle on/off instantly
  ├── Encoder push → back to list
  └── Auto-polls HA every 5s to sync external changes
```

---

## ⚙️ Customisation

| Setting | Location | Default |
|---------|----------|---------|
| Brightness step per tick | `config.h` | `5` % |
| Brightness send debounce | `config.h` | `300` ms |
| HA poll interval | `config.h` | `5000` ms |
| Max lights fetched | `config.h` | `20` |
| Entity filter prefix | `config.h` | `"light."` |
| All GPIO pins | `config.h` | see Wiring table |

---

## 📜 License

MIT © [dbzx6r](https://github.com/dbzx6r)
