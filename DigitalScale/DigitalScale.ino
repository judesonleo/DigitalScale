#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"

const int LOADCELL_DOUT_PIN = 4; 
const int LOADCELL_SCK_PIN = 5;  

#define BLYNK_TEMPLATE_ID "TMPL3Sg_7xi5T"
#define BLYNK_TEMPLATE_NAME "Digital Scale"
#define BLYNK_AUTH_TOKEN "YGmuOJbjjuDAifUKdY2umqpi4BWy017B"

HX711 scale;

float calibration_factor = 26.58;
int currentUser = -1;  

char auth[] = "YGmuOJbjjuDAifUKdY2umqpi4BWy017B"; 
char ssid[] = "Judeson"; 
char pass[] = "Judeson@143"; 

#define USER1_PIN V0
#define USER2_PIN V1
#define USER3_PIN V2
#define USER4_PIN V3

const int BUTTON1_PIN = 12;  
const int BUTTON2_PIN = 13;  
const int BUTTON3_PIN = 14;  
const int BUTTON4_PIN = 15; 

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  Blynk.begin(auth, ssid, pass);

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

  float averageWeight = scale.read_average(10) - 2000;

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

  Serial.print("User ");
  Serial.print(currentUser + 1);
  Serial.print(" Average Weight: ");
  Serial.println(averageWeight);

  currentUser = -1;
  delay(100);  
}
