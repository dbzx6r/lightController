// ──────────────────────────────────────────────────────────────────
//  HueDial – ESP32 Philips Hue Room Controller
//  Main sketch: state machine, WiFi, periodic HA sync.
// ──────────────────────────────────────────────────────────────────
#include <Arduino.h>
#include <WiFi.h>            // ESP32 WiFi
#include <TFT_eSPI.h>

#include "config.h"
#include "encoder.h"
#include "ha_client.h"
#include "display.h"

// Default backlight pin if not defined in config.h
#ifndef TFT_BL_PIN
  #define TFT_BL_PIN 27
#endif

// ── TFT ──────────────────────────────────────
TFT_eSPI tft = TFT_eSPI();

// ── State machine ────────────────────────────
enum AppState { STATE_BOOT, STATE_ROOM_LIST, STATE_ROOM_DETAIL };
static AppState appState = STATE_BOOT;

// ── Light data ───────────────────────────────
static LightEntity lights[MAX_LIGHTS];
static int         lightCount    = 0;
static int         selectedIdx   = 0;
static int         scrollOffset  = 0;

// ── Brightness debounce ──────────────────────
static int           pendingBrightness   = -1;
static unsigned long brightnessChangedAt = 0;

// ── Periodic HA poll (detail view) ───────────
static unsigned long lastPollAt = 0;

// ── Helpers ──────────────────────────────────
static void clampScroll() {
    if (lightCount == 0) return;
    int visRows = (240 - 22) / 36;
    if (selectedIdx < scrollOffset) scrollOffset = selectedIdx;
    if (selectedIdx >= scrollOffset + visRows) scrollOffset = selectedIdx - visRows + 1;
    if (scrollOffset < 0) scrollOffset = 0;
}

static void refreshLights() {
    display_splash(tft, "Fetching rooms...");
    int n = ha_fetchLights(lights, MAX_LIGHTS);
    if (n > 0) {
        lightCount   = n;
        selectedIdx  = 0;
        scrollOffset = 0;
    } else {
        // Keep old list if we had one; show error on screen
        char msg[64];
        if (n == -1)      snprintf(msg, sizeof(msg), "HA: connection failed");
        else if (n == -2) snprintf(msg, sizeof(msg), "HA: HTTP %d", ha_lastHttpCode);
        else if (n == -3) snprintf(msg, sizeof(msg), "HA: bad JSON");
        else              snprintf(msg, sizeof(msg), "No light.* entities found");
        display_splash(tft, msg);
        delay(3000);
    }
}

static void connectWiFi() {
    display_splash(tft, "Connecting to WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
        delay(250);
        tft.fillCircle(120, 180, 4, (millis() / 500 % 2) ? 0xFD20 : 0x2945);
    }

    if (WiFi.status() == WL_CONNECTED) {
        display_splash(tft, "Connected!");
        delay(600);
    } else {
        display_splash(tft, "WiFi failed - retrying...");
        delay(2000);
    }
}

// ── Setup ─────────────────────────────────────
void setup() {
    Serial.begin(115200);

    // Drive backlight HIGH before init so screen lights up immediately.
    // TFT_eSPI also sets TFT_BL via User_Setup.h, but being explicit here
    // ensures it works regardless of TFT_eSPI version.
    pinMode(TFT_BL_PIN, OUTPUT);
    digitalWrite(TFT_BL_PIN, HIGH);

    tft.init();
    tft.setRotation(0);
    tft.fillScreen(0x1082);
    display_init(tft);
    display_splash(tft, "Booting...");

    encoder_init();

    connectWiFi();
    refreshLights();

    appState = STATE_ROOM_LIST;
    display_statusBar(tft, WIFI_SSID, WiFi.RSSI());
    display_roomList(tft, lights, lightCount, selectedIdx, scrollOffset);
}

// ── Loop ──────────────────────────────────────
void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }

    int  delta = encoder_getDelta();
    bool push  = encoder_getPush();
    bool key0  = encoder_getKey0();

    // ── ROOM LIST ────────────────────────────
    if (appState == STATE_ROOM_LIST) {
        bool redraw = false;

        if (delta != 0 && lightCount > 0) {
            selectedIdx += delta;
            if (selectedIdx < 0)           selectedIdx = 0;
            if (selectedIdx >= lightCount) selectedIdx = lightCount - 1;
            clampScroll();
            redraw = true;
        }

        if (push && lightCount > 0) {
            appState    = STATE_ROOM_DETAIL;
            lastPollAt  = millis();
            pendingBrightness = -1;
            display_statusBar(tft, WIFI_SSID, WiFi.RSSI());
            display_roomDetail(tft, lights[selectedIdx]);
            return;
        }

        if (key0) {
            refreshLights();
            redraw = true;
        }

        if (redraw) {
            display_statusBar(tft, WIFI_SSID, WiFi.RSSI());
            display_roomList(tft, lights, lightCount, selectedIdx, scrollOffset);
        }
    }

    // ── ROOM DETAIL ──────────────────────────
    else if (appState == STATE_ROOM_DETAIL) {
        LightEntity &light = lights[selectedIdx];
        bool redraw = false;

        if (delta != 0 && light.isOn) {
            int newBri = light.brightness + delta * BRIGHTNESS_STEP;
            if (newBri < 1)   newBri = 1;
            if (newBri > 100) newBri = 100;
            light.brightness      = newBri;
            pendingBrightness     = newBri;
            brightnessChangedAt   = millis();
            redraw = true;
        }

        if (pendingBrightness >= 0 &&
            millis() - brightnessChangedAt >= BRIGHTNESS_DEBOUNCE) {
            ha_turnOn(light.entityId, pendingBrightness);
            pendingBrightness = -1;
        }

        if (key0) {
            if (light.isOn) {
                ha_turnOff(light.entityId);
                light.isOn = false;
            } else {
                int bri = (light.brightness > 0) ? light.brightness : 100;
                ha_turnOn(light.entityId, bri);
                light.isOn = true;
            }
            pendingBrightness = -1;
            redraw = true;
        }

        if (push) {
            appState = STATE_ROOM_LIST;
            display_statusBar(tft, WIFI_SSID, WiFi.RSSI());
            display_roomList(tft, lights, lightCount, selectedIdx, scrollOffset);
            return;
        }

        if (millis() - lastPollAt >= HA_POLL_INTERVAL) {
            lastPollAt = millis();
            if (ha_fetchLight(light)) redraw = true;
        }

        if (redraw) {
            display_statusBar(tft, WIFI_SSID, WiFi.RSSI());
            display_roomDetail(tft, light);
        }
    }
}
