#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Slave");

  while (!SerialBT.connected()) {
    delay(1000);
    Serial.println("Slave is waiting for Master to connect...");
  }

  Serial.println("Connected to Master.");

}

void loop() {
  // put your main code here, to run repeatedly:

}

