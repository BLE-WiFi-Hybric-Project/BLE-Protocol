#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

static BLEUUID SERVICE_UUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID CHARACTERISTIC_UUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

BLEScan* pBLEScan;
BLEClient* pClient;

bool isConnected = false;

void notifyCallback(BLERemoteCharacteristic* pCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  if(isNotify) {
    if(pData[0] == '1') {
      digitalWrite(2, HIGH);
    } else if (pData[0] == '0') {
      digitalWrite(2, LOW);
    }
    Serial.println("Received data from server"); 
  }
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.println("Found a device");
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(SERVICE_UUID)) {
      pClient = BLEDevice::createClient();
      pClient->connect(&advertisedDevice);
      BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
      BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
      pRemoteCharacteristic->registerForNotify(notifyCallback);
      isConnected = true;
    }
  }
};

void scanForDevices() {
  pBLEScan->start(10); // Scan for 10 seconds, adjust as needed
}

void setup() {
  Serial.begin(115200);
  
  BLEDevice::init("ESP32 Client");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);

  Serial.println("Done Setup");
}

void loop() {
  if(isConnected) {
    // You can do other tasks here
    scanForDevices();
    Serial.println("Connecting...");
  }
  delay(1000);
}
