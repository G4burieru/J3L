#include <IRremote.hpp>
#include <Arduino.h>

//includes para wifi e firebase
#include <WiFi.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

#define WIFI_SSID "LAB_ROBOTICA_2.4G"  //tem que mudar para rede wifi utilizada
#define WIFI_PASSWORD "UAV1X500"       //senha da rede wifi utilizada
#define DATABASE_SECRET "l9er27WiS2wvYI33aVrv5DxBiZP4wLNU3Ie8ZJXR"     ///def. relacionada ao firebase
#define DATABASE_URL "https://embarcados-40131-default-rtdb.firebaseio.com/" //def. relacionada ao fb

#define PIR_L 26
#define PIR_R 25
#define IR_SEND_PIN_L 17
#define IR_SEND_PIN_R 17
#define ADDRESS 0x88
#define POWER_ON 1
#define POWER_OFF 2
#define TEMP_UP 3
#define TEMP_DOWN 4

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

//variaveis relacionadas à config. do firebase
WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));
FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
LegacyToken dbSecret(DATABASE_SECRET);
////////////////////////////////////////

//variaveis globais relacionadas ao codigo
bool acOn = false;
int temp = 23;
string lastCommand = "";
string command = "";
int commandIntPart;


void printError(int code, const String &msg)
{
    Firebase.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void sendTemperature(int temp) {
  if (temp >= 16 && temp <= 30) {
    int command = 0x815 + (temp - 16) * 0x10;
    IrSender.sendLG(ADDRESS, command, 20);
    IrSender.sendLG(ADDRESS, command, 20);
    Serial.print("Temperatura ajustada para: ");
    Serial.println(temp);
  } else {
    Serial.println("Temperatura fora do intervalo permitido (16-30).");
  }
}

void sendMessage(int function){
  switch (function) {
    case POWER_ON:
      if (!acOn) {
        IrSender.sendLG(ADDRESS, 0xA5, 20); // POWER ON
        IrSender.sendLG(ADDRESS, 0xA5, 20);
        acOn = true;
        sendTemperature(temp); // Ajusta a temperatura inicial
      }
      break;
    case POWER_OFF:
      if (acOn) {
        IrSender.sendLG(ADDRESS, 0xC005, 25); // POWER OFF
        IrSender.sendLG(ADDRESS, 0xC005, 25);
        acOn = false;
      }
      break;
    case TEMP_UP:
      if (acOn && temp < 30) {
        temp++;
        sendTemperature(temp);
      } else {
        Serial.println("Temperatura já está no máximo.");
      }
      break;
    case TEMP_DOWN:
      if (acOn && temp > 16) {
        temp--;
        sendTemperature(temp);
      } else {
        Serial.println("Temperatura já está no mínimo.");
      }
      break;
    default:
      Serial.println("Comando não encontrado!");
  }
}

void setup() {
  pinMode(PIR_L, INPUT);
  pinMode(PIR_R, INPUT);

  Serial.begin(115200);
  IrSender.begin(IR_SEND_PIN_L);
  IrSender.begin(IR_SEND_PIN_R);
  Serial.println("Ready to send IR signal");
  
  //config. wifi/firebase
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("Connecting to Wi-Fi");
	while (WiFi.status() != WL_CONNECTED){
		Serial.print(".");
		delay(300);
	}
	Serial.println();
	Serial.print("Connected with IP: ");
	Serial.println(WiFi.localIP());
	Serial.println();
	Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);
	ssl.setInsecure();
  initializeApp(client, app, getAuth(dbSecret));
  // Binding the authentication handler with your Database class object.
  app.getApp<RealtimeDatabase>(Database);
  // Set your database URL
  Database.url(DATABASE_URL);
  // In sync functions, we have to set the operating result for the client that works with the function.
  client.setAsyncResult(result);
  lastCommand = Database.get<String>(client, "/messages");
}

void loop() {

  //pegando qualquer comando que venha do site
  Serial.print("Get string... ");
  String command = Database.get<String>(client, "/messages");  // pega o ultimo valor do banco de dados nesse caminho
  
  if (client.lastError().code() == 0) {
    Serial.println(command);  // Imprime o valor da string
    
    if(command != lastCommand){
    	
    	char lastCaracter = command.charAt(command.length() - 1);
    	int commandIntPart = atoi(lastCaracter);
    	if(commandIntPart == 1 or commandIntPart == 2 or commandIntPart == 3 or commandIntPart == 4){
    		sendMessage(commandIntPart);
    		lastCommand = command;
    	}
    	
    }else{
    	continue;        //se o comando for o mesmo do anterior, ja deve ter sido executado anteriormente entao nao fazemos nd;
    }
  }else {
    printError(client.lastError().code(), client.lastError().message()); 
  }
  /////////////////////////////////////////////////////

  int pirStateL = digitalRead(PIR_L);
  int pirStateR = digitalRead(PIR_R);
  

  if (pirStateL == HIGH || pirStateR == HIGH) {
    sendMessage(POWER_ON);
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
    sendMessage(POWER_OFF);  
  } else {
    Serial.println("Motion detected! Air conditioning remains on.");
  }
}
