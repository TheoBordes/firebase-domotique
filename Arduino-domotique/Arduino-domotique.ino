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
#define Caldata false
#define RST_PIN -1
#define SS_PIN 1
// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;
String state = "", Fname, Lname, name = "", Pname = "";

String Read_RFID();
// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String tempPath = "/temperature";
String lumPath = "/luminosity";
String presPath = "/pressure";
String rfidPath = "/rfid";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;
int redValue = 0;
int greenValue = 0;
int blueValue = 0;
int lumiere = 0, pression = 0, temperature = 0;
extern "C" void setLedBrightness(int r, int v, int b);
extern "C" void get_Value();
int tab_sensor[3];
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
uint16_t calData[5] = { 557, 3263, 369, 3493, 3 };


FirebaseJson json;

const char *ntpServer = "pool.ntp.org";

TFT_eSPI lcd = TFT_eSPI(); /* TFT entity */
MFRC522 mfrc522(SS_PIN, RST_PIN);
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

void setup() {
  Serial.begin(115200);
  SPI.begin(7, 0, 6, 1);
  Wire.begin(3, 2);  //Join I2C bus
  bmp280.begin();
  initWiFi();
  mfrc522.PCD_Init();
  lightMeter.begin();


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
  if (calData) {
    touch_calibrate();
  } else {
    if (!loadCalibration()) {  // Si le fichier n'existe pas, calibrer et sauvegarder
      touch_calibrate();
    }
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

String Read_RFID() {
  Fname = "";
  Lname = " ";
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.

  byte buffer1[18];

  block = 4;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));  //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    String state = "1 Authentication failed: " + String(mfrc522.GetStatusCodeName(status));
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    state += "1 Reading failed: " + String(mfrc522.GetStatusCodeName(status));
  }
  for (uint8_t i = 0; i < 16; i++) {
    if (buffer1[i] != 32) {
      Fname += (char)buffer1[i];  // Concaténer dans Fname
    }
  }
  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));  //line 834
  if (status != MFRC522::STATUS_OK) {
    String state = "2 Authentication failed: " + String(mfrc522.GetStatusCodeName(status));
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    state += " 2 Reading failed: " + String(mfrc522.GetStatusCodeName(status));
  }
  for (uint8_t i = 0; i < 16; i++) {
    Lname += (char)buffer2[i];  // Concaténer dans Fname
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  return Fname + Lname;
}


void loop() {

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    name = Read_RFID();
    Serial.println(name);
    if (Firebase.ready()) {
      //Get current timestamp
      timestamp = getTime();
      //Serial.print ("time: ");
      //Serial.println (timestamp);
     
      parentPath = databasePath + "/" + String(timestamp);
      json.set(rfidPath.c_str(), name);
      Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    }
    
  }

  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    //Get current timestamp
    timestamp = getTime();
    //Serial.print ("time: ");
    //Serial.println (timestamp);
    lumiere = lightMeter.readLightLevel();
    pression = bmp280.getPressure();
    temperature = bmp280.getTemperature();
    parentPath = databasePath + "/" + String(timestamp);

    json.set(tempPath.c_str(), String(temperature));
    json.set(rfidPath.c_str(), name);
    json.set(lumPath.c_str(), String(lumiere));
    json.set(presPath.c_str(), String(pression));
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
  lv_timer_handler();
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

  Serial.println("Calibration complete!");
}



void setLedBrightness(int r, int v, int b) {
  neopixelWrite(RGB_BUILTIN, r, v, b);
}
void get_Value() {
  int local_tab[3] = { lumiere, pression, temperature };
  for (int i = 0; i < 3; i++) {
    tab_sensor[i] = local_tab[i];
  }
}