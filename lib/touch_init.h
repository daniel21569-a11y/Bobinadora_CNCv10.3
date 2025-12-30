/**
 * @file touch_init.h
 * @brief Touch controller initialization for JC4827W543 board
 * Uses TouchLib for GT911 capacitive touch controller
 * Based on working example from Demo_VSCode_Platformio
 */

#ifndef TOUCH_INIT_H
#define TOUCH_INIT_H

#include "PINS_JC4827W543.h"
#include <Arduino.h>
#include <Wire.h>


// Incluir la librería TouchLib
#ifdef TOUCH_MODULES_GT911
#include "TouchLib.h"
#else
#error                                                                         \
    "TOUCH_MODULES_GT911 no está definido. Agregar -D TOUCH_MODULES_GT911 en build_flags"
#endif

// Global touch instance with RESET pin
TouchLib touch(Wire, TOUCH_SDA, TOUCH_SCL, TOUCH_ADDR, TOUCH_RES);

// Touch mapping variables (from working example)
bool touch_swap_xy = false;
int16_t touch_map_x1 = -1;
int16_t touch_map_x2 = -1;
int16_t touch_map_y1 = -1;
int16_t touch_map_y2 = -1;

int16_t touch_max_x = 0, touch_max_y = 0;
int16_t touch_raw_x = 0, touch_raw_y = 0;
int16_t touch_last_x = 0, touch_last_y = 0;

/**
 * @brief Translate raw touch coordinates to screen coordinates
 */
void translate_touch_raw() {
  if (touch_swap_xy) {
    touch_last_x = map(touch_raw_y, touch_map_x1, touch_map_x2, 0, touch_max_x);
    touch_last_y = map(touch_raw_x, touch_map_y1, touch_map_y2, 0, touch_max_y);
  } else {
    touch_last_x = map(touch_raw_x, touch_map_x1, touch_map_x2, 0, touch_max_x);
    touch_last_y = map(touch_raw_y, touch_map_y1, touch_map_y2, 0, touch_max_y);
  }
}

/**
 * @brief Initialize GT911 touch controller
 * @param w Screen width
 * @param h Screen height
 * @param rotation Screen rotation (0, 1, 2, 3)
 * @return true if successful, false otherwise
 */
bool touch_init(int16_t w, int16_t h, uint8_t rotation) {
  Serial.println("[Touch] Inicializando GT911...");
  Serial.printf("[Touch] Pines: SDA=%d, SCL=%d, RES=%d\n", TOUCH_SDA, TOUCH_SCL,
                TOUCH_RES);
  Serial.printf("[Touch] Pantalla: %dx%d, Rotación: %d\n", w, h, rotation);

  touch_max_x = w - 1;
  touch_max_y = h - 1;

  // Configure mapping based on rotation (from working example)
  if (touch_map_x1 == -1) {
    switch (rotation) {
    case 3:
      touch_swap_xy = true;
      touch_map_x1 = touch_max_x;
      touch_map_x2 = 0;
      touch_map_y1 = 0;
      touch_map_y2 = touch_max_y;
      break;
    case 2:
      touch_swap_xy = false;
      touch_map_x1 = touch_max_x;
      touch_map_x2 = 0;
      touch_map_y1 = touch_max_y;
      touch_map_y2 = 0;
      break;
    case 1:
      touch_swap_xy = true;
      touch_map_x1 = 0;
      touch_map_x2 = touch_max_x;
      touch_map_y1 = touch_max_y;
      touch_map_y2 = 0;
      break;
    default: // case 0:
      touch_swap_xy = false;
      touch_map_x1 = 0;
      touch_map_x2 = touch_max_x;
      touch_map_y1 = 0;
      touch_map_y2 = touch_max_y;
      break;
    }
  }

  Serial.printf("[Touch] Mapeo: swap_xy=%d, x1=%d, x2=%d, y1=%d, y2=%d\n",
                touch_swap_xy, touch_map_x1, touch_map_x2, touch_map_y1,
                touch_map_y2);

  // Intentar inicializar el touch con múltiples intentos
  bool init_success = false;
  for (int attempt = 1; attempt <= 3; attempt++) {
    Serial.printf("[Touch] Intento %d/3...\n", attempt);

    if (touch.init()) {
      init_success = true;
      Serial.println("[Touch] ✓ Touch controller inicializado correctamente!");

      // Obtener información del touch
      Serial.printf("[Touch] Puntos de touch soportados: %d\n",
                    touch.getPointNum());

      break;
    } else {
      Serial.printf("[Touch] Intento %d falló\n", attempt);
      if (attempt < 3) {
        delay(500);
      }
    }
  }

  if (!init_success) {
    Serial.println("[Touch] ADVERTENCIA: Touch controller no responde");
    Serial.println("[Touch] Verificar conexiones I2C y pines");
    return false;
  }

  return true;
}

/**
 * @brief Backward compatibility - init without parameters
 */
bool touch_init() { return touch_init(480, 272, 0); }

/**
 * @brief Read touch data
 * @param x Pointer to store X coordinate
 * @param y Pointer to store Y coordinate
 * @return true if touched, false otherwise
 */
bool touch_read(int16_t *x, int16_t *y) {
  if (touch.read()) {
    TP_Point t = touch.getPoint(0);
    touch_raw_x = t.x;
    touch_raw_y = t.y;

    // Transform coordinates
    translate_touch_raw();

    *x = touch_last_x;
    *y = touch_last_y;

    // Debug output (comentar después de calibrar)
    // Serial.printf("Touch: raw(%d,%d) -> mapped(%d,%d)\n", touch_raw_x,
    // touch_raw_y, touch_last_x, touch_last_y);

    return true;
  }
  return false;
}

/**
 * @brief Check if screen is being touched
 * @return true if touched, false otherwise
 */
bool touch_is_pressed() { return touch.read(); }

#endif // TOUCH_INIT_H