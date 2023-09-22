#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic *pCharacteristic;
bool buttonState = false;

const int buttonPin = 2;  // Define the pin for the button

void setup() {
  pinMode(buttonPin, INPUT);

  BLEDevice::init("ESP32_Peripheral");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(BLEUUID("2185cd30-5935-11ee-8c99-0242ac120002"));

  pCharacteristic = pService->createCharacteristic(
                      BLEUUID("2185cd30-5935-11ee-8c99-0242ac120002"),
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLEUUID("2185cd30-5935-11ee-8c99-0242ac120002"));
  pAdvertising->setScanResponse(false);
  pAdvertising->start();
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState) {
    pCharacteristic->setValue((uint8_t*)&buttonState, 1);
    pCharacteristic->notify();
    delay(1000);
  }
}
