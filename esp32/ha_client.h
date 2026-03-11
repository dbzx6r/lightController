#pragma once
#include <Arduino.h>

#define MAX_LIGHTS 20
#define ENTITY_NAME_LEN 48

struct LightEntity {
    char entityId[ENTITY_NAME_LEN];
    char friendlyName[ENTITY_NAME_LEN];
    bool isOn;
    int  brightness; // 0-100 percent
};

// Returns count of lights found, or negative on error:
//  -1  = could not begin HTTP connection
//  -2  = HTTP error (check ha_lastHttpCode for the code)
//  -3  = JSON parse error
int  ha_fetchLights(LightEntity lights[], int maxCount);
extern int ha_lastHttpCode; // set on every GET/POST attempt

// Fetch a single entity's current state. Returns false on failure.
bool ha_fetchLight(LightEntity &light);

// Turn a light on (optionally set brightness 0-100; pass -1 to keep current).
bool ha_turnOn(const char* entityId, int brightnessPct = -1);

// Turn a light off.
bool ha_turnOff(const char* entityId);
