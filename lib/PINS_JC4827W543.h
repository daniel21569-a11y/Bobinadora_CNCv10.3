/**
 * @file PINS_JC4827W543.h
 * @brief Pin definitions for JC4827W543 board (ESP32-S3 with QSPI LCD)
 *
 * Display: 4.3" 480x272 QSPI LCD (Driver: NV3041A)
 * Touch: GT911 Capacitive Touch Controller
 */

#ifndef PINS_JC4827W543_H
#define PINS_JC4827W543_H

// =========================================================================
// QSPI LCD PINS (Quad SPI interface)
// =========================================================================
#define GFX_BL 1 // Backlight control

// QSPI Interface pins
#define TFT_CS 45
#define TFT_SCK 47
#define TFT_D0 21
#define TFT_D1 48
#define TFT_D2 40
#define TFT_D3 39

// Reset pin (not defined in example, using GFX_NOT_DEFINED)
#define TFT_RST -1

// Display Resolution
#define TFT_WIDTH 480
#define TFT_HEIGHT 272

// Display Rotation
#define TFT_ROTATION 0

// =========================================================================
// TOUCH PINS (GT911)
// =========================================================================
#define TOUCH_SDA 8
#define TOUCH_SCL 4
#define TOUCH_INT 3
#define TOUCH_RES 38

// I2C Address for GT911
#define TOUCH_ADDR GT911_SLAVE_ADDRESS1 // 0x5D

#endif // PINS_JC4827W543_H