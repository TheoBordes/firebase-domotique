#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN -1
#define SS_PIN 1
#define IRQ_PIN 12        

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
volatile bool bNewInt = false;
byte regVal = 0x7F;
void activateRec(MFRC522 mfrc522);
void clearInt(MFRC522 mfrc522);


void setup() {
  Serial.begin(115200); 
  while (!Serial);    
  SPI.begin(7, 0, 6, 1);
  mfrc522.PCD_Init(); 
  Serial.print(F("Ver: 0x"));
  byte readReg = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.println(readReg, HEX);
  pinMode(IRQ_PIN, INPUT_PULLUP);
  regVal = 0xA0; 
  mfrc522.PCD_WriteRegister(mfrc522.ComIEnReg, regVal);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), readCard, FALLING);
  bNewInt = false;
  Serial.println(F("End setup"));
}

void loop() {
  if (bNewInt) { 
    Serial.print(F("Interrupt. "));
    mfrc522.PICC_ReadCardSerial();
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    clearInt(mfrc522);
    mfrc522.PICC_HaltA();
    bNewInt = false;
  }
  activateRec(mfrc522);
  delay(100);
} 


void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void readCard() {
  bNewInt = true;
}



void activateRec(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);
  mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
  mfrc522.PCD_WriteRegister(mfrc522.BitFramingReg, 0x87);
}


void clearInt(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F);
}

