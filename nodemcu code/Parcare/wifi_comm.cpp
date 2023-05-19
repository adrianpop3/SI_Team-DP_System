#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "wifi_comm.h"

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = MQTT_SERVER;
const char* topic = TOPIC;

char access_license[10], verdict[5];

DynamicJsonDocument doc(256);
char message_to_send[130];

char received_json[256];
void callback(char* topic, byte* payload, unsigned int length) {
  strncpy(received_json, (char*)payload, length);
  received_json[length]='\0';

  deserializeJson(doc, received_json);

  const char *type = doc["type"];
  if(strcmp(type, "reply_access")==0){
    strcpy(access_license, doc["license"]);
    strcpy(verdict, doc["verdict"]);
  }
  //Serial.println(type);
}


bool try_reconnect() {
  if (!client.connected()) {
    //Serial.print("Connecting to MQTT broker...");
    if (client.connect("NodeMCU")) {
      //Serial.println("connected");
      client.subscribe(topic);
      return true;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());   
      Serial.println(" retrying in 5 seconds");
      return false;
    }
  }
  return true;
}

void wifi_init() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    wifi_flash();
    Serial.println("Connecting to WiFi...");
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  while(!client.connected()) {
    try_reconnect();
    mqtt_flash();
  }
  
  Serial.println("Connected to mqtt");
}

bool wifi_update() {
  /*int i;
  for(i=0; i<10; i++){
    if (!client.connected()) {
      try_reconnect();
      delayMicroseconds(50);
    }else{
      break;
    }
  }
  if(i==10) {Serial.println("Client disconnected!!!");return false;}*/
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
    Serial.print("WiFi status: ");Serial.println(WiFi.status());
  }
  //Serial.print("WiFi rssi: ");Serial.println(WiFi.RSSI());
  while(!client.connected()){
    try_reconnect();
    //client.loop();
  }
  client.loop();
  return true;
}

void send_over_mqtt(char *s){
  client.publish("dpsystem",s);
  client.flush();
  wifi_update();
}

void send_license(char *s){
  doc.clear();
  doc["type"] = "req_car_access";
  doc["license"] = s;
  
  serializeJson(doc, message_to_send);
  
  send_over_mqtt(message_to_send);
}
