#include "my_tft.h"

// ------------------------- TFT setup -------------------------
#ifdef HX8357
  #ifdef ESP8266
    #define STMPE_CS 16
    #define TFT_CS   0
    #define TFT_DC   15
    #define SD_CS    2
  #elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3)
    #define STMPE_CS 32
    #define TFT_CS   15
    #define TFT_DC   33
    #define SD_CS    14
  #elif defined(TEENSYDUINO)
    #define TFT_DC   10
    #define TFT_CS   4
    #define STMPE_CS 3
    #define SD_CS    8
  #elif defined(ARDUINO_STM32_FEATHER)
    #define TFT_DC   PB4
    #define TFT_CS   PA15
    #define STMPE_CS PC7
    #define SD_CS    PC5
  #elif defined(ARDUINO_NRF52832_FEATHER)  /* BSP 0.6.5 and higher! */
    #define TFT_DC   11
    #define TFT_CS   31
    #define STMPE_CS 30
    #define SD_CS    27
  #elif defined(ARDUINO_MAX32620FTHR) || defined(ARDUINO_MAX32630FTHR)
    #define TFT_DC   P5_4
    #define TFT_CS   P5_3
    #define STMPE_CS P3_3
    #define SD_CS    P3_2
  #else
      // Anything else, defaults!
    #define STMPE_CS 6
    #define TFT_CS   9
    #define TFT_DC   10
    #define SD_CS    5
  #endif

  #define TFT_RST -1

  // Use hardware SPI and the above for CS/DC
  Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

// --------------------- touchscreen setup ---------------------

  #if defined(_ADAFRUIT_STMPE610H_)
    Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);
  #elif defined(_ADAFRUIT_TSC2007_H)
    // If you're using the TSC2007 there is no CS pin needed, so instead its an IRQ!
    #define TSC_IRQ STMPE_CS
    Adafruit_TSC2007 ts = Adafruit_TSC2007();             // newer rev 2 touch contoller
  #else
    #error("You must have either STMPE or TSC2007 headers included!")
  #endif

  // This is calibration data for the raw touch data to the screen coordinates
  // For STMPE811/STMPE610
  #define STMPE_TS_MINX 3800
  #define STMPE_TS_MAXX 100
  #define STMPE_TS_MINY 100
  #define STMPE_TS_MAXY 3750
  // For TSC2007
  #define TSC_TS_MINX 300
  #define TSC_TS_MAXX 3800
  #define TSC_TS_MINY 185
  #define TSC_TS_MAXY 3700
  // set during setup(); use during loop()
  int16_t touch_minx, touch_maxx;
  int16_t touch_miny, touch_maxy;
  bool touch_found;
#endif

void setup_hx8357_tft(void) {
  // Start TFT display
#ifdef HX8357
  #if defined(_ADAFRUIT_STMPE610H_)
    touch_found = ts.begin();
    if (!touch_found) {
      Serial.println("Couldn't start STMPE touchscreen controller");
    } else {
      touch_minx = STMPE_TS_MINX; touch_maxx = STMPE_TS_MAXX;
      touch_miny = STMPE_TS_MINY; touch_maxy = STMPE_TS_MAXY;
      Serial.println("Touchscreen STMPE started");
    }
  #else
    touch_found = ts.begin(0x48, &Wire);
    if (!touch_found) {
      Serial.println("Couldn't start TSC2007 touchscreen controller");
    } else {
      touch_minx = TSC_TS_MINX; touch_maxx = TSC_TS_MAXX;
      touch_miny = TSC_TS_MINY; touch_maxy = TSC_TS_MAXY;
      pinMode(TSC_IRQ, INPUT);
      Serial.println("Touchscreen TSC2007 started");
    }
  #endif

  tft.begin();

  // read TFT display diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(HX8357_RDPOWMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDCOLMOD);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDIM);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDSDR);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
#endif
}