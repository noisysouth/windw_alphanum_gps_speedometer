#define ESP32_S3_TFT // for ESP32-S3 TFT
//#define HX8357 // for the Adafruit 3.5" TFT (HX8357) FeatherWing

#include <Adafruit_GFX.h>    // Core graphics library
#ifdef ESP32_S3_TFT
  // This is a library for several Adafruit displays based on ST77* drivers.
  // Works with the Adafruit TFT Gizmo
  // ----> http://www.adafruit.com/products/4367
  #include <Adafruit_ST7789.h> // Hardware-specific library for ST7789 // on ESP32-S3 TFT
#endif
#ifdef HX8357
  // This is our library for the Adafruit 3.5" TFT (HX8357) FeatherWing
  // ----> http://www.adafruit.com/products/3651
  #include "Adafruit_HX8357.h" // on 3.5" TFT (HX8357) FeatherWing

  // If using the rev 1 with STMPE resistive touch screen controller uncomment this line:
  //#include <Adafruit_STMPE610.h>
  // If using the rev 2 with TSC2007, uncomment this line:
  #include <Adafruit_TSC2007.h>
#endif
