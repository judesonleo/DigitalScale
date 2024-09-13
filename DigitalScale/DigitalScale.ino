#define BLYNK_TEMPLATE_ID "TMPL3Sg_7xi5T"
#define BLYNK_TEMPLATE_NAME "Digital Scale"
#define BLYNK_AUTH_TOKEN "YGmuOJbjjuDAifUKdY2umqpi4BWy017B"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <PubSubClient.h>
#include "HX711.h"
#include <ArduinoJson.h>  // For formatting JSON

const int LOADCELL_DOUT_PIN = 4; 
const int LOADCELL_SCK_PIN = 5;  

HX711 scale;

float calibration_factor = 26.58;
int currentUser = -1;  

char auth[] = "YGmuOJbjjuDAifUKdY2umqpi4BWy017B"; 
char ssid[] = "Daiveek"; 
char pass[] = "123456789"; 
// char ssid[] = "IOTLAB1"; 
// char pass[] = "87654321"; 

#define USER1_PIN V0
#define USER2_PIN V1
#define USER3_PIN V2
#define USER4_PIN V3

const int BUTTON1_PIN = 12;  
const int BUTTON2_PIN = 13;  
const int BUTTON3_PIN = 14;  
const int BUTTON4_PIN = 27; 

// MQTT variables
const char* mqtt_server = "192.168.199.93";  // IP address of your MQTT server
const char* mqtt_topic = "sensor/data";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  Blynk.begin(auth, ssid, pass);
  calibration_factor = 26.58;
  currentUser = -1; 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();  

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);

  Serial.println("Setup complete.");
}

void loop() {
  Blynk.run();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (digitalRead(BUTTON1_PIN) == LOW) {
    currentUser = 0;
    delay(200);
  } else if (digitalRead(BUTTON2_PIN) == LOW) {
    currentUser = 1;
    delay(200);
  } else if (digitalRead(BUTTON3_PIN) == LOW) {
    currentUser = 2;
    delay(200);
  } else if (digitalRead(BUTTON4_PIN) == LOW) {
    currentUser = 3;
    delay(200);
  }

  float weight = scale.get_units(10); 
  double averageWeight = (weight - 1000) / 1000;

  if (currentUser != -1) {
    // Create JSON formatted message
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["user"] = String("user") + String(currentUser + 1);
    jsonDoc["temperature"] = averageWeight;

    char buffer[256];
    serializeJson(jsonDoc, buffer);

    // Publish the message to the MQTT broker
    client.publish(mqtt_topic, buffer);

    switch (currentUser) {
      case 0:
        Blynk.virtualWrite(USER1_PIN, averageWeight);
        break;
      case 1:
        Blynk.virtualWrite(USER2_PIN, averageWeight);
        break;
      case 2:
        Blynk.virtualWrite(USER3_PIN, averageWeight);
        break;
      case 3:
        Blynk.virtualWrite(USER4_PIN, averageWeight);
        break;
    }

    Serial.print("Published to MQTT: ");
    Serial.println(buffer);

    currentUser = -1;
  }

  delay(100);
}
