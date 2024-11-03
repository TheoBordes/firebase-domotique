/**************************CrowPanel ESP32 HMI Display Example Code************************
Version     :	1.1
Suitable for:	CrowPanel ESP32 HMI Display
Product link:	https://www.elecrow.com/esp32-display-series-hmi-touch-screen.html
Code	  link:	https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File
Lesson	link:	https://www.youtube.com/watch?v=WHfPH-Kr9XU
Description	:	The code is currently available based on the course on YouTube, 
				        if you have any questions, please refer to the course video: Introduction 
				        to ask questions or feedback.
**************************************************************/

#include <BMP280.h>
#include <MFRC522.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include <BH1750.h>


#include <Arduino.h>
#include <SPI.h>


#include <lvgl.h>
#include "ui.h"


#define CALIBRATION_FILE "/TouchCalData1"
#define Caldata false
#define SS_PIN 1  
#define RST_PIN -1 


int redValue = 0;    
int greenValue = 0;  
int blueValue = 0; 



int lumiere =0;
extern "C" void setLedBrightness(int r,int v, int b);
extern "C" int get_Value();

static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;
uint16_t calData[5] = { 557, 3263, 369, 3493, 3  };


BH1750 lightMeter;
BMP280 bmp280;
TFT_eSPI lcd = TFT_eSPI();
MFRC522 mfrc522(SS_PIN, RST_PIN);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[ screenWidth * screenHeight / 13 ];

void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );

  lcd.startWrite();
  lcd.setAddrWindow( area->x1, area->y1, w, h );
  lcd.pushColors( ( uint16_t * )&color_p->full, w * h, true );
  lcd.endWrite();

  lv_disp_flush_ready( disp );
}

uint16_t touchX, touchY;
/*touch read*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{

  bool touched = lcd.getTouch( &touchX, &touchY, 600);
  if ( !touched )
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
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


void setup()
{

  Serial.begin( 115200 ); /*serial init */
  SPI.begin(7,0,6,1); 
  Wire.begin(3,2); //Join I2C bus
  bmp280.begin();
  lightMeter.begin();
  mfrc522.PCD_Init();  
  //LCD init
  lcd.begin();          
  lcd.setRotation(1); 
  lcd.fillScreen(TFT_BLACK);
  if (calData){
    touch_calibrate();
  }
  else {
  if (!loadCalibration()) {// Si le fichier n'existe pas, calibrer et sauvegarder
      touch_calibrate();
    }
  }

  //lvgl init
  lv_init();
  
  lv_disp_draw_buf_init( &draw_buf, buf1, NULL, screenWidth * screenHeight / 13 );

  /* Initialize the display */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  /* Change the following line to your display resolution */
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register( &disp_drv );

  /* Initialize the (dummy) input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register( &indev_drv );

  lcd.fillScreen(TFT_BLACK);
  
  //lv_demo_widgets();    // LVGL demo
  ui_init();

  Serial.println( "Setup done" );
}


void loop()
{
  lumiere = lightMeter.readLightLevel();
  Serial.println(lumiere);
  lv_timer_handler();
  delay(5);
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



void setLedBrightness(int r,int v, int b) {
    neopixelWrite(RGB_BUILTIN,r,v,b);
}
int get_Value(){
  return lumiere;
}
