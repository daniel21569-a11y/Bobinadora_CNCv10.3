/**
 * @file display_init.h
 * @brief Display initialization for JC4827W543 board
 * Uses Arduino_GFX library with QSPI interface (NV3041A driver)
 * Display: 4.3" 480x272 pixels
 */

#ifndef DISPLAY_INIT_H
#define DISPLAY_INIT_H

#include "PINS_JC4827W543.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Global display instance
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx = nullptr;

// Parámetros PWM para Backlight
const int ledc_channel = 0;
const int ledc_freq = 5000;
const int ledc_res = 8;

/**
 * @brief Initialize the QSPI display (NV3041A 480x272)
 * @return true if successful, false otherwise
 */
bool display_init() {
  Serial.println(
      "[Display] Iniciando configuración QSPI LCD NV3041A 480x272...");

  // Configurar PWM para Backlight
  ledcSetup(ledc_channel, ledc_freq, ledc_res);
  ledcAttachPin(GFX_BL, ledc_channel);
  ledcWrite(ledc_channel, 255); // Brillo máximo inicial

  Serial.println("[Display] Creando bus QSPI...");

  // Create QSPI data bus
  // CS: 45, SCK: 47, D0: 21, D1: 48, D2: 40, D3: 39
  bus = new Arduino_ESP32QSPI(TFT_CS, TFT_SCK, TFT_D0, TFT_D1, TFT_D2, TFT_D3);

  if (!bus) {
    Serial.println("[Display] ERROR: No se pudo crear el bus QSPI!");
    return false;
  }

  Serial.println("[Display] Creando display instance (NV3041A)...");

  // Create display instance for NV3041A
  // RST: -1, Rotation: 0, IPS: true
  gfx = new Arduino_NV3041A(bus, TFT_RST, 0, true);

  if (!gfx) {
    Serial.println("[Display] ERROR: No se pudo crear el display!");
    return false;
  }

  // Initialize display
  Serial.println("[Display] Llamando a gfx->begin()...");
  if (!gfx->begin()) {
    Serial.println("[Display] ERROR: gfx->begin() falló!");
    return false;
  }

  Serial.println("[Display] Display inicializado, limpiando pantalla...");

  // Clear screen to black
  gfx->fillScreen(RGB565_BLACK);
  delay(100);

  // Show test pattern
  Serial.println("[Display] Mostrando patrón de prueba...");
  gfx->fillRect(0, 0, 100, 100, RGB565_RED);
  gfx->fillRect(100, 0, 100, 100, RGB565_GREEN);
  gfx->fillRect(200, 0, 100, 100, RGB565_BLUE);
  gfx->fillRect(300, 0, 100, 100, RGB565_WHITE);

  delay(500);

  Serial.println("[Display] ✓ Display inicializado correctamente!");
  Serial.printf("[Display] Resolución: %dx%d\n", gfx->width(), gfx->height());

  // Clear to black again
  gfx->fillScreen(RGB565_BLACK);

  return true;
}

/**
 * @brief Set backlight brightness
 * @param brightness 0-255 (0=off, 255=max)
 */
void display_backlight(uint8_t brightness) {
  ledcWrite(ledc_channel, brightness);
}

/**
 * @brief Turn backlight on
 */
void display_backlight_on() { ledcWrite(ledc_channel, 255); }

/**
 * @brief Turn backlight off
 */
void display_backlight_off() { ledcWrite(ledc_channel, 0); }

#endif // DISPLAY_INIT_H