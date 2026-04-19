#include <Arduino.h>
#include <ezButton.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "74c435d0-62d4-4ffb-8a95-bbc672744429"
#define CHARACTERISTIC_UUID "3091e16f-b28f-43fb-8fdc-4110e02990ea"

ezButton limitSwitch(17); //replace with desired pin

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE");

  if (!BLEDevice::init("Shoe BLE Server")) {
    Serial.println("BLE initialization failed!");
    return;
  }
  
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic =
    pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue(0);
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined');
  
  limitSwitch.setDebouncetime(50);

  //Serial.println("Connected to Slave.");
}

void loop() {
  limitSwitch.loop();

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
