#include "my_tft.h"

// ------------------------- TFT setup -------------------------
#ifdef ESP32_S3_TFT
  // Use dedicated hardware SPI pins
  Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

  // on ESP32-S3 TFT
  #define PIXELS_X 240
  #define PIXELS_Y 135
#endif

void setup_esp32_s3_tft(void) {
  // Start TFT display
#ifdef ESP32_S3_TFT
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  //pinMode(TFT_BACKLIGHT, OUTPUT);
  //digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on; for CircuitPlayground Express TFT Gizmo

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  tft.init(PIXELS_Y, PIXELS_X);                // Init ST7789 ST7789 240x135 on ESP32-S3 TFT
#endif
}