
void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("BLE Fake data Device");
  
  // Inicializa generador aleatorio
  randomSeed(analogRead(0)); // Puedes cambiar el pin si no usas el A0
}

void loop() {
  static uint32_t last_ms = 0;
  if (millis() - last_ms > 100) {
    last_ms = millis();

    int t = millis();
    float fsr0 = random(2); // devuelve 0 o 1
    float fsr1 = random(2);
    float fsr2 = random(2);
    float fsr3 = random(2);

    char msg[64];
    snprintf(msg, sizeof(msg), "%d,%.1f,%.1f,%.1f,%.1f\n", t, fsr0, fsr1, fsr2, fsr3);
    
    Serial.print(msg); // sin prefijo para facilitar lectura
  }
}

