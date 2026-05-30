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
bool geocodeLocation(const String& city);
void updateWeatherIcon(int wmoCode);
void startSetupPortal();
void drawSetupScreen();
void handleRoot();
void handleSave();

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("--- CUBE START ---");

    pinMode(TOUCH_PIN, INPUT);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

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

    Serial.print("Verbinde mit WLAN: "); Serial.println(ssid);
    WiFi.begin(ssid.c_str(), password.c_str());

    int counter = 0;
    while (WiFi.status() != WL_CONNECTED && counter < 20) {
        delay(500);
        Serial.print(".");
        counter++;
    }

    if (WiFi.status() != WL_CONNECTED) {
        setupMode = true;
        startSetupPortal();
        return;
    }

    Serial.println(" Verbunden!");
    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "europe.pool.ntp.org");

    // Falls noch keine Koordinaten gespeichert, Geocoding durchführen
    if (latitude == 0.0 && longitude == 0.0) {
        geocodeLocation(location);
    }

    fetchWeather();
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

    static unsigned long lastTimeUpdate = 0;
    if (millis() - lastTimeUpdate >= 1000) {
        lastTimeUpdate = millis();
        updateClock();
    }

    if (millis() - lastWeatherUpdate >= weatherInterval) {
        lastWeatherUpdate = millis();
        fetchWeather();
    }
}

// --- BUTTON ---

void checkTouchButton() {
    if (digitalRead(TOUCH_PIN) == HIGH) {
        if (millis() - lastTouchTime > 500) {
            lastTouchTime = millis();
            if (currentScreen == 1) {
                lv_scr_load_anim(ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
                currentScreen = 2;
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
    url += "&current=temperature_2m,relative_humidity_2m,apparent_temperature,wind_speed_10m,surface_pressure,weather_code";
    url += "&wind_speed_unit=kmh";

    Serial.print("Wetter-URL: "); Serial.println(url);
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
        String payload = http.getString();
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);

        JsonObject current = doc["current"];

        float temp = current["temperature_2m"].as<float>();
        lv_label_set_text(ui_LabelTemp, (String((int)round(temp)) + "°C").c_str());

        int humidity = current["relative_humidity_2m"].as<int>();
        lv_label_set_text(ui_LabelHum, (String(humidity) + "%").c_str());

        float feelsLike = current["apparent_temperature"].as<float>();
        lv_label_set_text(ui_LabelFeelsLike, (String((int)round(feelsLike)) + "°C").c_str());

        float wind = current["wind_speed_10m"].as<float>();  // bereits in km/h
        lv_label_set_text(ui_LabelWind, (String((int)round(wind)) + " km/h").c_str());

        int pressure = current["surface_pressure"].as<int>();
        lv_label_set_text(ui_LabelPress, (String(pressure) + " hPa").c_str());

        int wmoCode = current["weather_code"].as<int>();
        updateWeatherIcon(wmoCode);

        lastWeatherUpdate = millis();
        Serial.printf("Wetter OK: %.1f°C, %d%%, WMO %d\n", temp, humidity, wmoCode);
    } else {
        Serial.printf("Wetter-Fehler: HTTP %d\n", httpCode);
    }

    http.end();
}

// --- ICON-AUSWAHL nach WMO-Code ---
// WMO Codes: https://open-meteo.com/en/docs#weathervariables

void updateWeatherIcon(int wmoCode) {
    if (wmoCode == 0 || wmoCode == 1) {
        // Klarer Himmel / überwiegend klar
        lv_img_set_src(ui_ImageWetter, &ui_img_day_clear_png);
    } else if (wmoCode == 2 || wmoCode == 3) {
        // Teilweise bewölkt / bedeckt
        lv_img_set_src(ui_ImageWetter, &ui_img_overcast_png);
    } else if (wmoCode == 45 || wmoCode == 48) {
        // Nebel
        lv_img_set_src(ui_ImageWetter, &ui_img_fog_png);
    } else if ((wmoCode >= 51 && wmoCode <= 67) ||
               (wmoCode >= 80 && wmoCode <= 82)) {
        // Nieselregen, Regen, Regenschauer
        lv_img_set_src(ui_ImageWetter, &ui_img_rain_png);
    } else if ((wmoCode >= 71 && wmoCode <= 77) ||
               wmoCode == 85 || wmoCode == 86) {
        // Schnee
        lv_img_set_src(ui_ImageWetter, &ui_img_snow_png);
    } else if (wmoCode >= 95) {
        // Gewitter
        lv_img_set_src(ui_ImageWetter, &ui_img_thunder_png);
    } else {
        // Fallback: bewölkt
        lv_img_set_src(ui_ImageWetter, &ui_img_overcast_png);
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
