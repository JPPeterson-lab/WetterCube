# WetterCube – Entwicklungs-Log

Dieses Dokument protokolliert alle Funktionen, Entscheidungen und Code-Änderungen
des WetterCube-Projekts. Es dient als Referenz und Backup des Entwicklungsverlaufs.

---

## Projekt-Grundlage

| Eigenschaft | Wert |
|---|---|
| Hardware | ESP32-C3 |
| Display | ST7789, 1,54", 240×240 px, SPI |
| GUI-Framework | LVGL 8.3 |
| Display-Treiber | Arduino_GFX_Library |
| Wetter-API | Open-Meteo (kostenlos, kein API-Key) |
| UI-Design | SquareLine Studio 1.6.1 |
| Repo | https://github.com/JPPeterson-lab/wettercube |
| Web-Installer | https://jppeterson-lab.github.io/wettercube/ |

---

## Hardware-Pinbelegung

| Funktion | GPIO |
|---|---|
| TFT MOSI (SPI) | 20 |
| TFT SCLK (SPI) | 21 |
| TFT CS | 6 |
| TFT DC | 7 |
| TFT RST | 10 |
| TFT Backlight (PWM) | 5 |
| Touch-Taste TTP223 | 3 |

---

## Wichtige Konfiguration

### lv_conf.h (Bibliotheksordner)
```c
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0   // 0 = korrekt für Arduino_GFX + ST7789
```

### ui.c – Kompiliercheck dauerhaft deaktiviert
SquareLine Studio generiert bei jedem Export einen `#error`-Check für
`LV_COLOR_16_SWAP`. Dieser muss nach jedem Export manuell auskommentiert werden:
```c
// #if LV_COLOR_16_SWAP !=0
//     #error "LV_COLOR_16_SWAP should be 0 ..."
// #endif
```
**Merke:** Nach jedem SLS-Export → `ui.c` öffnen → Check auskommentieren.

---

## Screen-Übersicht

| Screen | Inhalt | currentScreen |
|---|---|---|
| ScreenBoot | Boot-Screen mit Ladebalken | – |
| Screen1 | Hauptscreen: Temp, Uhrzeit, Icon, Wind, Windrichtung | 1 |
| Screen4 | 3-Stunden-Forecast mit Icons + Temps | 4 |
| Screen2 | Luftfeuchtigkeit, Luftdruck, gefühlte Temp | 2 |
| Screen3 | UV-Index, Sonnenaufgang, Sonnenuntergang | 3 |
| uiScreenWarnung | Regen-Warnung, blinkend rot | 99 |

**Touch-Reihenfolge:** 1 → 4 → 2 → 3 → zurück zu 1

---

## Implementierte Funktionen

### Boot-Screen (`showBootScreen()`)
- Erscheint beim Start vor Screen 1
- Zeigt "WetterCube" in zwei Farben (Wetter weiß, Cube himmelblau)
- "by Jan Althaus"
- Ladebalken (`uic_BarWifi`) füllt sich während WLAN-Verbindung
- Nach erfolgreicher Verbindung: 4 Sekunden stehen bleiben, dann Fade zu Screen 1
- Bei Fehler: Meldung + Setup-Portal öffnen

### WLAN & Captive Portal
- SSID, Passwort, Standort werden im Flash gespeichert (Preferences)
- Kein WLAN oder kein Standort → Access Point "WetterCube-Setup" + Webformular
- Geocoding via Open-Meteo API (Stadtname → Koordinaten)
- Koordinaten werden dauerhaft gespeichert

### Wetter-Abruf (`fetchWeather()`)
- Intervall: alle 15 Minuten
- API: Open-Meteo forecast
- **Current:** temp, humidity, feelsLike, wind speed+direction, pressure, weather_code
- **Daily:** uv_index_max, sunrise, sunset
- **Hourly:** temperature_2m, weather_code (für Forecast + Regenwarnung)
- JSON-Buffer: 6144 Bytes
- Timezone: auto

### Temperatur-Farbwechsel (`setTempColor()`)
Gilt für Screen 1 und alle 3 Forecast-Labels:
| Temperatur | Farbe |
|---|---|
| unter 8°C | Blau `#00AAFF` |
| 8–14°C | Grün `#00CC44` |
| 15–23°C | Gelb `#FFCC00` |
| ab 24°C | Rot `#FF3300` |

### Wetter-Icons (`setWeatherIcon()`)
Wiederverwendbare Funktion für Main-Screen und Forecast-Screen:
| WMO-Code | Icon |
|---|---|
| 0, 1 | day_clear |
| 2, 3 | overcast |
| 45, 48 | fog |
| 51–67, 80–82 | rain |
| 71–77, 85, 86 | snow |
| ≥95 | thunder |

### Windrichtung (`getWindDirection()`)
8 Himmelsrichtungen als Text: N, NO, O, SO, S, SW, W, NW
Berechnung: `(deg + 22.5) / 45.0) % 8`

### 3-Stunden-Forecast (Screen 4)
- Nächste 3 Stunden ab aktueller Uhrzeit
- Je Stunde: Uhrzeit (HH:00), Wettericon, Temperatur mit Farbwechsel
- Widgets: `uic_LabelH1/2/3Zeit`, `uic_ImageH1/2/3`, `uic_LabelH1/2/3Temp`

### Regen-Warnung (`uiScreenWarnung`)
- Prüft stündliche Wettercodes für die nächsten 2 Stunden
- Auslöser: WMO 51–67 (Regen), 80–82 (Schauer), ≥95 (Gewitter)
- Anzeige: roter Screen blinkt zwischen `#CC0000` und `#660000` (alle 500ms)
- Bestätigung: Touch-Button → zurück zu Screen 1
- Reset: wenn kein Regen mehr vorhergesagt → kann erneut warnen
- Widgets: `uic_ImageWarnRegen`, `uic_LabelWarnTitel`, `uic_LabelWarnDetail`, `uic_LabelWarnHint`

### Display-Dimmen
- Normal: 80% Helligkeit (PWM 204/255)
- Timeout: 3 Minuten Inaktivität → 20% (PWM 51/255)
- Aufwecken: Touch-Taste → erste Betätigung nur aufhellen, kein Screen-Wechsel
- Pin: GPIO 5, PWM via `ledcAttach()` (ESP32 Arduino Core 3.x API)

### Uhrzeit & Datum (`updateClock()`)
- NTP: `europe.pool.ntp.org`
- Timezone: `CET-1CEST,M3.5.0,M10.5.0/3`
- Update: jede Sekunde
- Format Uhrzeit: `HH:MM`
- Format Datum: `Mo. 01.06.2025`

---

## Bekannte Eigenheiten

- **LV_COLOR_16_SWAP** muss nach jedem SLS-Export in `ui.c` auskommentiert werden
- Unicode-Pfeile (↑↗→) werden von Montserrat nicht unterstützt → Klartext-Richtungen verwenden
- ESP32 Arduino Core 3.x: `ledcSetup()`/`ledcAttachPin()` → ersetzt durch `ledcAttach(pin, freq, res)`
- JSON-Buffer muss groß genug sein (6144 Bytes) da hourly-Daten 24 Einträge liefern
- Web-Installer braucht Chrome oder Edge (WebSerial API)
- Nach GitHub-Repo privat/öffentlich Wechsel: GitHub Pages manuell reaktivieren unter Settings → Pages

---

## Versions-Historie

| Version | Inhalt |
|---|---|
| 1.0.0 | Erste Firmware: WLAN, Wetter, LVGL, Captive Portal |
| 1.0.1 | UI-Anpassungen |
| 1.1.0 | Screen 3 (UV, Sunrise, Sunset), Long-Press Reset (später entfernt) |
| 1.1.1 | Windrichtung, Display-Dimmen, Temperatur-Farbwechsel, UI-Überarbeitung |
| 1.2.0 | Boot-Screen, 3-Stunden-Forecast (Screen 4), Regen-Warnung |

---

*Zuletzt aktualisiert: Juni 2025*
