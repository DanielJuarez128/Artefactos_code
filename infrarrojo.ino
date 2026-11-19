/*const int sensorIR= 26;

void setup() {
  pinMode (sensorIR, INPUT);
  Serial.begin(9600);
}

void loop() {
  //int estado = digitalRead(sensorIR);
  //Serial.print("Estado del sensor: ");
  //Serial.println(estado);
  //delay(300);

  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervalo) {

    tiempoAnterior = tiempoActual;

    int estado = digitalRead(sensorIR);

    Serial.print("Estado del sensor: ");
    Serial.println(estado);
  }

}*/


const int sensorIR = 26;
const int ledRojo = 27;

unsigned long tiempoAnterior = 0;

void setup() {
  pinMode(sensorIR, INPUT);
  pinMode(ledRojo, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  /*if (millis() - tiempoAnterior >= 300) {
    tiempoAnterior = millis();
  }*/
    detectarMovimiento();
}

void detectarMovimiento() {
  int estado = digitalRead(sensorIR);

  /*Serial.println(estado);

  if (estado == LOW) {
    digitalWrite(ledRojo, HIGH);
    Serial.println("MOVIMIENTO DETECTADO");
  } else {
    digitalWrite(ledRojo, LOW);
    Serial.println("SIN MOVIMIENTO");
  }*/

  if (estadoActual == LOW && estadoAnterior == HIGH) {
    digitalWrite(ledRojo, HIGH);
    Serial.println("MOVIMIENTO DETECTADO");
  }

  // Detectar cuando se retira la mano
  if (estadoActual == HIGH && estadoAnterior == LOW) {
    digitalWrite(ledRojo, LOW);
    Serial.println("SIN MOVIMIENTO");
  }

  estadoAnterior = estadoActual;

}