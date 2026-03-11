// ──────────────────────────────────────────────────────────────────
//  TFT_eSPI User_Setup.h  –  ST7789 240×240  (ESP32)
//
//  IMPORTANT: Copy this file to your TFT_eSPI library folder,
//  replacing the existing User_Setup.h.
//  Location: <Arduino libraries>/TFT_eSPI/User_Setup.h
// ──────────────────────────────────────────────────────────────────

// Driver
#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// Pins — values are GPIO numbers (must match config.h)
// Uses ESP32 VSPI bus
#define TFT_CS   5   // GPIO5
#define TFT_DC   2   // GPIO2
#define TFT_RST  4   // GPIO4
#define TFT_MOSI 23  // GPIO23  (VSPI MOSI)
#define TFT_SCLK 18  // GPIO18  (VSPI SCLK)

// SPI bus frequency
#define SPI_FREQUENCY  40000000

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT
