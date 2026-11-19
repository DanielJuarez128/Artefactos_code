//OPCION 1
//analog set pin atenuation
// Pin donde conectamos el LM35
/*const int pinLM35 = 34; // GPIO34 (entrada ADC)

// Resolución ADC del ESP32
const float VREF = 3.3;   // Voltaje de referencia del ADC
const int ADC_RES = 4095; // Resolución de 12 bits (0-4095)

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Leer valor ADC
  int lecturaADC = analogRead(pinLM35);

  // Convertir a voltaje
  float voltaje = (lecturaADC * VREF) / ADC_RES;

  // Convertir voltaje a temperatura
  float temperaturaC = voltaje * 100.0; // LM35: 10mV = 0.01V por °C

  // Mostrar resultados
  Serial.print("ADC: ");
  Serial.print(lecturaADC);
  Serial.print(" | Voltaje: ");
  Serial.print(voltaje, 3);
  Serial.print(" V | Temp: ");
  Serial.print(temperaturaC, 2);
  Serial.println(" °C");

  delay(5000); 
}*/


//OPCION 2
int lm35 = 34;
float valorLeido;
float temperatura;
  
  void setup(){
    Serial.begin(9600);
    pinMode(lm35, INPUT);
  }
  
  void loop(){
    valorLeido = analogReadMilliVolts(lm35);
    temperatura = valorLeido/10.0;
    Serial.println("Temperatura: ");
    Serial.println(temperatura);
    Serial.print("°C");
    delay(3000);
  }
