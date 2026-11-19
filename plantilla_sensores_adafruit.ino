#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID       "TU_WIFI"
#define WLAN_PASS       "TU_CLAVE_WIFI"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "TU_USUARIO_AIO"
#define AIO_KEY         "TU_AIO_KEY"

const int PIN_TRIG  = 25;
const int PIN_ECHO  = 26;

const int PIN_LM35  = 34;

const int PIN_LDR   = 35;

const int PIN_DHT   = 4;

const int PIN_SERVO = 13;

const int PIN_SS    = 5;
const int PIN_RST   = 22;
const int PIN_SCK   = 18;
const int PIN_MISO  = 19;
const int PIN_MOSI  = 23;

#define TIPO_DHT DHT11

const unsigned long INTERVALO_ENVIO = 15000;
const unsigned long INTERVALO_PING  = 60000;
const unsigned long TIEMPO_ABIERTO  = 3000;

DHT dht(PIN_DHT, TIPO_DHT);
Servo miServo;
MFRC522 lector(PIN_SS, PIN_RST);

byte uidAutorizado[4] = {0xA3, 0x7F, 0x2C, 0x91};

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish feedDistancia = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensores.distancia");
Adafruit_MQTT_Publish feedLM35      = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensores.lm35");
Adafruit_MQTT_Publish feedLuz       = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensores.luz");
Adafruit_MQTT_Publish feedTempDHT   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensores.temperatura");
Adafruit_MQTT_Publish feedHumedad   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensores.humedad");
Adafruit_MQTT_Publish feedRFID      = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sensores.rfid");

Adafruit_MQTT_Subscribe feedServo   = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/sensores.servo");

unsigned long ultimoEnvio = 0;
unsigned long ultimoPing = 0;
unsigned long momentoApertura = 0;
bool puertaAbierta = false;

void servoCallback(char *data, uint16_t len) {
  Serial.print(F("Mensaje recibido: "));
  Serial.println(data);
  int angulo = atoi(data);
  angulo = constrain(angulo, 0, 180);
  miServo.write(angulo);
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print(F("Conectando a MQTT... "));
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println(F("Reintentando en 5 segundos..."));
    mqtt.disconnect();
    delay(5000);
  }
  Serial.println(F("MQTT conectado"));
}

void publicarNumero(Adafruit_MQTT_Publish &feed, float valor, const char *nombre) {
  Serial.print(nombre);
  Serial.print(": ");
  Serial.print(valor);
  if (feed.publish((double)valor)) {
    Serial.println(F("  -> publicado"));
  } else {
    Serial.println(F("  -> fallo al publicar"));
  }
}

float leerDistanciaCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duracion = pulseIn(PIN_ECHO, HIGH, 30000);
  if (duracion == 0) {
    return -1.0;
  }
  return duracion * 0.0343 / 2.0;
}

float leerTemperaturaLM35() {
  float suma = 0;
  for (int i = 0; i < 20; i = i + 1) {
    suma = suma + analogReadMilliVolts(PIN_LM35);
    delay(5);
  }
  float promedioMv = suma / 20.0;
  return promedioMv / 10.0;
}

float leerLuzPorcentaje() {
  int valorCrudo = analogRead(PIN_LDR);
  return (valorCrudo / 4095.0) * 100.0;
}

bool esAutorizada() {
  if (lector.uid.size != 4) {
    return false;
  }
  for (byte i = 0; i < 4; i = i + 1) {
    if (lector.uid.uidByte[i] != uidAutorizado[i]) {
      return false;
    }
  }
  return true;
}

void revisarRFID() {
  if (!lector.PICC_IsNewCardPresent()) {
    return;
  }
  if (!lector.PICC_ReadCardSerial()) {
    return;
  }

  String uidTexto = "";
  for (byte i = 0; i < lector.uid.size; i = i + 1) {
    if (lector.uid.uidByte[i] < 0x10) {
      uidTexto = uidTexto + "0";
    }
    uidTexto = uidTexto + String(lector.uid.uidByte[i], HEX);
  }
  uidTexto.toUpperCase();

  Serial.print(F("UID leido: "));
  Serial.println(uidTexto);

  if (esAutorizada()) {
    Serial.println(F("Acceso permitido"));
    miServo.write(90);
    puertaAbierta = true;
    momentoApertura = millis();
    feedRFID.publish("PERMITIDO");
  } else {
    Serial.println(F("Acceso denegado"));
    feedRFID.publish("DENEGADO");
  }

  lector.PICC_HaltA();
  lector.PCD_StopCrypto1();
}

void cerrarPuertaSiCorresponde() {
  if (puertaAbierta && (millis() - momentoApertura >= TIEMPO_ABIERTO)) {
    miServo.write(0);
    puertaAbierta = false;
  }
}

void enviarSensores() {
  float distancia = leerDistanciaCm();
  if (distancia >= 0) {
    publicarNumero(feedDistancia, distancia, "Distancia (cm)");
  } else {
    Serial.println(F("Distancia: sin objeto en rango"));
  }

  float temperaturaLM35 = leerTemperaturaLM35();
  publicarNumero(feedLM35, temperaturaLM35, "LM35 (C)");

  float luz = leerLuzPorcentaje();
  publicarNumero(feedLuz, luz, "Luz (%)");

  float humedad = dht.readHumidity();
  float temperaturaDHT = dht.readTemperature();
  if (isnan(humedad) || isnan(temperaturaDHT)) {
    Serial.println(F("Error al leer el DHT11"));
  } else {
    publicarNumero(feedTempDHT, temperaturaDHT, "DHT11 temperatura (C)");
    publicarNumero(feedHumedad, humedad, "DHT11 humedad (%)");
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  analogReadResolution(12);
  dht.begin();

  miServo.setPeriodHertz(50);
  miServo.attach(PIN_SERVO, 500, 2400);
  miServo.write(0);

  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_SS);
  lector.PCD_Init();

  Serial.println(F("Conectando a Wi-Fi..."));
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(F("\nWi-Fi conectado"));

  feedServo.setCallback(servoCallback);
  mqtt.subscribe(&feedServo);
}

void loop() {
  MQTT_connect();

  mqtt.processPackets(100);

  revisarRFID();
  cerrarPuertaSiCorresponde();

  if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {
    ultimoEnvio = millis();
    enviarSensores();
  }

  if (millis() - ultimoPing >= INTERVALO_PING) {
    ultimoPing = millis();
    if (!mqtt.ping()) {
      mqtt.disconnect();
    }
  }
}
