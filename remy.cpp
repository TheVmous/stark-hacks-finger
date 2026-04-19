#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ezButton.h>

#define SERVICE_UUID        "ABCD"
#define CHARACTERISTIC_UUID "1234"

ezButton limitSwitch(17);
NimBLERemoteCharacteristic* pCharacteristic = nullptr;
bool connected = false;

bool connectToSlave() {
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setActiveScan(true);
    Serial.println("Scanning for Slave...");
    
    NimBLEScanResults results = pScan->start(5, false);
    NimBLEUUID serviceUuid(SERVICE_UUID);
    const NimBLEAdvertisedDevice* advDevice = nullptr;

    for (int i = 0; i < results.getCount(); i++) {
        const NimBLEAdvertisedDevice* device = results.getDevice(i);
        if (device->isAdvertisingService(serviceUuid)) {
            advDevice = device;
            break;
        }
    }

    if (!advDevice) return false;

    NimBLEClient* pClient = NimBLEDevice::createClient();
    if (!pClient->connect(advDevice)) {
        NimBLEDevice::deleteClient(pClient);
        return false;
    }

    NimBLERemoteService* pService = pClient->getService(serviceUuid);
    if (!pService) {
        pClient->disconnect();
        return false;
    }

    pCharacteristic = pService->getCharacteristic(NimBLEUUID(CHARACTERISTIC_UUID));
    if (!pCharacteristic || !pCharacteristic->canWrite()) {
        pClient->disconnect();
        return false;
    }

    return true;
}

void setup() {
    Serial.begin(115200);
    limitSwitch.setDebounceTime(50); 

    NimBLEDevice::init("Stark-Master");

    int tries = 0;
    while (!connected && tries < 5) {
        connected = connectToSlave();
        if (!connected) {
            Serial.println("Failed to connect. Retrying...");
            tries++;
            delay(1000);
        }
    }

    if (!connected) {
        Serial.println("5 failures. Restarting...");
        ESP.restart();
    }
    Serial.println("Connected to Slave.");
}

void loop() {
    limitSwitch.loop();

    if (connected && pCharacteristic != nullptr) {
        if (limitSwitch.isPressed()) {
            pCharacteristic->writeValue("1", 1);
            Serial.println("Switch pressed: Sent 1");
        }

        if (limitSwitch.isReleased()) {
            pCharacteristic->writeValue("0", 1);
            Serial.println("Switch released: Sent 0");
        }
    }

    delay(20);
}
