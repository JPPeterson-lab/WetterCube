# ☁️ WetterCube

Eine kompakte WLAN-Wetterstation auf Basis des **ESP32-C3 Super Mini**, die aktuelle
Wetterdaten auf einem 1,54"-Farbdisplay (ST7789, 240×240 px) anzeigt.
Dies ist mein erstes eigenes Hardware-Projekt von Grund auf selbst gebaut und mit KI Hilfe programmiert.
Das Projekt ist aktuell nur in Deutscher Sprache verfügbar, eine Portierung in Englisch ist geplant für Q3/26.

Die Wetterdaten kommen kostenlos von [Open-Meteo](https://open-meteo.com) –
kein API-Key nötig.

---

## ✨ Funktionen

- **Aktuelle Temperatur** (Ist- & gefühlte Temperatur)
- **Luftfeuchtigkeit** & **Luftdruck**
- **Windgeschwindigkeit** (km/h)
- **Wetter-Icons** nach WMO-Code (klar, bewölkt, Regen, Schnee, Gewitter, Nebel)
- **Uhrzeit & Datum** per NTP (Zeitzone Europa/Berlin)
- **Pollenanzeige** für Birke, Gräser, Erle, Beifuß und Ambrosia
- **Screen 2:** 3 Stunden Wetter Forecast
- **Screen 3:** Wind, Luftdruck, gefühlte Temperatur
- **Screen 4:** UV-Index (Max), Sonnenaufgang & Sonnenuntergang
- **Touch-Taste** TTP223 kapazitiver Touch-Sensor zum Durchschalten der Screens und Quittieren von Warnungen
- **Captive-Portal-Setup:** WLAN + Standort bequem per Browser einstellen
- **Regen-Warnung** Roter Screen blinkt 60 Min bevor der angesagte Regen eintreten soll, wird durch Touch-Berührung quittiert
- **Pollen-Warnscreen** Orangener Screen blinkt bei Belastung Hoch (>30) oder Sehr hoch (>100), wird durch Touch-Berührung quittiert oder setzt sich selbst zurück bei Belastung <30
- **Automatisches Dimmen** Nach 3 Min automatisches Dimmen bis die Touch-Funktion genutzt wird
- **Web-Konfiguration** Einstellungen bequem per Browser anpassen – kein Flashen nötig (siehe unten)

---

## 🎬 Showcase

<video src="https://github.com/user-attachments/assets/ee37fd90-d35f-4771-976b-9f5b130c3df7" controls width="460"></video>

<img src="https://github.com/user-attachments/assets/83afa5cc-405c-47f4-8ed1-395f43120ae1" width="460" alt="WetterCube in verschiedenen Designfarben">

---

## 📸 Screenshots

<table>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/7d44195b-d34a-47a4-9a6a-96ac4a90117f" width="200"><br><sub>Hauptscreen</sub></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/35a868d7-1ee0-4758-ba8e-9ad3a814f9bb" width="200"><br><sub>3h-Forecast</sub></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/33250787-9aff-4c1b-8768-685cf4746f6f" width="200"><br><sub>Luftfeuchte & Luftdruck</sub></td>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/52d5c9d9-0ce6-423f-8494-f2a8a9e3eed4" width="200"><br><sub>UV-Index & Sonnenauf-/-untergang</sub></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/47125d06-c454-4345-929f-5271cdd97e5b" width="200"><br><sub>Pollenbelastung</sub></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/36dc4fa6-f778-45de-9230-ef686976ba54" width="200"><br><sub>Regen-Warnung</sub></td>
  </tr>
  <tr>
    <td align="center"><img src="https://github.com/user-attachments/assets/bcaab0f3-f329-4bdd-8b18-008927d5eb12" width="200"><br><sub>Pollen-Warnung</sub></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/c1873d5d-aa35-45dd-b6f7-010914dcf626" width="200"><br><sub>Web-Konfiguration</sub></td>
    <td align="center"><img src="https://github.com/user-attachments/assets/ba505494-ce5e-46c3-8caf-9a8b7faca99e" width="200"><br><sub>Web-Konfiguration (Detail)</sub></td>
  </tr>
</table>

---

## 🏠 Gehäuse

Als Gehäuse empfehle ich den großartigen Cube von **Keralots** zu nutzen, welchen ihr in der Bambu Makerworld findet:

👉 [Simple Print Monitor – ST7789 1.54" Display Case](https://makerworld.com/de/models/2501721-simple-print-monitor-st7789-1-54-display-case#profileId-2750174)

Schaut euch auch seine weiteren Projekte an: [github.com/Keralots](https://github.com/Keralots)

---

## ⚡ Web-Installer (kein Arduino IDE nötig)

Die Firmware lässt sich direkt im Browser flashen – ganz ohne
Entwicklungsumgebung:

**👉 [WetterCube Web-Installer öffnen](https://jppeterson-lab.github.io/wettercube/)**

> Funktioniert nur in **Google Chrome** oder **Microsoft Edge** (WebSerial API). Der Anleitung folgen, die auf dem Monitor erscheint. Das Flashen ist nur einmalig nötig. Bei Updates kann der Vorgang wiederholt werden – sowie wenn sich das WLAN-Netz bzw. der Zugang ändert oder der Ort, von welchem das Wetter angezeigt werden soll.

### Einrichtung nach dem Flashen

1. Der Cube öffnet das WLAN **`WetterCube-Setup`**
2. Damit verbinden – auf iOS/Android öffnet sich das Portal automatisch, sonst `192.168.4.1` im Browser aufrufen
3. WLAN-Name und Passwort aus der Liste wählen oder manuell eingeben
4. Standort (Stadtname) eingeben → Cube verbindet sich und zeigt sofort das Wetter an

> **Hinweis zu Umlauten:** Bei Städten mit Umlauten bitte den Buchstaben **ohne Punkt** schreiben.  
> Beispiele: `Munchen` statt `München` · `Koln` statt `Köln` · `Dusseldorf` statt `Düsseldorf`

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

### Kabeldiagramm

<img src="https://github.com/user-attachments/assets/ef40fa90-2510-46f7-98e0-1fd44cf1b384" width="460" alt="Kabeldiagramm WetterCube">

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

| Ressource | Urheber | Lizenz / Link |
|---|---|---|
| Wetter-Icons | [Dovora Weather Icons](https://www.dovora.com/resources/weather-icons/) | [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/) |
| Wetterdaten | [Open-Meteo](https://open-meteo.com) | Open-Meteo API (kostenlos) |
| Gehäuse-Design | [Keralots](https://github.com/Keralots) | [Bambu Makerworld](https://makerworld.com/de/models/2501721-simple-print-monitor-st7789-1-54-display-case#profileId-2750174) |

Weitere Details in [CREDITS.md](CREDITS.md).

---

## 📄 Lizenz

Der Quellcode dieses Projekts steht unter der **MIT-Lizenz** – frei nutzbar,
veränderbar und weitergabefähig. Weitere Details in [LICENSE](LICENSE).

Die enthaltenen Wetter-Icons unterliegen der **CC BY-SA 4.0**-Lizenz
(siehe [CREDITS.md](CREDITS.md)).


---
---

# ☁️ WetterCube (English)

A compact Wi-Fi weather station based on the **ESP32-C3 Super Mini**, displaying current weather data on a 1.54" color display (ST7789, 240×240 px).

This is my first self-built hardware project, designed from scratch and programmed with the help of AI.

The project is currently available in German only. An English version is planned for Q3 2026.

Weather data is provided free of charge by [Open-Meteo](https://open-meteo.com) – no API key required.

---

## ✨ Features

- **Current temperature** (actual & feels-like temperature)
- **Humidity** & **air pressure**
- **Wind speed** (km/h)
- **Weather icons** based on WMO weather codes (clear, cloudy, rain, snow, thunderstorms, fog)
- **Time & date** via NTP (Europe/Berlin timezone)
- **Pollen display** for birch, grasses, alder, mugwort, and ragweed
- **Screen 2:** 3-hour weather forecast
- **Screen 3:** Wind, air pressure, and feels-like temperature
- **Screen 4:** Maximum UV index, sunrise & sunset
- **Touch button** TTP223 capacitive touch sensor for switching screens and acknowledging warnings
- **Captive Portal Setup:** Configure Wi-Fi and location conveniently through a web browser
- **Rain Warning:** A red warning screen flashes up to 60 minutes before forecasted rainfall – acknowledged via touch
- **Pollen Warning:** An orange warning screen flashes when pollen levels are High (>30) or Very High (>100) – acknowledged via touch or resets automatically when levels drop
- **Automatic Dimming:** Display dims after 3 minutes of inactivity, wakes on touch
- **Web Configuration:** Adjust settings through a browser – no reflashing required (see below)

---

## 🏠 Case / Enclosure

I recommend using the fantastic Cube enclosure by **Keralots**, available on Bambu Makerworld:

👉 [Simple Print Monitor – ST7789 1.54" Display Case](https://makerworld.com/de/models/2501721-simple-print-monitor-st7789-1-54-display-case#profileId-2750174)

Check out his other projects at: [github.com/Keralots](https://github.com/Keralots)

---

## ⚡ Web Installer (No Arduino IDE Required)

The firmware can be flashed directly from your browser:

**👉 [Open WetterCube Web Installer](https://jppeterson-lab.github.io/wettercube/)**

> Works only in **Google Chrome** or **Microsoft Edge** (WebSerial API). Follow the on-screen instructions. Flashing is only required once and can be repeated for updates or when changing Wi-Fi credentials or location.

### Initial Setup After Flashing

1. The cube creates a Wi-Fi network called **`WetterCube-Setup`**
2. Connect to it – on iOS/Android the portal opens automatically, otherwise open `192.168.4.1` in your browser
3. Select or enter your Wi-Fi name and password
4. Enter your location (city name) → the cube connects and immediately displays weather data

> **Note on special characters:** For cities with umlauts, write the letter **without the dots**.  
> Examples: `Munich` · `Cologne` · `Dusseldorf` instead of `Düsseldorf`

---

## ⚙️ Web Configuration

After the first startup, the cube connects to Wi-Fi and briefly displays its IP address. A browser-based configuration interface is then available – no reflashing or serial monitor required.

### Accessing the Web Interface

**Option 1 – mDNS (recommended)**

```
http://wettercube.local
```

Works on macOS, iPhone, Android, and Windows 11 out of the box.
Older Windows versions require [Bonjour](https://support.apple.com/kb/DL999) (free from Apple).

**Option 2 – IP Address**

The IP is displayed for **5 seconds** on the boot screen after connecting, e.g.:

```
Connected!  IP: 192.168.1.47
```

Enter this address in your browser. The IP may change after a reboot – just restart the cube and note the new address.

### Available Settings

| Category | Setting |
|---|---|
| **Warnings** | Rain warning on/off |
| | Pollen warning on/off |
| | Pollen threshold: Moderate / High / Very High |
| **Screens** | Enable or disable screens 2–5 individually |
| **Display** | Dimming timeout: Off / 1 / 3 / 5 / 10 minutes |

All settings are stored permanently in flash memory.

---

## 🔌 Pin Assignment (ESP32-C3)

| Function | GPIO |
|---|---|
| TFT MOSI (SPI) | 20 |
| TFT SCLK (SPI) | 21 |
| TFT CS | 6 |
| TFT DC | 7 |
| TFT RST | 10 |
| TFT Backlight | 5 |
| Touch Button (TTP223) | 3 |

> Display: **ST7789** · 1.54" · 240×240 px · SPI

---

## 🛠 Libraries Used

| Library | Purpose |
|---|---|
| [LVGL 8.3](https://lvgl.io) | GUI Framework |
| [Arduino_GFX_Library](https://github.com/moononournation/Arduino_GFX) | Display Driver |
| [ArduinoJson](https://arduinojson.org) | JSON Parsing |
| WiFi / HTTPClient / WebServer | Arduino ESP32 Core |
| Preferences | Store Wi-Fi credentials & settings in flash |
| ESPmDNS | Access via `wettercube.local` |

---

## 🙏 Credits

| Resource | Author | License / Link |
|---|---|---|
| Weather Icons | [Dovora Weather Icons](https://www.dovora.com/resources/weather-icons/) | [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/) |
| Weather Data | [Open-Meteo](https://open-meteo.com) | Open-Meteo API (free) |
| Case Design | [Keralots](https://github.com/Keralots) | [Bambu Makerworld](https://makerworld.com/de/models/2501721-simple-print-monitor-st7789-1-54-display-case#profileId-2750174) |

See [CREDITS.md](CREDITS.md) for additional details.

---

## 📄 License

The source code is licensed under the **MIT License** – free to use, modify, and redistribute. See [LICENSE](LICENSE).

The included weather icons are licensed under **CC BY-SA 4.0** (see [CREDITS.md](CREDITS.md)).
