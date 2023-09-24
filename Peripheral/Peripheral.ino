#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool buttonState = false;

const int buttonPin = 2;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      // When a client connects
    }

    void onDisconnect(BLEServer* pServer) {
      // When a client disconnects
    }
};

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);

  BLEDevice::init("ButtonServer");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(BLEUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b"));

  pCharacteristic = pService->createCharacteristic(
                      BLEUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8"),
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    pCharacteristic->setValue("1");
    pCharacteristic->notify();
    delay(1000); // Delay to avoid sending too frequently
  }

  delay(10);
}
