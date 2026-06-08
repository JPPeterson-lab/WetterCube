#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <time.h>
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ui.h>

// --- SPEICHER & WEB-SERVER ---
Preferences preferences;
WebServer server(80);

String ssid      = "";
String password  = "";
String location  = "";   // Stadtname (nur für Anzeige gespeichert)
float  latitude  = 0.0;
float  longitude = 0.0;

unsigned long lastWeatherUpdate = 0;
const unsigned long weatherInterval = 900000; // 15 Minuten
bool setupMode = false;

// --- HARDWARE BUTTON (TTP223) ---
#define TOUCH_PIN 3
int currentScreen = 1;
unsigned long lastTouchTime = 0;

// --- REGEN-WARNUNG ---
bool regenWarnungAktiv       = false;
bool regenWarnungBestaetigt  = false;
unsigned long lastBlinkTime  = 0;
bool blinkState              = false;

// --- DISPLAY DIMMEN ---
#define BL_CHANNEL     0
#define BL_FREQ        5000
#define BL_RESOLUTION  8          // 8-bit → 0-255
#define BL_BRIGHT      204        // 80%
#define BL_DIM         51         // 20%
#define DIM_TIMEOUT    180000UL   // 3 Minuten in ms
bool isDimmed = false;
unsigned long lastActivityTime = 0;

// --- DISPLAY HARDWARE PINS ---
#define TFT_MOSI 20
#define TFT_SCLK 21
#define TFT_CS   6
#define TFT_DC   7
#define TFT_RST  10
#define TFT_BL   5

Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 0, true, 240, 240, 0, 0);

static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
    lv_disp_flush_ready(disp);
}

// Funktions-Deklarationen
void checkTouchButton();
void updateClock();
void fetchWeather();
String getWindDirection(int deg);
bool geocodeLocation(const String& city);
void setWeatherIcon(lv_obj_t* img, int wmoCode);
void setTempColor(lv_obj_t* label, float temp);
void fetchPollen();
void setPollenLabel(lv_obj_t* label, float value);
void updateWeatherIcon(int wmoCode);
void startSetupPortal();
void drawSetupScreen();
void handleRoot();
void handleSave();
void showBootScreen();

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("--- CUBE START ---");

    pinMode(TOUCH_PIN, INPUT);
    ledcAttach(TFT_BL, BL_FREQ, BL_RESOLUTION);
    ledcWrite(TFT_BL, BL_BRIGHT);
    lastActivityTime = millis();

    gfx->begin();
    gfx->fillScreen(0x0000);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    ui_init();

    // Daten aus dem Flash-Speicher laden
    preferences.begin("wettercube", false);
    ssid      = preferences.getString("ssid", "");
    password  = preferences.getString("password", "");
    location  = preferences.getString("location", "");
    latitude  = preferences.getFloat("lat", 0.0);
    longitude = preferences.getFloat("lon", 0.0);

    // Setup-Modus wenn kein WLAN oder kein Standort gespeichert
    if (ssid.length() < 4 || location.length() < 2) {
        setupMode = true;
        startSetupPortal();
        return;
    }

    showBootScreen();
    if (setupMode) return;

    Serial.println(" Verbunden!");
    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "europe.pool.ntp.org");

    // Falls noch keine Koordinaten gespeichert, Geocoding durchführen
    if (latitude == 0.0 && longitude == 0.0) {
        geocodeLocation(location);
    }

    fetchWeather();
    fetchPollen();
}

void loop() {
    if (setupMode) {
        server.handleClient();
        delay(20);
        return;
    }

    lv_timer_handler();
    delay(5);

    checkTouchButton();

    // --- Display dimmen nach Inaktivität ---
    if (!isDimmed && millis() - lastActivityTime >= DIM_TIMEOUT) {
        ledcWrite(TFT_BL, BL_DIM);
        isDimmed = true;
    }

    // --- Regen-Warnung blinken ---
    if (regenWarnungAktiv && millis() - lastBlinkTime >= 500) {
        lastBlinkTime = millis();
        blinkState = !blinkState;
        lv_obj_set_style_bg_color(ui_uiScreenWarnung,
            blinkState ? lv_color_hex(0xCC0000) : lv_color_hex(0x660000),
            LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    static unsigned long lastTimeUpdate = 0;
    if (millis() - lastTimeUpdate >= 1000) {
        lastTimeUpdate = millis();
        updateClock();
    }

    if (millis() - lastWeatherUpdate >= weatherInterval) {
        lastWeatherUpdate = millis();
        fetchWeather();
        fetchPollen();
    }
}

// --- BUTTON ---

void checkTouchButton() {
    if (digitalRead(TOUCH_PIN) == HIGH) {
        lastActivityTime = millis();
        if (isDimmed) {
            ledcWrite(TFT_BL, BL_BRIGHT);
            isDimmed = false;
            return;
        }
        // Regen-Warnung bestätigen
        if (regenWarnungAktiv) {
            if (millis() - lastTouchTime > 500) {
                lastTouchTime = millis();
                regenWarnungAktiv = false;
                regenWarnungBestaetigt = true;
                lv_scr_load_anim(ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, false);
                currentScreen = 1;
            }
            return;
        }
        if (millis() - lastTouchTime > 500) {
            lastTouchTime = millis();
            if (currentScreen == 1) {
                lv_scr_load_anim(ui_Screen4, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
                currentScreen = 4;
            } else if (currentScreen == 4) {
                lv_scr_load_anim(ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
                currentScreen = 2;
            } else if (currentScreen == 2) {
                lv_scr_load_anim(ui_Screen3, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
                currentScreen = 3;
            } else if (currentScreen == 3) {
                lv_scr_load_anim(ui_ScreenPollen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
                currentScreen = 5;
            } else {
                lv_scr_load_anim(ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
                currentScreen = 1;
            }
        }
    }
}

// --- SETUP PORTAL ---

void startSetupPortal() {
    WiFi.softAP("WetterCube-Setup");
    drawSetupScreen();
    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.begin();
}

void drawSetupScreen() {
    gfx->fillScreen(0x0000);
    gfx->setTextColor(0x05ED); gfx->setTextSize(2); gfx->setCursor(20, 25); gfx->println("WETTER-CUBE");
    gfx->drawFastHLine(20, 50, 200, 0x31A6);
    gfx->setTextColor(0xFFFF); gfx->setTextSize(2); gfx->setCursor(20, 70); gfx->println("1. Verbinde WLAN:");
    gfx->setCursor(20, 100); gfx->setTextColor(0x07E0); gfx->println("WetterCube-Setup");
    gfx->setTextColor(0xFFFF); gfx->setCursor(20, 145); gfx->println("2. Oeffne IP:");
    gfx->setCursor(20, 175); gfx->setTextColor(0x05ED); gfx->println("192.168.4.1");
}

void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<style>body{font-family:Arial; background:#121212; color:#fff; text-align:center; padding:20px;}";
    html += ".card{background:#1e1e1e; padding:30px; border-radius:15px; display:inline-block; text-align:left; box-shadow: 0 4px 10px rgba(0,0,0,0.5); width:90%; max-width:400px;}";
    html += "input[type=text], input[type=password]{width:100%; padding:10px; margin:10px 0 20px 0; border-radius:5px; border:1px solid #333; background:#2a2a2a; color:#fff; box-sizing:border-box;}";
    html += "input[type=submit]{background:#00adb5; color:#fff; border:none; padding:12px 20px; border-radius:5px; cursor:pointer; width:100%; font-size:16px;}";
    html += "input[type=submit]:hover{background:#007a80;}</style></head><body>";
    html += "<h2>WetterCube Einrichtung</h2>";
    html += "<div class='card'><form action='/save' method='POST'>";
    html += "<label>WLAN Name (SSID):</label><input type='text' name='ssid' placeholder='z.B. FRITZ!Box 7590'>";
    html += "<label>WLAN Passwort:</label><input type='password' name='password'>";
    html += "<label>Dein Standort:</label><input type='text' name='location' placeholder='z.B. Berlin oder Hamburg'>";
    html += "<small style='color:#aaa;display:block;margin-top:-15px;margin-bottom:20px;'>Stadtname auf Englisch oder Deutsch</small>";
    html += "<input type='submit' value='Speichern & Verbinden'>";
    html += "</form></div></body></html>";
    server.send(200, "text/html", html);
}

void handleSave() {
    if (server.hasArg("ssid")) {
        preferences.putString("ssid",     server.arg("ssid"));
        preferences.putString("password", server.arg("password"));
        preferences.putString("location", server.arg("location"));
        // Koordinaten zurücksetzen, damit beim nächsten Start Geocoding läuft
        preferences.putFloat("lat", 0.0);
        preferences.putFloat("lon", 0.0);

        String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'></head>";
        html += "<body style='font-family:Arial; background:#121212; color:#fff; text-align:center; padding:50px;'>";
        html += "<h3>Gespeichert! Der Cube startet jetzt neu...</h3></body></html>";
        server.send(200, "text/html", html);

        delay(2000);
        preferences.end();
        ESP.restart();
    }
}

// --- BOOT SCREEN ---

void showBootScreen() {
    lv_disp_load_scr(ui_ScreenBoot);
    lv_bar_set_value(uic_BarWifi, 0, LV_ANIM_OFF);
    lv_label_set_text(uic_LabelStatus, "Verbinde mit WLAN...");
    lv_timer_handler();

    Serial.print("Verbinde mit WLAN: "); Serial.println(ssid);
    WiFi.begin(ssid.c_str(), password.c_str());

    int maxVersuche = 20;
    for (int i = 0; i <= maxVersuche; i++) {
        if (WiFi.status() == WL_CONNECTED) break;
        delay(500);
        Serial.print(".");
        int progress = (i * 100) / maxVersuche;
        lv_bar_set_value(uic_BarWifi, progress, LV_ANIM_ON);
        lv_timer_handler();
    }

    if (WiFi.status() == WL_CONNECTED) {
        lv_bar_set_value(uic_BarWifi, 100, LV_ANIM_ON);
        lv_label_set_text(uic_LabelStatus, "Verbunden!");
        lv_timer_handler();
        delay(4000);
        lv_scr_load_anim(ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);
        currentScreen = 1;
        lv_timer_handler();
    } else {
        lv_label_set_text(uic_LabelStatus, "Kein WLAN – Setup wird geoeffnet...");
        lv_timer_handler();
        delay(2000);
        setupMode = true;
        startSetupPortal();
    }
}

// --- GEOCODING (Stadtname → Koordinaten) ---

bool geocodeLocation(const String& city) {
    HTTPClient http;
    String url = "https://geocoding-api.open-meteo.com/v1/search?name=";
    url += city;
    url += "&count=1&language=de&format=json";

    Serial.print("Geocoding: "); Serial.println(url);
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        String payload = http.getString();
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);

        if (doc["results"].size() > 0) {
            latitude  = doc["results"][0]["latitude"].as<float>();
            longitude = doc["results"][0]["longitude"].as<float>();

            // Koordinaten dauerhaft speichern
            preferences.putFloat("lat", latitude);
            preferences.putFloat("lon", longitude);

            Serial.printf("Koordinaten: %.4f, %.4f\n", latitude, longitude);
            http.end();
            return true;
        }
    }

    Serial.println("Geocoding fehlgeschlagen!");
    http.end();
    return false;
}

// --- WINDRICHTUNG (Grad → Pfeil + Himmelsrichtung) ---

String getWindDirection(int deg) {
    // 8 Sektoren à 45°, versetzt um 22.5° damit N zentriert ist
    const char* richtungen[] = {
        "N",   // 0°
        "NO",  // 45°
        "O",   // 90°
        "SO",  // 135°
        "S",   // 180°
        "SW",  // 225°
        "W",   // 270°
        "NW"   // 315°
    };
    int index = (int)((deg + 22.5) / 45.0) % 8;
    return String(richtungen[index]);
}

// --- WETTER ABRUFEN (Open-Meteo, kein API-Key) ---

void fetchWeather() {
    if (WiFi.status() != WL_CONNECTED) return;

    // Sicherheitsnetz: falls keine Koordinaten vorhanden
    if (latitude == 0.0 && longitude == 0.0) {
        if (!geocodeLocation(location)) return;
    }

    HTTPClient http;
    String url = "https://api.open-meteo.com/v1/forecast";
    url += "?latitude="  + String(latitude, 4);
    url += "&longitude=" + String(longitude, 4);
    url += "&current=temperature_2m,relative_humidity_2m,apparent_temperature,wind_speed_10m,wind_direction_10m,surface_pressure,weather_code";
    url += "&daily=uv_index_max,sunrise,sunset";
    url += "&hourly=temperature_2m,weather_code";
    url += "&wind_speed_unit=kmh&timezone=auto&forecast_days=2";

    Serial.print("Wetter-URL: "); Serial.println(url);
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        String payload = http.getString();
        DynamicJsonDocument doc(8192);
        deserializeJson(doc, payload);

        JsonObject current = doc["current"];

        float temp = current["temperature_2m"].as<float>();
        lv_label_set_text(ui_LabelTemp, (String((int)round(temp)) + "°C").c_str());

        setTempColor(ui_LabelTemp, temp);

        int humidity = current["relative_humidity_2m"].as<int>();
        lv_label_set_text(ui_LabelHum, (String(humidity) + "%").c_str());

        float feelsLike = current["apparent_temperature"].as<float>();
        lv_label_set_text(ui_LabelFeelsLike, (String((int)round(feelsLike)) + "°C").c_str());

        float wind = current["wind_speed_10m"].as<float>();  // bereits in km/h
        lv_label_set_text(ui_LabelWind, (String((int)round(wind)) + " km/h").c_str());

        int windDeg = current["wind_direction_10m"].as<int>();
        lv_label_set_text(uic_LabelWindDir, getWindDirection(windDeg).c_str());

        int pressure = current["surface_pressure"].as<int>();
        lv_label_set_text(ui_LabelPress, (String(pressure) + " hPa").c_str());

        int wmoCode = current["weather_code"].as<int>();
        updateWeatherIcon(wmoCode);

        // --- Screen 3: UV-Index, Sonnenaufgang, Sonnenuntergang ---
        float uvMax = doc["daily"]["uv_index_max"][0].as<float>();
        lv_label_set_text(uic_LabelUV, String((int)round(uvMax)).c_str());

        String sunriseRaw  = doc["daily"]["sunrise"][0].as<String>();
        String sunsetRaw   = doc["daily"]["sunset"][0].as<String>();
        String sunriseTime = (sunriseRaw.length()  >= 16) ? sunriseRaw.substring(11, 16)  : "--:--";
        String sunsetTime  = (sunsetRaw.length()   >= 16) ? sunsetRaw.substring(11, 16)   : "--:--";
        lv_label_set_text(uic_LabelAufgang,   sunriseTime.c_str());
        lv_label_set_text(uic_LabelUntergang, sunsetTime.c_str());

        // --- Screen 4: Stündliche Vorhersage ---
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            int h = timeinfo.tm_hour;

            lv_obj_t* zeitLabels[3] = { uic_LabelH1Zeit, uic_LabelH2Zeit, uic_LabelH3Zeit };
            lv_obj_t* tempLabels[3] = { uic_LabelH1Temp, uic_LabelH2Temp, uic_LabelH3Temp };
            lv_obj_t* icons[3]      = { uic_ImageH1,     uic_ImageH2,     uic_ImageH3     };

            for (int i = 0; i < 3; i++) {
                int idx = h + 1 + i; // max 23+3=26, liegt sicher in 48h-Array
                float hTemp = doc["hourly"]["temperature_2m"][idx].as<float>();
                int   hCode = doc["hourly"]["weather_code"][idx].as<int>();
                // Zeit direkt aus API-Array lesen (z.B. "2025-06-02T01:00" → "01:00")
                String timeRaw = doc["hourly"]["time"][idx].as<String>();
                String timeStr = (timeRaw.length() >= 16) ? timeRaw.substring(11, 16) : "--:--";
                lv_label_set_text(zeitLabels[i], timeStr.c_str());
                lv_label_set_text(tempLabels[i], (String((int)round(hTemp)) + "°C").c_str());
                setTempColor(tempLabels[i], hTemp);
                setWeatherIcon(icons[i], hCode);
            }

            // --- Regen-Warnung: nächste 2 Stunden prüfen ---
            bool regenKommt = false;
            for (int i = 1; i <= 2; i++) {
                int idx   = h + i;
                int hCode = doc["hourly"]["weather_code"][idx].as<int>();
                if ((hCode >= 51 && hCode <= 67) ||
                    (hCode >= 80 && hCode <= 82) ||
                     hCode >= 95) {
                    regenKommt = true;
                    break;
                }
            }
            if (!regenKommt) regenWarnungBestaetigt = false; // Reset für nächste Warnung
            if (regenKommt && !regenWarnungBestaetigt && !regenWarnungAktiv) {
                regenWarnungAktiv = true;
                lv_scr_load(ui_uiScreenWarnung);
                currentScreen = 99;
            }
        }

        lastWeatherUpdate = millis();
        Serial.printf("Wetter OK: %.1f°C, %d%%, WMO %d, UV %.1f\n", temp, humidity, wmoCode, uvMax);
    } else {
        Serial.printf("Wetter-Fehler: HTTP %d\n", httpCode);
    }

    http.end();
}

// --- POLLEN ---

void setPollenLabel(lv_obj_t* label, float value) {
    const char* text;
    uint32_t    color;

    if (value <= 0) {
        text = "Keine";    color = 0x888888;
    } else if (value <= 10) {
        text = "Gering";   color = 0x00CC44;
    } else if (value <= 30) {
        text = "Maessig";  color = 0xFFCC00;
    } else if (value <= 100) {
        text = "Hoch";     color = 0xFF8800;
    } else {
        text = "Sehr hoch"; color = 0xFF3300;
    }
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
}

void fetchPollen() {
    if (WiFi.status() != WL_CONNECTED) return;
    if (latitude == 0.0 && longitude == 0.0) return;

    HTTPClient http;
    String url = "https://air-quality-api.open-meteo.com/v1/air-quality";
    url += "?latitude="  + String(latitude, 4);
    url += "&longitude=" + String(longitude, 4);
    url += "&hourly=birch_pollen,grass_pollen,alder_pollen,mugwort_pollen,ragweed_pollen";
    url += "&timezone=auto&forecast_days=1";

    Serial.print("Pollen-URL: "); Serial.println(url);
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        String payload = http.getString();
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, payload);

        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) { http.end(); return; }
        int h = timeinfo.tm_hour;

        float birke    = doc["hourly"]["birch_pollen"][h].as<float>();
        float graeser  = doc["hourly"]["grass_pollen"][h].as<float>();
        float erle     = doc["hourly"]["alder_pollen"][h].as<float>();
        float beifuss  = doc["hourly"]["mugwort_pollen"][h].as<float>();
        float ambrosia = doc["hourly"]["ragweed_pollen"][h].as<float>();

        setPollenLabel(uic_LabelBirkeWert,    birke);
        setPollenLabel(uic_LabelGraeserWert,  graeser);
        setPollenLabel(uic_LabelErleWert,     erle);
        setPollenLabel(uic_LabelBeifussWert,  beifuss);
        setPollenLabel(uic_LabelAmbrosiaWert, ambrosia);

        Serial.printf("Pollen OK: Birke %.1f, Graeser %.1f, Erle %.1f, Beifuss %.1f, Ambrosia %.1f\n",
                      birke, graeser, erle, beifuss, ambrosia);
    } else {
        Serial.printf("Pollen-Fehler: HTTP %d\n", httpCode);
    }
    http.end();
}

// --- ICON-AUSWAHL nach WMO-Code ---
// WMO Codes: https://open-meteo.com/en/docs#weathervariables

void setWeatherIcon(lv_obj_t* img, int wmoCode) {
    if (wmoCode == 0 || wmoCode == 1) {
        lv_img_set_src(img, &ui_img_day_clear_png);
    } else if (wmoCode == 2 || wmoCode == 3) {
        lv_img_set_src(img, &ui_img_overcast_png);
    } else if (wmoCode == 45 || wmoCode == 48) {
        lv_img_set_src(img, &ui_img_fog_png);
    } else if ((wmoCode >= 51 && wmoCode <= 67) ||
               (wmoCode >= 80 && wmoCode <= 82)) {
        lv_img_set_src(img, &ui_img_rain_png);
    } else if ((wmoCode >= 71 && wmoCode <= 77) ||
               wmoCode == 85 || wmoCode == 86) {
        lv_img_set_src(img, &ui_img_snow_png);
    } else if (wmoCode >= 95) {
        lv_img_set_src(img, &ui_img_thunder_png);
    } else {
        lv_img_set_src(img, &ui_img_overcast_png);
    }
}

void updateWeatherIcon(int wmoCode) {
    setWeatherIcon(ui_ImageWetter, wmoCode);
}

void setTempColor(lv_obj_t* label, float temp) {
    if (temp >= 24.0) {
        lv_obj_set_style_text_color(label, lv_color_hex(0xFF3300), LV_PART_MAIN | LV_STATE_DEFAULT); // Rot
    } else if (temp >= 15.0) {
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFCC00), LV_PART_MAIN | LV_STATE_DEFAULT); // Gelb
    } else if (temp >= 8.0) {
        lv_obj_set_style_text_color(label, lv_color_hex(0x00CC44), LV_PART_MAIN | LV_STATE_DEFAULT); // Grün
    } else {
        lv_obj_set_style_text_color(label, lv_color_hex(0x00AAFF), LV_PART_MAIN | LV_STATE_DEFAULT); // Blau
    }
}

// --- UHRZEIT & DATUM ---

void updateClock() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return;

    char zeitPuffer[10];
    sprintf(zeitPuffer, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    lv_label_set_text(ui_LabelUhr, zeitPuffer);

    const char* wochentage[] = {"So.", "Mo.", "Di.", "Mi.", "Do.", "Fr.", "Sa."};
    char datumsPuffer[32];
    sprintf(datumsPuffer, "%s %02d.%02d.%d",
        wochentage[timeinfo.tm_wday],
        timeinfo.tm_mday,
        timeinfo.tm_mon + 1,
        timeinfo.tm_year + 1900);
    lv_label_set_text(ui_uiLabelDatum, datumsPuffer);
}
