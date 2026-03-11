// ──────────────────────────────────────────────────────────────────
//  TFT_eSPI User_Setup.h  –  ST7789 240×240
//
//  IMPORTANT: Copy this file to your TFT_eSPI library folder,
//  replacing the existing User_Setup.h.
//  Location: <Arduino libraries>/TFT_eSPI/User_Setup.h
// ──────────────────────────────────────────────────────────────────

// Driver
#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// Pins (must match config.h)
#define TFT_CS   4   // D2 / GPIO4
#define TFT_DC   5   // D1 / GPIO5
#define TFT_RST  16  // D0 / GPIO16
#define TFT_MOSI 13  // D7 / GPIO13  (hardware SPI MOSI)
#define TFT_SCLK 14  // D5 / GPIO14  (hardware SPI SCLK)

// Use hardware SPI
#define USE_HSPI_PORT

// SPI bus frequency
#define SPI_FREQUENCY  40000000

// Optional DMA
// #define ESP8266_DMA

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT
