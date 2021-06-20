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

#define WIFI_SSID         "TNI_ROBOT_WIFI"    
#define WIFI_PASS         "tnieng406"
#define APP_KEY_3           "106b1eb0-a3c7-42f8-89d2-7f381b0243d1"      
#define APP_SECRET_3        "5fa82026-a64d-4f10-b399-78658a385d4a-a1f86937-43cf-4c00-a9c3-c54cb68cd68b" 
#define R_ID          "60cddeae8cf8a303b93a0d0e"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BD_ID        "60cdded48cf8a303b93a0d12"    // Should look like "5dc1564130xxxxxxxxxxxxxx"

#define BAUD_RATE         9600                // Change baudrate to your need

//#define BAUD_RATE         115200                // Change baudrate to your need


#define BD_LED      22
#define R_LED       23


bool onPowerState(const String &deviceId, bool &state) {
    if(deviceId==R_ID){
//       Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
      if(state){
        digitalWrite(R_LED,HIGH);
        Serial.println("RED LIGHT TURNED ON");
      }
      else{
        digitalWrite(R_LED,LOW);
        }
    }
    else if(deviceId==BD_ID){
//       Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
      if(state){
        digitalWrite(BD_LED,HIGH);
      }
      else{
        digitalWrite(BD_LED,LOW);
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
  SinricProLight &myLight1 = SinricPro[R_ID];
  SinricProLight &myLight2 = SinricPro[BD_ID];

  // set callback function to RED LIGHT
  myLight1.onPowerState(onPowerState);

    // set callback function to RED LIGHT
  myLight2.onPowerState(onPowerState);


  // setup SinricPro
  SinricPro.onConnected([](){ Serial.println("Connected to SinricPro"); }); 
  SinricPro.onDisconnected([](){ Serial.println("Disconnected from SinricPro"); });
  SinricPro.begin(APP_KEY_3, APP_SECRET_3);
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.println("");

  pinMode(BD_LED,OUTPUT);
  pinMode(R_LED,OUTPUT);

  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}
