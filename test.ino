void conectarWiFi();
void conectarMQTT();

void escribirRGB(uint8_t r, uint8_t g, uint8_t b);
void alarmaLED();

void setup() {
  Serial.begin(115200);
  delay(10);

  dht.begin();
  pinMode(32, INPUT);
  analogReadResolution(12);

  pinMode(R,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(B,OUTPUT);

  digitalWrite(R,LOW);
  digitalWrite(G,LOW);
  digitalWrite(B,LOW);

  

  escribirRGB(255, 255, 255);   

  conectarWiFi();
  
}

void loop() {
  conectarMQTT();   // mantiene la conexión con Adafruit (reconecta si se cae)
  
  uint8_t r;
  uint8_t g;
  uint8_t b;
  Adafruit_MQTT_Subscribe *sub;
  publicarSensores();
  escribirRGB(r, g, b);
  alarmaLED();
}

// LED RGB SEGUN TEMPERATURA
void escribirRGB(uint8_t r, uint8_t g, uint8_t b) {

  const int R=25;
  const int G=26;
  const int B=27;

  if (RGB_ANODO_COMUN) {          
    r = 255 - r;  g = 255 - g;  b = 255 - b;
  }

  ledcWrite(R, r);
  ledcWrite(G, g);
  ledcWrite(B, b);
}

// El color cambia segun la temperatura que percibe
void alarmaLED() {
  
  Serial.println("Temperatura: ");
  Serial.println(temperatura);
  Serial.println("°C");
  delay(3000);

//Condicion para que encienda el led de color Verde
  if(temperatura>Temperatura_Minima && temperatura< Temperatura_Maxima)
  {
    digitalWrite(R,LOW);
    digitalWrite(G,HIGH);
    digitalWrite(B,LOW);
  }

  //Condicion para que encienda el led de color Azul
  if(temperatura<=Temperatura_Minima)
  {
    digitalWrite(R,LOW);
    digitalWrite(G,LOW);
    digitalWrite(B,HIGH);
  }

  //Condicion para que encienda el led de color Rojo
  if(temperatura>=Temperatura_Maxima)
  {
    digitalWrite(R,HIGH);
    digitalWrite(G,LOW);
    digitalWrite(B,LOW);
  }
  delay(500);

  // Mantener viva la conexión MQTT
  if (millis() - ultimoPing >= 30000) {
    ultimoPing = millis();
    mqtt.ping();
  }
}


void publicarSensores() {
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}

// =====================================================================
//  CONEXIONES
// =====================================================================
void conectarWiFi() {
  Serial.print("Conectando a "); Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi conectado. IP: "); Serial.println(WiFi.localIP());
}

void conectarMQTT() {
  if (mqtt.connected()) return;

  Serial.print("Conectando a Adafruit IO... ");
  int8_t ret;
  uint8_t intentos = 3;
  while ((ret = mqtt.connect()) != 0) {         
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintentando en 5 segundos...");
    mqtt.disconnect();
    delay(5000);
    if (--intentos == 0) {
      Serial.println("No se pudo conectar. Reiniciando la ESP32...");
      ESP.restart();
    }
  }
  Serial.println("¡Conectado a Adafruit IO!");
}