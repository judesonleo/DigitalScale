#include "HX711.h"

const int LOADCELL_DOUT_PIN = 4; 
const int LOADCELL_SCK_PIN = 5;  

HX711 scale;

float calibration_factor = 26.16; 

void setup() {
  Serial.begin(115200);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  scale.set_scale(calibration_factor);

  scale.tare();  

  Serial.println("Calibration complete. You can now use scale.get_units() to get weight.");
}

void loop() {
  float weight = scale.get_units(10); 
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" grams");
  delay(1000);
}
