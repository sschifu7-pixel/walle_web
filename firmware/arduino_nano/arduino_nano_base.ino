/*
  ============================================================================
  PROYECTO ARDUINO NANO - SERVOMOTORES REACTIVOS AL SONIDO + LCD I2C
  ============================================================================
  Funcionamiento:
  - Los servomotores permanecen en posición de REPOSO (90°).
  - Al DETECTAR SONIDO (por pin digital o por superar el umbral analógico):
    Los servos se mueven/reaccionan activamente durante un tiempo.
  - La pantalla LCD muestra el nivel de sonido y el estado (REPOSO / REACTIVO).
  ============================================================================
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// --- CONFIGURACIÓN DE PINES ---
const int PIN_SONIDO_ANALOG  = A0; // Entrada Analógica (nivel de ruido)
const int PIN_SONIDO_DIGITAL = 2;  // Entrada Digital (ajustada con el potenciómetro del sensor)

const int PIN_SERVO_1 = 9;  // Servo 1
const int PIN_SERVO_2 = 10; // Servo 2

// --- UMBRALES Y TIEMPOS ---
const int UMBRAL_SONIDO_ANALOG = 400;  // Umbral analógico (ajustar según el sensor)
const unsigned long TIEMPO_MOVIMIENTO = 1500; // Tiempo en ms que se mueven los servos al oír sonido

// --- OBJETOS Y VARIABLES ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo servo1;
Servo servo2;

unsigned long tiempoUltimoSonido = 0;
unsigned long tiempoAnteriorLCD = 0;
const long intervaloLCD = 200;

int anguloServo = 90;
bool direccionServo = true;
bool estadoMovimiento = false;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_SONIDO_DIGITAL, INPUT);

  // Inicializar Servos en posición neutra (90 grados)
  servo1.attach(PIN_SERVO_1);
  servo2.attach(PIN_SERVO_2);
  servo1.write(90);
  servo2.write(90);

  // Inicializar Pantalla LCD I2C
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print(" ARDUINO NANO  ");
  lcd.setCursor(0, 1);
  lcd.print("Reactivo a Sonido");
  delay(1500);
  lcd.clear();
}

void loop() {
  unsigned long tiempoActual = millis();

  // 1. Lectura del Sensor de Sonido
  int nivelSonido = analogRead(PIN_SONIDO_ANALOG);
  bool sonidoDigital = (digitalRead(PIN_SONIDO_DIGITAL) == HIGH);

  // 2. Comprobar si hay sonido (por pin digital o umbral analógico)
  if (sonidoDigital || nivelSonido > UMBRAL_SONIDO_ANALOG) {
    tiempoUltimoSonido = tiempoActual; // Registrar tiempo de la última detección
  }

  // Evaluar si aún estamos dentro de la ventana de movimiento tras el sonido
  if (tiempoActual - tiempoUltimoSonido < TIEMPO_MOVIMIENTO) {
    estadoMovimiento = true;
  } else {
    estadoMovimiento = false;
  }

  // 3. Control de los Servomotores según la detección de sonido
  if (estadoMovimiento) {
    // Si hay sonido: Mover servos en barrido dinámico
    if (direccionServo) {
      anguloServo += 10;
      if (anguloServo >= 160) direccionServo = false;
    } else {
      anguloServo -= 10;
      if (anguloServo <= 20) direccionServo = true;
    }
    servo1.write(anguloServo);
    servo2.write(180 - anguloServo);
  } else {
    // Si HAY SILENCIO: Volver a posición de reposo (90°)
    anguloServo = 90;
    servo1.write(90);
    servo2.write(90);
  }

  // 4. Actualización de pantalla LCD y Monitor Serie sin bloquear (cada 200 ms)
  if (tiempoActual - tiempoAnteriorLCD >= intervaloLCD) {
    tiempoAnteriorLCD = tiempoActual;

    // Monitor Serie
    Serial.print("Sonido A0: ");
    Serial.print(nivelSonido);
    Serial.print(" | Digital D2: ");
    Serial.print(sonidoDigital ? "SI" : "NO");
    Serial.print(" | Estado: ");
    Serial.println(estadoMovimiento ? "MOVIENDO" : "REPOSO");

    // Fila 0 LCD: Nivel de sonido y señal visual
    lcd.setCursor(0, 0);
    lcd.print("Sonido: ");
    lcd.print(nivelSonido);
    lcd.print("    "); // Limpiar caracteres sobrantes

    lcd.setCursor(12, 0);
    if (estadoMovimiento) {
      lcd.print(" 🔊 ");
    } else {
      lcd.print(" 🤫 ");
    }

    // Fila 1 LCD: Estado de los servos
    lcd.setCursor(0, 1);
    if (estadoMovimiento) {
      lcd.print("Servo: MOVIENDO ");
    } else {
      lcd.print("Servo: REPOSO   ");
    }
  }

  delay(30); // Pausa suave para el movimiento de los servos
}
