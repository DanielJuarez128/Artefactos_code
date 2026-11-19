int boton = 2;
int led = 13;
int estadoBoton = 0;

void setup()
{
    pinMode(boton, INPUT);
    pinMode(led, OUTPUT);
}

void loop()
{
    estadoBoton = digitalRead(boton);

    if (estadoBoton == HIGH)
    {
        digitalWrite(led, HIGH);
    }
    else
    {
        digitalWrite(led, LOW);
    }
}