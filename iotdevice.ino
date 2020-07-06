/**
 * -----------------------------------------------------------
 * ESP8266 MCU routing for DHT11 reading and MQTT writing
 * -----------------------------------------------------------
*/
  
// SDHT.h - Class for DHT11, DHT12, DHT21, DHT22 sensors
// https://github.com/offcircuit/SDHT
#include <SDHT.h>
// PubSubClient.h - Arduino Client for MQTT
// https://pubsubclient.knolleary.net/
#include <PubSubClient.h>
// ESP8266WiFi.h - esp8266 Wifi support
#include <ESP8266WiFi.h>
 
// MQTT Server IP--thus the Raspberry Pi Zero W IP
const char* IOTGATEWAY = "nn.nn.nn.nn"; 
// Local WiFi SSID
const char* WIFISSID = "********"; 
// Local WiFi password
const char* WIFIPASS = "********"; 
// "/house/temperature" topic used to write 
// the sensor temperature value
const char* TOPICTEMP = "/casa/temperatura"; 
// "/house/humidity" topic used to write 
// the sensor humidity value
const char* TOPICHUMI = "/casa/humedad"; 
// "/house/humidifier" topic used to read 
// the humidity actuator status value
const char* TOPICCTRL = "/casa/humidificador"; 
// Test topic
const char* TOPICTEST = "hello_topic"; 
  
// NodeMCU ESP12 Dev Kit V1.0 Pin Definition
// https://iotbytes.wordpress.com/nodemcu-pinout/
// Sensor input signal pin
#define DHTPIN 14     
// Red LED output signal pin
#define LEDPIN 13     
// DHT11 sensor internal code
// https://www.instructables.com/id/DHT11-Humidity-Sensor-Module-Interface-With-Arduin/
#define DHTTYPE DHT11  
// Data report interval in seconds
#define REPORT_INTERVAL 1
// BPS baud rate for communication with ESP12E 
#define SERIALBPS 9600 
  
SDHT dht;
String clientName;
WiFiClient wifiClient;
char message_buff[100];
  
// Callback function to check actuator status value 
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print(">> Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    String message;
    for (int i = 0; i < length; i++) {
        //Converts *byte into String
        message = message + (char)payload[i];  
    }
    Serial.print(message);
    //LED turned on
    if (message == "on") digitalWrite(LEDPIN,HIGH); 
    //LED turned off
    if (message == "off") digitalWrite(LEDPIN,LOW);  
    Serial.println();
    Serial.println("-----------------------"); 
}
  
PubSubClient client(IOTGATEWAY, 1883, callback, wifiClient);
  
void setup() {
    Serial.begin(SERIALBPS);
    pinMode(LEDPIN,OUTPUT);
    Serial.println("DHT Initialization");
    delay(1000);
    Serial.println();
    Serial.println();
    Serial.print("Connected to ");
    Serial.println(WIFISSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFISSID, WIFIPASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP Address: ");
    Serial.println(WiFi.localIP());
    clientName += "esp8266-";
    uint8_t mac[6];
    WiFi.macAddress(mac);
    clientName += macToStr(mac);
    clientName += "-";
    clientName += String(micros() & 0xff, 16);
    Serial.print("Connected to ");
    Serial.print(IOTGATEWAY);
    Serial.print(" as ");
    Serial.println(clientName);
    digitalWrite(LEDPIN, LOW); // Inits LED to off
    if (client.connect((char*) clientName.c_str())) {
        Serial.println("Connected to MQTT Broker");
        Serial.print("Test topic is: ");
        Serial.println(TOPICTEST);
        if (client.publish(TOPICTEST, "Hello Publish from ESP8266")) {
            Serial.println("Published OK");
            if (client.publish(TOPICCTRL, "off")) {
                Serial.println("Initializing Control topic to off");
            } else {
                Serial.println("Failed to initialize control topic");
            }
            client.subscribe(TOPICCTRL);
            Serial.print("Subscribed to ");
            Serial.println(TOPICCTRL);
        } else {
            Serial.println("Publish failed");
        }
    } else {
        Serial.println("Failed connection to MQTT");
        Serial.println("Resetting and testing again...");
        abort();
    }
}
 
void loop() {
    client.loop();
    if (dht.read(DHTTYPE, DHTPIN)) layout();
    double temp=dht.celsius;
    double humi=dht.humidity;
 
    String payloadH = String(humi/10,1);
    String payloadT = String(temp/10,1);
 
    sendTemperature(payloadH,payloadT);
    int cnt = REPORT_INTERVAL;
    while (cnt--) delay(1000);
}
  
void layout() {
    Serial.print("   Celsius => ");
    Serial.println(String(double(dht.celsius) / 10, 1));
    Serial.print("   Humdity => ");
    Serial.println(String(double(dht.humidity) / 10, 1));
}
  
void sendTemperature(String plHum, String plTem) {
    if (!client.connected()) {
        if (client.connect((char*) clientName.c_str())) {
            Serial.println("Connected again to MQTT Broker");
            Serial.print("Topics are: ");
            Serial.print(TOPICTEMP);
            Serial.print(" and ");
            Serial.println(TOPICHUMI);
        } else {
            Serial.println("Connection to MQTT failed");
            Serial.println("Resetting and testing again...");
            abort();
        }
    }
    if (client.connected()) {
        Serial.print("<< Sent reading to server: Humidity ");
        Serial.println(plHum);
        if (client.publish(TOPICHUMI, (char*) plHum.c_str())) {
            Serial.println("Humidity Published OK");
        } else {
            Serial.println("Failed to publish Humidity");
        }
        Serial.print("<< Sent reading to server: Temperature ");
        Serial.println(plTem);
        if (client.publish(TOPICTEMP, (char*) plTem.c_str())) {
            Serial.println("Temperature Published OK");
        } else {
            Serial.println("Failed to publish Temperature");
        }
    }
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
