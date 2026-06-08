# ☁️ WetterCube

Eine kompakte WLAN-Wetterstation auf Basis des **ESP32-C3 Super Mini**, die aktuelle
Wetterdaten auf einem 1,54"-Farbdisplay (ST7789, 240×240 px) anzeigt.
Dies ist mein erstes eigenes Hardware-Projekt von Grund auf selbst gebaut und mit KI Hilfe programmiert.
DAs Projekt ist aktuell nur in Deutscher Sprache verfügbar, eine portierung in englisch ist geplant für Q3/26

Die Wetterdaten kommen kostenlos von [Open-Meteo](https://open-meteo.com) –
kein API-Key nötig.

---

## ✨ Funktionen

- **Aktuelle Temperatur** (Ist- & gefühlte Temperatur)
- **Luftfeuchtigkeit** & **Luftdruck**
- **Windgeschwindigkeit** (km/h)
- **Wetter-Icons** nach WMO-Code (klar, bewölkt, Regen, Schnee, Gewitter, Nebel)
- **Uhrzeit & Datum** per NTP (Zeitzone Europa/Berlin)
- **Pollenanzeige** für Birke, Graeser, Erle, Beifuß und Ambrosia
- **Screen 2:** 3 Stunden Wetter Forecast
- **Screen 3:** Wind, Luftdruck, gefühlte Temperatur
- **Screen 4:** UV-Index (Max), Sonnenaufgang & Sonnenuntergang
- **Captive-Portal-Setup:** WLAN + Standort bequem per Browser einstellen
- **Regen-Warnung** Roter Screen blinkt 60 Min bevor der angesagte Regen eintreten soll, wird durch Touch berührung quittiert
- **Pollen-Warnscreen** Roter Screen blinkt bei Belastung Hoch (>30) oder Sehr hoch (>100), wird durch Touch berührung quittiert oder restettet sich selbst bei Belastung <30
- **Automatisches Dimmen** Nach 3 Min automatisches Dimmen bis die Touch Funktion genutzt wird
- **Web-Konfiguration** Einstellungen bequem per Browser anpassen – kein Flashen nötig (siehe unten)

---

## ⚡ Web-Installer (kein Arduino IDE nötig)

Die Firmware lässt sich direkt im Browser flashen – ganz ohne
Entwicklungsumgebung:

**👉 [WetterCube Web-Installer öffnen](https://jppeterson-lab.github.io/wettercube/)**

> Funktioniert nur in **Google Chrome** oder **Microsoft Edge** (WebSerial API). Der Anleitung folgen dei auf dem Monitor erscheint. Das flashen ist nur einmalig nötig. Bei Updates kann der Vorgang wiederholt werden...sowie wenn sich das WLAN Netz bzw. der Zugang ändert oder der Ort von welchem das Wetter angezeigt werden soll.

### Einrichtung nach dem Flashen

1. Der Cube öffnet das WLAN **`WetterCube-Setup`**
2. Damit verbinden und `192.168.4.1` im Browser aufrufen
3. WLAN-Name, Passwort und Wohnort eingeben → Cube verbindet sich und zeigt sofort das Wetter an

---

## ⚙️ Web-Konfiguration

Nach dem ersten Start verbindet sich der Cube mit dem WLAN und zeigt die IP-Adresse
kurz auf dem Display an. Ab diesem Moment ist ein kleines Konfigurations-Interface
per Browser erreichbar – ganz ohne Flashen oder Serial-Monitor.

### Zugang zum Webinterface

Es gibt zwei Wege:

**Option 1 – mDNS (feste URL, empfohlen)**

```
http://wettercube.local
```

Funktioniert auf **Mac, iPhone und Android** direkt. Unter **Windows 11** ebenfalls.
Ältere Windows-Versionen benötigen [Bonjour](https://support.apple.com/kb/DL999) (Apple, kostenlos).

**Option 2 – IP-Adresse**

Die IP wird nach dem WLAN-Connect **5 Sekunden lang** auf dem Boot-Screen angezeigt, z. B.:

```
Verbunden!  IP: 192.168.1.47
```

Diese Adresse einfach im Browser eingeben. Die IP kann sich nach einem Neustart ändern –
in dem Fall einfach den Cube neu starten und die neue IP ablesen.

### Was sich konfigurieren lässt

| Bereich | Einstellung |
|---|---|
| **Warnungen** | Regen-Warnung ein/aus |
| | Pollen-Warnung ein/aus |
| | Pollen-Schwellwert: Mäßig / Hoch / Sehr hoch |
| **Screens** | Screens 2–5 einzeln aktivieren oder deaktivieren |
| **Display** | Dimm-Timeout: Aus / 1 / 3 / 5 / 10 Minuten |

Alle Einstellungen werden dauerhaft im Flash gespeichert und bleiben nach einem Neustart erhalten.

---

## 🔌 Pinbelegung (ESP32-C3)

| Funktion             | GPIO |
|----------------------|------|
| TFT MOSI (SPI)       | 20   |
| TFT SCLK (SPI)       | 21   |
| TFT CS               | 6    |
| TFT DC               | 7    |
| TFT RST              | 10   |
| TFT Backlight        | 5    |
| Touch-Taste (TTP223) | 3    |

> Display: **ST7789** 1,54" · 240×240 px · SPI

---

## 🛠 Verwendete Bibliotheken

| Bibliothek | Zweck |
|---|---|
| [LVGL 8.3](https://lvgl.io) | GUI-Framework |
| [Arduino_GFX_Library](https://github.com/moononournation/Arduino_GFX) | Display-Treiber |
| [ArduinoJson](https://arduinojson.org) | JSON-Parsing |
| WiFi / HTTPClient / WebServer | Arduino ESP32 Core |
| Preferences | WLAN-Daten & Einstellungen im Flash speichern |
| ESPmDNS | Erreichbarkeit unter `wettercube.local` |

---

## 🙏 Credits

| Ressource | Urheber | Lizenz |
|---|---|---|
| Wetter-Icons | [Dovora Weather Icons](https://www.dovora.com/resources/weather-icons/) | [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/) |
| Wetterdaten | [Open-Meteo](https://open-meteo.com) | Open-Meteo API (kostenlos) |

Weitere Details in [CREDITS.md](CREDITS.md).

---

## 📄 Lizenz

Der Quellcode dieses Projekts steht unter der **MIT-Lizenz** – frei nutzbar,
veränderbar und weitergabefähig. Weitere Details in [LICENSE](LICENSE).

Die enthaltenen Wetter-Icons unterliegen der **CC BY-SA 4.0**-Lizenz
(siehe [CREDITS.md](CREDITS.md)).


# ☁️ WetterCube

A compact Wi-Fi weather station based on the ESP32-C3 Super Mini, displaying current weather data on a 1.54" color display (ST7789, 240×240 px).

This is my first self-built hardware project, designed from scratch and programmed with the help of AI.

The project is currently available in German only. An English version is planned for Q3 2026.

Weather data is provided free of charge by Open-Meteo – no API key required.

---

## ✨ Features

- Current temperature (actual & feels-like temperature)
- Humidity & air pressure
- Wind speed (km/h)
- Weather icons based on WMO weather codes (clear, cloudy, rain, snow, thunderstorms, fog)
- Time & date via NTP (Europe/Berlin timezone)
- Pollen information for birch, grasses, alder, mugwort, and ragweed
- Screen 2: 3-hour weather forecast
- Screen 3: Wind, air pressure, and feels-like temperature
- Screen 4: Maximum UV index, sunrise & sunset
- Captive Portal Setup: Configure Wi-Fi and location conveniently through a web browser
- Rain Warning: A red warning screen flashes up to 60 minutes before forecasted rainfall. The alert can be acknowledged via touch input.
- Pollen Warning Screen: A red warning screen flashes when pollen levels are High (>30) or Very High (>100). It can be acknowledged via touch input or automatically clears when levels drop below 30.
- Automatic Dimming: The display automatically dims after 3 minutes of inactivity and wakes up when the touch button is used.
- Web Configuration: Easily adjust settings through a browser – no reflashing required (see below).

---

## ⚡ Web Installer (No Arduino IDE Required)

The firmware can be flashed directly from your browser without installing any development tools:

👉 Open the WeatherCube Web Installer

https://jppeterson-lab.github.io/wettercube/

> Works only in Google Chrome or Microsoft Edge (WebSerial API required). Follow the instructions shown on screen. Flashing is only required once. The process can be repeated for firmware updates, when changing Wi-Fi credentials, or when relocating the device to display weather for a different location.

### Initial Setup After Flashing

1. The cube creates a Wi-Fi network called WetterCube-Setup
2. Connect to it and open 192.168.4.1 in your browser
3. Enter your Wi-Fi name, password, and location → the cube connects and immediately starts displaying weather data

---

## ⚙️ Web Configuration

After the first startup, the cube connects to Wi-Fi and briefly displays its IP address on the screen. From that point onward, a small browser-based configuration interface becomes available – no reflashing or serial monitor required.

### Accessing the Web Interface

There are two options:

Option 1 – mDNS (Recommended)

```
http://wettercube.local 
```

Works directly on macOS, iPhone, Android, and Windows 11.

Older Windows versions require Bonjour (free from Apple).

Option 2 – IP Address

The IP address is displayed for 5 seconds after connecting to Wi-Fi, for example:

```
IP: 192.168.1.47 
```

Simply enter this address into your browser. The IP address may change after a reboot. If that happens, restart the cube and note the new address.

### Available Settings

| Category | Setting |
|-----------|-----------|
| Warnings | Rain warning on/off |
| | Pollen warning on/off |
| | Pollen threshold: Moderate / High / Very High |
| Screens | Enable or disable screens 2–5 individually |
| Display | Dimming timeout: Off / 1 / 3 / 5 / 10 minutes |

All settings are stored permanently in flash memory and remain available after rebooting.

---

## 🔌 Pin Assignment (ESP32-C3)

| Function | GPIO |
|-----------|------|
| TFT MOSI (SPI) | 20 |
| TFT SCLK (SPI) | 21 |
| TFT CS | 6 |
| TFT DC | 7 |
| TFT RST | 10 |
| TFT Backlight | 5 |
| Touch Button (TTP223) | 3 |

> Display: ST7789 · 1.54" · 240×240 px · SPI

---

## 🛠 Libraries Used

| Library | Purpose |
|-----------|-----------|
| LVGL 8.3 | GUI Framework |
| Arduino_GFX_Library | Display Driver |
| ArduinoJson | JSON Parsing |
| WiFi / HTTPClient / WebServer | Arduino ESP32 Core |
| Preferences | Store Wi-Fi credentials & settings in flash memory |
| ESPmDNS | Access via wettercube.local |

---

## 🙏 Credits

| Resource | Author | License |
|-----------|-----------|-----------|
| Weather Icons | Dovora Weather Icons | CC BY-SA 4.0 |
| Weather Data | Open-Meteo | Open-Meteo API (free) |

See CREDITS.md for additional details.

---

## 📄 License

The source code of this project is licensed under the MIT License – free to use, modify, and redistribute.

For details, see LICENSE.

The included weather icons are licensed under CC BY-SA 4.0 (see CREDITS.md)
