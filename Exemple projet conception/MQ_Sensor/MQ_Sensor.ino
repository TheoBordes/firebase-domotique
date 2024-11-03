#define MQ_PIN 2

float Vc = 5.0;
float RL = 10.0;
float Ro = 10.0;
float Rs_Ro_ratio;
float LPG_PPM;
unsigned long temps = 0, ptemps = 0;


float calculatePPM(float Rs_Ro, float m, float b) {
  float logPPM = m * log10(Rs_Ro) + b;
  return pow(10, logPPM);
}

void setup() {
  Serial.begin(115200);
  pinMode(MQ_PIN, INPUT); 
}

void loop() {
  temps = millis();
  if (temps - ptemps > 1000) { 
    int gazSensorValue = analogRead(MQ_PIN);
    float Vout = (gazSensorValue / 1023.0) * Vc;
    float Rs = ((5.0 - Vout) / Vout) * RL;
    Rs_Ro_ratio = Rs / Ro;
    LPG_PPM = calculatePPM(Rs_Ro_ratio, -0.45, 2.3);
      Serial.print("La quantité de CO2 est : ");
      Serial.print(LPG_PPM);
      Serial.println(" ppm");
    ptemps = temps;
  }
}
