#include <ArduinoBLE.h>

// UUID del servicio UART Nordic
const char* UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
// UUID de la característica TX (de periférico a central)
const char* UART_TX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

BLEDevice peripheral;
BLECharacteristic txCharacteristic;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Fallo al iniciar BLE");
    while (1);
  }

  Serial.println("Escaneando periféricos BLE...");
  BLE.scanForName("FSR_Sens");  // Usa el nombre que configuraste en el periférico
}

void loop() {
  if (!peripheral) {
    peripheral = BLE.available();

    if (peripheral && peripheral.localName() == "FSR_Sens") {
      BLE.stopScan();
      connectToPeripheral(peripheral);
    }
  } else if (!peripheral.connected()) {
    Serial.println("Desconectado. Escaneando nuevamente...");
    peripheral = BLEDevice();
    BLE.scanForName("FSR_Sens");
  }
}

void connectToPeripheral(BLEDevice device) {
  Serial.print("Conectando a ");
  Serial.println(device.localName());

  if (!device.connect()) {
    Serial.println("Fallo de conexión");
    return;
  }

  Serial.println("Conectado. Descubriendo servicios...");
  if (!device.discoverService(UART_SERVICE_UUID)) {
    Serial.println("Servicio UART no encontrado");
    device.disconnect();
    return;
  }

  txCharacteristic = device.characteristic(UART_TX_UUID);
  if (!txCharacteristic) {
    Serial.println("Característica TX no encontrada");
    device.disconnect();
    return;
  }

  if (txCharacteristic.canSubscribe()) {
    txCharacteristic.subscribe();
    Serial.println("Suscrito a TX. Esperando datos...");
  } else {
    Serial.println("TX no soporta notificaciones");
    device.disconnect();
    return;
  }

  while (device.connected()) {
    if (txCharacteristic.valueUpdated()) {
      char buffer[64] = {0};
      txCharacteristic.readValue(buffer, sizeof(buffer));
      Serial.println(buffer);  // imprime la cadena completa enviada por el periférico
    }
  }

  Serial.println("Desconectado del periférico.");
}

