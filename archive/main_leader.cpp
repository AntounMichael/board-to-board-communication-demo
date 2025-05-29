#include <Wire.h>

#define SLAVE_ADDR 0x09

void setup() {
  Wire.begin();  // leader
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  Wire.beginTransmission(SLAVE_ADDR);
  int rawValue = analogRead(26);
  Wire.write((String(rawValue) + ",").c_str());  // send a char
  Serial.println((String(rawValue) + ",").c_str());
  Wire.endTransmission();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

