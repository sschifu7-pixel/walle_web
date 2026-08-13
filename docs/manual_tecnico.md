# Manual de Implementación Técnica: Sistema Guali - Integración NFC y Optimización Web

<p align="center">
  <img src="images/Resultado.jpeg" alt="Robot Gualí Final" width="500" style="border-radius: 8px;"/>
  <br>
  <em>Figura 1: Prototipo Robótico Gualí (Wall-E) Ensamblado y Operativo</em>
</p>

## 1. Fundamentación Estratégica y Visión del Proyecto

El proyecto Guali representa una evolución en la arquitectura de interacción para la robótica educativa. El propósito fundamental es la transformación de un objeto tradicionalmente pasivo —el libro— en un nodo de datos interactivo mediante tecnología de comunicación de campo cercano (NFC). En esta arquitectura, el robot no actúa meramente como un dispositivo periférico, sino como un mediador de lectura, una entidad física diseñada para cerrar la brecha entre el soporte analógico y la gestión digital, dotando de tridimensionalidad al registro de información bibliotecaria.

<p align="center">
  <img src="images/idea%20principal.jpeg" alt="Boceto Conceptual Guali" width="450"/>
  <br>
  <em>Figura 2: Esquema y Concepción Estructural del Robot Mediador</em>
</p>

### Análisis del Propósito: Rescate de la Pasividad Digital
La visión de Guali se sustenta en la desvinculación del usuario de la "pasividad de la pantalla", fomentando una interacción táctil y tangible con el conocimiento. La estrategia se articula bajo los siguientes pilares:
- **Propósito**: Recontextualizar la experiencia de aprendizaje, moviendo al estudiante de una postura receptiva digital hacia un compromiso físico con el material bibliográfico.
- **Visión**: Consolidar un autómata que actúe como puente interactivo, utilizando el hardware NFC para digitalizar procesos de préstamo sin perder la esencia del entorno físico.
- **Flujo de Trabajo**: Captura de UID (Identificador Único) mediante proximidad en la extremidad del robot, procesamiento asíncrono de datos y validación visual inmediata en la interfaz de gestión.

### Evaluación del Impacto Operativo
La integración del sensor NFC en la "mano" del robot constituye el núcleo de la operación. Esta ubicación no es solo funcional, sino simbólica: el gesto de "entregar" el libro al robot valida su rol como asistente proactivo. Técnicamente, esta disposición optimiza el rango de inducción para el usuario, permitiendo que el registro de préstamos sea una transacción fluida y natural.

---

## 2. Arquitectura Física y Selección de Componentes

En la robótica de servicio, la integridad estructural y la selección de materiales son determinantes para el rendimiento electromagnético. Para el chasis de Guali, el uso de madera no responde solo a una estética inspirada en Wall-E para generar empatía, sino a una decisión técnica de transparencia de RF. A diferencia de los chasis metálicos que provocan corrientes de Foucault y desintonización de la antena, la madera permite una propagación limpia de la señal de 13.56 MHz del sensor NFC.

### Especificaciones del Chasis y Hardware

<p align="center">
  <img src="images/dimenciones_caja.jpeg" width="300"/>
  <img src="images/verificacion%20medidas.jpeg" width="300"/>
  <br>
  <em>Figura 3: Planos de Dimensiones de Caja y Verificación de Medidas del Chasis</em>
</p>

| Componente | Especificación Técnica | Muestra de Hardware | Función y Justificación |
| :--- | :--- | :---: | :--- |
| **Estructura Principal** | Chasis de madera (Base Wall-E) | — | Soporte estructural no metálico; evita interferencias en la antena NFC. |
| **Controlador Central** | ESP32 DevKit V1 | — | MCU con stack TCP/IP nativo para servidor web y manejo de eventos mDNS. |
| **Sensor de Proximidad** | Módulo PN532 (I2C) | <img src="images/lector_rfid.jpeg" width="100"/> | Lector de alta precisión para etiquetas NFC y emulación de tarjetas. |
| **Pantalla Telemetría** | LCD 16x2 I2C | <img src="images/pantalla.jpeg" width="100"/> | Monitorización en tiempo real de estados del robot. |
| **Actuadores** | Servomotores SG90 (x2) | <img src="images/servomotor.jpeg" width="100"/> | Control de animación y movimiento mecánico de brazos. |
| **Fuente de Alimentación** | Batería Recargable | <img src="images/bateria.jpeg" width="100"/> | Sistema de alimentación autónomo e independiente. |
| **Protocolo de Comunicación** | I2C / SPI | — | Bus de datos de alta velocidad entre el PN532 y el ESP32. |

### Configuración del Sensor NFC (PN532)
La ubicación del lector PN532 en la extremidad del robot es imperativa según los planos de diseño. Esta disposición requiere un ruteo de cables interno que evite el acoplamiento inductivo con los motores o actuadores. La accesibilidad para el mantenimiento del cableado dentro de la estructura de madera es crucial para prevenir la degradación de la señal por tensión mecánica o interferencias electromagnéticas (EMI).

---

## 📸 3. Proceso de Fabricación, Armado y Ensamble

### Ensamble Estructural
<p align="center">
  <img src="images/Armado_1.jpeg" width="220"/>
  <img src="images/armado_1.1.jpeg" width="220"/>
  <img src="images/Armado_2.jpeg" width="220"/>
  <br>
  <em>Figura 4: Etapas iniciales de ensamble del chasis de madera</em>
</p>

### Rediseño Mecánico de Brazos
<p align="center">
  <img src="images/brazo.jpeg" width="300"/>
  <img src="images/redise%C3%B1o_brazos.jpeg" width="300"/>
  <br>
  <em>Figura 5: Implementación y rediseño de servomecanismos de articulación</em>
</p>

### Acabados y Pintura
<p align="center">
  <img src="images/Pintado1.jpeg" width="220"/>
  <img src="images/pintado2.jpeg" width="220"/>
  <img src="images/pintado3.jpeg" width="220"/>
  <br>
  <em>Figura 6: Aplicación de pintura y detalles temáticos Wall-E</em>
</p>

---

## 4. Optimización del Firmware: Lógica No Bloqueante (`esp32_nfc_server.ino`)

Como arquitecto de sistemas, el desafío principal en el ESP32 es la gestión de la concurrencia. Un servidor web y un sensor de alta frecuencia deben coexistir sin que uno degrade el rendimiento del otro.

### Análisis de Optimización (C++): Adiós al Bloqueo
Se ha eliminado por completo el uso de `delay()` en el firmware. En sistemas embebidos, un retardo de un segundo provocaría el hambre de recursos del Watchdog Timer (WDT) y congelaría el stack de red del ESP32, resultando en peticiones HTTP fallidas.

La implementación de una arquitectura basada en eventos utilizando `millis()` permite que el sistema realice un "polling" del sensor NFC a intervalos precisos mientras el procesador permanece libre para atender las peticiones de la interfaz web de forma asíncrona. Esto elimina el jitter en la comunicación y garantiza una latencia mínima.

### Procesamiento y Tipado de Datos UID
El sistema es capaz de discriminar entre diferentes longitudes de UID, una característica crítica para la integridad de la base de datos bibliotecaria:
- **Identificación de Longitud**: El log registra automáticamente si el UID es de 4 Bytes (4B) (estándar Classic) o 7 Bytes (7B) (estándar Ultralight/NTAG).
- **Formatos de salida procesados**:
  - **Hexadecimal (HEX)**: Estructura de bytes en formato legible por máquina (ej. `0x04 0x2A 0x51 0x7E`).
  - **Decimal (DEC) - Dos Interpretaciones**:
    - **Decimal Entero / Wiegand (32-bit)**: Interpretación del UID como un entero de 32 bits sin signo (ej. `3735928559`). Este valor es el estándar para la integración con sistemas de control de acceso legacy.
    - **Decimal por Bytes**: Representación individual de cada byte (0-255), separada por puntos (ej. `222.173.190.239`), útil para filtrado de datos a nivel de capa de aplicación.

---

## 5. Interfaz Web y Consola de Diagnóstico en Tiempo Real

La retroalimentación visual es el componente crítico de la experiencia del usuario técnico. La interfaz alojada en `http://guali.local` ha sido diseñada para ofrecer una visibilidad total del estado del sistema.

### Dinámica de la Interfaz
El frontend utiliza un script JavaScript con un ciclo de actualización de 250ms (4Hz). Esta frecuencia de muestreo asegura que el cambio de estado en el hardware sea percibido como instantáneo por el usuario.

### Consola Terminal de Diagnóstico en Vivo
Ubicada estratégicamente bajo los recuadros de lectura, la consola implementa un diseño de alta visibilidad:
- **Estética "Neon Terminal"**: Fondo negro con tipografía en verde y cian para facilitar la depuración en campo.
- **Estampas de Tiempo (`[hh:mm:ss]`)**: Cada evento de lectura genera un log preciso, fundamental para auditorías de préstamos.
- **Gestión de Flujo**: Función de Auto-scroll para visualización de logs en tiempo real y un botón de "Limpieza de Consola" para liberar memoria visual en sesiones de prueba prolongadas.

### Máquina de Estados Visual
Para confirmar la detección exitosa, el sistema implementa una transición de estados visuales en los recuadros:
- **Estado Inactivo (Idle)**: Borde azul sólido (espera de datos).
- **Estado de Lectura (Active)**: Destello Verde Neón con transición de texto inmediata de "Ninguna tarjeta escaneada" al UID capturado.

---

## 6. Protocolo de Despliegue y Verificación de Sistema

Para garantizar la estabilidad del sistema mDNS y la persistencia del servidor web, el técnico debe seguir estrictamente este protocolo de carga.

### Guía de Implementación Técnica
1. **Carga de Firmware**: Compilar y subir `esp32_nfc_server.ino` mediante Arduino IDE asegurando la selección correcta de la partición de memoria del ESP32.
2. **Configuración de Comunicación Serie**: Establecer la velocidad de la consola en 115200 baudios. Es aquí donde se validará el arranque del stack TCP/IP y la asignación de IP.
3. **Resolución de Nombres y Acceso**:
   - **mDNS**: Acceder vía `http://guali.local` o `http://Wall-e.local`. Nota: Requiere que el dispositivo cliente tenga activo un servicio de descubrimiento de red como Bonjour (macOS/Windows) o Avahi (Linux).
   - **Acceso Directo (AP)**: En caso de fallo de mDNS, utilizar la IP estática `192.168.4.1`.

### Checklist de Verificación Final (FAT - Factory Acceptance Test)
- [x] **Validación de Datos**: Confirmar que el UID HEX en la consola web coincide con el valor 32-bit DEC procesado.
- [x] **Detección de Tipología**: Verificar que el sistema identifique correctamente etiquetas de 4B y 7B.
- [x] **Latencia de Interfaz**: Validar que el destello verde neón ocurra en menos de 300ms tras la aproximación física.
- [x] **Estabilidad del Servidor**: Confirmar que la navegación por la interfaz no se interrumpa durante lecturas consecutivas de tarjetas (prueba de lógica no bloqueante).

La implementación rigurosa de estas especificaciones asegura que Guali opere no solo como un prototipo educativo, sino como una solución de ingeniería IoT robusta, eficiente y lista para la mediación de lectura profesional.