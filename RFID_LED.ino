#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

#define LED_VERDE 6
#define LED_ROJO 7

MFRC522 rfid(SS_PIN, RST_PIN);

String uidPermitido = "43A12BC4";

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
}

void loop()
{
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String uidLeido = "";

  for (byte i = 0; i < rfid.uid.size; i++)
  {
      uidLeido += String(rfid.uid.uidByte[i], HEX);
  }

  uidLeido.toUpperCase();

  if(uidLeido == uidPermitido)
  {
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_ROJO, LOW);
  }
  else
  {
      digitalWrite(LED_ROJO, HIGH);
      digitalWrite(LED_VERDE, LOW);
  }

  delay(2000);

  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, LOW);

  rfid.PICC_HaltA();
}