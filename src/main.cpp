#include "DHT.h"
#include "DHT_U.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//**************************************
//*********** DHT 22 *******************
//**************************************
#define DHTPIN 4     // Digital pin connected to the DHT sensor (18650 Board is pin D2)
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

//**************************************
//*********** WIFI CONFIG **************
//**************************************
const char* ssid     = "YOUR WIFI SSID";
const char* password = "YOUR WIFI PASS";
IPAddress ip(192, 168, 1, 70);
IPAddress dns(1, 1, 1, 1);
IPAddress gateway(192, 168, 1, 1); 
IPAddress subnet(255, 255, 255, 0);

//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char* mqttServer = "YOUR MQTT BROKER ADDRESS";
const char* clientID = "GIVE YOUR CLIENT A NAME";
const int mqttPort = 1883;
#define humidity_topic "humedad/salon"
#define temperature_topic "temperatura/salon"

//**************************************
//************ GLOBALES ****************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);
#define DEBUG 0     // Activar debug ==1 Desactivar Debug==0

//**************************************
//************ FUNCIONES ***************
//**************************************

// Connecting to a WiFi network
void WiFiConnect (){
  if (DEBUG==1){
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);
  }  
  WiFi.config( ip, dns, gateway, subnet );
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) { // and the executes the WiFi.status() function to try to connect to WiFi Network. The program checks whether the WiFi.status() function returns a true value when its connect.
    delay(200);
    if (DEBUG==1){
      Serial.print(".");
    }
  }

if (DEBUG==1){
  Serial.println("");
  Serial.println("WiFi conectado!!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}
}

void MQTTConnect (){
  client.setServer(mqttServer, mqttPort); //Conecto a servidor MQTT
  client.connect(clientID);
  if (DEBUG==1){
    if (client.connected()) {
      Serial.println("Conectado a servidor MQTT");  
    }
  }
}

void setup() {
  if (DEBUG==1){
    Serial.begin(9600);
  }
  pinMode(5, OUTPUT); // GPIO5 VCC for sensor
  digitalWrite(5, HIGH); //Power the sensor
  dht.begin(); //Inicio sensor DHT
  WiFiConnect(); //Conecto WiFi
  MQTTConnect(); //Conecto MQTT
  delay(600);
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  digitalWrite(5, LOW); //Remove power to sensor
  if (DEBUG==1){
    Serial.print(F("Humedad: "));
    Serial.print(humidity);
    Serial.print(F("%  Temperatura: "));
    Serial.print(temperature);
    Serial.println(F("°C "));
  }
// Check if any reads failed and exit early (to try again).
//  if (isnan(humidity) || isnan(temperature)) {
//    Serial.println(F("Fallo al leer el Sensor DHT!"));
//    return;
//  }

  client.publish(temperature_topic, String(temperature).c_str(), true);// Publish temperature on temperature_topic
  client.publish(humidity_topic, String(humidity).c_str(), true);      // and humidity

  delay(500);
  ESP.deepSleep(5000000);
  delay(200);
}
