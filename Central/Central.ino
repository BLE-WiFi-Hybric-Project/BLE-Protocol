#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

BLEUUID serviceUUID("2185cd30-5935-11ee-8c99-0242ac120002");
BLEClient* pClient;

const int ledPin = 13;  // Define the pin for the LED

void setup() {
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
  BLEDevice::init("ESP32_Central");
  pClient  = BLEDevice::createClient();
}

void connectToServer() {
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  BLEScanResults scanResults = pBLEScan->start(5);
  int count = scanResults.getCount();
  for(int i = 0; i < count; i++) {
    BLEAdvertisedDevice device = scanResults.getDevice(i);
    if (device.haveServiceUUID() && device.isAdvertisingService(serviceUUID)) {
      pClient->connect(&device);
      return;
    }
  }
}

void onConnect(BLEClient* pclient) {
  Serial.println("Connected");

  BLERemoteService* pRemoteService = pclient->getService(serviceUUID);

  if (pRemoteService == nullptr) {
    Serial.print("Failed to find service with UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    return;
  }

  BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(BLEUUID("2185cd30-5935-11ee-8c99-0242ac120002"));

  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find characteristic with UUID: ");
    Serial.println("2185cd30-5935-11ee-8c99-0242ac120002");
    return;
  }

  pRemoteCharacteristic->registerForNotify([](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    Serial.print("Notify callback called for characteristic: ");
    Serial.println(pBLERemoteCharacteristic->getUUID().toString().c_str());

    bool receivedState = pData[0];

    if (receivedState) {
      Serial.println("Received ON signal");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("Received OFF signal");
      digitalWrite(ledPin, LOW);
    }
  });
}

void loop() {
  if (!pClient->isConnected()) {
    connectToServer();
  }

  delay(1000);
}
