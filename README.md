# ☁️ WetterCube

Eine kompakte WLAN-Wetterstation auf Basis des **ESP32-C3 Super Mini**, die aktuelle
Wetterdaten auf einem 1,54"-Farbdisplay (ST7789, 240×240 px) anzeigt.
Dies ist mein erstes eigenes Hardware-Projekt von Grund auf selbst gebaut und mit KI Hilfe programmiert.

Die Wetterdaten kommen kostenlos von [Open-Meteo](https://open-meteo.com) –
kein API-Key nötig.

---

## ✨ Funktionen

- **Aktuelle Temperatur** (Ist- & gefühlte Temperatur)
- **Luftfeuchtigkeit** & **Luftdruck**
- **Windgeschwindigkeit** (km/h)
- **Wetter-Icons** nach WMO-Code (klar, bewölkt, Regen, Schnee, Gewitter, Nebel)
- **Uhrzeit & Datum** per NTP (Zeitzone Europa/Berlin)
- **Screen 2:** 3 Stunden Wetter Forecast
- **Screen 3:** Wind, Luftdruck, gefühlte Temperatur
- **Screen 4:** UV-Index (Max), Sonnenaufgang & Sonnenuntergang
- **Captive-Portal-Setup:** WLAN + Standort bequem per Browser einstellen
- **Regen-Warnung** Roter Screen blinkt 60 Min bevor der angesagte Regen eintreten soll, wird durch Touch berührung quittiert
- **Automatisches Dimmern** Nach 3 Min automatisches Dimmen bis die Touch Funktion genutzt wird

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
| Preferences | WLAN-Daten im Flash speichern |

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
