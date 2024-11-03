#include <SPI.h>
#include <MFRC522.h>
#include <TFT_eSPI.h> 

#define SS_PIN 1  
#define RST_PIN -1  
#define TFT_GREY 0x6c0277 

MFRC522 mfrc522(SS_PIN, RST_PIN);  
TFT_eSPI tft = TFT_eSPI();  

void setup() {
  Serial.begin(115200);  
  SPI.begin(7,0,6,1);        
  mfrc522.PCD_Init();  
  tft.init();
  tft.setRotation(2);
}

void loop() {
  tft.fillScreen(TFT_GREY);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_GREY, TFT_BLACK);
  tft.setTextSize(2);
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.print("UID de la carte : ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tft.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    tft.print(mfrc522.uid.uidByte[i], HEX);
  }
  mfrc522.PICC_HaltA();
  delay(1000);
}
