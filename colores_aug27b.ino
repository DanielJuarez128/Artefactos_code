int rojo = 5;
int verde = 6;
int azul = 7;


void setup() {
  pinMode(rojo, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(azul, OUTPUT);

}

void set_color(int red, int green, int blue){
  analogWrite(rojo, red);
  analogWrite(verde, green);
  analogWrite(azul, blue);
}

void loop() {
  set_color(255,0,0);
  delay(1000);
  set_color(0,255,0);
  delay(1000);
  set_color(0,0,255);
  delay(1000);
  set_color(194,0,10);
  delay(1000);
  set_color(100,255,0);
  delay(1000);
  set_color(50,70,255);
  delay(1000);
  set_color(183,190,232);
  delay(1000);
  set_color(10,255,192);
  delay(1000);
  set_color(0,100,249);
  delay(1000);
  set_color(144,138,200);
  delay(1000);
}
