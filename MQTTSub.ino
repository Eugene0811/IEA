#include <WiFi.h>

#include <PubSubClient.h>

#define SSID_N          "Miracle-"
#define SSID_P          "00000000"
#define SERVER_IP_ADDR  "127.0.0.0"
#define PORT            0

WiFiClient espClient;
PubSubClient client(espClient);

String name[3] = {"Teeraphat Kullanankanjana",
                  "Thanaporn Sanamontre",
                  "Chayakotchamon Boonsanong"};

void reconnectmqttserver(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())){
     Serial.println("connected");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String MQTT_DATA = "";
  for (int i=0;i<length;i++) {
   MQTT_DATA += (char)payload[i];}

}

void setup()
{
  Serial.begin(9600);

  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
  WiFi.begin(SSID_N, SSID_P);
  while((!(WiFi.status() == WL_CONNECTED))){
    delay(3000);
    Serial.print("..");
  }
  Serial.print("Connected");
  Serial.println("Your IP is");
  Serial.println((WiFi.localIP()));
  client.setServer(SERVER_IP_ADDR, PORT);
  client.setCallback(callback);
}

void loop ()
{
  if(!client.connected()) {
    reconnectmqttserver();
  }
  client.loop();
}
