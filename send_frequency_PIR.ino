#include <IRremote.hpp>
#include <Arduino.h>

#define PIR_L 26
#define PIR_R 25
#define IR_SEND_PIN 17
// POWER OFF = 0x88, 0xC005, 10
// POWER ON = 0x88, 0xA5, 10


// TEMPERATURAS
// 16 = 0x88, 0x815, 10
// 17 = 0x88, 0x825, 10
// 18 = 0x88, 0x835, 10
// 19 = 0x88, 0x845, 10
// 20 = 0x88, 0x855, 10
// 21 = 0x88, 0x865, 10
// 22 = 0x88, 0x875, 10
// 23 = 0x88, 0x885, 10
// 24 = 0x88, 0x895, 10
// 25 = 0x88, 0x8A5, 10
// 26 = 0x88, 0x8B5, 10
// 27 = 0x88, 0x8C5, 10
// 28 = 0x88, 0x8D5, 10
// 29 = 0x88, 0x8E5, 10
// 30 = 0x88, 0x8F5, 10


unsigned long inactiveUntil = 0;
bool acOn = false;

void turnOn() {
  if (!acOn) {
    IrSender.sendLG(0x88, 0xA5, 10);
    Serial.println("Motion detected!");
    Serial.println("Turning on the air conditioning.");  
    acOn = true;
  }
}

void turnOff() {
  if (acOn) {
    IrSender.sendLG(0x88, 0xC005, 10);
    Serial.println("No motion detected!");
    Serial.println("Turning off the air conditioning.");  
    acOn = false;
  }
}

void setup() {
  pinMode(PIR_L, INPUT);
  pinMode(PIR_R, INPUT);

  Serial.begin(115200);
  IrSender.begin(IR_SEND_PIN);
  Serial.println("Ready to send IR signal");
}

void loop() {
  unsigned long currentTime = millis();

  //
  if (currentTime < inactiveUntil) {
    Serial.println("Sensor inactive. Waiting for 5 minutes period to end.");
    delay(1000); 
    return;
  }
  
  int pirStateL = digitalRead(PIR_L);
  int pirStateR = digitalRead(PIR_R);

  if (pirStateL == HIGH || pirStateR == HIGH) {
    turnOn(); 
    inactiveUntil = currentTime + 300000;
    return;
  }


  Serial.println("Monitoring for 30 seconds to check for motion.");
  unsigned long startTime = millis();
  bool movementDetected = false;

  while (millis() - startTime < 30000) {
    pirStateL = digitalRead(PIR_L);
    pirStateR = digitalRead(PIR_R);

    if (pirStateL == HIGH || pirStateR == HIGH) {
      movementDetected = true;
      break;
    }
  }

  if (!movementDetected) {
    turnOff();  
  } else {
    Serial.println("Motion detected! Air conditioning remains on.");
  }
}
