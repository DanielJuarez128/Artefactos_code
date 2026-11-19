#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// ==================================================
// CONFIGURACION WIFI Y ADAFRUIT IO (cambia estos datos)
// ==================================================
#define WIFI_SSID      ""
#define WIFI_PASSWORD  ""

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    ""
#define AIO_KEY         ""

// ==================================================
// PINES
// ==================================================
// Sensor IR
const int PIN_IR = 4;
int PIN_LDR = 35;

// ==================================================
// OBJETOS Y VARIABLES
// ==================================================

WiFiClient cliente;
Adafruit_MQTT_Client mqtt(&cliente, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Un feed por sensor
Adafruit_MQTT_Publish feedIR   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/IR");
Adafruit_MQTT_Publish feedLDR  = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/porcentaje_luz");

// ---- IR ----
int estadoIR = 0;      // 1 = detectado, 0 = nada
int ultimoIR = -1;     // -1 fuerza el primer envio
unsigned long tEnvioIR = 0;
int estadoLDR = 0;
int ultimoLDR = -1;


// ---- Tiempos generales ----
const unsigned long INTERVALO_LECTURA   = 50;      // lectura de IR y PIR (ms)
const unsigned long INTERVALO_MIN_ENVIO = 2500;    // minimo entre envios del mismo feed (ms)
const unsigned long INTERVALO_PERIODICO = 20000;   // reenvio periodico de IR y PIR (ms)
const unsigned long INTERVALO_REINTENTO = 5000;    // reintento de conexion MQTT (ms)

unsigned long tLectura    = 0;
unsigned long tPeriodico  = 0;
unsigned long tReintento  = 0;

// ==================================================
// WIFI
// ==================================================
void conectarWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("[WiFi] Conectando");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("[WiFi] Conectado");
}

// ==================================================
// ADAFRUIT IO (MQTT)
// ==================================================
void conectarMQTT() {
  if (mqtt.connected()) return;

  // Un solo intento cada 5 s, sin bloquear la lectura de sensores
  if (millis() - tReintento < INTERVALO_REINTENTO && tReintento != 0) return;
  tReintento = millis();

  Serial.println("[AIO] Conectando a Adafruit IO...");
  int8_t resultado = mqtt.connect();

  if (resultado == 0) {
    Serial.println("[AIO] Conectado");
  } else {
    Serial.print("[AIO] Error: ");
    Serial.println(mqtt.connectErrorString(resultado));
    mqtt.disconnect();
  }
}

void enviarNumero(Adafruit_MQTT_Publish &feed, int valor, const char *nombre) {
  if (feed.publish((int32_t)valor)) {
    Serial.print("[AIO] Enviado ");
    Serial.print(nombre);
    Serial.print(": ");
    Serial.println(valor);
  } else {
    Serial.print("[AIO] Error al enviar ");
    Serial.println(nombre);
  }
}

// ==================================================
// SENSOR IR
// ==================================================
void iniciarIR() {
  pinMode(PIN_IR, INPUT);
  Serial.println("[IR] Listo");
}

void leerIR() {
  // La mayoria de modulos IR dan LOW cuando detectan algo
  estadoIR = (digitalRead(PIN_IR) == LOW) ? 1 : 0;

  if (estadoIR != ultimoIR) {
    Serial.print("[IR] ");
    Serial.println(estadoIR ? "Objeto detectado" : "Sin objeto");

    // Enviar a Adafruit IO si ya paso el tiempo minimo
    if (mqtt.connected() && millis() - tEnvioIR >= INTERVALO_MIN_ENVIO) {
      enviarNumero(feedIR, estadoIR, "IR");
      tEnvioIR = millis();
      ultimoIR = estadoIR;
    } else if (!mqtt.connected()) {
      ultimoIR = estadoIR;
    }
  }
}

// ==================================================
// SETUP
// ==================================================
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  delay(500);

  Serial.println("=== Iniciando sensores ===");

  iniciarIR();

  Serial.println("=== Iniciando conexion ===");
  conectarWiFi();
  conectarMQTT();

  Serial.println("=== Sistema en marcha ===");
}


// ==================================================
// LOOP
// ==================================================
void loop() {
  // Mantener las conexiones
  conectarWiFi();
  conectarMQTT();
  if (mqtt.connected()) mqtt.processPackets(10);

  int rawValue = analogRead(PIN_LDR);

  float porcentaje_luz = (rawValue/4095.0) * 100.0;
  Serial.print(porcentaje_luz);
  Serial.println(" %");

  // IR y PIR se leen cada 50 ms
  unsigned long ahora = millis();
  if (ahora - tLectura >= INTERVALO_LECTURA) {
    tLectura = ahora;

    leerIR();
  }

  if (mqtt.connected() && ahora - tPeriodico >= INTERVALO_PERIODICO) {
    tPeriodico = ahora;
    enviarNumero(feedIR, estadoIR, "IR (periodico)");
    enviarNumero(feedLDR, estadoLDR, "LDR");
  }

  delay(1000);
}
