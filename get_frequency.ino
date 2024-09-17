#include <IRremote.hpp>

// Definindo os pinos para o receptor e emissor IR
const int IR_RECEIVE_PIN = 23;  // Pino do receptor IR

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
  Serial.println("Pronto para copiar o sinal IR");
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.println("Sinal IR recebido!");
    
    // Exibir o código recebido em hexadecimal
    Serial.print("Código: ");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
    IrReceiver.printIRSendUsage(&Serial);   // Print the statement required to send this data
    
    IrReceiver.resume();  // Recebe o próximo valor IR
  }
}

