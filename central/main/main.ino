#include <ArduinoBLE.h>

// UUIDs del servicio y características
const char* UUID_SERVICIO = "84582cd0-3df0-4e73-9496-29010d7445dd";
const char* UUID_CH[] = {
  "84582cd1-3df0-4e73-9496-29010d7445dd", // CH0
  "84582cd2-3df0-4e73-9496-29010d7445dd", // CH1
  "84582cd3-3df0-4e73-9496-29010d7445dd", // CH2
  "84582cd4-3df0-4e73-9496-29010d7445dd", // CH3
  "84582cd5-3df0-4e73-9496-29010d7445dd", // CH4
  "84582cd6-3df0-4e73-9496-29010d7445dd", // CH5
  "84582cd7-3df0-4e73-9496-29010d7445dd", // CH6
  "84582cd8-3df0-4e73-9496-29010d7445dd"  // CH7
};

BLEDevice peripheral;
BLECharacteristic ch[8];

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Error inicializando BLE");
    while (1);
  }

  Serial.println("Escaneando periféricos BLE...");
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
    Serial.println("Perdida conexión. Escaneando nuevamente...");
    peripheral = BLEDevice(); // limpiar referencia
    BLE.scanForName("FSR_Sens");
  }
}

void connectToPeripheral(BLEDevice device) {
  Serial.print("Conectando a ");
  Serial.println(device.localName());

  if (device.connect()) {
    Serial.println("Conectado.");
  } else {
    Serial.println("Fallo de conexión.");
    return;
  }

  if (!device.discoverService(UUID_SERVICIO)) {
    Serial.println("Servicio no encontrado");
    device.disconnect();
    return;
  }

  for (int i = 0; i < 8; i++) {
    ch[i] = device.characteristic(UUID_CH[i]);
    if (!ch[i]) {
      Serial.print("Característica ");
      Serial.print(i);
      Serial.println(" no encontrada.");
      device.disconnect();
      return;
    }

    if (ch[i].canSubscribe()) {
      ch[i].subscribe();
    }
  }

  Serial.println("Suscrito. Recibiendo datos...");
  while (device.connected()) 
  {
    for (int i = 0; i < 8; i++) 
    {
     if (ch[i].valueUpdated()) 
     {
        uint8_t buffer[4];  // float = 4 bytes
        ch[i].readValue(buffer, 4);

        float val;
        memcpy(&val, buffer, sizeof(float));  // interpretar los bytes como float
        Serial.print(val);
        Serial.print(i < 5 ? "," : "\n");
      }
    }
  }


  Serial.println("Desconectado.");
}

