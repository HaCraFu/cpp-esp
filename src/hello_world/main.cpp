#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  Serial.print("Hello world ");
  Serial.println(millis() / 1000.);
  delay(1000);
}