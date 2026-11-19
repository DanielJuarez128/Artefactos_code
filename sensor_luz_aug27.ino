int sensor;
float voltaje;

void setup() {
  //variables de configuracion
  //voltaje = valor leido/ resolucion ADC (1023) * voltaje de referencia
  Serial.begin(9600);
}

void loop() {
  sensor = analogRead(A0);
  voltaje = (sensor*5.0)/1023;
  Serial.print("Sensor: ");
  Serial.println(sensor);
  Serial.print("Voltaje: ");
  Serial.println(voltaje);
  Serial.println("V");
  delay(2000);

}
