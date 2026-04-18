#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ezButton.h>

BluetoothSerial SerialBT;
exButton limitSwitch(19); //replace with desired pin

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Master");
  limitSwitch.setDebouncetime(50);

    delay(1000);
    Serial.println("Slave is waiting for Master to connect...");
  }

  Serial.println("Connected to Master.");
}

void loop() {
  limitSwitch.loop();

  if (SerialBT.available()) {
    if (limitSwitch.isPressed()) {
      Serial.write(1);
      Serial.println("Switch pressed!");
    }

    if (limitSwitch.isReleased()) {
      Serial.write(0);
      Serial.println("Switch released!");
    }
  }

  delay(50);
}

