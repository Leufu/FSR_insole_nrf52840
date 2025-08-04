//#include <bluefruit.h>
//#include "Adafruit_SPIFlash.h"
//#include <Adafruit_TinyUSB.h>
//BLEUart bleuart;  // UART sobre BLE

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("BLE UART ready");
}

void loop() {
  static uint32_t last_ms = 0;
  if (millis() - last_ms > 100) {
    last_ms = millis();

    int t = millis();

    // Lectura y binarización
    int fsr0 = (analogRead(A0) > 4000) ? 1 : 0;
    int fsr1 = (analogRead(A1) > 4000) ? 1 : 0;
    int fsr2 = (analogRead(A2) > 4000) ? 1 : 0;
    int fsr3 = (analogRead(A3) > 4000) ? 1 : 0;

    char msg[64];
    snprintf(msg, sizeof(msg), "%d,%d,%d,%d,%d\n", t, fsr0, fsr1, fsr2, fsr3);

    // Envío por serial
    Serial.print("Enviado: ");
    Serial.print(msg);
  }
}

