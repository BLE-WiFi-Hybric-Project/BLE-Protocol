#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

int buttonPin = 2; // Define the pin where the button is connected
bool buttonState = false;

void setup() {
  Serial.begin(115200);
 
  BLEDevice::init("ESP32 Server");
  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  pinMode(buttonPin, INPUT);
  Serial.println("Done Setup");
}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  if(buttonState) {
    pCharacteristic->setValue("1"); // Button pressed
  } else {
    pCharacteristic->setValue("0"); // Button released
  }
  pCharacteristic->notify();
  delay(1000); // Adjust as needed
}
