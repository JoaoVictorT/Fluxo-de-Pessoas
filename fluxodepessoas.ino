#if defined(ESP8266)
#include <ESP8266WiFi.h>         
#else
#include <WiFi.h>      
#endif

#include <DNSServer.h> 
#if defined(ESP8266)
#include <ESP8266WebServer.h> 
#else
#include <WebServer.h> 
#endif
#include <WiFiManager.h>   
#include <PubSubClient.h>
#include <EEPROM.h>
#define pinSensorFora 32
#define pinSensorDentro 33
#define ESPERA 0
#define ENTRANDO 1
#define CONFIRMAENTRADA 2
#define SAINDO 3
#define CONFIRMASAIDA 4
int modo= ESPERA;
unsigned long millisSensorUltimo = 0;
int pessoas = 0;
String brokerUser = "";
String brokerPass = "";
String broker = "";
const char* outTopic ="sensorLdr";
WiFiClient espClient;
PubSubClient client(espClient);
bool shouldSaveConfig = false;

//pino do botão
const int PIN_AP = 2;

void reconnect() {
  while (!client.connected()) {
    Serial.print("\nConnecting to ");
    Serial.println(broker);
    Serial.println(brokerUser);
    Serial.println(brokerPass);
    client.setServer(broker.c_str(), 1883);
    if (client.connect("koikoikoi", brokerUser.c_str(), brokerPass.c_str())) {
      Serial.print("Connected to ");
      Serial.println(broker);
      
    } else {
      Serial.println("\nTrying connect again ");
      delay(5000);
      client.setServer(broker.c_str(), 1883);
      
    }
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(PIN_AP, INPUT);

  WiFiManager wifiManager;

  wifiManager.setAPCallback(configModeCallback); 
  wifiManager.setSaveConfigCallback(saveConfigCallback); 

  wifiManager.autoConnect("Pflux","pflux123"); 

brokerUser = wifiManager.getBrokerUser();
brokerPass = wifiManager.getBrokerPass();
broker = wifiManager.getBroker();




}

void loop() {
  WiFiManager wifiManager;
   if ( digitalRead(PIN_AP) == HIGH ) {
      Serial.println("resetar"); 
      if(!wifiManager.startConfigPortal("ESP_AP", "12345678") ){
        Serial.println("Falha ao conectar");
        delay(2000);
        ESP.restart();
        delay(1000);
      }
      Serial.println("Conectou ESP_AP!!!");
   }
     if (!client.connected()) {
    reconnect();
  }
  client.loop();
contagemDePessoas();
}

void configModeCallback (WiFiManager *myWiFiManager) {  
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); 
  Serial.println(myWiFiManager->getConfigPortalSSID()); 

}

void saveConfigCallback () {
  Serial.println("Configuração salva");
  Serial.println(WiFi.softAPIP()); 
}
void contagemDePessoas(){
  
int estadoSensorFora = analogRead(pinSensorFora);
int estadoSensorDentro = analogRead(pinSensorDentro);

if(modo==ESPERA){
if(estadoSensorFora > 4010){
  Serial.println("ESTADOSENSORFORA");
  millisSensorUltimo = millis();
  modo = ENTRANDO;

}
else if(estadoSensorDentro > 4010){
  Serial.println("ESTADOSENSORDENTRO");
    millisSensorUltimo = millis();
  modo= SAINDO;
}
}
if(modo==ENTRANDO){

    if(estadoSensorFora > 4010 && estadoSensorDentro > 4010){
    Serial.println("ENTROU");
    modo= CONFIRMAENTRADA;
    
}
else if(millis() >= millisSensorUltimo+3000){
  Serial.println("passou 3000s");
  millisSensorUltimo = millis();
  delay(500);
  modo = ESPERA;
}


}
if(modo== CONFIRMAENTRADA){
       if(estadoSensorFora < 4010){
      Serial.println("Confirma Entrada");
      
     pessoas=pessoas+1;
    Serial.println(pessoas);
   String jsonSensor = "{\"Entrada\": 1, \"Saida\": 0,\"TotaldePessoas\": "+String(pessoas)+"}";
   char charBuffer1[jsonSensor.length() + 1];
   jsonSensor.toCharArray(charBuffer1, jsonSensor.length() + 1);
   client.publish(outTopic, charBuffer1);
  
   // client.publish(outTopic, String(pessoas).c_str());
    delay(500);
    modo= ESPERA;
       }
     
     
}

if(modo==SAINDO){
  if(estadoSensorDentro > 4010 && estadoSensorFora > 4010){
    Serial.println("SAINDO"); 
    modo = CONFIRMASAIDA;
  }
  else if(millis() >= millisSensorUltimo+3000){
  Serial.println("passou 3000s");
  millisSensorUltimo = millis();
  delay(500);
  modo = ESPERA;
}



  }

if(modo==CONFIRMASAIDA){
  if(estadoSensorDentro < 4010){
    Serial.println("CONFIRMASAIDA");
    pessoas = pessoas-1;
    if(pessoas <= 0){
      pessoas = 0;
    }
    Serial.println(pessoas);
  String jsonSensor = "{\"Entrada\": 0, \"Saida\": 1,\"TotaldePessoas\": "+String(pessoas)+"}";
  
  char charBuffer1[jsonSensor.length() + 1];
  jsonSensor.toCharArray(charBuffer1, jsonSensor.length() + 1);
  client.publish(outTopic, charBuffer1);
  
    //client.publish(outTopic, String(pessoas).c_str());
    
    delay(500);
    modo=ESPERA;
  }
}
Serial.print(estadoSensorFora);
Serial.print("\t");
Serial.println(estadoSensorDentro);
delay(100);
} 