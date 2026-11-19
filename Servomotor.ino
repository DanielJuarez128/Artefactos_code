#include <ESP32Servo.h>

Servo miServo;
const int PIN_SERVO = 18;

void setup() {
  miServo.setPeriodHertz(50);              // los servos usan 50 Hz
  miServo.attach(PIN_SERVO, 500, 2400);    // pin, pulso mínimo y máximo (µs)
}

void loop() {
  // Barrido de 0° a 180°
  for (int angulo = 0; angulo <= 180; angulo++) {
    miServo.write(angulo);
    delay(15);
  }
  // Barrido de 180° a 0°
  for (int angulo = 180; angulo >= 0; angulo--) {
    miServo.write(angulo);
    delay(15);
  }
}