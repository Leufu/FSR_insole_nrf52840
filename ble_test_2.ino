// #include <Adafruit_TinyUSB.h>
#include <bluefruit.h>

BLEUart bleuart;

void setup() {
  Serial.begin(115200);
  delay(1000);
  // while (!Serial) delay(10);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setName("nRF52840_BromoOro");

  bleuart.begin();
  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.ScanResponse.addName();

  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start();

  Serial.println("BLE UART iniciado. Esperando conexión...");
}

void loop() {
  if (bleuart.available()) {
    String input = bleuart.readStringUntil('\n');
    input.trim();  // elimina espacios o saltos

    Serial.print("Recibido: ");
    Serial.println(input);

    if (input.equalsIgnoreCase("R")) {
      digitalWrite(LED_RED, LOW);  // LED azul se enciende (inverso)
      bleuart.println("ROJO ENCENDIDO");
    } else if (input.equalsIgnoreCase("B")) {
      digitalWrite(LED_BLUE, LOW); // LED azul se apaga
      bleuart.println("AZUL ENCENDIDO");
    } else if (input.equalsIgnoreCase("G")) {
      digitalWrite(LED_GREEN, LOW); // LED azul se apaga
      bleuart.println("VERDE ENCENDIDO");
    } else if (input.equalsIgnoreCase("OFF")) {
      digitalWrite(LED_RED, HIGH); // LED azul se apaga
      digitalWrite(LED_BLUE, HIGH); // LED azul se apaga
      digitalWrite(LED_GREEN, HIGH); // LED azul se apaga
      bleuart.println("APAGADOS");
    } else {
      bleuart.println("Comando no reconocido");
    }
  }

  // Puedes seguir enviando mensajes cada cierto tiempo si deseas
}
