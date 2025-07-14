#include <SPI.h>
#include <ArduinoBLE.h> 
#include<float.h>
//SN: 02EC 23-2A-1C-7F-77-DE

// UUid for Service
const char* UUID_serv = "84582cd0-3df0-4e73-9496-29010d7445dd";
// UUids characteristics
const char* UUID_CH0   = "84582cd1-3df0-4e73-9496-29010d7445dd"; //adc1(0..1024)- AX
const char* UUID_CH1   = "84582cd2-3df0-4e73-9496-29010d7445dd";// voltage1() - AY
const char* UUID_CH2   = "84582cd3-3df0-4e73-9496-29010d7445dd";//adc2(0..1204)-AZ
const char* UUID_CH3   = "84582cd4-3df0-4e73-9496-29010d7445dd";//voltage2-GX
const char* UUID_CH4   = "84582cd5-3df0-4e73-9496-29010d7445dd";//vBat(0-3.3)-GY
const char* UUID_CH5   = "84582cd6-3df0-4e73-9496-29010d7445dd";//vBat(0-3.3) GZ BATTERY
const char* UUID_CH6  =  "84582cd7-3df0-4e73-9496-29010d7445dd";//triangularWave(300..400)-chaBAP
const char* UUID_CH7   = "84582cd8-3df0-4e73-9496-29010d7445dd";//chaBA

// BLE Service
BLEService myService(UUID_serv); 

// BLE Characteristics
BLEFloatCharacteristic  ch00(UUID_CH0,  BLERead|BLENotify);
BLEFloatCharacteristic  ch01(UUID_CH1,  BLERead|BLENotify);
BLEFloatCharacteristic  ch02(UUID_CH2,  BLERead|BLENotify);
BLEFloatCharacteristic  ch03(UUID_CH3,  BLERead|BLENotify);
BLEFloatCharacteristic  ch04(UUID_CH4,  BLERead|BLENotify);
BLEFloatCharacteristic  ch05(UUID_CH5,  BLERead|BLENotify);
BLEFloatCharacteristic  ch06(UUID_CH6,  BLERead|BLENotify);
BLEFloatCharacteristic  ch07(UUID_CH7,  BLERead|BLENotify);

static long preMillis = 0, curMillis=0;
float adc0=0,adc1=0,adc2=0,adc3=0,adc4=0,adc5=0;
void setup(){
  Serial.begin(115200); // Open serial connection to report values to host
  delay(1000);
  curMillis = millis();
  while(millis() < curMillis + 300){}
  if (!BLE.begin()) 
    {
    Serial.println("BLE: failed");
    }
  Serial.println("BLE: ok");
  curMillis = millis();
  while(millis() < curMillis + 100){}

  BLE.setLocalName("FSR_Sens");
  BLE.setDeviceName("Arduino"); // Arduino is the default value on this module
  
  // Set advertised Service
  BLE.setAdvertisedService(myService);
  
  // Add characteristics to the Service
  myService.addCharacteristic(ch00);  myService.addCharacteristic(ch01);
  myService.addCharacteristic(ch02);  myService.addCharacteristic(ch03);
  myService.addCharacteristic(ch04);  myService.addCharacteristic(ch05);
  myService.addCharacteristic(ch06);  myService.addCharacteristic(ch07);
  
  // add service to BLE
  BLE.addService(myService);
  
  // characteristics initial values
  ch00.writeValue(0);  ch01.writeValue(0);
  ch02.writeValue(0);  ch03.writeValue(0);
  ch04.writeValue(0);  ch05.writeValue(0);
  ch06.writeValue(0);  ch07.writeValue(1);
 
  // start advertising
  BLE.advertise();
  Serial.println("Central loop scanning");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  preMillis = millis();
}

void loop()
{
  BLEDevice central = BLE.central();
  //Serial.println(central.address());

  if (central) 
  {
   Serial.print("Connected to central MAC: ");
   Serial.println(central.address());
    preMillis = millis();
    while (central.connected()) 
    {
      curMillis = millis();
      if (preMillis>curMillis) preMillis=0;
      if ( millis() - preMillis > 100) // check values every 100mS
        {
          preMillis = curMillis;
            adc0 = analogRead(0);adc0 = analogRead(0);adc0 = analogRead(0);
            curMillis = millis();
            while(millis() < curMillis + 2){}
            adc1 = analogRead(1);adc1 = analogRead(1);adc1 = analogRead(1);
            curMillis = millis();
            while(millis() < curMillis + 2){}
            adc2 = analogRead(2);adc2 = analogRead(2);adc2 = analogRead(2);
            curMillis = millis();
            while(millis() < curMillis + 2){}
            adc3 = analogRead(3);adc3 = analogRead(3);adc3 = analogRead(3);
            curMillis = millis();
            while(millis() < curMillis + 2){}
            adc4 = analogRead(4);adc4 = analogRead(4);adc4 = analogRead(4);
            curMillis = millis();
            while(millis() < curMillis + 2){}
            adc5 = analogRead(5);adc5 = analogRead(5);adc5 = analogRead(5);
            
            ch00.writeValue(adc0);
            ch01.writeValue(adc1);
            ch02.writeValue(adc2);
            ch03.writeValue(adc3);
            ch04.writeValue(adc4);
            ch05.writeValue(adc5);
            
   Serial.print(adc0);Serial.print(",");
   Serial.print(adc1);Serial.print(",");
   Serial.print(adc2);Serial.print(",");
   Serial.print(adc3);Serial.print(",");
   Serial.print(adc4);Serial.print(",");
   Serial.println(adc5);
        }
    }
  }
}

