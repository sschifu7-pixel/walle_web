/*
 * ============================================================================
 * PROYECTO Wall-E / Robot Gualí — ESP32 Firmware
 * Lector NFC PN532 + Pantalla LCD 16x2 I2C + 2 Servomotores + Servidor Web
 * ============================================================================
 */

#include <Adafruit_PN532.h>
#include <ESP32Servo.h>
#include <ESPmDNS.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>

// ==========================================
// CONFIGURACIÓN DE RED WI-FI
// ==========================================
const char *WIFI_SSID = "Tu red";
const char *WIFI_PASSWORD = "Tu contraseña";

// Punto de Acceso propio si no hay Wi-Fi local:
const char *AP_SSID = "WALL-E-WiFi";
const char *AP_PASSWORD = "Proyecto";

WebServer server(80);

// ==========================================
// CONFIGURACIÓN PANTALLA LCD 16x2 I2C
// ==========================================
// Dirección I2C común: 0x27 o 0x3F (compartida en bus SDA=21, SCL=22)
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool lcdConectado = false;

// ==========================================
// CONFIGURACIÓN SERVOMOTORES SG90
// ==========================================
Servo servoBrazoIzquierdo;
Servo servoBrazoDerecho;

const int PIN_SERVO_IZQ = 18; // GPIO 18 (Brazo Izquierdo)
const int PIN_SERVO_DER = 19; // GPIO 19 (Brazo Derecho)

int anguloServoIzq = 90;
int anguloServoDer = 90;

// ==========================================
// CONFIGURACIÓN LECTOR PN532 (I2C)
// ==========================================
#define PN532_IRQ (-1)
#define PN532_RESET (-1)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
bool pn532Conectado = false;

// ==========================================
// VARIABLES DE ESTADO Y LECTURA
// ==========================================
String ultimaTarjetaUID = "Ninguna tarjeta escaneada";
String ultimaTarjetaDecimal = "---";
String ultimaTarjetaDecBytes = "---";

int ultimaLongitudUID = 0;
uint32_t contadorLecturas = 0;
unsigned long ultimoTiempoLectura = 0;

// Declaración de funciones
bool leerRFID();
void moverServosAlDetectar();
void actualizarPantallaLCD(String linea1, String linea2);
void handleRoot();
void handleStatus();

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(F("\n=========================================="));
  Serial.println(F(" ROBOT WALL-E - NFC + SERVOS + LCD I2C   "));
  Serial.println(F("=========================================="));

  // Inicializar bus I2C (SDA = 21, SCL = 22)
  Wire.begin(21, 22);

  // 1. Inicializar Pantalla LCD 16x2 I2C
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  ROBOT WALL-E  ");
  lcd.setCursor(0, 1);
  lcd.print(" Iniciando...   ");
  lcdConectado = true;

  // 2. Inicializar Servomotores SG90
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  servoBrazoIzquierdo.setPeriodHertz(50);
  servoBrazoDerecho.setPeriodHertz(50);
  
  servoBrazoIzquierdo.attach(PIN_SERVO_IZQ, 500, 2400);
  servoBrazoDerecho.attach(PIN_SERVO_DER, 500, 2400);
  
  // Posición inicial de reposo (90°)
  servoBrazoIzquierdo.write(90);
  servoBrazoDerecho.write(90);
  Serial.println(F("✅ Servomotores inicializados en GPIO 18 y 19."));

  // 3. Inicializar Lector NFC PN532
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (versiondata) {
    pn532Conectado = true;
    nfc.SAMConfig();
    Serial.println(F("✅ Lector PN532 detectado e inicializado (I2C)."));
    actualizarPantallaLCD("WALL-E LISTO", "Acerque tarjeta");
  } else {
    Serial.println(F("❌ ERROR: No se encontró el lector PN532."));
    actualizarPantallaLCD("ERR: PN532", "No detectado");
  }

  // 4. Conexión Wi-Fi
  Serial.print(F("Conectando a Wi-Fi: "));
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 12) {
    delay(500);
    Serial.print(F("."));
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("\n✅ ¡Conectado a Wi-Fi!"));
    Serial.print(F("🌐 IP Local: http://"));
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(F("\n⚠️ Creando Punto de Acceso (Access Point)..."));
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.print(F("🌐 IP Accesible: http://"));
    Serial.println(WiFi.softAPIP());
  }

  // 5. Configurar Rutas del Servidor Web
  server.on("/", handleRoot);
  server.on("/api/status", handleStatus);
  server.begin();

  if (MDNS.begin("guali")) {
    Serial.println(F("🌐 Nombre de dominio: http://Wall-e.local"));
  }

  Serial.println(F("🚀 Servidor Web iniciado correctamente."));
}

void loop() {
  server.handleClient();

  if (pn532Conectado) {
    leerRFID();
  }

  delay(30);
}

// ============================================================================
// FUNCIONES DE CONTROL DE SERVOMOTORES Y LCD
// ============================================================================

void actualizarPantallaLCD(String linea1, String linea2) {
  if (!lcdConectado) return;
  lcd.clear();
  
  // Limitar cadenas a 16 caracteres para la pantalla 16x2
  if (linea1.length() > 16) linea1 = linea1.substring(0, 16);
  if (linea2.length() > 16) linea2 = linea2.substring(0, 16);

  lcd.setCursor(0, 0);
  lcd.print(linea1);
  lcd.setCursor(0, 1);
  lcd.print(linea2);
}

void moverServosAlDetectar() {
  Serial.println(F("🤖 [ANIMACIÓN SERVOS] Movimiento de brazos por tarjeta detectada..."));
  
  // Gesto 1: Levantar brazos
  anguloServoIzq = 150;
  anguloServoDer = 30;
  servoBrazoIzquierdo.write(anguloServoIzq);
  servoBrazoDerecho.write(anguloServoDer);
  delay(350);

  // Gesto 2: Saludo alternado
  anguloServoIzq = 30;
  anguloServoDer = 150;
  servoBrazoIzquierdo.write(anguloServoIzq);
  servoBrazoDerecho.write(anguloServoDer);
  delay(350);

  // Gesto 3: Regreso suave a posición de reposo (90°)
  anguloServoIzq = 90;
  anguloServoDer = 90;
  servoBrazoIzquierdo.write(anguloServoIzq);
  servoBrazoDerecho.write(anguloServoDer);
}

// ============================================================================
// LECTURA DE LECTOR NFC PN532
// ============================================================================

bool leerRFID() {
  if (millis() - ultimoTiempoLectura < 800)
    return false;

  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 40);

  if (success) {
    ultimoTiempoLectura = millis();
    contadorLecturas++;
    ultimaLongitudUID = uidLength;

    // Convertir UID a Hexadecimal y Decimal
    ultimaTarjetaUID = "";
    ultimaTarjetaDecBytes = "";
    unsigned long uidEnteroDec = 0;

    for (uint8_t i = 0; i < uidLength; i++) {
      if (i > 0) {
        ultimaTarjetaUID += " ";
        ultimaTarjetaDecBytes += ".";
      }
      ultimaTarjetaUID += "0x";
      if (uid[i] < 0x10)
        ultimaTarjetaUID += "0";
      ultimaTarjetaUID += String(uid[i], HEX);

      ultimaTarjetaDecBytes += String(uid[i], DEC);

      if (i < 4) {
        uidEnteroDec = (uidEnteroDec << 8) | uid[i];
      }
    }
    ultimaTarjetaUID.toUpperCase();
    ultimaTarjetaDecimal = String(uidEnteroDec);

    Serial.print(F("\n🎉 [LECTURA DETECTADA] HEX: "));
    Serial.print(ultimaTarjetaUID);
    Serial.print(F(" | DEC: "));
    Serial.println(ultimaTarjetaDecimal);

    // 1. Mostrar información de la tarjeta en la pantalla LCD 16x2
    String lineaLcd1 = "HEX: " + ultimaTarjetaUID;
    String lineaLcd2 = "DEC: " + ultimaTarjetaDecimal;
    actualizarPantallaLCD(lineaLcd1, lineaLcd2);

    // 2. Ejecutar animación de movimiento de los 2 Servomotores
    moverServosAlDetectar();

    return true;
  }

  return false;
}

// ============================================================================
// SERVIDOR WEB INTERACTIVO EN TIEMPO REAL
// ============================================================================

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Robot Wall-E - Control NFC & Servos</title>
  <style>
    * { box-sizing: border-box; font-family: 'Segoe UI', system-ui, sans-serif; margin: 0; padding: 0; }
    body { background: #070d19; color: #f8fafc; display: flex; justify-content: center; align-items: center; min-height: 100vh; padding: 20px; }
    .card { background: #0f172a; border: 1px solid #1e293b; border-radius: 24px; padding: 28px; width: 100%; max-width: 650px; box-shadow: 0 25px 50px -12px rgba(0,0,0,0.8); }
    .header { text-align: center; margin-bottom: 20px; }
    .header h1 { font-size: 28px; color: #38bdf8; font-weight: 800; margin-bottom: 4px; }
    .header p { font-size: 13px; color: #94a3b8; }
    .status-badge { display: inline-block; padding: 5px 14px; border-radius: 20px; font-size: 12px; font-weight: 700; margin-top: 8px; background: #065f46; color: #34d399; }
    
    .display-box { background: #020617; border: 2px solid #0284c7; border-radius: 16px; padding: 18px; text-align: center; margin: 14px 0; transition: all 0.3s ease; }
    .display-box.flash { border-color: #22c55e; box-shadow: 0 0 30px rgba(34, 197, 94, 0.5); background: #052e16; }
    .display-label { font-size: 11px; text-transform: uppercase; color: #94a3b8; letter-spacing: 2px; margin-bottom: 6px; font-weight: 800; }
    .display-value { font-size: 24px; font-family: 'Consolas', monospace; font-weight: 900; color: #38bdf8; word-break: break-all; }
    .display-value-dec { font-size: 22px; font-family: 'Consolas', monospace; font-weight: 900; color: #4ade80; margin-top: 2px; }
    
    .info-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; margin-bottom: 16px; }
    .info-item { background: #1e293b; padding: 12px; border-radius: 12px; text-align: center; border: 1px solid #334155; }
    .info-item .num { font-size: 20px; font-weight: 800; color: #ffffff; }
    .info-item .lbl { font-size: 11px; color: #94a3b8; margin-top: 2px; text-transform: uppercase; font-weight: 700; }
    
    .terminal-section { margin-top: 20px; }
    .terminal-header { display: flex; justify-content: space-between; align-items: center; background: #1e293b; border: 1px solid #334155; border-bottom: none; border-radius: 12px 12px 0 0; padding: 10px 14px; font-size: 12px; font-weight: 700; color: #38bdf8; }
    .terminal-btn { background: #334155; color: #f8fafc; border: none; padding: 4px 10px; border-radius: 6px; font-size: 11px; cursor: pointer; }
    
    .terminal-body { background: #020617; border: 1px solid #334155; border-radius: 0 0 12px 12px; padding: 12px; height: 160px; overflow-y: auto; font-family: 'Consolas', monospace; font-size: 12px; color: #4ade80; line-height: 1.5; }
    .log-line { margin-bottom: 4px; word-break: break-all; }
    .log-line .time { color: #64748b; margin-right: 8px; }
    .log-line.tag { color: #38bdf8; font-weight: bold; }
    .log-line.sys { color: #facc15; }

    .footer { text-align: center; margin-top: 18px; font-size: 12px; color: #64748b; }
  </style>
</head>
<body>
  <div class="card">
    <div class="header">
      <h1>🤖 ROBOT WALL-E</h1>
      <p>Lector RFID PN532 + 2 Servos + LCD 16x2</p>
      <span class="status-badge">● PN532 + SERVOS + LCD ACTIVOS (I2C)</span>
    </div>

    <!-- RECUADRO HEXADECIMAL -->
    <div class="display-box" id="mainBox">
      <div class="display-label">🔑 CÓDIGO UID (HEXADECIMAL)</div>
      <div class="display-value" id="uidDisplay">Esperando tarjeta...</div>
    </div>

    <!-- RECUADRO DECIMAL -->
    <div class="display-box" style="border-color: #16a34a;" id="decBox">
      <div class="display-label">🔢 CÓDIGO UID (DECIMAL WIEGAND)</div>
      <div class="display-value-dec" id="decDisplay">---</div>
    </div>

    <div class="info-grid">
      <div class="info-item">
        <div class="num" id="readCount">0</div>
        <div class="lbl">Total Lecturas</div>
      </div>
      <div class="info-item">
        <div class="num" id="servoStatus">90° / 90°</div>
        <div class="lbl">Servos Izq / Der</div>
      </div>
    </div>

    <div class="terminal-section">
      <div class="terminal-header">
        <span>💻 CONSOLA EN TIEMPO REAL</span>
        <button class="terminal-btn" onclick="limpiarConsola()">Limpiar</button>
      </div>
      <div class="terminal-body" id="consoleBody">
        <div class="log-line sys"><span class="time">[SISTEMA]</span> Servidor ESP32 listo. Lector PN532 y Servos activos.</div>
      </div>
    </div>

    <div class="footer">
      ESP32 DevKit | Bus I2C SDA=21, SCL=22 | Servos GPIO 18 & 19
    </div>
  </div>

  <script>
    let ultimoUIDLeido = "";

    function agregarLogConsola(mensaje, tipo = 'tag') {
      const consoleBody = document.getElementById('consoleBody');
      const now = new Date();
      const timeStr = now.toTimeString().split(' ')[0];
      const div = document.createElement('div');
      div.className = 'log-line ' + tipo;
      div.innerHTML = `<span class="time">[${timeStr}]</span> ${mensaje}`;
      consoleBody.appendChild(div);
      consoleBody.scrollTop = consoleBody.scrollHeight;
    }

    function limpiarConsola() {
      document.getElementById('consoleBody').innerHTML = '<div class="log-line sys"><span class="time">[SISTEMA]</span> Consola limpiada.</div>';
    }

    async function actualizarLectura() {
      try {
        const respuesta = await fetch('/api/status?t=' + new Date().getTime());
        const data = await respuesta.json();
        
        const mainBox = document.getElementById('mainBox');
        const decBox = document.getElementById('decBox');
        
        if (data.uid && data.uid !== "") {
          document.getElementById('uidDisplay').innerText = data.uid;
          document.getElementById('decDisplay').innerText = data.decimal;
        }

        document.getElementById('readCount').innerText = data.total;
        document.getElementById('servoStatus').innerText = `${data.servoIzq}° / ${data.servoDer}°`;

        if (data.uid !== "Ninguna tarjeta escaneada" && data.uid !== ultimoUIDLeido) {
          ultimoUIDLeido = data.uid;

          mainBox.classList.add('flash');
          decBox.classList.add('flash');
          setTimeout(() => {
            mainBox.classList.remove('flash');
            decBox.classList.remove('flash');
          }, 1000);

          const msg = `🎉 Tarjeta #${data.total} | HEX: ${data.uid} | DEC: ${data.decimal} | 🦾 Servos Activados!`;
          agregarLogConsola(msg, 'tag');
        }
      } catch (e) {
        console.log('Conectando al ESP32...');
      }
    }

    setInterval(actualizarLectura, 250);
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// API JSON para consultas web y Django
void handleStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");

  String json = "{";
  json += "\"uid\":\"" + ultimaTarjetaUID + "\",";
  json += "\"decimal\":\"" + ultimaTarjetaDecimal + "\",";
  json += "\"decBytes\":\"" + ultimaTarjetaDecBytes + "\",";
  json += "\"longitud\":" + String(ultimaLongitudUID) + ",";
  json += "\"total\":" + String(contadorLecturas) + ",";
  json += "\"servoIzq\":" + String(anguloServoIzq) + ",";
  json += "\"servoDer\":" + String(anguloServoDer);
  json += "}";

  server.send(200, "application/json", json);
}
