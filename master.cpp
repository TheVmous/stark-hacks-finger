#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ezButton.h>

BluetoothSerial SerialBT;
exButton limitSwitch(17); //replace with desired pin

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Master", true);
  
  limitSwitch.setDebouncetime(50);

  int tries = 0;
  while (!SerialBT.connect("Slave")) {
    delay(1000);
    Serial.println("Slave is waiting for Master to connect...");

    if (tries >= 5) {
      Serial.println("Failed to connect. Restarting...");
      ESP.restart();
    }
    tries++;
  }

  Serial.println("Connected to Master.");
}

void loop() {
  limitSwitch.loop();

  if (SerialBT.available()) {
    if (limitSwitch.isPressed()) {
      SerialBT.write('1');
      Serial.println("Switch pressed!");
    }

    if (limitSwitch.isReleased()) {
      SerialBT.write('0');
      Serial.println("Switch released!");
    }
  }

  delay(20);
}

