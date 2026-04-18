#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
Servo myservo;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Slave");

  myservo.setPeriodHertz(50);
  myservo.attach(13, 1000, 2000);
  //might need to callibrate these min maxes to our servo

  while (!SerialBT.connected()) {
    delay(1000);
    Serial.println("Slave is waiting for Master to connect...");
  }

  Serial.println("Connected to Master.");
}

void loop() {
  if (SerialBT.available()) {
    char input = SerialBT.read();

    if (input == '1') {
      myservo.write(180);  //close
      SerialBT.println("Finger closed");
    } else {
      myservo.write(0);  //open
      SerialBT.println("Finger open");
    }
  }

  delay(50);
}
