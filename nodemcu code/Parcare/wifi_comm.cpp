#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "wifi_comm.h"

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = SSID;
const char* password = PASSWORD;
const char* mqtt_server = MQTT_SERVER;
const char* topic = TOPIC;

void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


bool try_reconnect() {
  if (!client.connected()) {
    //Serial.print("Connecting to MQTT broker...");
    if (client.connect("NodeMCU")) {
      //Serial.println("connected");
      client.subscribe(topic);
      return true;
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());   
      //Serial.println(" retrying in 5 seconds");
      return false;
    }
  }
  return true;
}

void wifi_init() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
    //Serial.println("Connecting to WiFi...");
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  
  Serial.println("Connected to WiFi");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  while(!client.connected()) {
    try_reconnect();
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
  while(!client.connected()){
    try_reconnect();
  }
  client.loop();
  return true;
}

void send_over_mqtt(char *s){
  client.publish("dpsystem",s);
}
