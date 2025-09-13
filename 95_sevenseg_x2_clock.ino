/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED 7-Segment backpacks 
  ----> http://www.adafruit.com/products/881
  ----> http://www.adafruit.com/products/880
  ----> http://www.adafruit.com/products/879
  ----> http://www.adafruit.com/products/878

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
//#define USE_MATRIX // 7 segment numeric display(s)
#define USE_ALPHA // 14-segment alphanumeric display(s)

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define TIMET_1JAN2016 1451628000

#ifdef USE_MATRIX
const int matrix_count = 0;
Adafruit_7segment matrix[matrix_count];
#endif

#ifdef USE_ALPHA
const int  alpha_count = 2;
Adafruit_AlphaNum4 alpha4[alpha_count];
#endif



char *dayNames[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char *monNames[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

void sevenseg_bright_set(void) {
  int idx;

  if (app_settings->bright_steps >= 0) {
#ifdef USE_MATRIX
    for (idx = 0; idx < matrix_count; idx++) {
      matrix[idx].setDisplayState(1);
      matrix[idx].setBrightness(app_settings->bright_steps);
    }
#endif
#ifdef USE_ALPHA
    for (idx = 0; idx < alpha_count; idx++) {
      alpha4[idx].setDisplayState(1);
      alpha4[idx].setBrightness(app_settings->bright_steps);
    }
#endif
    if (app_settings->bright_steps == 0) {
      ScreenDraw(); // draw not-dimmed
    }
  } else { // bright_steps < 0
#ifdef USE_MATRIX
    for (idx = 0; idx < matrix_count; idx++) {
      matrix[idx].setDisplayState(0);
    }
#endif
#ifdef USE_ALPHA
    for (idx = 0; idx < alpha_count; idx++) {
      alpha4[idx].setDisplayState(0);
    }
#endif
    ScreenDraw(); // draw dimmer
  }
}

void sevenseg_setup(void) {
#ifdef USE_MATRIX
  matrix[0] = Adafruit_7segment();
  matrix[1] = Adafruit_7segment();
  Serial.println("7 Segment Backpack Initialized");
  matrix[0].begin(0x70);
  matrix[1].begin(0x71);
  matrix[0].drawColon(true);
  matrix[0].writeDisplay();
#endif

#ifdef USE_ALPHA
  alpha4[0] = Adafruit_AlphaNum4();
  alpha4[1] = Adafruit_AlphaNum4();
  //alpha4[2] = Adafruit_AlphaNum4();
  //alpha4[0].begin(0x72);
  //alpha4[1].begin(0x73);
  //alpha4[2].begin(0x74);
  alpha4[0].begin(0x70);
  alpha4[1].begin(0x71);
#endif

  sevenseg_bright_set();
}

void sevenseg_loop(void) {
  struct timeval tv_now;
  struct tm now_tm;
  int _12hour;
  char *mon_str;
  char *wday_str;
  int year_int;

  gettimeofday(&tv_now, NULL);
  //Serial.print("Time tv_now.tv_sec: ");
  //Serial.print(tv_now.tv_sec);
  //Serial.print(", tv_now.tv_usec: ");
  //Serial.println(tv_now.tv_usec);
  if (tv_now.tv_sec < TIMET_1JAN2016) {
    return; // quit early - don"t know what time it is
  }
  if(!getLocalTime(&now_tm)){ // this function will block for 5sec, if Real Time Clock < 1 Jan 2016
    Serial.println("Failed to obtain time");
    return;
  }
  _12hour = now_tm.tm_hour % 12;
  if (_12hour == 0) {
    _12hour = 12;
  }

  // Hours
#ifdef USE_MATRIX
  if (_12hour >= 10) {
    matrix[0].writeDigitNum(0, _12hour/10);
  } else {
    matrix[0].writeDigitRaw(0, 0 /* all segments off */);
  }
  matrix[0].writeDigitNum(1, _12hour%10);
  // Minutes
  matrix[0].writeDigitNum(3, now_tm.tm_min/10);
  matrix[0].writeDigitNum(4, now_tm.tm_min%10);

  matrix[0].drawColon(true);

  if (app_settings->last_colon) { // use on large 1.2" red and yellow displays, which do not have a decimal point here.
    // Tip: cover up the top of the final colon with black duct tape. Now it looks like a (slightly raised) decimal point.
    matrix[1].writeDigitRaw(2, 0xe); // middle and left colons: location 2, bits (0x8 + 0x4 + 0x2 = 0xe)
  } else {
    matrix[1].writeDigitRaw(2, 0xc); // left colon: location 2, both bits (0x8 + 0x4 = 0xc)
  }
  // Seconds
  matrix[1].writeDigitNum(0, now_tm.tm_sec/10);
  matrix[1].writeDigitNum(1, now_tm.tm_sec%10, true /* draw decimal, 0.56" height only*/);

  // fractional seconds
  matrix[1].writeDigitNum(3, tv_now.tv_usec/100000);
  matrix[1].writeDigitNum(4, (tv_now.tv_usec/10000) % 10);
#endif // USE_MATRIX

#ifdef USE_ALPHA
  // Month
  mon_str = monNames[now_tm.tm_mon];
  alpha4[0].writeDigitAscii(0, mon_str[0]);
  alpha4[0].writeDigitAscii(1, mon_str[1]);
  alpha4[0].writeDigitAscii(2, mon_str[2]);

  // Day of Month
  if (now_tm.tm_mday >= 10) {
    alpha4[0].writeDigitAscii(3, '0' + (now_tm.tm_mday/10));
  } else {
    alpha4[0].writeDigitRaw(3, 0 /* all segments off */);
  }
  alpha4[1].writeDigitAscii(0, '0' + (now_tm.tm_mday%10));

  // Day of Week
  wday_str = dayNames[now_tm.tm_wday];
  alpha4[1].writeDigitAscii(1, wday_str[0]);
  alpha4[1].writeDigitAscii(2, wday_str[1]);
  alpha4[1].writeDigitAscii(3, wday_str[2]);
#endif

#if 0
  // Year
  year_int = now_tm.tm_year + 1900;
  alpha4[2].writeDigitAscii(0, '0' + ( year_int/1000));
  alpha4[2].writeDigitAscii(1, '0' + ((year_int/ 100)%10));
  alpha4[2].writeDigitAscii(2, '0' + ((year_int/  10)%10));
  alpha4[2].writeDigitAscii(3, '0' + ( year_int      %10));
#endif

#ifdef USE_MATRIX
  matrix[0].writeDisplay();
  matrix[1].writeDisplay();
#endif
#ifdef USE_ALPHA
  alpha4[0].writeDisplay();
  alpha4[1].writeDisplay();
  //alpha4[2].writeDisplay();
#endif
}
