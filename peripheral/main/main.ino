//#include <SPI.h>
#include <ArduinoBLE.h> 
#include<float.h>
//SN: 02EC 23-2A-1C-7F-77-DE
#include "nrf.h"

// need to add a process for sampling and another for sending. Maybe implement a buffer 
// timer Interrupt not working. have to add library https://github.com/khoih-prog/NRF52_TimerInterrupt
///////////////////////////////
typedef struct 
{
	int64_t time_stamp;
	float	  FSR0;
	float	  FSR1;
	float	  FSR2;
	float	  FSR3;
}FSR_data_t;
///////////////////////////
BLEService fsrService("84582cd0-3df0-4e73-9496-29010d7445dd");
BLECharacteristic fsrStringChar("84582cd9-3df0-4e73-9496-29010d7445dd", BLERead | BLENotify, 64);  // 64 bytes máx

char payload[64];
volatile bool sample_flag = false;

extern "C" void TIMER1_IRQHandler(void) {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    sample_flag = true;
  }
}

void setupTimer1(uint32_t frequency_hz) {
  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER1->PRESCALER = 4; // 1 MHz clock
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
  NRF_TIMER1->CC[0] = 1000000 / frequency_hz;
  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
  NVIC_EnableIRQ(TIMER1_IRQn);
  NRF_TIMER1->TASKS_START = 1;
}

void setup() {
  Serial.begin(115200);
  //while (!Serial);

  //pinMode(LED_BUILTIN,OUTPUT);
  //digitalWrite(LED_BUILTIN,LOW);

  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }

  BLE.setLocalName("FSR_Sens");
  BLE.setDeviceName("nRF52840");
  BLE.setAdvertisedService(fsrService);

  fsrService.addCharacteristic(fsrStringChar);
  BLE.addService(fsrService);

  fsrStringChar.writeValue("Ready");

  BLE.advertise();
  Serial.println("BLE advertising...");

  //setupTimer1(50); // 50 Hz por ejemplo
}

void loop() {
  BLEDevice central = BLE.central();

  //digitalWrite(LED_BUILTIN,LOW);
  if(central){
    Serial.print("Connected to: ");
    Serial.println(central.address());

    while(central.connected())
    {
      
      //if (sample_flag) {
      //  sample_flag = false;

        int64_t ts = millis();
        float f0 = analogRead(D7);
        float f1 = analogRead(D8);
        float f2 = analogRead(D9);
        float f3 = analogRead(D10);

        snprintf(payload, sizeof(payload), "%lld,%.2f,%.2f,%.2f,%.2f", ts, f0, f1, f2, f3);
        fsrStringChar.writeValue(payload);
        Serial.println(payload);
      //}
    }

    Serial.println("Disconnected.");
  }
}

