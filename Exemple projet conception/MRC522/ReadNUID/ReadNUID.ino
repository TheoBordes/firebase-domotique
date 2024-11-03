#include <SPI.h>
#include <MFRC522.h>
#include <TFT_eSPI.h> 

#define SS_PIN 1  
#define RST_PIN -1  
#define TFT_GREY 0x6c0277 

MFRC522 mfrc522(SS_PIN, RST_PIN);  
TFT_eSPI tft = TFT_eSPI();  

String dump_byte_array_to_string(byte *buffer, byte bufferSize) {
  String result = "";
  for (byte i = 0; i < bufferSize; i++) {
    if (buffer[i] < 0x10) {
      result += "0";  
    }
    result += String(buffer[i], HEX);  
    if (i < bufferSize - 1) {
      result += " ";  
    }
  }
  result.toUpperCase();  
  return result;        
}

void setup() {
  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  digitalWrite(4, HIGH);
  Serial.begin(115200);  
  SPI.begin(7,0,6,1);        
  mfrc522.PCD_Init();  
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.print("UID de la carte : ");
  Serial.println(dump_byte_array_to_string(mfrc522.uid.uidByte,mfrc522.uid.size));
  mfrc522.PICC_HaltA();
  delay(1000);
}
