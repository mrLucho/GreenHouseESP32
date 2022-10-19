#include <Arduino.h>
#include <DHT.h>

#define DHT11PIN 27
#define FOTO_ANALOG_INPUT_PIN 26

//921600
DHT dht;


void setup() {
  Serial.begin(74880);
  
  dht.setup(DHT11PIN);
  
}

void loop() {
  // Serial.println(dht.getTemperature()); // despite wrong borders func returns reasonable feedback
  int analogReadVal = analogRead(FOTO_ANALOG_INPUT_PIN);
  Serial.println(analogReadVal);
  delay(1200);
}