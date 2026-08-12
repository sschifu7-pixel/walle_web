# 🤖 Wall-e (Robot Gualí) — Sistema Robótico Bibliotecario con NFC y Web

Sistema integral de robótica educativa y gestión bibliotecaria que combina un autómata físico inspirador (**Wall-e / Gualí**), captura de tarjetas/etiquetas NFC mediante lectores PN532, control de microcontroladores (ESP32 y Arduino Nano) y una plataforma web centralizada desarrollada en **Django**.

---

## 📌 Tabla de Contenidos
- [Visión del Proyecto](#-visión-del-proyecto)
- [Arquitectura del Sistema](#-arquitectura-del-sistema)
- [Estructura del Repositorio](#-estructura-del-repositorio)
- [Hardware y Componentes](#-hardware-y-componentes)
- [Software y Firmware](#-software-y-firmware)
  - [Firmware ESP32 (Lector NFC + Servidor Web)](#1-firmware-esp32-lector-nfc--servidor-web)
  - [Firmware Arduino Nano (Movimiento y Sensores)](#2-firmware-arduino-nano-movimiento-y-sensores)
  - [Aplicación Web Django](#3-aplicación-web-django)
- [Instalación y Configuración](#-instalación-y-configuración)
- [Licencia y Créditos](#-licencia-y-créditos)

---

## 🎯 Visión del Proyecto

El propósito de **Gualí (Wall-e)** es transformar el proceso tradicional de préstamo de libros en una experiencia táctil, interactiva y dinámica. 

Al aproximar un libro equipado con etiqueta NFC a la mano del robot, el sensor **PN532** captura instantáneamente el identificador único (UID) en formatos **Hexadecimal** y **Decimal (Wiegand / Bytes)**, transmitiendo los datos de manera asíncrona tanto al servidor web incrustado del ESP32 como al sistema central de gestión bibliotecaria en **Django**.

---

## 🏗️ Arquitectura del Sistema

```mermaid
graph TD
    A[Libro con Etiqueta NFC / Tarjeta RFID] -->|13.56 MHz| B[Lector PN532 en la Mano de Wall-e]
    B -->|Bus I2C| C[ESP32 Microcontroller]
    C -->|Servidor Web Async + mDNS| D[Consola Web de Diagnóstico http://Wall-e.local]
    C -->|API JSON / HTTP POST| E[Servidor Web Django]
    F[Arduino Nano] -->|Control Servo / Sensores| G[Movimiento de Brazos, LCD 16x2 I2C, HC-SR04]
    E -->|Base de Datos| H[Gestión de Préstamos y Catálogo]
```

---

## 📁 Estructura del Repositorio

```text
Wall-e/
├── docs/                             # Documentación técnica e imágenes
│   ├── images/                       # Fotografías del robot e ilustraciones
│   └── manual_tecnico.md             # Manual detallado de implementación técnica
├── firmware/                         # Código fuente de microcontroladores
│   ├── esp32/                        # Firmware ESP32 (Lector NFC PN532 + Servidor Web)
│   │   └── esp32_nfc_server.ino
│   └── arduino_nano/                 # Firmware Arduino Nano (Servos, LCD 16x2, Sensores)
│       ├── arduino_nano_full.ino     # Versión completa con ultrasónico HC-SR04
│       └── arduino_nano_base.ino     # Versión base de servomotores y sonido
├── libraries/                        # Librerías Arduino requeridas
│   ├── Adafruit_PN532/               # Control del chip NFC PN532
│   ├── LiquidCrystal_I2C/            # Control de la pantalla LCD I2C
│   └── ...
├── web/                              # Aplicación Web principal (Django)
│   ├── biblioteca/                   # App principal de gestión bibliotecaria
│   ├── walle_biblioteca/             # Configuración del proyecto Django (settings, urls)
│   ├── templates/                    # Plantillas HTML5 de la interfaz web
│   ├── static/                       # Estilos CSS, JavaScript y assets estáticos
│   ├── manage.py                     # Script de gestión de Django
│   ├── requirements.txt              # Dependencias de Python
│   ├── render.yaml                   # Configuración para despliegue en Render
│   └── build.sh                      # Script de construcción para producción
├── .gitignore                        # Exclusiones de Git (archivos temporales, BD, claves)
└── README.md                         # Documentación principal del repositorio
```

---

## 🔌 Hardware y Componentes

| Componente | Modelo / Especificación | Función en el Robot |
| :--- | :--- | :--- |
| **Controlador Principal IoT** | ESP32 DevKit V1 | Manejo de stack TCP/IP, servidor web async, mDNS y lectura NFC |
| **Sensor NFC / RFID** | Módulo PN532 (I2C) | Lectura de etiquetas NTAG/Mifare 13.56 MHz instaladas en libros |
| **Controlador Secundario** | Arduino Nano | Control de servomotores y captura de datos de sensores ambientales |
| **Pantalla** | LCD 16x2 con adaptador I2C (0x27) | Muestra de estados locales e indicadores de distancia y sonido |
| **Actuadores** | Servomotores SG90 (x2) | Movimiento expresivo de los brazos de Wall-e |
| **Sensores Auxiliares** | HC-SR04 y Detector de Sonido | Detección de proximidad de usuarios y nivel de sonido ambiente |
| **Estructura / Chasis** | Madera personalizada (Estilo Wall-e) | Estructura no metálica que evita interferencias electromagnéticas (EMI) en la antena NFC |

---

## 🚀 Software y Firmware

### 1. Firmware ESP32 (`firmware/esp32/esp32_nfc_server.ino`)
- **Lógica No Bloqueante**: Uso de `millis()` para evitar retrasos que congelen el stack de red del ESP32.
- **Doble Modo Wi-Fi**: Conexión a red local Wi-Fi o fallback automático a punto de acceso propio (`WALL-E-WiFi`).
- **Navegación mDNS**: Accesible mediante `http://Wall-e.local` o `http://guali.local`.
- **API JSON**: Endpoint `/api/status` para consumo de datos de lectura NFC en tiempo real.

### 2. Firmware Arduino Nano (`firmware/arduino_nano/`)
- Control suave de movimiento de servos para animación corporal.
- Despliegue de telemetría en tiempo real en la pantalla LCD 16x2.

### 3. Aplicación Web Django (`web/`)
- Sistema de catálogo de libros y registro de usuarios.
- Integración para la vinculación de UIDs de tarjetas NFC con registros de préstamos.

---

## 🛠️ Instalación y Configuración

### Carga del Firmware (Arduino IDE)
1. Abrir **Arduino IDE**.
2. Instalar las librerías ubicadas en la carpeta `libraries/` (`Adafruit_PN532`, `LiquidCrystal_I2C`, etc.).
3. Para la **ESP32**:
   - Seleccionar la placa `ESP32 Dev Module`.
   - Configurar los pines I2C: `SDA = 21`, `SCL = 22`.
   - Modificar las constantes `WIFI_SSID` y `WIFI_PASSWORD` en `firmware/esp32/esp32_nfc_server.ino` con los datos de tu red.
   - Compilar y subir.
4. Para el **Arduino Nano**:
   - Abrir `firmware/arduino_nano/arduino_nano_full.ino`.
   - Seleccionar procesador `ATmega328P` (o `Old Bootloader` si aplica).
   - Compilar y subir.

### Ejecución de la Aplicación Web (Django)
```bash
# Navegar al directorio de la aplicación web
cd web

# Instalar dependencias de Python
pip install -r requirements.txt

# Aplicar migraciones a la base de datos
python manage.py migrate

# Iniciar el servidor de desarrollo
python manage.py runserver
```
La aplicación estará disponible en `http://127.0.0.1:8000/`.

---

## 📝 Licencia y Créditos

Desarrollado como proyecto de robótica asistencial y mediación tecnológica en la lectura.  
Inspirado en la estética de Wall-E. Todos los derechos reservados.
