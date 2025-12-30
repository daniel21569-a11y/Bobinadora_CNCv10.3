# Bobinadora CNC v10.3

Proyecto de Bobinadora CNC automatizada basada en **ESP32-S3** con una interfaz gráfica moderna utilizando **LVGL 9.x**. Esta solución integral permite el bobinado preciso de transformadores y bobinas de nido de abeja, ofreciendo un control total sobre parámetros como el diámetro del hilo, la velocidad y la distribución de capas.

Esta versión (10.3) está optimizada para la placa profesional **JC4827W543** (4.3" 480x272 RGB LCD), aprovechando su potencia para gestionar una interfaz fluida y un control de motores en tiempo real de alta fidelidad.

## 🚀 Novedades v10.3
- **Corrección de Bugs**: Solucionado el error donde las vueltas totales mostraban siempre 0.
- **Mejora de Interfaz**: Títulos de modo dinámicos en la pantalla de bobinado (Transformador / Nido de Abeja).
- **Optimización**: Mejora en la estabilidad de la tarea de UI en el Core 0.
- **Limpieza**: Eliminación de código obsoleto y mejora en la legibilidad.

## 🛠 Hardware Necesario

### Controlador
- **Placa**: JC4827W543 (ESP32-S3 con pantalla integrada de 4.3").
- **Pantalla**: 480x272 RGB LCD (NV3041A).
- **Touch**: Capacitivo GT911.

### Motores y Control
- **Driver de Motores**: Recomendados A4988, DRV8825 o TB6600.
- **Motores**: NEMA 17 o similares.
- **Eje X (Carro)**: Desplaza el hilo a lo largo del carrete.
- **Eje Y (Mandril)**: Gira el carrete para recibir el hilo.
- **Finales de Carrera**: 2 microswitches (Eje X y Eje Y).

### Pinout (Configuración por Defecto)
| Componente | Pin ESP32-S3 |
|------------|--------------|
| **Eje X - STEP** | 5 |
| **Eje X - DIR** | 9 |
| **Eje X - EN** | 14 |
| **Eje X - LIMIT** | 46 |
| **Eje Y - STEP** | 6 |
| **Eje Y - DIR** | 7 |
| **Eje Y - EN** | 15 |
| **Eje Y - LIMIT** | 16 |
| **Touch SDA** | 19 |
| **Touch SCL** | 20 |
| **Touch INT** | 18 |

## 💻 Instalación y Carga

Este proyecto está desarrollado bajo **PlatformIO**.

1. **Requisitos**: Tener instalado VS Code con la extensión PlatformIO.
2. **Clonar**: Descarga o clona este repositorio.
3. **Dependencias**: PlatformIO descargará automáticamente las librerías necesarias:
   - `lvgl 9.2.2`
   - `GFX Library for Arduino`
   - `TouchLib`
4. **Cargar**: 
   - Conecta la placa JC4827W543 vía USB.
   - Pulsa el icono de **Upload** (flecha derecha) en la barra inferior de PlatformIO.

## ⚙️ Funcionamiento

### Modos de Bobinado
1. **Transformador**: Bobinado tradicional capa por capa. El sistema calcula automáticamente el avance del carro basándose en el diámetro del hilo.
2. **Nido de Abeja (Honeycomb)**: Bobinado cruzado con desfase angular, ideal para bobinas de alta frecuencia o de aire.

### Pasos de Operación
1. **Homing**: Al encender, el sistema realiza un homing automático para encontrar el punto cero.
2. **Configuración**: Selecciona el modo y ajusta los parámetros (diámetro de hilo, vueltas, ancho, etc.).
3. **Control Manual**: Puedes mover los ejes manualmente desde la pantalla de control manual para ajustar el inicio del bobinado.
4. **Inicio**: Pulsa "BOBINAR" e inicia el proceso. La pantalla mostrará progreso real, RPM y tiempo restante.

---
---
**Nota**: El soporte para tarjetas MicroSD (ProfileManager) está actualmente deshabilitado en esta versión para mayor estabilidad del hardware ESP32-S3.

## 👥 Créditos y Copyright
- **Daniel Rodriguez Gonzalez** ([@daniel21569-a11y](https://github.com/daniel21569-a11y)) - Desarrollador y diseñador principal.

Copyright (c) 2025 Daniel Rodriguez Gonzalez. Todos los derechos reservados bajo la licencia GPL v3.
