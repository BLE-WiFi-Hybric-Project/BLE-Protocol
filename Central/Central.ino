#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

BLEClient* pClient;
bool ledState = false;

class MyClientCallbacks : public BLEClientCallbacks {
    void onConnect(BLEClient* pClient) {
      // When connected to the server
    }

    void onDisconnect(BLEClient* pClient) {
      // When disconnected from the server
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  BLEDevice::init("LEDClient");
  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallbacks());
}

void connectToServer(BLEAddress pAddress) {
  pClient->connect(pAddress);
  delay(1000); // Delay to establish connection
  
  BLERemoteService* pRemoteService = nullptr;
  BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

  // Perform service discovery
  pRemoteService = pClient->getService(serviceUUID);

  if (pRemoteService != nullptr) {
    BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(BLEUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8"));
    pRemoteCharacteristic->registerForNotify(onNotification);
  }
}

void onNotification(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  Serial.print("Notify callback, data: ");
  Serial.println(*pData);

  if (*pData == '1') {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }
}

void loop() {
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  BLEScanResults foundDevices = pBLEScan->start(5);
  for (int i = 0; i < foundDevices.getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    if (device.haveServiceUUID() && device.isAdvertisingService(BLEUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b"))) {
      connectToServer(device.getAddress());
    }
  }
  delay(1000); // Delay between scans
}
