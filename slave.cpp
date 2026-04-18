#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ESP32Servo.h>

Servo myservo;

void setup() {
  Serial.begin(115200);
  NimBLEDevice::init("");

  NimBLEScan *pScan = NimBLEDevice::getScan();
  NimBLEScanResults results = pScan->getResults(10 * 1000); //10 sec scan for devices
  NimBLEUUID masterId("master");

  //iterating through results untill we find the desired UUID
  for (int i = 0; i < results.getCount(); i++) {
    const NimBLEAdvertisedDevice *device = results.getDevice(i);

    if (device->isAdvertisingService(masterId)) {
      NimBLEClient *pClient = NimBLEDevice::createClient();

      if (!(pClient->connect(&device))) {
        Serial.println("Failed to connect, restarting...");
        ESP.restart();
      } else {
        Serial.println("Connected!");
      }
    }
  }

  myservo.setPeriodHertz(50);
  myservo.attach(13, 500, 3000);
  //might need to callibrate these min maxes to our servo
  myservo.write(0); //open position

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
    } else if (input == '0') {
      myservo.write(0);  //open
      SerialBT.println("Finger open");
    }
  }

  delay(50);
}
