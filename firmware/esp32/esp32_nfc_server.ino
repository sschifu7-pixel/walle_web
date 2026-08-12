/*
 * ============================================================================
 * PROYECTO Wall-e
 * ============================================================================
 * W
 * ============================================================================
 */

#include <Adafruit_PN532.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>

// ==========================================
// CONFIGURACIÓN DE RED WI-FI
// ==========================================
const char *WIFI_SSID = "Tu red";
const char *WIFI_PASSWORD = "Tu contraseña";

// Punto de Acceso propio si no hay Wi-Fi:
const char *AP_SSID = "WALL-E-WiFi";
const char *AP_PASSWORD = "Proyecto";

WebServer server(80);

// ==========================================
// CONFIGURACIÓN LECTOR PN532 (I2C)
// ==========================================
#define PN532_IRQ (-1)
#define PN532_RESET (-1)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// ==========================================
// VARIABLES DE ESTADO Y LECTURA
// ==========================================
String ultimaTarjetaUID = "Ninguna tarjeta escaneada";
String ultimaTarjetaDecimal = "---";
String ultimaTarjetaDecBytes = "---";

int ultimaLongitudUID = 0;
uint32_t contadorLecturas = 0;
bool pn532Conectado = false;

// Funciones
bool leerRFID();
void handleRoot();
void handleStatus();

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(F("\n=========================================="));
  Serial.println(F(" WAlle - SERVIDOR WEB + TERMINAL CONSOLA "));
  Serial.println(F("=========================================="));

  Wire.begin(21, 22);

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (versiondata) {
    pn532Conectado = true;
    nfc.SAMConfig();
    Serial.println(F("✅ Lector PN532 detectado e inicializado."));
  } else {
    Serial.println(F("❌ ERROR: No se encontró el lector PN532."));
  }

  Serial.print(F("Conectando a Wi-Fi: "));
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 15) {
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
    Serial.print(F("🌐 Conéctate a Wi-Fi: "));
    Serial.println(AP_SSID);
    Serial.print(F("🌐 IP Accesible: http://"));
    Serial.println(WiFi.softAPIP());
  }

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

  delay(40);
}

// ============================================================================
// FUNCIONES DEL LECTOR PN532
// ============================================================================
unsigned long ultimoTiempoLectura = 0;

bool leerRFID() {
  if (millis() - ultimoTiempoLectura < 800)
    return false;

  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;

  success =
      nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 40);

  if (success) {
    ultimoTiempoLectura = millis();
    contadorLecturas++;
    ultimaLongitudUID = uidLength;

    // 1. Convertir UID a Hexadecimal
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

    return true;
  }

  return false;
}

// ============================================================================
// SERVIDOR WEB: PÁGINA HTML + TERMINAL CONSOLA EN TIEMPO REAL
// ============================================================================
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Robot Gualí - Panel RFID/NFC PN532</title>
  <style>
    * { box-sizing: border-box; font-family: 'Segoe UI', system-ui, sans-serif; margin: 0; padding: 0; }
    body { background: #070d19; color: #f8fafc; display: flex; justify-content: center; align-items: center; min-height: 100vh; padding: 20px; }
    .card { background: #0f172a; border: 1px solid #1e293b; border-radius: 24px; padding: 28px; width: 100%; max-width: 650px; box-shadow: 0 25px 50px -12px rgba(0,0,0,0.8); }
    .header { text-align: center; margin-bottom: 20px; }
    .header h1 { font-size: 28px; color: #38bdf8; font-weight: 800; margin-bottom: 4px; letter-spacing: 0.5px; }
    .header p { font-size: 13px; color: #94a3b8; }
    .status-badge { display: inline-block; padding: 5px 14px; border-radius: 20px; font-size: 12px; font-weight: 700; margin-top: 8px; background: #065f46; color: #34d399; }
    
    .display-box { background: #020617; border: 2px solid #0284c7; border-radius: 16px; padding: 18px; text-align: center; margin: 14px 0; transition: all 0.3s ease; }
    .display-box.flash { border-color: #22c55e; box-shadow: 0 0 30px rgba(34, 197, 94, 0.5); background: #052e16; }
    .display-label { font-size: 11px; text-transform: uppercase; color: #94a3b8; letter-spacing: 2px; margin-bottom: 6px; font-weight: 800; }
    .display-value { font-size: 24px; font-family: 'Consolas', 'Courier New', monospace; font-weight: 900; color: #38bdf8; word-break: break-all; }
    .display-value-dec { font-size: 22px; font-family: 'Consolas', 'Courier New', monospace; font-weight: 900; color: #4ade80; margin-top: 2px; }
    .display-sub { font-size: 12px; color: #cbd5e1; margin-top: 4px; font-family: monospace; }
    
    .info-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 12px; margin-bottom: 16px; }
    .info-item { background: #1e293b; padding: 12px; border-radius: 12px; text-align: center; border: 1px solid #334155; }
    .info-item .num { font-size: 20px; font-weight: 800; color: #ffffff; }
    .info-item .lbl { font-size: 11px; color: #94a3b8; margin-top: 2px; text-transform: uppercase; font-weight: 700; }
    
    /* TERMINAL CONSOLA */
    .terminal-section { margin-top: 20px; }
    .terminal-header { display: flex; justify-content: space-between; align-items: center; background: #1e293b; border: 1px solid #334155; border-bottom: none; border-radius: 12px 12px 0 0; padding: 10px 14px; font-size: 12px; font-weight: 700; color: #38bdf8; letter-spacing: 1px; }
    .terminal-btn { background: #334155; color: #f8fafc; border: none; padding: 4px 10px; border-radius: 6px; font-size: 11px; cursor: pointer; transition: background 0.2s; }
    .terminal-btn:hover { background: #475569; }
    
    .terminal-body { background: #020617; border: 1px solid #334155; border-radius: 0 0 12px 12px; padding: 12px; height: 160px; overflow-y: auto; font-family: 'Consolas', 'Courier New', monospace; font-size: 12px; color: #4ade80; line-height: 1.5; box-shadow: inset 0 2px 8px rgba(0,0,0,0.8); }
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
      <h1>🤖 ROBOT GUALÍ</h1>
      <p>Lector RFID / NFC PN532 en Tiempo Real</p>
      <span class="status-badge">● PN532 CONECTADO (I2C)</span>
    </div>

    <!-- RECUADRO HEXADECIMAL -->
    <div class="display-box" id="mainBox">
      <div class="display-label">🔑 CÓDIGO UID (HEXADECIMAL)</div>
      <div class="display-value" id="uidDisplay">Esperando tarjeta...</div>
    </div>

    <!-- RECUADRO DECIMAL -->
    <div class="display-box" style="border-color: #16a34a;" id="decBox">
      <div class="display-label">🔢 CÓDIGO UID (DECIMAL ENTERO / WIEGAND)</div>
      <div class="display-value-dec" id="decDisplay">---</div>
      <div class="display-sub" id="decBytesDisplay">Decimal por bytes: ---</div>
    </div>

    <div class="info-grid">
      <div class="info-item">
        <div class="num" id="readCount">0</div>
        <div class="lbl">Total Lecturas</div>
      </div>
      <div class="info-item">
        <div class="num" id="uidBytes">0 bytes</div>
        <div class="lbl">Longitud UID</div>
      </div>
    </div>

    <!-- SECCIÓN CONSOLA TERMINAL EN VIVO -->
    <div class="terminal-section">
      <div class="terminal-header">
        <span>💻 CONSOLA DE DIAGNÓSTICO EN VIVO</span>
        <button class="terminal-btn" onclick="limpiarConsola()">Limpiar</button>
      </div>
      <div class="terminal-body" id="consoleBody">
        <div class="log-line sys"><span class="time">[SISTEMA]</span> Servidor Web listo. Esperando lecturas del PN532...</div>
      </div>
    </div>

    <div class="footer">
      ESP32 DevKit | Lector PN532 I2C | Bus SDA=21, SCL=22
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
          document.getElementById('decBytesDisplay').innerText = "Decimal por bytes: " + data.decBytes;
        }

        document.getElementById('readCount').innerText = data.total;
        document.getElementById('uidBytes').innerText = data.longitud + ' bytes';

        if (data.uid !== "Ninguna tarjeta escaneada" && data.uid !== ultimoUIDLeido) {
          ultimoUIDLeido = data.uid;

          // Animación destello
          mainBox.classList.add('flash');
          decBox.classList.add('flash');
          setTimeout(() => {
            mainBox.classList.remove('flash');
            decBox.classList.remove('flash');
          }, 1000);

          // Imprimir en la Consola Terminal Web
          const msg = `🎉 Tarjeta #${data.total} | HEX: ${data.uid} | DEC: ${data.decimal} (${data.longitud}B)`;
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

// API JSON con valores Hexadecimales y Decimales
void handleStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");

  String json = "{";
  json += "\"uid\":\"" + ultimaTarjetaUID + "\",";
  json += "\"decimal\":\"" + ultimaTarjetaDecimal + "\",";
  json += "\"decBytes\":\"" + ultimaTarjetaDecBytes + "\",";
  json += "\"longitud\":" + String(ultimaLongitudUID) + ",";
  json += "\"total\":" + String(contadorLecturas);
  json += "}";

  server.send(200, "application/json", json);
}
