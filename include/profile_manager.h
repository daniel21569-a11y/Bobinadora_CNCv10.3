#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include "config.h"
#include <SD.h>
#include <SPI.h>
#include <vector>

namespace ProfileManager {

// Estructura para información de perfil
struct ProfileInfo {
  char nombre[32];
  ModoBobinado modo;
  bool existe;
};

// Inicialización
bool init();
bool isSDAvailable();

// Funciones para Transformador
bool saveTransformadorProfile(const char *nombre,
                              const ConfigTransformador &config);
bool loadTransformadorProfile(const char *nombre, ConfigTransformador &config);
bool deleteTransformadorProfile(const char *nombre);
std::vector<ProfileInfo> listTransformadorProfiles();

// Funciones para Nido de Abeja
bool saveHoneycombProfile(const char *nombre, const ConfigNidoAbeja &config);
bool loadHoneycombProfile(const char *nombre, ConfigNidoAbeja &config);
bool deleteHoneycombProfile(const char *nombre);
std::vector<ProfileInfo> listHoneycombProfiles();

// Utilidades
String sanitizeFilename(const String &nombre);
void createDirectories();

} // namespace ProfileManager

#endif // PROFILE_MANAGER_H
