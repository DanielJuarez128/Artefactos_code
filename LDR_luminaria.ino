int ldr = A0;

int led1 = 8;
int led2 = 9;
int led3 = 10;

int valorLuz;

void setup()
{
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);

    Serial.begin(9600);
}

void loop()
{
    valorLuz = analogRead(ldr);

    Serial.println(valorLuz);

    if(valorLuz < 500) // poca luz
    {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        digitalWrite(led3, HIGH);
    }
    else // suficiente luz
    {
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
    }

    delay(100);
}