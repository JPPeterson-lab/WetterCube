# Changelog – WetterCube

Alle relevanten Änderungen je Version. Neueste Version zuerst.

---

## [1.7.1] – 2026-06-19

### Behoben
- **Nachtmodus Touch-Wake** – nach Touch kehrt Display nach 15 Sekunden Inaktivität automatisch zur Nachtmodus-Helligkeit zurück; weitere Touches verlängern den Timer; normaler Dimm-Timeout greift im Nachtmodus nicht mehr
- **Config-Seite Hang** – WLAN-Scan wird nicht mehr automatisch beim Laden ausgelöst, sondern nur noch auf Knopfdruck
- **Standort-Formular** – war innerhalb des Haupt-Formulars verschachtelt (ungültiges HTML), dadurch konnte Speichern nicht funktionieren; jetzt eigenes Formular außerhalb

---

## [1.7.0] – 2026-06-14

### Neu
- **WLAN-Wechsel im Web-UI** – Netzwerk direkt über `http://wettercube.local` wechseln, kein Captive Portal nötig. Scan-Button lädt verfügbare Netzwerke, nach Speichern startet der Cube neu und verbindet sich automatisch.
- **Custom Partition Table** – SPIFFS-Partition entfernt (wird nicht genutzt), App-Partitionen von 1,87 MB auf **1,94 MB** vergrößert. Löst das Größenproblem mit ESP32 Arduino Core 3.3.10 dauerhaft.
- **lv_conf.h projektlokal** – `LV_COLOR_16_SWAP` und `LV_MEM_SIZE` werden jetzt projektspezifisch über `build_opt.h` gesetzt (`-DLV_COLOR_16_SWAP=0`). Andere Projekte können abweichende Werte nutzen ohne den WetterCube zu beeinflussen.

### Hinweis
- Erster Flash nach v1.7.0 muss **vollständig** sein (alle 4 Bins via Web-Installer oder USB), danach läuft OTA wieder normal.

---

## [1.6.2] – 2026-06

### Behoben
- **Crash nach WLAN-Verbindung** – LVGL-Animations-Callback feuerte während des ersten HTTP-Requests → Null-Pointer-Zugriff (MTVAL=0x0020). Fix: 600 ms `lv_timer_handler()`-Loop nach `lv_scr_load_anim()` vor HTTP-Calls.
- **Pollen-Warnung NULL-Guard** – `uic_LabelPollenWarnArt` wird vor Zugriff auf `nullptr` geprüft.

---

## [1.6.1] – 2026-06

### Geändert
- OTA-Testrelease – bestätigt funktionierende OTA-Update-Kette (Version → Check → Download → Flash → Neustart).

---

## [1.6.0] – 2026-06

### Neu
- **OTA-Update via GitHub Pages** – Firmware-Update komplett ohne USB unter `http://wettercube.local/update`.
- **Helligkeitsregler** – Slider 10–100 % im Webinterface, wird sofort angewendet, bleibt nach Neustart erhalten.
- **Eigenes Captive Portal** (ersetzt WiFiManager/tzapu) – DNSServer + WebServer, schlanker, kein externer Library-Overhead. Sketch-Größe von 103 % → 99 %.

### Entfernt
- WiFiManager (tzapu) – zu groß für das Partition-Schema mit OTA.

---

## [1.5.1] – 2026

### Geändert
- WiFiManager (tzapu) ersetzt das eigene Captive Portal.

---

## [1.5.0] – 2026

### Neu
- **Web-Konfiguration** – mDNS (`wettercube.local`), IP-Anzeige auf Boot-Screen, Config-UI im Browser.
- Einstellungen: Regen-Warnung, Pollen-Warnung, Pollen-Schwellwert, Screens 2–5, Dimm-Timeout.

---

## [1.4.1] – 2026

### Neu
- **Pollen-Warnscreen** – blinkt orange bei Belastung Hoch/Sehr hoch, Quittierung per Touch.

---

## [1.4.0] – 2026

### Neu
- **Pollen-Screen** – Open-Meteo Air Quality API, 5 Pollenarten (Birke, Gräser, Erle, Beifuß, Ambrosia), Farbindikator je Stufe.

---

## [1.3.1] – 2026

### Behoben
- Forecast-Überlauf nach 23:00 Uhr – `forecast_days=2` und Zeit direkt aus API-Array lesen statt Modulo.

---

## [1.3.0] – 2026

### Geändert
- Display-Helligkeit auf 80 % gesetzt.
- Temperaturfarben auf Forecast-Labels ausgeweitet.
- Screen-Reihenfolge: 1 → 4 → 2 → 3.

---

## [1.2.0] – 2026

### Neu
- **Boot-Screen** mit Ladebalken und IP-Anzeige.
- **3-Stunden-Forecast** (Screen 4).
- **Regen-Warnscreen** – blinkt rot bei Regen in den nächsten 2 Stunden, Quittierung per Touch.

---

## [1.1.1] – 2026

### Neu
- Windrichtung (8 Himmelsrichtungen als Text).
- Display-Dimmen nach Inaktivität.
- Temperatur-Farbwechsel (blau / grün / gelb / rot).
- UI-Überarbeitung.

---

## [1.1.0] – 2026

### Neu
- Screen 3: UV-Index, Sonnenaufgang, Sonnenuntergang.

---

## [1.0.1] – 2026

### Geändert
- UI-Anpassungen.

---

## [1.0.0] – 2026

### Erstveröffentlichung
- WLAN-Verbindung, Wetterdaten (Open-Meteo), LVGL-UI, Captive Portal.
