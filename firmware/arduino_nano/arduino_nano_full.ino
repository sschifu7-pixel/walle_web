/*
  ============================================================================
  PROYECTO ARDUINO NANO - SENSOR ULTRASÓNICO, SONIDO, SERVOS Y LCD I2C
  ============================================================================
*/

#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>

// --- CONFIGURACIÓN DE PINES ---
// Sensor Ultrasónico HC-SR04
const int PIN_TRIG = 6;
const int PIN_ECHO = 7;

// Detector de Sonido RQ-S004
const int PIN_SONIDO_ANALOG = A0;
const int PIN_SONIDO_DIGITAL = 2; // Opcional

// Servomotores
const int PIN_SERVO_1 = 9;
const int PIN_SERVO_2 = 10;

// --- OBJETOS Y VARIABLES ---
// Dirección I2C habitual: 0x27 (si no muestra texto, probar cambiando a 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo servo1;
Servo servo2;

unsigned long tiempoAnterior = 0;
const long intervalo = 500; // Actualización LCD cada 500 ms

int anguloServo = 0;
bool direccionServo = true;

// Función para medir distancia en cm con el sensor HC-SR04
float medirDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duracion =
      pulseIn(PIN_ECHO, HIGH, 30000); // Timeout de 30ms (~5 metros máx)
  if (duracion == 0)
    return -1; // Fuera de rango / Sin eco

  return (duracion * 0.0343) / 2.0; // Distancia en centímetros
}

void setup() {
  Serial.begin(9600);

  // Configuración de pines de sensores
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_SONIDO_DIGITAL, INPUT);

  // Inicialización de Servomotores
  servo1.attach(PIN_SERVO_1);
  servo2.attach(PIN_SERVO_2);
  servo1.write(90);
  servo2.write(90);

  // Inicialización de Pantalla LCD I2C
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  ARDUINO NANO  ");
  lcd.setCursor(0, 1);
  lcd.print("  Iniciando...  ");
  delay(1500);
  lcd.clear();
}

void loop() {
  unsigned long tiempoActual = millis();

  // Lectura continua de sensores
  float distancia = medirDistancia();
  int nivelSonido = analogRead(PIN_SONIDO_ANALOG);
  bool detectaSonido = (digitalRead(PIN_SONIDO_DIGITAL) == HIGH);

  // Mover Servos en barrido suave
  if (direccionServo) {
    anguloServo += 5;
    if (anguloServo >= 180)
      direccionServo = false;
  } else {
    anguloServo -= 5;
    if (anguloServo <= 0)
      direccionServo = true;
  }
  servo1.write(anguloServo);
  servo2.write(180 - anguloServo);

  // Actualización de pantalla LCD y monitor serie sin bloquear (cada 500 ms)
  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;

    // Monitor Serie
    Serial.print("Distancia: ");
    if (distancia >= 0)
      Serial.print(distancia);
    else
      Serial.print("---");
    Serial.print(" cm | Sonido (A0): ");
    Serial.print(nivelSonido);
    Serial.print(" | Sonido (D8): ");
    Serial.println(detectaSonido ? "DETECTADO" : "NORMAL");

    // Fila 0: Distancia
    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    if (distancia >= 0 && distancia < 1000) {
      lcd.print(distancia, 1);
      lcd.print("cm  ");
    } else {
      lcd.print("Out Range");
    }

    // Fila 1: Nivel de sonido
    lcd.setCursor(0, 1);
    lcd.print("Sonido: ");
    lcd.print(nivelSonido);
    lcd.print("  ");
    if (detectaSonido) {
      lcd.setCursor(14, 1);
      lcd.print("!");
    } else {
      lcd.setCursor(14, 1);
      lcd.print(" ");
    }
  }

  delay(30); // Pequeña pausa para suavidad de servos
}
