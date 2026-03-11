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

// Fetch all light entities from HA. Returns count (0 on failure).
int  ha_fetchLights(LightEntity lights[], int maxCount);

// Fetch a single entity's current state. Returns false on failure.
bool ha_fetchLight(LightEntity &light);

// Turn a light on (optionally set brightness 0-100; pass -1 to keep current).
bool ha_turnOn(const char* entityId, int brightnessPct = -1);

// Turn a light off.
bool ha_turnOff(const char* entityId);
