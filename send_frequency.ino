#include <IRremote.hpp>
#include <Arduino.h>

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

