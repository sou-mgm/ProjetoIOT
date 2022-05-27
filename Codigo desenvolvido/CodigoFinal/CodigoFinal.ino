// Mackenzie - IoT
// Matheus Gaudencio Matias
// TIA 20016621


// Bibliotecas e definições.
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <PubSubClient.h>


Adafruit_BMP280 bmp;

//Declarando a rede
const char* ssid = "ADIVINHA";  // Enter SSID here
const char* password = "tecohot09";  //Enter Password here
const char* mqttServer = "driver.cloudmqtt.com"; 
const int mqttPort = 18707; 
const char* mqttUser = "gsqmoxwf"; 
const char* mqttPassword = "g8E1Vk2BLdVU"; 
WiFiClient espClient; 
PubSubClient client(espClient);

//Declaração de variaveis
float temperature;
int pressure;
String split;


 void setup() { 
  // Declara Pino do rele
  pinMode(16, OUTPUT);
  // Inicializa sensor
  bmp.begin(0x76);
  //Inicializa porta serial
  Serial.begin(115200);
  
  // Cria conexão IP
   WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) 
  {   
     delay(100);
    Serial.println(".");
  }
  Serial.println("Conectado!"); 
  client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Conectando ao servidor MQTT...");
    
    if (client.connect("Projeto", mqttUser, mqttPassword ))
    {
 
      Serial.println("Conectado ao servidor MQTT!");  
 
    } else {
 
      Serial.print("Falha ao conectar ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("Placa","Em funcionamento!");
  client.subscribe("temperatura"); 
}


// Recebendo a leitura do sensor.
void bmp_OnConnect() {
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure() / 100.0F;

}

 
// Converte para texto os valores lidos e 
void conversaoTEMP(){

  String temperaturaMSG;
  temperaturaMSG = String (temperature);
  String temperaturaMSG_CEL = temperaturaMSG + "°C";
  char messageTEMP[58];
  temperaturaMSG_CEL.toCharArray(messageTEMP,58);
  client.publish("Temperatura", messageTEMP);
 }

void conversaoPRESSAO(){

  String pressaoMSG;
  pressaoMSG = String (pressure);
  String pressaoMSG_CEL = pressaoMSG + "hpa";
  char messagePRESSAO[58];
  pressaoMSG_CEL.toCharArray(messagePRESSAO,58);
  client.publish("Pressao", messagePRESSAO);
 }

//envia para o Broker
void enviaMQTT(){
 conversaoTEMP();
 conversaoPRESSAO();
 delay(6000);

}
 
  
void rele(){
   // Se a temperatura do CPD estiver acima do setpoint , aciona o Split. Se não, o split fica desligado. 
  
  
  if (temperature >= 26){
  delay(10);
  digitalWrite(16, HIGH); 
  split = "Ligado";
  Serial.print(split);
  char messageRELE[58];
  split.toCharArray(messageRELE,58);
  client.publish ("Split",messageRELE);
  client.publish ("Estado","Crítico");
  delay(100);
  }
  
  if (temperature <=25.50){  
  delay(10);   
  digitalWrite(16, LOW);
  split = "Desligado"; 
  Serial.print(split);
  char messageRELE[58];
  split.toCharArray(messageRELE,58);
  client.publish ("Split",messageRELE);
  client.publish ("Estado","Ideal");
  delay(100);
    
  }
 }

void loop() {
  bmp_OnConnect();
  enviaMQTT();
  rele(); 
  delay(100);
  client.loop();
  }
