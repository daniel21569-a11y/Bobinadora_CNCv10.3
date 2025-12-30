#include "ui_handlers.h"
#include "config.h"
#include "persistence.h"
#include "ui_components.h"
#include "ui_screens.h"
#include <Arduino.h>

namespace UIHandlers {

void btn_navegacion_handler(lv_event_t *e) {
  lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
  const char *id = (const char *)lv_obj_get_user_data(target);

  if (strcmp(id, "SELECCION_MODO") == 0)
    lv_scr_load_anim(UIScreens::screen_modo_selection, LV_SCR_LOAD_ANIM_FADE_IN,
                     300, 0, false);
  else if (strcmp(id, "CONFIG") == 0)
    lv_scr_load_anim(UIScreens::screen_config, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0,
                     false);
  else if (strcmp(id, "CONFIG_HC") == 0)
    lv_scr_load_anim(UIScreens::screen_config_honeycomb,
                     LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
  else if (strcmp(id, "BOBINADO") == 0)
    lv_scr_load_anim(UIScreens::screen_winding, LV_SCR_LOAD_ANIM_FADE_IN, 300,
                     0, false);
  else if (strcmp(id, "MANUAL") == 0)
    lv_scr_load_anim(UIScreens::screen_manual_control, LV_SCR_LOAD_ANIM_FADE_IN,
                     300, 0, false);
  else if (strcmp(id, "PRINCIPAL") == 0)
    lv_scr_load_anim(UIScreens::screen_main, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0,
                     false);
}

void btn_modo_handler(lv_event_t *e) {
  lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
  const char *id = (const char *)lv_obj_get_user_data(target);

  if (strcmp(id, "MODO_TRANSFORMADOR") == 0) {
    Sistema::estado.modo = ModoBobinado::TRANSFORMADOR;
    Serial.println("Modo TRANSFORMADOR seleccionado");
    lv_scr_load_anim(UIScreens::screen_config, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0,
                     false);
  } else if (strcmp(id, "MODO_HONEYCOMB") == 0) {
    Sistema::estado.modo = ModoBobinado::NIDO_ABEJA;
    Serial.println("Modo NIDO DE ABEJA seleccionado");
    lv_scr_load_anim(UIScreens::screen_config_honeycomb,
                     LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
  }
}

void btn_comando_handler(lv_event_t *e) {
  lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
  const char *id = (const char *)lv_obj_get_user_data(target);

  if (strcmp(id, "HOME") == 0) {
    if (Sistema::estado.estado == EstadoBobinado::LISTO ||
        Sistema::estado.estado == EstadoBobinado::PAUSADO ||
        Sistema::estado.estado == EstadoBobinado::ERROR) {
      Serial.println(">>> COMANDO: HOME MANUAL <<<");
      homing_ejes();
      if (UIScreens::label_estado) {
        lv_label_set_text(UIScreens::label_estado, "HOME OK");
      }
    }
  } else if (strcmp(id, "INICIAR") == 0 &&
             Sistema::estado.estado == EstadoBobinado::LISTO) {
    Sistema::estado.iniciar_bobinado();
  } else if (strcmp(id, "PROSEGUIR") == 0 &&
             Sistema::estado.estado == EstadoBobinado::PAUSADO) {
    Sistema::estado.reanudar_bobinado();
  } else if (strcmp(id, "PAUSAR") == 0 &&
             Sistema::estado.estado == EstadoBobinado::BOBINANDO) {
    Sistema::estado.pausar_bobinado();
  } else if (strcmp(id, "PARAR") == 0) {
    Sistema::estado.detener_bobinado();
    Sistema::estado.reset_solicitado = true;
  }
}

void btn_manual_handler(lv_event_t *e) {
  lv_event_code_t event_code = lv_event_get_code(e);

  if (!Sistema::estado.puede_mover_manual())
    return;

  lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
  const char *id = (const char *)lv_obj_get_user_data(target);

  // Variables estáticas para rastrear el estado de pulsación
  static bool is_long_press = false;

  // Parámetros de movimiento
  const int delay_us = 500;
  const int pasos_X_corto = Hardware::Motor::PASOS_POR_MM_X / 2; // 0.5 mm
  const int pasos_X_continuo = Hardware::Motor::PASOS_POR_MM_X /
                               10; // Paso pequeño para movimiento continuo
  const int pasos_Y_corto =
      Hardware::Motor::PASOS_POR_VUELTA_Y / 4; // 0.25 vueltas
  const int pasos_Y_continuo = Hardware::Motor::PASOS_POR_VUELTA_Y /
                               40; // Paso pequeño para movimiento continuo

  // Evento PRESSED: Inicio de pulsación
  if (event_code == LV_EVENT_PRESSED) {
    is_long_press = false;
  }

  // Evento LONG_PRESSED_REPEAT: Pulsación larga (movimiento continuo)
  else if (event_code == LV_EVENT_LONG_PRESSED_REPEAT) {
    is_long_press = true;

    if (strcmp(id, "X+") == 0)
      move_motor_steps_safe(Hardware::Motor::STEP_X_PIN,
                            Hardware::Motor::DIR_X_PIN, pasos_X_continuo,
                            delay_us);
    else if (strcmp(id, "X-") == 0)
      move_motor_steps_safe(Hardware::Motor::STEP_X_PIN,
                            Hardware::Motor::DIR_X_PIN, -pasos_X_continuo,
                            delay_us);
    else if (strcmp(id, "Y+") == 0)
      move_motor_steps_safe(Hardware::Motor::STEP_Y_PIN,
                            Hardware::Motor::DIR_Y_PIN, pasos_Y_continuo,
                            delay_us);
    else if (strcmp(id, "Y-") == 0)
      move_motor_steps_safe(Hardware::Motor::STEP_Y_PIN,
                            Hardware::Motor::DIR_Y_PIN, -pasos_Y_continuo,
                            delay_us);
  }

  // Evento RELEASED: Fin de pulsación
  else if (event_code == LV_EVENT_RELEASED) {
    // Si fue una pulsación corta (no long press), hacer movimiento fijo
    if (!is_long_press) {
      if (strcmp(id, "X+") == 0)
        move_motor_steps_safe(Hardware::Motor::STEP_X_PIN,
                              Hardware::Motor::DIR_X_PIN, pasos_X_corto,
                              delay_us);
      else if (strcmp(id, "X-") == 0)
        move_motor_steps_safe(Hardware::Motor::STEP_X_PIN,
                              Hardware::Motor::DIR_X_PIN, -pasos_X_corto,
                              delay_us);
      else if (strcmp(id, "Y+") == 0)
        move_motor_steps_safe(Hardware::Motor::STEP_Y_PIN,
                              Hardware::Motor::DIR_Y_PIN, pasos_Y_corto,
                              delay_us);
      else if (strcmp(id, "Y-") == 0)
        move_motor_steps_safe(Hardware::Motor::STEP_Y_PIN,
                              Hardware::Motor::DIR_Y_PIN, -pasos_Y_corto,
                              delay_us);
    }
    is_long_press = false;
  }
}

void btn_manual_back_handler(lv_event_t *e) {
  // Establecer la posición actual como el nuevo origen (Cero)
  Sistema::estado.pasos_X_acumulados = 0;
  Sistema::estado.vueltas_completadas = 0;
  Sistema::estado.vueltas_capa_actual = 0;
  Sistema::estado.capas_completadas = 0;

  // Asegurar que el estado sea LISTO
  Sistema::estado.estado = EstadoBobinado::LISTO;

  Serial.println("[Manual] Nueva posición establecida como origen.");

  // Volver a la pantalla principal
  lv_scr_load_anim(UIScreens::screen_main, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0,
                   false);
}

void ta_event_cb(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
    UI::Numpad::show(target);
  }
}

void screen_winding_load_handler(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_SCREEN_LOADED) {
    Sistema::actualizar_configuracion();

    // Actualizar el label de vueltas totales al entrar en la pantalla
    if (UIScreens::label_vueltas_totales) {
      uint32_t total = (Sistema::estado.modo == ModoBobinado::TRANSFORMADOR)
                           ? Sistema::config_transformador.vueltas_total
                           : Sistema::config_nido_abeja.num_vueltas;
      lv_label_set_text_fmt(UIScreens::label_vueltas_totales, "/ %lu", total);
    }

    // También actualizar el modo en el header
    if (UIScreens::label_modo_actual) {
      if (Sistema::estado.modo == ModoBobinado::TRANSFORMADOR) {
        lv_label_set_text(UIScreens::label_modo_actual,
                          LV_SYMBOL_LOOP " Transformador");
      } else {
        lv_label_set_text(UIScreens::label_modo_actual,
                          LV_SYMBOL_IMAGE " Nido de Abeja");
      }
    }
  }
}

void back_and_save_handler(lv_event_t *e) {
  Sistema::config_transformador.diametro_alambre_mm =
      atof(lv_textarea_get_text(UIScreens::ta_diametro_alambre));
  Sistema::config_transformador.longitud_bobinado_mm =
      atof(lv_textarea_get_text(UIScreens::ta_longitud_bobinado));
  Sistema::config_transformador.vueltas_total =
      atoi(lv_textarea_get_text(UIScreens::ta_vueltas_total));
  Sistema::config_transformador.velocidad_rpm =
      atof(lv_textarea_get_text(UIScreens::ta_velocidad_rpm));
  Sistema::config_transformador.detener_en_capas =
      lv_obj_has_state(UIScreens::sw_detener_en_capas, LV_STATE_CHECKED);

  if (Sistema::config_transformador.validar()) {
    Sistema::config_transformador.calcular_parametros();
    Sistema::config_transformador.imprimir_debug();
    Persistence.saveTransformadorConfig();
  }

  lv_scr_load_anim(UIScreens::screen_main, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0,
                   false);
}

void back_and_save_honeycomb_handler(lv_event_t *e) {
  Sistema::config_nido_abeja.diametro_hilo =
      atof(lv_textarea_get_text(UIScreens::ta_hc_diametro_hilo));
  Sistema::config_nido_abeja.diametro_carrete =
      atof(lv_textarea_get_text(UIScreens::ta_hc_diametro_carrete));
  Sistema::config_nido_abeja.ancho_carrete =
      atof(lv_textarea_get_text(UIScreens::ta_hc_ancho_carrete));
  Sistema::config_nido_abeja.desfase_grados =
      atof(lv_textarea_get_text(UIScreens::ta_hc_desfase_grados));
  Sistema::config_nido_abeja.num_vueltas =
      atoi(lv_textarea_get_text(UIScreens::ta_hc_num_vueltas));
  Sistema::config_nido_abeja.velocidad_rpm =
      atof(lv_textarea_get_text(UIScreens::ta_hc_velocidad));

  if (Sistema::config_nido_abeja.validar()) {
    Sistema::config_nido_abeja.calcular_parametros();
    Sistema::config_nido_abeja.imprimir_debug();
    Persistence.saveHoneycombConfig();
  }

  lv_scr_load_anim(UIScreens::screen_main, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0,
                   false);
}

void validate_and_wind_handler(lv_event_t *e) {
  back_and_save_handler(e);
  lv_scr_load_anim(UIScreens::screen_winding, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0,
                   false);
}

void validate_and_wind_honeycomb_handler(lv_event_t *e) {
  back_and_save_honeycomb_handler(e);
  lv_scr_load_anim(UIScreens::screen_winding, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0,
                   false);
}

// =========================================================================
// FUNCIÓN DE ACTUALIZACIÓN DE UI - VERSIÓN OPTIMIZADA CON CACHE
// =========================================================================
void update_winding_screen() {
  // Verificar que estamos en la pantalla correcta
  if (lv_scr_act() != UIScreens::screen_winding)
    return;

  // Variables estáticas para cachear valores previos (EVITAR REDIBUJADO
  // INNECESARIO)
  static uint32_t prev_vueltas = 0xFFFFFFFF;
  static uint32_t prev_vueltas_capa = 0xFFFFFFFF;
  static uint32_t prev_capas = 0xFFFFFFFF;
  static float prev_rpm = -1.0f;
  static EstadoBobinado prev_estado = EstadoBobinado::LISTO;
  static int32_t prev_progreso = -1;

  // Leer variables volatile UNA SOLA VEZ
  uint32_t vueltas_actual_local = Sistema::estado.vueltas_completadas;
  uint32_t vueltas_capa_local = Sistema::estado.vueltas_capa_actual;
  uint32_t capas_actual_local = Sistema::estado.capas_completadas;
  float rpm_actual_local = Sistema::estado.rpm_actual;
  EstadoBobinado estado_local = Sistema::estado.estado;
  ModoBobinado modo_local = Sistema::estado.modo;

  // SOLO actualizar estado si cambió
  if (estado_local != prev_estado) {
    const char *estado_str;
    lv_color_t estado_color;

    switch (estado_local) {
    case EstadoBobinado::BOBINANDO:
      estado_str = "BOBINANDO";
      estado_color = UI::color_success;
      break;
    case EstadoBobinado::PAUSADO:
      estado_str = "PAUSADO";
      estado_color = UI::color_warning;
      break;
    case EstadoBobinado::ERROR:
      estado_str = "ERROR";
      estado_color = UI::color_danger;
      break;
    case EstadoBobinado::HOMING:
      estado_str = "HOMING";
      estado_color = UI::color_primary;
      break;
    default:
      estado_str = "LISTO";
      estado_color = UI::color_secondary;
      break;
    }

    if (UIScreens::label_estado) {
      lv_label_set_text(UIScreens::label_estado, estado_str);
      lv_obj_set_style_text_color(UIScreens::label_estado, estado_color, 0);
    }
    prev_estado = estado_local;
  }

  uint32_t vueltas_objetivo = (modo_local == ModoBobinado::TRANSFORMADOR)
                                  ? Sistema::config_transformador.vueltas_total
                                  : Sistema::config_nido_abeja.num_vueltas;

  // SOLO actualizar vueltas si cambiaron
  if (vueltas_actual_local != prev_vueltas) {
    if (UIScreens::label_vueltas_actuales) {
      lv_label_set_text_fmt(UIScreens::label_vueltas_actuales, "%lu",
                            vueltas_actual_local);
    }
    prev_vueltas = vueltas_actual_local;
  }

  // SOLO actualizar progreso si cambió significativamente
  int32_t progreso =
      (vueltas_objetivo > 0)
          ? (int32_t)((vueltas_actual_local * 100) / vueltas_objetivo)
          : 0;
  if (progreso > 100)
    progreso = 100;

  if (progreso != prev_progreso) {
    if (UIScreens::bar_progreso) {
      lv_bar_set_value(UIScreens::bar_progreso, progreso, LV_ANIM_OFF);
    }
    if (UIScreens::label_progreso) {
      lv_label_set_text_fmt(UIScreens::label_progreso, "%ld%%", progreso);
    }
    prev_progreso = progreso;
  }

  // SOLO actualizar RPM si cambió significativamente (>= 1 RPM de diferencia)
  if (fabs(rpm_actual_local - prev_rpm) >= 1.0f) {
    if (UIScreens::label_velocidad_actual) {
      lv_label_set_text_fmt(UIScreens::label_velocidad_actual,
                            LV_SYMBOL_CHARGE " %.0f RPM", rpm_actual_local);
    }
    prev_rpm = rpm_actual_local;
  }

  // Actualizar información de capas solo si cambió
  if (vueltas_capa_local != prev_vueltas_capa ||
      capas_actual_local != prev_capas) {
    uint32_t capas_totales = (modo_local == ModoBobinado::TRANSFORMADOR)
                                 ? Sistema::config_transformador.capas_estimadas
                                 : Sistema::config_nido_abeja.capas_estimadas;

    uint32_t vueltas_por_capa =
        (modo_local == ModoBobinado::TRANSFORMADOR)
            ? Sistema::config_transformador.vueltas_por_capa
            : Sistema::config_nido_abeja.vueltas_por_capa;

    if (UIScreens::label_capa_info) {
      if (modo_local == ModoBobinado::TRANSFORMADOR) {
        uint32_t capa_actual_display =
            (capas_totales > 0 && capas_actual_local < capas_totales)
                ? capas_actual_local + 1
                : capas_totales;

        float grosor_actual_mm =
            capas_actual_local *
            Sistema::config_transformador.diametro_alambre_mm;

        lv_label_set_text_fmt(
            UIScreens::label_capa_info,
            "Capa: %lu/%lu | Vueltas: %lu/%lu | Grosor: %.2f mm",
            capa_actual_display, capas_totales, vueltas_capa_local,
            vueltas_por_capa, grosor_actual_mm);
      } else {
        float grosor_actual = 0.0f;
        if (vueltas_por_capa > 0) {
          float capas_actuales_float =
              (float)vueltas_actual_local / (float)vueltas_por_capa;
          grosor_actual =
              capas_actuales_float * Sistema::config_nido_abeja.diametro_hilo;
        }

        lv_label_set_text_fmt(
            UIScreens::label_capa_info,
            "Grosor: %.2f mm | Desfase: %.1f° | Ancho: %.1f mm", grosor_actual,
            Sistema::config_nido_abeja.desfase_grados,
            Sistema::config_nido_abeja.ancho_carrete);
      }
    }
    prev_vueltas_capa = vueltas_capa_local;
    prev_capas = capas_actual_local;
  }

  // Actualizar tiempo restante (cada segundo aproximadamente)
  if (UIScreens::label_tiempo_restante) {
    if (estado_local == EstadoBobinado::BOBINANDO && rpm_actual_local > 1.0f) {
      if (vueltas_actual_local < vueltas_objetivo) {
        uint32_t vueltas_restantes = vueltas_objetivo - vueltas_actual_local;
        float minutos_float = (float)vueltas_restantes / rpm_actual_local;
        int total_seconds = (int)(minutos_float * 60.0f);

        int horas = total_seconds / 3600;
        int min = (total_seconds % 3600) / 60;
        int sec = total_seconds % 60;

        if (horas > 0) {
          lv_label_set_text_fmt(UIScreens::label_tiempo_restante,
                                LV_SYMBOL_POWER " %02d:%02d:%02d", horas, min,
                                sec);
        } else {
          lv_label_set_text_fmt(UIScreens::label_tiempo_restante,
                                LV_SYMBOL_POWER " %02d:%02d", min, sec);
        }
      } else {
        lv_label_set_text(UIScreens::label_tiempo_restante,
                          LV_SYMBOL_POWER " 00:00");
      }
    } else {
      lv_label_set_text(UIScreens::label_tiempo_restante,
                        LV_SYMBOL_POWER " --:--");
    }
  }
}

// Implementaciones stub de funciones de hardware
__attribute__((weak)) void homing_ejes() {
  Serial.println("homing_ejes() llamada desde UI handler");
}

__attribute__((weak)) void move_motor_steps_safe(int step_pin, int dir_pin,
                                                 int steps, int delay_us) {
  Serial.println("move_motor_steps_safe() llamada desde UI handler");
}
} // namespace UIHandlers