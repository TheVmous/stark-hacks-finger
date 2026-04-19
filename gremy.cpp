#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ESP32Servo.h>

Servo myservo;

// BLE uses UUIDs (Unique Identifiers) instead of names to route data
#define SERVICE_UUID        "ABCD" 
#define CHARACTERISTIC_UUID "1234"

// 1. Create a Callback class to handle incoming data
class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
        // Retrieve the data written by the Master
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0) {
            char input = value[0];
            
            // Replicating your old loop logic here
            if (input == '1') {
                myservo.write(180);
                Serial.println("Finger closed");
            } else if (input == '0') {
                myservo.write(0);
                Serial.println("Finger open");
            }
        }
    }
};

void setup() {
    Serial.begin(115200);
    
    // Servo setup
    myservo.setPeriodHertz(50);
    myservo.attach(13, 500, 3000); 
    myservo.write(0); // open position

    // 2. Initialize NimBLE and set device name
    NimBLEDevice::init("Stark-Slave");

    // 3. Create the Server
    NimBLEServer *pServer = NimBLEDevice::createServer();

    // 4. Create the Service
    NimBLEService *pService = pServer->createService(SERVICE_UUID);

    // 5. Create the Characteristic with WRITE permission
    NimBLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            NIMBLE_PROPERTY::WRITE
                                        );

    // 6. Attach the callback class so the ESP knows what to do when data arrives
    pCharacteristic->setCallbacks(new CharacteristicCallbacks());

    // 7. Start the Service
    pService->start();

    // 8. Start Advertising so the Master can find it
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();

    Serial.println("Slave is advertising. Waiting for Master to write data...");
}

void loop() {
    // The loop is now empty. 
    // BLE callbacks run asynchronously in the background. 
    delay(2000); 
}
