// Rutina MCU para ESP8266, lectura DHT22 y escritura con MQTT

#include "DHT.h" // Bibliotecas con funciones de acceso al sensor DHT22
#include <PubSubClient.h> // Bibliotecas con funciones que implementan MQTT
#include <ESP8266WiFi.h> // Bibiliotecas con funciones de conexión a WiFi

const char* ssid = "********"; // En la implementación, el nombre de la WiFi
const char* password = "********"; // En la implementación, el password de la WiFi

char* topicTemp = "/casa/temperatura"; // Tópico en el que va a escribir la temperatura 
char* topicHum = "/casa/humedad"; // Tópico en el que va a escribir la temperatura 
char* server = "XX.XX.XX.XX"; // Server MQTT--aquí va la IP del Raspberry Pi Zero W
char* hellotopic = "hello_topic"; //Tópico de Prueba 

#define DHTPIN 17     // En cuál PIN está conectado 
#define DHTTYPE DHT22   // DHT 22  (AM2302); código interno de DHT
#define REPORT_INTERVAL 30 // reporta la temperatura cada 30 segundos

String clientName;
DHT dht(DHTPIN, DHTTYPE, 15);
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

float oldH ;
float oldT ;

void setup() {
  Serial.begin(38400);
  Serial.println("Incialización DHT");
  delay(20);

  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

  clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Conectando a ");
  Serial.print(server);
  Serial.print(" como ");
  Serial.println(clientName);

  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Conectado a Broker MQTT");
    Serial.print("Los tópicos son: ");
    Serial.print(topicTemp);
    Serial.print(" y ");
    Serial.println(topicHum);
    
    if (client.publish(hellotopic, "hello from ESP8266")) {
      Serial.println("Publicado OK");
    }
    else {
      Serial.println("Falló Publicación");
    }
  }
  else {
    Serial.println("Falló conexión a MQTT");
    Serial.println("Se resetea y prueba de nuevo...");
    abort();
  }

  dht.begin();
  oldH = -1;
  oldT = -1;
}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("¡Falla al leer del sensor DHT!");
    return;
  }

  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C ");

  String payloadH = h;
  String payloadT = t;
 
  if (t != oldT || h != oldH )
  {
    sendTemperature(payloadH,payloadT);
    oldT = t;
    oldH = h;
  }

  int cnt = REPORT_INTERVAL;

  while (cnt--)
    delay(1000);
}

void sendTemperature(String plHum, String plTem) {
  if (!client.connected()) {
    if (client.connect((char*) clientName.c_str())) {
      Serial.println("Conectado nuevamente a Broker MQTT");
      Serial.print("Los tópicos son: ");
      Serial.print(topicTemp);
      Serial.print(" y ");
      Serial.println(topicHum);
    }
    else {
      Serial.println("Falló conexión a MQTT");
      Serial.println("Se resetea y prueba de nuevo...");
      abort();
    }
  }

  if (client.connected()) {
    Serial.print("Enviando lectura al server: Humedad ");
    Serial.println(plHum);

    if (client.publish(topicHum, (char*) plHum.c_str())) {
      Serial.println("Publicada Humedad OK");
    }
    else {
      Serial.println("Falló publicación Humedad");
    }
 
    Serial.print("Enviando lectura al server: Temperatura ");
    Serial.println(plTem);

    if (client.publish(topicTemp, (char*) plTem.c_str())) {
      Serial.println("Publicada Temperatura OK");
    }
    else {
      Serial.println("Falló publicación Temperatura");
    }
 
  }

}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}