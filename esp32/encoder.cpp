#include "encoder.h"
#include "config.h"

// ── State ────────────────────────────────────
static volatile int   _delta      = 0;
static volatile bool  _pushFlag   = false;
static volatile bool  _key0Flag   = false;

static volatile unsigned long _lastClkTime  = 0;
static volatile unsigned long _lastSwTime   = 0;
static volatile unsigned long _lastKey0Time = 0;

static const unsigned long DEBOUNCE_MS = 5;

// ── ISRs ─────────────────────────────────────
// ESP32 uses IRAM_ATTR (vs ESP8266's ICACHE_RAM_ATTR)
IRAM_ATTR void isr_clk() {
    unsigned long now = millis();
    if (now - _lastClkTime < DEBOUNCE_MS) return;
    _lastClkTime = now;
    if (digitalRead(ENCODER_DT_PIN) != digitalRead(ENCODER_CLK_PIN)) {
        _delta++;
    } else {
        _delta--;
    }
}

IRAM_ATTR void isr_sw() {
    unsigned long now = millis();
    if (now - _lastSwTime < 50) return;
    _lastSwTime = now;
    if (digitalRead(ENCODER_SW_PIN) == LOW) {
        _pushFlag = true;
    }
}

IRAM_ATTR void isr_key0() {
    unsigned long now = millis();
    if (now - _lastKey0Time < 50) return;
    _lastKey0Time = now;
    if (digitalRead(KEY0_PIN) == LOW) {
        _key0Flag = true;
    }
}

// ── Public API ───────────────────────────────
void encoder_init() {
    pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
    pinMode(ENCODER_DT_PIN,  INPUT_PULLUP);
    pinMode(ENCODER_SW_PIN,  INPUT_PULLUP);
    pinMode(KEY0_PIN,        INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), isr_clk,  CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_SW_PIN),  isr_sw,   FALLING);
    attachInterrupt(digitalPinToInterrupt(KEY0_PIN),        isr_key0, FALLING);
}

int encoder_getDelta() {
    portDISABLE_INTERRUPTS();
    int d = _delta;
    _delta = 0;
    portENABLE_INTERRUPTS();
    return d;
}

bool encoder_getPush() {
    portDISABLE_INTERRUPTS();
    bool v = _pushFlag;
    _pushFlag = false;
    portENABLE_INTERRUPTS();
    return v;
}

bool encoder_getKey0() {
    portDISABLE_INTERRUPTS();
    bool v = _key0Flag;
    _key0Flag = false;
    portENABLE_INTERRUPTS();
    return v;
}
