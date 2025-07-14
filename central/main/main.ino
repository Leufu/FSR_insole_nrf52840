
#include <ArduinoBLE.h>

const char* UUID_SERVICE = "84582cd0-3df0-4e73-9496-29010d7445dd";
const char* UUID_STRING  = "84582cd9-3df0-4e73-9496-29010d7445dd";

BLEDevice peripheral;
BLECharacteristic fsrChar;

void setup() {
  Serial.begin(115200);
  //while (!Serial);

  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }

  Serial.println("Scanning for FSR_Sens...");
  BLE.scanForName("FSR_Sens");
}

void loop() {
  if (!peripheral) {
    peripheral = BLE.available();
    if (peripheral && peripheral.localName() == "FSR_Sens") {
      BLE.stopScan();
      connectToPeripheral(peripheral);
    }
  } else if (!peripheral.connected()) {
    Serial.println("Disconnected. Re-scanning...");
    peripheral = BLEDevice();
    BLE.scanForName("FSR_Sens");
  }
}

void connectToPeripheral(BLEDevice device) {
  Serial.print("Connecting to ");
  Serial.println(device.localName());

  if (!device.connect()) {
    Serial.println("Connection failed");
    return;
  }

  if (!device.discoverService(UUID_SERVICE)) {
    Serial.println("Service not found");
    device.disconnect();
    return;
  }

  fsrChar = device.characteristic(UUID_STRING);
  if (!fsrChar) {
    Serial.println("Characteristic not found");
    device.disconnect();
    return;
  }

  if (fsrChar.canSubscribe()) {
    fsrChar.subscribe();
  }

  Serial.println("Subscribed. Receiving data...");

  while (device.connected()) {
    if (fsrChar.valueUpdated()) {
      char buffer[64] = {0};
      fsrChar.readValue(buffer, sizeof(buffer));
      Serial.println(buffer);
    }
  }

  Serial.println("Disconnected from peripheral.");
}

