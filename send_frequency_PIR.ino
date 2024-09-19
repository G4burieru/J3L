#include <IRremote.hpp>
#include <Arduino.h>

#define PIR_L 26
#define PIR_R 25
#define IR_SEND_PIN 17
// POWER OFF = 0x88, 0xC005, 10
// POWER ON = 0x88, 0xA5, 10

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
