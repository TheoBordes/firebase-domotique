/*
  Rui Santos
  Complete project details at our blog: https://RandomNerdTutorials.com/esp32-data-logging-firebase-realtime-database/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <BH1750.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <TFT_eSPI.h>
#include <BMP280.h>
#include "time.h"
#include <FS.h>
#include <lvgl.h>
#include "ui.h"

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "abc"
#define WIFI_PASSWORD "12345678"
// Insert Firebase project API Key
#define API_KEY "AIzaSyALggtPTOCEyw0RQW2k3icytNeCiU2NzK8"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "theo.bordes.parcoursup@gmail.com"
#define USER_PASSWORD "12345678"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://domotique-426ee-default-rtdb.europe-west1.firebasedatabase.app"
#define CALIBRATION_FILE "/TouchCalData1"
#define Calldata false
#define RST_PIN -1
#define SS_PIN 1
#define IRQ_PIN 12
// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

#define MQ_PIN 2
#define relais_PIN 18
// Variable to save USER UID
String uid;
String state = "", Fname, Lname, Pname = "";

String Read_RFID();
// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String tempPath = "/temperature";
String lumPath = "/luminosity";
String presPath = "/pressure";
String gazPath = "/gaz";
String rfidPath = "/rfid";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

float Vc = 5.0;
float RL = 10.0;
float Ro = 10.0;
float Rs_Ro_ratio;
float LPG_PPM;

volatile int timestamp;
int redValue = 0;
int greenValue = 0;
int blueValue = 0;
int lumiere = 0, pression = 0, temperature = 0;
char rfid[12];
extern "C" void setLedBrightness(int r, int v, int b);
extern "C" void get_Value();
extern "C" void relais_once();
int tab_sensor[4];
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
uint16_t calData[5] = { 264, 3403, 416, 3262, 1 };
int currentTime = 0, previousTime = 0;
bool state_led = 0;

FirebaseJson json;

const char *ntpServer = "pool.ntp.org";

TFT_eSPI lcd = TFT_eSPI(); /* TFT entity */


//MRC522
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
volatile bool bNewInt = false;
byte regVal = 0x7F;
void activateRec(MFRC522 mfrc522);
void clearInt(MFRC522 mfrc522);




BH1750 lightMeter;
BMP280 bmp280;


static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[screenWidth * screenHeight / 13];
// Timer variables (send new readings every thirty seconds)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 30000;


// Initialize WiFi
void initWiFi() {

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lcd.startWrite();
  lcd.setAddrWindow(area->x1, area->y1, w, h);
  lcd.pushColors((uint16_t *)&color_p->full, w * h, true);
  lcd.endWrite();

  lv_disp_flush_ready(disp);
}

uint16_t touchX, touchY;
/*touch read*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {

  bool touched = lcd.getTouch(&touchX, &touchY, 600);
  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;

    /*set location*/
    data->point.x = touchX;
    data->point.y = touchY;

    //Serial.print( "Data x " );
    //Serial.println( touchX );

    //Serial.print( "Data y " );
    //Serial.println( touchY );
  }
}

bool loadCalibration() {
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    File f = SPIFFS.open(CALIBRATION_FILE, "r");
    if (f) {
      uint16_t calData[5];
      f.read((uint8_t *)calData, 14);
      lcd.setTouch(calData);  // Applique les données de calibration
      f.close();
      return true;
    }
  }
  return false;
}

void touch_calibrate() {
  uint16_t calData[5];  // Tableau pour stocker les données de calibration
  lcd.fillScreen(TFT_BLACK);
  lcd.setCursor(20, 0);
  lcd.setTextFont(2);
  lcd.setTextSize(1);
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.println("Touch corners as indicated");

  lcd.calibrateTouch(calData, TFT_WHITE, TFT_RED, 15);  // Calibration de l'écran


  File f = SPIFFS.open(CALIBRATION_FILE, "w");
  if (f) {
    f.write((const unsigned char *)calData, 14);
    f.close();
  }
  for (int i = 0; i < 5; i++) {
    Serial.println(calData[i]);
  }
  Serial.println("Calibration complete!");
}


void relais_once() {
  state_led = !state_led;
  digitalWrite(relais_PIN, state_led);
}

void setLedBrightness(int r, int v, int b) {
  neopixelWrite(RGB_BUILTIN, r, v, b);
}
void get_Value() {
  int local_tab[4] = {
    lumiere,
    pression,
    temperature,
    LPG_PPM
  };
  for (int i = 0; i < 4; i++) {
    tab_sensor[i] = local_tab[i];
  }
}

String dump_byte_array_to_string(byte *buffer, byte bufferSize) {
  String result = "";
  for (byte i = 0; i < bufferSize; i++) {
    if (buffer[i] < 0x10) {
      result += "0";  // Ajouter un 0 pour les valeurs inférieures à 0x10
    }
    result += String(buffer[i], HEX);  // Ajouter l'octet converti en hexadécimal
    if (i < bufferSize - 1) {
      result += " ";  // Ajouter un espace entre les octets pour le formatage
    }
  }
  result.toUpperCase();  // Convertir en majuscules
  return result;         // Renvoyer la chaîne résultante
}
void readCard() {
  bNewInt = true;
}

/*
 * The function sending to the MFRC522 the needed commands to activate the reception
 */
void activateRec(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);
  mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
  mfrc522.PCD_WriteRegister(mfrc522.BitFramingReg, 0x87);
}

/*
 * The function to clear the pending interrupt bits after interrupt serving routine
 */
void clearInt(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F);
}

float calculatePPM(float Rs_Ro, float m, float b) {
  float logPPM = m * log10(Rs_Ro) + b;
  return pow(10, logPPM);
}


void setup() {
  pinMode(relais_PIN, OUTPUT);
  pinMode(MQ_PIN, INPUT);
  pinMode(1, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(1, HIGH);
  Serial.begin(115200);
  SPI.begin(7, 0, 6, 1);
  Wire.begin(3, 15);  //Join I2C bus
  bmp280.begin();
  mfrc522.PCD_Init();

  //interrupt
  pinMode(IRQ_PIN, INPUT_PULLUP);
  regVal = 0xA0;  //rx irq
  mfrc522.PCD_WriteRegister(mfrc522.ComIEnReg, regVal);

  bNewInt = false;  //interrupt flag

  /*Activate the interrupt*/
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), readCard, FALLING);
  //interrupt
  delay(1000);
  lightMeter.begin();



  initWiFi();
  configTime(0, 0, ntpServer);

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  //Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  //Serial.print("User UID: ");
  //Serial.println(uid);
  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";
  lcd.begin();
  lcd.setRotation(1);
  lcd.fillScreen(TFT_BLACK);
  if (Calldata) {
    touch_calibrate();
  } else {
    lcd.setTouch(calData);
  }
  //lvgl init
  lv_init();

  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, screenWidth * screenHeight / 13);

  /* Initialize the display */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  lcd.fillScreen(TFT_BLACK);

  ui_init();
  Serial.println("Setup done");
}



void loop() {

  if (bNewInt) {
    digitalWrite(1, HIGH);
    mfrc522.PICC_ReadCardSerial();
    String name = dump_byte_array_to_string(mfrc522.uid.uidByte, mfrc522.uid.size);
    if (Firebase.ready()) {
      //Get current timestamp
      timestamp = getTime();
      //Serial.print ("time: ");
      //Serial.println (timestamp);
      parentPath = databasePath + "/" + String(timestamp);
      json.set(rfidPath.c_str(), name);
      Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    }
    clearInt(mfrc522);
    mfrc522.PICC_HaltA();
    //strncpy(rfid, name.c_str(), sizeof(rfid) - 1);
    name = "";
    bNewInt = false;
  }
  activateRec(mfrc522);

  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
    lumiere = lightMeter.readLightLevel();
    pression = bmp280.getPressure();
    temperature = bmp280.getTemperature();
    int gazSensorValue = analogRead(MQ_PIN);
    float Vout = (gazSensorValue / 1023.0) * Vc;
    float Rs = ((5.0 - Vout) / Vout) * RL;
    Rs_Ro_ratio = Rs / Ro;
    LPG_PPM = calculatePPM(Rs_Ro_ratio, -0.45, 2.3);


    sendDataPrevMillis = millis();
    //Get current timestamp
    //Serial.print ("time: ");
    //Serial.println (timestamp);

    parentPath = databasePath + "/" + String(timestamp);

    json.set(tempPath.c_str(), String(temperature));
    json.set(rfidPath.c_str(), "");
    json.set(lumPath.c_str(), String(lumiere));
    json.set(presPath.c_str(), String(pression));
    json.set(gazPath.c_str(), String(LPG_PPM));
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
  lv_timer_handler();
}
