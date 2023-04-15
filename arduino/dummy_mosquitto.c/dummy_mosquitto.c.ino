#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "wifi";
const char* password = "***";
const char* mqtt_server = "192.168.0.115";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    // Attempt to connect to MQTT broker
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT broker");
      //client.subscribe("test/topic");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(2000);
      return;
    }
  }

  client.publish("dpsystem", "Test");
  delay(1000);
}





