#include <Arduino.h>
#include <ezButton.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "74c435d0-62d4-4ffb-8a95-bbc672744429"
#define CHARACTERISTIC_UUID "3091e16f-b28f-43fb-8fdc-4110e02990ea"

ezButton limitSwitch(17); 
BLECharacteristic *pCharacteristic; // Make this global so loop() can see it
bool deviceConnected = false;

// Callback to track connection status
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      // Restart advertising so the client can reconnect
      BLEDevice::startAdvertising();
    }
};

void setup() {
  Serial.begin(115200);
  limitSwitch.setDebounceTime(50);

  BLEDevice::init("Shoe BLE Server");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Added PROPERTY_NOTIFY so the client gets updates automatically
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("Server started! Waiting for client...");
}

void loop() {
  limitSwitch.loop();

  if (deviceConnected) {
    if (limitSwitch.isPressed()) {
      pCharacteristic->setValue("1");
      pCharacteristic->notify(); // Push the '1' to the other ESP32
      Serial.println("Switch pressed - Notified Client: 1");
    }

    if (limitSwitch.isReleased()) {
      pCharacteristic->setValue("0");
      pCharacteristic->notify(); // Push the '0' to the other ESP32
      Serial.println("Switch released - Notified Client: 0");
    }
  }
}
