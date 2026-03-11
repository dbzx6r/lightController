#pragma once
#include <TFT_eSPI.h>
#include "ha_client.h"

// Call once after TFT init
void display_init(TFT_eSPI &tft);

// Full-screen splash shown during boot / WiFi connect
void display_splash(TFT_eSPI &tft, const char* message);

// Status bar at top of screen (WiFi SSID + RSSI indicator)
void display_statusBar(TFT_eSPI &tft, const char* ssid, int rssi);

// Scrollable room list.
// selectedIdx = absolute index of highlighted item
// scrollOffset = index of first visible item
void display_roomList(TFT_eSPI &tft, LightEntity lights[], int count,
                      int selectedIdx, int scrollOffset);

// Room detail screen.
void display_roomDetail(TFT_eSPI &tft, const LightEntity &light);
