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

char access_license[10], exit_license[10], verdict[5];

DynamicJsonDocument doc(256);
char message_to_send[130];

char received_json[256];
char ack;
int prev_seq_nr = 0;
uint8_t assigned_reservation_space;
uint8_t reservation_assign();

void callback(char* topic, byte* payload, unsigned int length) {
  strncpy(received_json, (char*)payload, length);
  received_json[length]='\0';

  deserializeJson(doc, received_json);

  const char *type = doc["type"];
  if(strcmp(type, "enter_reply")==0){
    strcpy(access_license, doc["plateNumber"]);
    strcpy(verdict, doc["parkingSpaceNumber"]);
  }else if(strcmp(type, "reset_reply")==0){
    ack=1;
  }else if(strcmp(type, "exit_reply")==0){
    strcpy(exit_license, doc["plateNumber"]);
  }else if(strcmp(type, "reservation_request")==0){
    if(ack == 0) return;//do not service reservations until reset is finished
    int seq_cr = doc["seq_nr"];
    if(seq_cr > prev_seq_nr){
      //new request
      assigned_reservation_space = reservation_assign();//0-15 or 16 if not found
      prev_seq_nr = seq_cr;
    }
    if(seq_cr == prev_seq_nr){
      //same request; resend reply
      doc.clear();
      doc["type"] = "reservation_reply";
      char space[3]; space[2]='\0';
      if(assigned_reservation_space == 16){
        space[0]='X'; space[1]='\0';
      }else{
        space[0] = 'A'+assigned_reservation_space/4;
        space[1] = '1'+assigned_reservation_space%4;
      }
      doc["parkingSpaceNumber"] = space;
      doc["seq_nr"] = seq_cr;
      serializeJson(doc, message_to_send);
      send_over_mqtt(message_to_send);
    }
  }
  //Serial.println(type);
}

extern char spaces_states[16];

uint8_t reservation_assign(){
  uint8_t assign;
  for(assign = 0; assign<16; assign++)
    if(spaces_states[assign]=='F')
      break;
  if(assign != 16)
    spaces_states[assign] = 'R';
  return assign;
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
    mqtt_flash();
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
  doc["type"] = "enter_request";
  doc["plateNumber"] = s;
  
  serializeJson(doc, message_to_send);
  
  send_over_mqtt(message_to_send);
}

void send_reset(){
  doc.clear();
  doc["type"] = "reset_request";
  serializeJson(doc, message_to_send);
  send_over_mqtt(message_to_send);
}

void send_exit(char *s){
  doc.clear();
  doc["type"] = "exit_request";
  doc["plateNumber"] = s;
  serializeJson(doc, message_to_send);
  send_over_mqtt(message_to_send);
}

void send_states(char *states){
  doc.clear();
  doc["type"] = "led_state";
  for(int i=0; i<16; i++)
    doc["values"][i] = states[i]=='F'?1:0;
  serializeJson(doc, message_to_send);
  send_over_mqtt(message_to_send);
}
