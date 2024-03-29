/*
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */
//#define ESP32 
// Uncomment the following line to enable serial debug output
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
       #include <WiFi.h>
#endif

//#include <Arduino.h>

#include "SinricPro.h"
#include "SinricProLight.h"
#include <PubSubClient.h>

#define SERVER          "test.mosquitto.org"
#define PORT            1883

#define RED_LED         22
#define GREEN_LED       23

#define WIFI_SSID         "TNI_ROBOT_WIFI"    
#define WIFI_PASS         "tnieng406"
#define APP_KEY_1           "e78da579-8001-4b78-95a9-d4fcfa779b64"      
#define APP_SECRET_1        "97862b77-3915-42d7-89bd-68d12931e1b7-3dc233e9-ef03-4511-a3fb-d220b7c45bf0"   
#define K_ID                "60cccb918cf8a303b93a07b8"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define L_ID                "60cc9aaf2a231603cf26dca3"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define D_ID                "60cc9e4a2a231603cf26dcc7"

#define BAUD_RATE         9600                // Change baudrate to your need

//#define BAUD_RATE         115200                // Change baudrate to your need

#define L_LED       5
#define D_LED       27
#define K_LED       26


WiFiClient espClient;
PubSubClient client(espClient);

String name_set[3] = {"Teeraphat Kullanankanjana\n",
                  "Thanaporn Sanamontre\n",
                  "Chayakotchamon Boonsanong\n"};

void reconnectmqttserver() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("TNI-Robot/ntt");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String MQTT_DATA = "";
  int led;
  for (int i=0;i<length;i++) {
   MQTT_DATA += (char)payload[i];}
   Serial.println(MQTT_DATA);
  if(check(MQTT_DATA) == 0)
  {
    digitalWrite(RED_LED,LOW);
    digitalWrite(GREEN_LED,HIGH);
  }
  delay(5000);
  digitalWrite(GREEN_LED,LOW);
  digitalWrite(RED_LED,HIGH);
}

int check(String name)
{
  int rc = 1;
  for(int i = 0; i < 3; i++)
    {
      if(name == name_set[i])
      rc = 0;   
    }
    return rc;  
}

bool onPowerState(const String &deviceId, bool &state) {
    if(deviceId==K_ID){
//       Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
      if(state){
        digitalWrite(K_LED,HIGH);
        Serial.println("RED LIGHT TURNED ON");
      }
      else{
        digitalWrite(K_LED,LOW);
        }
    }
    else if(deviceId==L_ID){
//       Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
      if(state){
        digitalWrite(L_LED,HIGH);
      }
      else{
        digitalWrite(L_LED,LOW);
        }
    }
      else if(deviceId==D_ID){
//       Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
      if(state){
        digitalWrite(D_LED,HIGH);
      }
      else{
        digitalWrite(D_LED,LOW);
        }
    }
  return true; // request handled properly
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
//  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
  Serial.println("connected!");
  Serial.print("[WiFi]: IP-Address is ");
  Serial.print(localIP[0]);
  Serial.print(".");
  Serial.print(localIP[1]);
  Serial.print(".");
  Serial.print(localIP[2]);
  Serial.print(".");
  Serial.print(localIP[3]);
  
}

void setupSinricPro() {
  // get a new Light device from SinricPro
  SinricProLight &myLight1 = SinricPro[K_ID];
  SinricProLight &myLight2 = SinricPro[D_ID];
  SinricProLight &myLight3 = SinricPro[L_ID];

  // set callback function to RED LIGHT
  myLight1.onPowerState(onPowerState);

    // set callback function to RED LIGHT
  myLight2.onPowerState(onPowerState);

   // set callback function to RED LIGHT
  myLight3.onPowerState(onPowerState);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.println("Connected to SinricPro"); }); 
  SinricPro.onDisconnected([](){ Serial.println("Disconnected from SinricPro"); });
  SinricPro.begin(APP_KEY_1, APP_SECRET_1);
/*---------------------------------------------------------------------*/
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.println("");
  pinMode(L_LED,OUTPUT);
  pinMode(D_LED,OUTPUT);
  pinMode(K_LED,OUTPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED,HIGH);
  setupWiFi();
  setupSinricPro();
  client.setServer(SERVER, PORT);
  client.setCallback(callback);
}

void loop() {
  SinricPro.handle();

  if(!client.connected()) {
    reconnectmqttserver();
  }
  client.loop();
}
