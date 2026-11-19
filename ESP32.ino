// Daniel Isaías Juárez Gutiérrez - 00012922

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=================");
  Serial.println("PRUEBA DE ESP32");
  Serial.println("=================");
  Serial.println();
}

void loop() {
  Serial.print("ESP32 funcionando correctamente - Tiempo activa: ");
  Serial.print(millis()/1000.0);
  Serial.println(" segundos.");
  delay(200);
}
