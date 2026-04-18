#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEClient.h>
#include <ezButton.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "abcd1234-ab12-ab12-ab12-abcdef123456"

ezButton limitSwitch(17);
BLEClient* pClient;
BLERemoteCharacteristic* pRemoteChar;
bool connected = false;

class ClientCallbacks : public BLEClientCallbacks {
  void onConnect(BLEClient* client) {
    connected = true;
    Serial.println("Connected to Slave.");
  }
  void onDisconnect(BLEClient* client) {
    connected = false;
    Serial.println("Disconnected from Slave.");
  }
};

bool connectToSlave() {
  BLEScan* pScan = BLEDevice::getScan();
  pScan->setActiveScan(true);
  BLEScanResults results = pScan->start(5);

  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice device = results.getDevice(i);
    if (device.getName() == "Slave") {
      pClient = BLEDevice::createClient();
      pClient->setClientCallbacks(new ClientCallbacks());
      pClient->connect(&device);

      BLERemoteService* pService = pClient->getService(SERVICE_UUID);
      if (!pService) {
        Serial.println("Service not found.");
        return false;
      }

      pRemoteChar = pService->getCharacteristic(CHARACTERISTIC_UUID);
      if (!pRemoteChar) {
        Serial.println("Characteristic not found.");
        return false;
      }
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  BLEDevice::init("Master");
  limitSwitch.setDebounceTime(50);

  Serial.println("Scanning for Slave...");
  int tries = 0;
  while (!connectToSlave()) {
    Serial.println("Retrying...");
    tries++;
    if (tries >= 5) {
      Serial.println("Failed to connect. Restarting...");
      ESP.restart();
    }
    delay(1000);
  }
}

void loop() {
  limitSwitch.loop();

  if (!connected) {
    Serial.println("Lost connection. Restarting...");
    ESP.restart();
  }

  if (limitSwitch.isPressed()) {
    pRemoteChar->writeValue("1");
    Serial.println("Switch pressed!");
  }
  if (limitSwitch.isReleased()) {
    pRemoteChar->writeValue("0");
    Serial.println("Switch released!");
  }

  delay(20);
}
