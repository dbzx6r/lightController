#include "display.h"
#include <TFT_eSPI.h>

// ── Colour palette ────────────────────────────
#define C_BG        0x1082  // dark grey ~#101010
#define C_ACCENT    0xFD20  // warm amber
#define C_WHITE     0xFFFF
#define C_GREY      0x6B4D
#define C_DARK_GREY 0x39E7
#define C_GREEN     0x07E0
#define C_RED       0xF800
#define C_BAR_BG    0x2945
#define C_BAR_FG    0xFD20

#define SCREEN_W    240
#define SCREEN_H    240
#define STATUS_H    22   // height of status bar
#define ROW_H       36   // height of each list row
#define VISIBLE_ROWS ((SCREEN_H - STATUS_H) / ROW_H)  // 6

static TFT_eSPI* _tft = nullptr;

void display_init(TFT_eSPI &tft) {
    _tft = &tft;
    tft.fillScreen(C_BG);
}

// ── Status bar ────────────────────────────────
void display_statusBar(TFT_eSPI &tft, const char* ssid, int rssi) {
    tft.fillRect(0, 0, SCREEN_W, STATUS_H, C_DARK_GREY);

    tft.setTextColor(C_WHITE, C_DARK_GREY);
    tft.setTextSize(1);
    tft.setCursor(4, 7);
    tft.print("HueDial");

    // RSSI bars (4 bars)
    int bars = 0;
    if      (rssi >= -55) bars = 4;
    else if (rssi >= -65) bars = 3;
    else if (rssi >= -75) bars = 2;
    else if (rssi >= -85) bars = 1;

    int bx = SCREEN_W - 22;
    for (int i = 0; i < 4; i++) {
        uint16_t col = (i < bars) ? C_GREEN : C_GREY;
        int bh = 3 + i * 3;
        tft.fillRect(bx + i * 5, STATUS_H - 2 - bh, 4, bh, col);
    }

    // SSID (truncated)
    tft.setTextColor(C_GREY, C_DARK_GREY);
    tft.setCursor(55, 7);
    char buf[20];
    strncpy(buf, ssid, 19);
    buf[19] = '\0';
    tft.print(buf);
}

// ── Splash ────────────────────────────────────
void display_splash(TFT_eSPI &tft, const char* message) {
    tft.fillScreen(C_BG);

    // Title
    tft.setTextColor(C_ACCENT);
    tft.setTextSize(3);
    int16_t tw = tft.textWidth("HueDial");
    tft.setCursor((SCREEN_W - tw) / 2, 80);
    tft.print("HueDial");

    // Subtitle
    tft.setTextColor(C_GREY);
    tft.setTextSize(1);
    tw = tft.textWidth("Hue Room Controller");
    tft.setCursor((SCREEN_W - tw) / 2, 118);
    tft.print("Hue Room Controller");

    // Status message
    tft.setTextColor(C_WHITE);
    tw = tft.textWidth(message);
    tft.setCursor((SCREEN_W - tw) / 2, 155);
    tft.print(message);
}

// ── Room list ─────────────────────────────────
void display_roomList(TFT_eSPI &tft, LightEntity lights[], int count,
                      int selectedIdx, int scrollOffset) {
    int y0 = STATUS_H;

    for (int i = 0; i < VISIBLE_ROWS; i++) {
        int idx = scrollOffset + i;
        int y = y0 + i * ROW_H;

        if (idx >= count) {
            tft.fillRect(0, y, SCREEN_W, ROW_H, C_BG);
            continue;
        }

        bool selected = (idx == selectedIdx);
        uint16_t bgCol = selected ? C_ACCENT : C_BG;
        uint16_t fgCol = selected ? C_BG     : C_WHITE;
        uint16_t subCol = selected ? C_DARK_GREY : C_GREY;

        tft.fillRect(0, y, SCREEN_W, ROW_H, bgCol);

        // Power indicator dot
        uint16_t dotCol = lights[idx].isOn ? C_GREEN : C_RED;
        if (selected) dotCol = lights[idx].isOn ? 0x0600 : 0x8000;
        tft.fillCircle(12, y + ROW_H / 2, 5, dotCol);

        // Friendly name
        tft.setTextColor(fgCol, bgCol);
        tft.setTextSize(1);
        tft.setCursor(24, y + 8);
        // Truncate to fit (approx 20 chars at size 1 = 6px each)
        char buf[28];
        strncpy(buf, lights[idx].friendlyName, 27);
        buf[27] = '\0';
        tft.print(buf);

        // Brightness sub-text
        tft.setTextColor(subCol, bgCol);
        tft.setCursor(24, y + 20);
        if (lights[idx].isOn) {
            char bri[12];
            snprintf(bri, sizeof(bri), "%d%%", lights[idx].brightness);
            tft.print(bri);
        } else {
            tft.print("Off");
        }

        // Divider line (skip for selected or last)
        if (!selected && idx < count - 1) {
            tft.drawFastHLine(0, y + ROW_H - 1, SCREEN_W, C_DARK_GREY);
        }
    }

    // Scrollbar
    if (count > VISIBLE_ROWS) {
        int sbH = (SCREEN_H - STATUS_H) * VISIBLE_ROWS / count;
        int sbY = STATUS_H + (SCREEN_H - STATUS_H) * scrollOffset / count;
        tft.fillRect(SCREEN_W - 3, STATUS_H, 3, SCREEN_H - STATUS_H, C_DARK_GREY);
        tft.fillRect(SCREEN_W - 3, sbY, 3, sbH, C_ACCENT);
    }
}

// ── Room detail ───────────────────────────────
void display_roomDetail(TFT_eSPI &tft, const LightEntity &light) {
    tft.fillRect(0, STATUS_H, SCREEN_W, SCREEN_H - STATUS_H, C_BG);

    // Room name
    tft.setTextColor(C_WHITE);
    tft.setTextSize(2);
    // Truncate
    char name[20];
    strncpy(name, light.friendlyName, 19);
    name[19] = '\0';
    int16_t tw = tft.textWidth(name);
    tft.setCursor((SCREEN_W - tw) / 2, STATUS_H + 12);
    tft.print(name);

    // On/Off status badge
    const char* stateStr = light.isOn ? "ON" : "OFF";
    uint16_t badgeCol = light.isOn ? C_GREEN : C_RED;
    int bw = 48, bh = 24, bx = (SCREEN_W - bw) / 2, by = STATUS_H + 44;
    tft.fillRoundRect(bx, by, bw, bh, 6, badgeCol);
    tft.setTextColor(C_WHITE, badgeCol);
    tft.setTextSize(2);
    tw = tft.textWidth(stateStr);
    tft.setCursor(bx + (bw - tw) / 2, by + 4);
    tft.print(stateStr);

    // Brightness label
    tft.setTextColor(C_GREY, C_BG);
    tft.setTextSize(1);
    tft.setCursor(10, STATUS_H + 86);
    tft.print("BRIGHTNESS");

    // Big brightness percentage
    char pctStr[8];
    snprintf(pctStr, sizeof(pctStr), "%d%%", light.brightness);
    tft.setTextColor(C_ACCENT, C_BG);
    tft.setTextSize(4);
    tw = tft.textWidth(pctStr);
    tft.setCursor((SCREEN_W - tw) / 2, STATUS_H + 98);
    tft.print(pctStr);

    // Brightness bar
    int barX = 20, barY = STATUS_H + 148, barW = SCREEN_W - 40, barH = 16;
    tft.fillRoundRect(barX, barY, barW, barH, 4, C_BAR_BG);
    int filled = (barW * light.brightness) / 100;
    if (filled > 0) {
        tft.fillRoundRect(barX, barY, filled, barH, 4, C_BAR_FG);
    }

    // Hint text
    tft.setTextColor(C_DARK_GREY, C_BG);
    tft.setTextSize(1);
    tft.setCursor(10, STATUS_H + 178);
    tft.print("Encoder: brightness  KEY0: on/off");
    tft.setCursor(10, STATUS_H + 192);
    tft.print("Push encoder: back");
}
