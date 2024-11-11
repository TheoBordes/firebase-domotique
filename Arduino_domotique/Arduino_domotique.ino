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
#include <lvgl.h>
#include "ui.h"


#include "addons/TokenHelper.h"

#include "addons/RTDBHelper.h"


#define WIFI_SSID "Bbox-E470D957"
#define WIFI_PASSWORD "gp9344TWZPxw7jpK31"

#define API_KEY "AIzaSyALggtPTOCEyw0RQW2k3icytNeCiU2NzK8"


#define USER_EMAIL "theo.bordes.parcoursup@gmail.com"
#define USER_PASSWORD "12345678"


#define DATABASE_URL "https://domotique-426ee-default-rtdb.europe-west1.firebasedatabase.app"
#define CALIBRATION_FILE "/TouchCalData1"
#define Calldata false
#define RST_PIN -1
#define SS_PIN 1
#define IRQ_PIN 12
#define sda 3 
#define scl 15

// Prototypes des fonctions
void initWiFi();
unsigned long getTime();
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void touch_calibrate();
extern "C" void relais_once();
extern "C" void setLedBrightness(int r, int v, int b);
extern "C" void get_Value();
String dump_byte_array_to_string(byte *buffer, byte bufferSize);
void readCard();
void activateRec(MFRC522 mfrc522);
void clearInt(MFRC522 mfrc522);
float calculatePPM(float Rs_Ro, float m, float b);

// Déclaration de variables pour Firebase
FirebaseData stream;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
String uid;
String databasePath, parentPath;
String tempPath = "/temperature";
String lumPath = "/luminosity";
String presPath = "/pressure";
String gazPath = "/gaz";
String rfidPath = "/rfid";
String timePath = "/timestamp";

// Déclaration de variables pour les capteurs et les valeurs de mesure
BH1750 lightMeter; // instanciation de l'objet BH1750
BMP280 bmp280; // instanciation de l'objet BMP280
volatile int timestamp;
int redValue = 0;
int greenValue = 0;
int blueValue = 0;
int lumiere = 0, pression = 0, temperature = 0;
float Vc = 5.0;
float RL = 10.0;
float Ro = 10.0;
float Rs_Ro_ratio;
float LPG_PPM;
int tab_sensor[4];

// Déclaration de variables pour le LCD et l'interface utilisateur
TFT_eSPI lcd = TFT_eSPI(); // instanciation de l'objet TFT
static lv_disp_draw_buf_t draw_buf;
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;
static lv_color_t buf1[screenWidth * screenHeight / 13];
uint16_t calData[5] = { 264, 3403, 416, 3262, 1 };
uint16_t touchX, touchY;

// Déclaration de variables pour le relais et la LED
#define MQ_PIN 2
#define relais_PIN 18
unsigned long currentTime = 0, previousTime = 0, led_time = 0;
bool state_relais = 0;
bool state_relais_net = 0;
extern bool timer4,button1;

// Déclaration de variables pour l'authentification et la gestion de l'heure
const char *ntpServer = "pool.ntp.org";

// Initialisation du module RFID
MFRC522 mfrc522(SS_PIN, RST_PIN); // instanciation de l'objet RFID
volatile bool bNewInt = false;
byte regVal = 0x7F;

// Déclaration de variables de temporisation
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 30000;

void streamCallback(FirebaseStream data){
  state_relais_net = !state_relais_net;
   if (!state_relais & !timer4 & !button1) {
      relais_once();
   }
  }
  
void streamTimeoutCallback(bool timeout){
  if (timeout)
    Serial.println("stream timeout, resuming...\n");
  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
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
    Wire.begin(sda, scl);  
    bmp280.begin();
    mfrc522.PCD_Init();
    lightMeter.begin();

    pinMode(IRQ_PIN, INPUT_PULLUP);
    regVal = 0xA0;  
    mfrc522.PCD_WriteRegister(mfrc522.ComIEnReg, regVal);

    bNewInt = false;  

    attachInterrupt(digitalPinToInterrupt(IRQ_PIN), readCard, FALLING);
    delay(1000);
   

    initWiFi();
    configTime(0, 0, ntpServer);
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;

    Firebase.reconnectWiFi(true);
    fbdo.setResponseSize(4096);
    config.token_status_callback = tokenStatusCallback;  
    config.max_token_generation_retry = 5;
    Firebase.begin(&config, &auth);

    while ((auth.token.uid) == "") {
        Serial.print('.');
        delay(1000);
    }
    uid = auth.token.uid.c_str();

    databasePath = "/UsersData/" + uid + "/readings";
    String listenerPath = "/UsersData/" + uid + "/outputs/";  
  if (!Firebase.RTDB.beginStream(&stream, listenerPath.c_str()))
    Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());
  Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);


    lcd.begin();
    lcd.setRotation(1);
    lcd.fillScreen(TFT_BLACK);
    if (Calldata) {
        touch_calibrate();
    } else {
        lcd.setTouch(calData);
    }
    lv_init();

    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, screenWidth * screenHeight / 13);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

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
  currentTime = millis();
  if (bNewInt & !timer4 & !button1) {
    mfrc522.PICC_ReadCardSerial();
    String name = dump_byte_array_to_string(mfrc522.uid.uidByte, mfrc522.uid.size);
    if (Firebase.ready()) {
      timestamp = getTime();
      parentPath = databasePath + "/" + String(timestamp);
      json.set(rfidPath.c_str(), name);
      Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    }
    clearInt(mfrc522);
    mfrc522.PICC_HaltA();
    if (name == "53 7C C8 A9" & !state_relais & !timer4 & !button1 & !state_relais_net){relais_once(); led_time = currentTime;}
    name = "";
    bNewInt = false;
    
  }
  activateRec(mfrc522);
  
 if ( state_relais & (currentTime - led_time > 10000) & !timer4 & !button1 & !state_relais_net ){relais_once();}

    if (Firebase.ready() && (currentTime - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
        lumiere = lightMeter.readLightLevel();
        pression = bmp280.getPressure();
        temperature = bmp280.getTemperature();
        int gazSensorValue = analogRead(MQ_PIN);
        float Vout = (gazSensorValue / 1023.0) * Vc;
        float Rs = ((5.0 - Vout) / Vout) * RL;
        Rs_Ro_ratio = Rs / Ro;
        LPG_PPM = calculatePPM(Rs_Ro_ratio, -0.45, 2.3);

        sendDataPrevMillis = currentTime;
        timestamp = getTime();
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


//définition des fonctions 


void initWiFi() { // permet d'initialiser le WIFI 

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

unsigned long getTime() { //permet d'obtenir le temps sous la forme du temps en seconde depuis 1970
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return (0);
  }
  time(&now);
  return now;
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {// permet d'afficher sur l'écran lvgl
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lcd.startWrite();
  lcd.setAddrWindow(area->x1, area->y1, w, h);
  lcd.pushColors((uint16_t *)&color_p->full, w * h, true);
  lcd.endWrite();

  lv_disp_flush_ready(disp);
}


void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) { // permet d'envoyer les données du touch de l'écran dans la librairie lvgl 

  bool touched = lcd.getTouch(&touchX, &touchY, 600);
  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
  }
}



void touch_calibrate() { // permet de faire une calibration du touch de l'écran 
  uint16_t calData[5];
  lcd.fillScreen(TFT_BLACK);
  lcd.setCursor(20, 0);
  lcd.setTextFont(2);
  lcd.setTextSize(1);
  lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  lcd.println("Touch corners as indicated");
  lcd.calibrateTouch(calData, TFT_WHITE, TFT_RED, 15); 
  for (int i = 0; i < 5; i++) {
    Serial.println(calData[i]);
  }
  Serial.println("Calibration complete!");
}


void relais_once() { // permet d'activer ou de désactiver le relais par rapport à l'état précedent
  state_relais = !state_relais; 
  digitalWrite(relais_PIN, state_relais);
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

float calculatePPM(float Rs_Ro, float m, float b) {
  float logPPM = m * log10(Rs_Ro) + b;
  return pow(10, logPPM);
}

