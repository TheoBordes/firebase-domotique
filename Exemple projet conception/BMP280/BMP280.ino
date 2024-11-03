#include <Wire.h>
#include <BMP280.h>

int pression = 0, temperature = 0;
BMP280 bmp280;
unsigned long temps = 0, ptemps = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(3, 15);  
  bmp280.begin();

}

void loop() {
  
 temps = millis();
  if (temps - ptemps > 1000) {  
    pression = bmp280.getPressure();
    temperature = bmp280.getTemperature();
    Serial.print("Pression: ");
    Serial.print(pression);
    Serial.println("");
    Serial.print("Température: ");
    Serial.print(temperature);
    Serial.println(" °C");
    ptemps = temps;
  }
}




