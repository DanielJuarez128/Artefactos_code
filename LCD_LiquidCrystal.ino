#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2);

int contador = 0;

void setup()
{
  lcd.begin(16,2);
}

void loop()
{
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Contador:");

  lcd.setCursor(0,1);
  lcd.print(contador);

  contador++;

  delay(1000);
}