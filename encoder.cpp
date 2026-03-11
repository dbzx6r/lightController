#include "encoder.h"
#include "config.h"

// ── State ────────────────────────────────────
static volatile int   _delta      = 0;
static volatile bool  _pushFlag   = false;
static volatile bool  _key0Flag   = false;

// Debounce timestamps
static volatile unsigned long _lastClkTime  = 0;
static volatile unsigned long _lastSwTime   = 0;
static volatile unsigned long _lastKey0Time = 0;

static const unsigned long DEBOUNCE_MS = 5;

// ── ISRs ─────────────────────────────────────
ICACHE_RAM_ATTR void isr_clk() {
    unsigned long now = millis();
    if (now - _lastClkTime < DEBOUNCE_MS) return;
    _lastClkTime = now;
    // Read DT to determine direction
    if (digitalRead(ENCODER_DT_PIN) != digitalRead(ENCODER_CLK_PIN)) {
        _delta++;
    } else {
        _delta--;
    }
}

ICACHE_RAM_ATTR void isr_sw() {
    unsigned long now = millis();
    if (now - _lastSwTime < 50) return; // 50ms debounce for button
    _lastSwTime = now;
    if (digitalRead(ENCODER_SW_PIN) == LOW) {
        _pushFlag = true;
    }
}

ICACHE_RAM_ATTR void isr_key0() {
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
    noInterrupts();
    int d = _delta;
    _delta = 0;
    interrupts();
    return d;
}

bool encoder_getPush() {
    noInterrupts();
    bool v = _pushFlag;
    _pushFlag = false;
    interrupts();
    return v;
}

bool encoder_getKey0() {
    noInterrupts();
    bool v = _key0Flag;
    _key0Flag = false;
    interrupts();
    return v;
}
