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

#define WIFI_SSID         "Miracle-"    
#define WIFI_PASS         "00000000"
#define APP_KEY_2           "1e081fa6-7fba-41f3-9cc9-5b92649eb5e7"      
#define APP_SECRET_2        "4f216cf2-4980-4127-a84e-20152aa53bc2-c2f71101-582d-4cce-9a95-ddd66cb6ef42" 
#define F_ID        "60cdc1c08cf8a303b93a0c21"    // Should look like "5dc1564130xxxxxxxxxxxxxx"
#define BK_ID        "60cdc22f2a231603cf26e24e"
#define BAUD_RATE         9600                // Change baudrate to your need

//#define BAUD_RATE         115200                // Change baudrate to your need

#define F_LED       25
#define BK_LED      26

bool onPowerState(const String &deviceId, bool &state) {
      if(deviceId==F_ID){
//       Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
      if(state){
        digitalWrite(F_LED,HIGH);
      }
      else{
        digitalWrite(F_LED,LOW);
        }
    }
      else if(deviceId==BK_ID){
//       Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
      if(state){
        digitalWrite(BK_LED,HIGH);
      }
      else{
        digitalWrite(BK_LED,LOW);
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
  SinricProLight &myLight1 = SinricPro[F_ID];
  SinricProLight &myLight2 = SinricPro[BK_ID];

  // set callback function to RED LIGHT
  myLight1.onPowerState(onPowerState);

    // set callback function to RED LIGHT
  myLight2.onPowerState(onPowerState);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.println("Connected to SinricPro"); }); 
  SinricPro.onDisconnected([](){ Serial.println("Disconnected from SinricPro"); });
  SinricPro.begin(APP_KEY_2, APP_SECRET_2);
/*---------------------------------------------------------------------*/
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); Serial.println("");
  pinMode(F_LED,OUTPUT);
  pinMode(BK_LED,OUTPUT);
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
}
