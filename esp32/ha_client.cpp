#include "ha_client.h"
#include "config.h"
#include <HTTPClient.h>      // ESP32 HTTPClient
#include <WiFiClient.h>
#include <ArduinoJson.h>

// ── Helpers ──────────────────────────────────
static void addAuthHeaders(HTTPClient &http) {
    http.addHeader("Authorization", String("Bearer ") + HA_TOKEN);
    http.addHeader("Content-Type", "application/json");
}

static int bri255ToPct(int b) { return (b * 100 + 127) / 255; }
static int pctToBri255(int p) { return (p * 255 + 50) / 100; }

// ── Fetch all lights ─────────────────────────
int ha_fetchLights(LightEntity lights[], int maxCount) {
    WiFiClient wifiClient;
    HTTPClient http;
    String url = String(HA_BASE_URL) + "/api/states";

    if (!http.begin(wifiClient, url)) return 0;
    addAuthHeaders(http);

    int code = http.GET();
    if (code != 200) { http.end(); return 0; }

    DynamicJsonDocument doc(32768);
    DeserializationError err = deserializeJson(doc, http.getStream());
    http.end();
    if (err) return 0;

    int count = 0;
    const char* prefix = HA_ENTITY_PREFIX;
    size_t prefixLen = strlen(prefix);

    for (JsonObject obj : doc.as<JsonArray>()) {
        if (count >= maxCount) break;

        const char* id = obj["entity_id"] | "";
        if (strncmp(id, prefix, prefixLen) != 0) continue;

        LightEntity &e = lights[count];
        strncpy(e.entityId, id, ENTITY_NAME_LEN - 1);
        e.entityId[ENTITY_NAME_LEN - 1] = '\0';

        const char* name = obj["attributes"]["friendly_name"] | id;
        strncpy(e.friendlyName, name, ENTITY_NAME_LEN - 1);
        e.friendlyName[ENTITY_NAME_LEN - 1] = '\0';

        const char* state = obj["state"] | "off";
        e.isOn = (strcmp(state, "on") == 0);

        int bri = obj["attributes"]["brightness"] | 0;
        e.brightness = e.isOn ? bri255ToPct(bri) : 0;

        count++;
    }
    return count;
}

// ── Fetch single light ────────────────────────
bool ha_fetchLight(LightEntity &light) {
    WiFiClient wifiClient;
    HTTPClient http;
    String url = String(HA_BASE_URL) + "/api/states/" + light.entityId;

    if (!http.begin(wifiClient, url)) return false;
    addAuthHeaders(http);

    int code = http.GET();
    if (code != 200) { http.end(); return false; }

    DynamicJsonDocument doc(1024);
    DeserializationError err = deserializeJson(doc, http.getStream());
    http.end();
    if (err) return false;

    const char* state = doc["state"] | "off";
    light.isOn = (strcmp(state, "on") == 0);
    int bri = doc["attributes"]["brightness"] | 0;
    light.brightness = light.isOn ? bri255ToPct(bri) : 0;
    return true;
}

// ── Control ───────────────────────────────────
bool ha_turnOn(const char* entityId, int brightnessPct) {
    WiFiClient wifiClient;
    HTTPClient http;
    String url = String(HA_BASE_URL) + "/api/services/light/turn_on";

    if (!http.begin(wifiClient, url)) return false;
    addAuthHeaders(http);

    StaticJsonDocument<128> doc;
    doc["entity_id"] = entityId;
    if (brightnessPct >= 0) {
        doc["brightness"] = pctToBri255(brightnessPct);
    }
    String body;
    serializeJson(doc, body);

    int code = http.POST(body);
    http.end();
    return (code == 200 || code == 201);
}

bool ha_turnOff(const char* entityId) {
    WiFiClient wifiClient;
    HTTPClient http;
    String url = String(HA_BASE_URL) + "/api/services/light/turn_off";

    if (!http.begin(wifiClient, url)) return false;
    addAuthHeaders(http);

    StaticJsonDocument<64> doc;
    doc["entity_id"] = entityId;
    String body;
    serializeJson(doc, body);

    int code = http.POST(body);
    http.end();
    return (code == 200 || code == 201);
}
