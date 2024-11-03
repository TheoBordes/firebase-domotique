#include <BH1750.h>
#include <Wire.h>

int lumiere = 0;
BH1750 lightMeter;
unsigned long temps = 0, ptemps = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(3, 15); 
  lightMeter.begin();
  
}

void loop() {
  temps = millis();
  if (temps - ptemps > 1000) { 
    lumiere = lightMeter.readLightLevel();
      Serial.print("Lumière: ");
      Serial.print(lumiere);
      Serial.println(" lx");
    ptemps = temps;
  }
}
