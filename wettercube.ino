#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <time.h> // Native Zeit-Bibliothek für den ESP32
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ui.h> // Deine SquareLine Studio UI

// --- SPEICHER & WEB-SERVER ---
Preferences preferences;
WebServer server(80);

String ssid     = "";
String password = "";
String apiKey   = "";
String location = "";

unsigned long lastWeatherUpdate = 0;
const unsigned long weatherInterval = 900000; // 15 Minuten
bool setupMode = false;

// --- HARDWARE BUTTON (TTP223) ---
#define TOUCH_PIN 3
int currentScreen = 1;       // 1 = Hauptseite, 2 = Wetterdetails
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
void updateWeatherIcon(int id);
void startSetupPortal();
void drawSetupScreen();
void handleRoot();
void handleSave();

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("--- CUBE START ---");
    
    // TTP223 Touch-Pin als Eingang deklarieren
    pinMode(TOUCH_PIN, INPUT);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    gfx->begin();
    gfx->fillScreen(0x0000); 

    // LVGL und UI starten
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
    ssid     = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    apiKey   = preferences.getString("apikey", "");
    location = preferences.getString("location", "");

    if (ssid.length() < 4 || apiKey.length() < 10) {
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

    // NEU: Den Hardware-Touch-Button abfragen
    checkTouchButton();

    // Uhrzeit und Datum jede Sekunde aktualisieren
    static unsigned long lastTimeUpdate = 0;
    if (millis() - lastTimeUpdate >= 1000) {
        lastTimeUpdate = millis();
        updateClock();
    }

    // Wetter alle 15 Minuten aktualisieren
    if (millis() - lastWeatherUpdate >= weatherInterval) {
        lastWeatherUpdate = millis();
        fetchWeather();
    }
}

// --- PORTAL & DISPLAY STEUERUNG ---

void checkTouchButton() {
    // Der TTP223 liefert HIGH (1), wenn er berührt wird
    if (digitalRead(TOUCH_PIN) == HIGH) {
        // Entprellung: Nur alle 500ms einen Wechsel zulassen
        if (millis() - lastTouchTime > 500) {
            lastTouchTime = millis();
            
            if (currentScreen == 1) {
                // Wechsel zu Screen 2 mit einer schicken Wisch-Animation nach links
                lv_scr_load_anim(ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
                currentScreen = 2;
                Serial.println("Wechsle zu Seite 2 (Details)");
            } else {
                // Wechsel zurück zu Screen 1 mit einer Wisch-Animation nach rechts
                lv_scr_load_anim(ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
                currentScreen = 1;
                Serial.println("Wechsle zu Seite 1 (Hauptseite)");
            }
        }
    }
}

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
    html += "input[type=text], input[type=password]{width:100%; padding:10px; margin:10px 0 20px 0; border-radius:5px; border:1px solid #333; background:#2a2a2a; color:#fff; box-sizing: border-box;}";
    html += "input[type=submit]{background:#00adb5; color:#fff; border:none; padding:12px 20px; border-radius:5px; cursor:pointer; width:100%; font-size:16px;}";
    html += "input[type=submit]:hover{background:#007a80;}</style></head><body>";
    html += "<h2>WetterCube Konfiguration</h2>";
    html += "<div class='card'><form action='/save' method='POST'>";
    html += "<label>WLAN Name (SSID):</label><input type='text' name='ssid' placeholder='z.B. FRITZ!Box 7590 UE'>";
    html += "<label>WLAN Passwort:</label><input type='password' name='password'>";
    html += "<label>OpenWeather API-Key:</label><input type='text' name='apikey'>";
    html += "<label>Standort (Ort,Land):</label><input type='text' name='location' placeholder='z.B. Stockelsdorf,de'>";
    html += "<input type='submit' value='Speichern & Verbinden'>";
    html += "</form></div></body></html>";
    server.send(200, "text/html", html);
}

void handleSave() {
    if (server.hasArg("ssid")) {
        preferences.putString("ssid", server.arg("ssid"));
        preferences.putString("password", server.arg("password"));
        preferences.putString("apikey", server.arg("apikey"));
        preferences.putString("location", server.arg("location"));
        
        String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body style='font-family:Arial; background:#121212; color:#fff; text-align:center; padding:50px;'>";
        html += "<h3>Daten gespeichert! Der Cube startet jetzt neu...</h3></body></html>";
        server.send(200, "text/html", html);
        
        delay(2000);
        preferences.end();
        ESP.restart(); 
    }
}

// --- LOGIK-FUNKTIONEN ---

void updateClock() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return;
    }

    char zeitPuffer[10];
    sprintf(zeitPuffer, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    lv_label_set_text(ui_LabelUhr, zeitPuffer);

    const char* wochentage[] = {"So.", "Mo.", "Di.", "Mi.", "Do.", "Fr.", "Sa."};
    String wochentagString = wochentage[timeinfo.tm_wday];

    int tag = timeinfo.tm_mday;
    int monat = timeinfo.tm_mon + 1;     
    int jahr = timeinfo.tm_year + 1900; 

    char datumsPuffer[32];
    sprintf(datumsPuffer, "%s %02d.%02d.%d", wochentagString.c_str(), tag, monat, jahr);
    lv_label_set_text(ui_uiLabelDatum, datumsPuffer);
}

void fetchWeather() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "http://api.openweathermap.org/data/2.5/weather?q=" + location + "&appid=" + apiKey + "&units=metric";
        
        http.begin(url);
        int httpCode = http.GET();
        
        if (httpCode > 0) {
            String payload = http.getString();
            DynamicJsonDocument doc(2048);
            deserializeJson(doc, payload);
            
            float temp = doc["main"]["temp"];
            String tempString = String((int)round(temp)) + "°C";
            lv_label_set_text(ui_LabelTemp, tempString.c_str());
            
            int humidity = doc["main"]["humidity"];
            String humString = String(humidity) + "%";
            lv_label_set_text(ui_LabelHum, humString.c_str());
            
            float feelsLike = doc["main"]["feels_like"];
            String feelsLikeString = String((int)round(feelsLike)) + "°C";
            lv_label_set_text(ui_LabelFeelsLike, feelsLikeString.c_str());

            float windSpeed = doc["wind"]["speed"];
            int windKmH = (int)round(windSpeed * 3.6);
            String windString = String(windKmH) + " km/h";
            lv_label_set_text(ui_LabelWind, windString.c_str());

            int pressure = doc["main"]["pressure"];
            String pressString = String(pressure) + " hPa";
            lv_label_set_text(ui_LabelPress, pressString.c_str());
            
            int weatherId = doc["weather"][0]["id"];
            updateWeatherIcon(weatherId);
        }
        http.end();
    }
}

void updateWeatherIcon(int id) {
    if (id >= 200 && id < 300) {
        lv_img_set_src(ui_ImageWetter, &ui_img_thunder_png); 
    } else if (id >= 300 && id < 600) {
        lv_img_set_src(ui_ImageWetter, &ui_img_rain_png);  
    } else if (id >= 600 && id < 700) {
        lv_img_set_src(ui_ImageWetter, &ui_img_snow_png);  
    } else if (id == 741) {
        lv_img_set_src(ui_ImageWetter, &ui_img_fog_png);
    } else if (id == 800) {
        lv_img_set_src(ui_ImageWetter, &ui_img_day_clear_png); 
    } else if (id > 800) {
        lv_img_set_src(ui_ImageWetter, &ui_img_overcast_png); 
    }
}