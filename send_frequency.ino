#include <IRremote.hpp>
#include <Arduino.h>

#define IR_SEND_PIN 17
// POWER OFF = 0x88, 0xC005, 10
// POWER ON = 0x88, 0xA5, 10

void setup() {
  Serial.begin(115200);
  IrSender.begin(IR_SEND_PIN);
  Serial.println("Pronto para enviar o sinal IR");
}

void loop() {
  Serial.println("Enviando sinal de desligamento");
  IrSender.sendLG(0x88, 0xC005, 10);
  delay(5000);
}

