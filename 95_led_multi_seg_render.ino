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
//#define USE_7SEG_NUMERIC // 7 segment numeric display(s)
#define USE_ALPHA // 14-segment alphanumeric display(s)

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define TIMET_1JAN2016 1451628000

enum led_seg_field_e {
  led_field_hours_minutes = 0,    // e.g. 11:59
  led_field_seconds_milliseconds, // e.g. :59.99
  led_field_seconds_am_pm,        // e.g. :59pm
  led_field_date_begin,           // e.g. Dec3
  led_field_date_end,             // e.g.     1Mon
  led_field_year,                 // e.g. 2025
  led_field_velocity,
  led_field_latitude,
  led_field_longitude,
  led_seg_field_count,
};

const int _7seg_numeric_count = 0;
      int _7seg_numeric_addr[_7seg_numeric_count];// = { 0x70, 0x71, };
      int _7seg_numeric_field[_7seg_numeric_count];// = { led_field_hours_minutes, led_field_seconds_milliseconds };
Adafruit_7segment _7seg_numeric[_7seg_numeric_count];

const int  alpha_count = 2;
      // THESE ADDRESS VALUES DEPEND ON THE A0,A1,A2 SOLDER BRIDGES ON THE BACK OF THE LED I2C BACKPACK MODULE
      int  alpha_addr[alpha_count] = { 0x70, 0x71, }; // { 0x72, 0x73, 0x74, };
      int  alpha_field[alpha_count] = { led_field_hours_minutes, led_field_seconds_am_pm }; // what we want to display on each modules
Adafruit_AlphaNum4 alpha4[alpha_count];


char *dayNames[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char *monNames[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

void led_multi_seg_bright_set(void) {
  int idx;

  if (app_settings->bright_steps >= 0) {
    for (idx = 0; idx < _7seg_numeric_count; idx++) {
      _7seg_numeric[idx].setDisplayState(1);
      _7seg_numeric[idx].setBrightness(app_settings->bright_steps);
    }
    for (idx = 0; idx < alpha_count; idx++) {
      alpha4[idx].setDisplayState(1);
      alpha4[idx].setBrightness(app_settings->bright_steps);
    }
    if (app_settings->bright_steps == 0) {
      ScreenDraw(); // draw not-dimmed
    }
  } else { // bright_steps < 0
    for (idx = 0; idx < _7seg_numeric_count; idx++) {
      _7seg_numeric[idx].setDisplayState(0);
    }
    for (idx = 0; idx < alpha_count; idx++) {
      alpha4[idx].setDisplayState(0);
    }
    ScreenDraw(); // draw dimmer
  }
}

void led_multi_seg_setup(void) {
  int idx;

  for (idx = 0; idx < _7seg_numeric_count; idx++) {
    _7seg_numeric[idx] = Adafruit_7segment();
    _7seg_numeric[idx].begin(_7seg_numeric_addr[idx]);
  }
  if (_7seg_numeric_count > 0) {
    if (_7seg_numeric_field[0] == led_field_hours_minutes) {
      _7seg_numeric[0].drawColon(true);
      _7seg_numeric[0].writeDisplay();
    }
  }

  for (idx = 0; idx < alpha_count; idx++) {
    alpha4[idx] = Adafruit_AlphaNum4();
    alpha4[idx].begin(alpha_addr[idx]);
  }

  led_multi_seg_bright_set();
  Serial.println("Multi-Segment LED Backpacks Initialized");
}

// *** Rendering functions ***

// 7-segment numeric display 'idx'
//  update with hours and minutes  HH:MM
static void led_7seg_numeric_render_hours_minutes(int idx, struct tm *now_tm) {
  int _12hour;

  _12hour = now_tm->tm_hour % 12;
  if (_12hour == 0) {
    _12hour = 12;
  }

  if (_12hour >= 10) {
    _7seg_numeric[idx].writeDigitNum(0, _12hour/10);
  } else {
    _7seg_numeric[idx].writeDigitRaw(0, 0 /* all segments off */);
  }
  _7seg_numeric[idx].writeDigitNum(1, _12hour%10);
  // Minutes
  _7seg_numeric[idx].writeDigitNum(3, now_tm->tm_min/10);
  _7seg_numeric[idx].writeDigitNum(4, now_tm->tm_min%10);

  _7seg_numeric[idx].drawColon(true);
}

// 14-segment alpha display 'idx'
//  update with hours and minutes  HH.MM.
static void led_alpha_render_hours_minutes(int idx, struct tm *now_tm) {
  int _12hour;

  _12hour = now_tm->tm_hour % 12;
  if (_12hour == 0) {
    _12hour = 12;
  }

  if (_12hour >= 10) {
    alpha4[idx].writeDigitAscii(0, '0' + _12hour/10);
  } else {
    alpha4[idx].writeDigitRaw(0, 0 /* all segments off */);
  }
  alpha4[idx].writeDigitAscii(1, '0' + _12hour%10, true); // include decimal
  // Minutes
  alpha4[idx].writeDigitAscii(2, '0' + now_tm->tm_min/10);
  alpha4[idx].writeDigitAscii(3, '0' + now_tm->tm_min%10, true); // include decimal
}

// 7-segment numeric display 'idx'
//  update with seconds and milliseconds  :SS.mm
static void led_7seg_numeric_render_seconds_milliseconds(int idx, struct tm *now_tm, struct timeval *tv_now) {
  if (app_settings->last_colon) { // use on large 1.2" red and yellow displays, which do not have a decimal point here.
    // Tip: cover up the top of the final colon with black duct tape. Now it looks like a (slightly raised) decimal point.
    _7seg_numeric[idx].writeDigitRaw(2, 0xe); // middle and left colons: location 2, bits (0x8 + 0x4 + 0x2 = 0xe)
  } else {
    _7seg_numeric[idx].writeDigitRaw(2, 0xc); // left colon: location 2, both bits (0x8 + 0x4 = 0xc)
  }
  // Seconds
  _7seg_numeric[idx].writeDigitNum(0, now_tm->tm_sec/10);
  _7seg_numeric[idx].writeDigitNum(1, now_tm->tm_sec%10, true /* draw decimal, 0.56" height only*/);

  // fractional seconds
  _7seg_numeric[idx].writeDigitNum(3,  tv_now->tv_usec/100000);
  _7seg_numeric[idx].writeDigitNum(4, (tv_now->tv_usec/10000) % 10);
}

// 14-segment alpha display 'idx'
//  update with seconds and milliseconds  SS.mm
static void led_alpha_render_seconds_milliseconds(int idx, struct tm *now_tm, struct timeval *tv_now) {
  // Seconds
  alpha4[idx].writeDigitAscii(0, '0' + now_tm->tm_sec/10);
  alpha4[idx].writeDigitAscii(1, '0' + now_tm->tm_sec%10, true /* draw decimal, 0.56" height only*/);

  // fractional seconds
  alpha4[idx].writeDigitAscii(2, '0' +  tv_now->tv_usec/100000);
  alpha4[idx].writeDigitAscii(3, '0' + (tv_now->tv_usec/10000) % 10);
}

// 14-segment alpha display 'idx'
//  update with seconds am or pm  SSam or SSpm
static void led_alpha_render_seconds_am_pm(int idx, struct tm *now_tm) {
  bool is_am;

  if (now_tm->tm_hour < 12) {
    is_am = true;
  } else {
    is_am = false;
  }
  
  alpha4[idx].writeDigitAscii(0, '0' + now_tm->tm_sec/10);
  alpha4[idx].writeDigitAscii(1, '0' + now_tm->tm_sec%10);

  // am/pm
  if (is_am) {
    alpha4[idx].writeDigitAscii(2, 'a');
    alpha4[idx].writeDigitAscii(3, 'm');
  } else { // !is_am
    alpha4[idx].writeDigitAscii(2, 'p');
    alpha4[idx].writeDigitAscii(3, 'm');
  }
}

// 14-segment alpha display 'idx'
//  update with date start, e.g. Dec3
static void led_alpha_render_date_begin(int idx, struct tm *now_tm) {
  char *mon_str;

  // Month
  mon_str = monNames[now_tm->tm_mon];
  alpha4[idx].writeDigitAscii(0, mon_str[0]);
  alpha4[idx].writeDigitAscii(1, mon_str[1]);
  alpha4[idx].writeDigitAscii(2, mon_str[2]);

  // Day of Month
  //  10s digit (for 1s digit, see field 'date_end')
  if (now_tm->tm_mday >= 10) {
    alpha4[idx].writeDigitAscii(3, '0' + (now_tm->tm_mday/10));
  } else {
    alpha4[idx].writeDigitRaw(3, 0 /* all segments off */);
  }
}

// 14-segment alpha display 'idx'
//  update with date end, e.g. 1Mon
static void led_alpha_render_date_end(int idx, struct tm *now_tm) {
  char *wday_str;

  // Day of Month
  //  1s digit (for 10s digit, see field 'date_start')
  alpha4[idx].writeDigitAscii(0, '0' + (now_tm->tm_mday%10));

  // Day of Week
  wday_str = dayNames[now_tm->tm_wday];
  alpha4[idx].writeDigitAscii(1, wday_str[0]);
  alpha4[idx].writeDigitAscii(2, wday_str[1]);
  alpha4[idx].writeDigitAscii(3, wday_str[2]);
}

// 14-segment alpha display 'idx'
//  update with year, e.g. 2025
static void led_alpha_render_year(int idx, struct tm *now_tm) {
  int year_int;

  // Year
  year_int = now_tm->tm_year + 1900;
  alpha4[idx].writeDigitAscii(0, '0' + ( year_int/1000));
  alpha4[idx].writeDigitAscii(1, '0' + ((year_int/ 100)%10));
  alpha4[idx].writeDigitAscii(2, '0' + ((year_int/  10)%10));
  alpha4[idx].writeDigitAscii(3, '0' + ( year_int      %10));
}

// 14-segment alpha display 'idx'
//  update with velocity
static void led_alpha_render_velocity(int idx) {
  // TODO
}

// 14-segment alpha display 'idx'
//  update with latitude
static void led_alpha_render_latitude(int idx) {
  // TODO
}

// 14-segment alpha display 'idx'
//  update with longitude
static void led_alpha_render_longitude(int idx) {
  // TODO
}



// *** top level for this file ***
// every iteration, update the display with latest values
void led_multi_seg_loop(void) {
  struct timeval tv_now;
  struct tm now_tm;
  int idx;

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

  for (idx = 0; idx < _7seg_numeric_count; idx++) {
    switch (_7seg_numeric_field[idx]) {
    case led_field_hours_minutes:
      led_7seg_numeric_render_hours_minutes (idx, &now_tm);
      break;
    case led_field_seconds_milliseconds:
      led_7seg_numeric_render_seconds_milliseconds (idx, &now_tm, &tv_now);
      break;
    default: // no change. Alternative: blank it
      break;
    }
  }

  for (idx = 0; idx < alpha_count; idx++) {
    switch (alpha_field[idx]) {
    case led_field_hours_minutes:
      led_alpha_render_hours_minutes (idx, &now_tm);
      break;
    case led_field_seconds_milliseconds:
      led_alpha_render_seconds_milliseconds (idx, &now_tm, &tv_now);
      break;
    case led_field_seconds_am_pm:
      led_alpha_render_seconds_am_pm (idx, &now_tm);
      break;
    case led_field_date_begin:
      led_alpha_render_date_begin (idx, &now_tm);
      break;
    case led_field_date_end:
      led_alpha_render_date_end (idx, &now_tm);
      break;
    case led_field_year:
      led_alpha_render_year (idx, &now_tm);
      break;
    case led_field_velocity:
      led_alpha_render_velocity (idx);
      break;
    case led_field_latitude:
      led_alpha_render_latitude (idx);
      break;
    case led_field_longitude:
      led_alpha_render_longitude (idx);
      break;
    default: // no change. Alternative: blank it
      break;
    }
  }

  for (idx = 0; idx < _7seg_numeric_count; idx++) {
    _7seg_numeric[idx].writeDisplay();
  }
  for (idx = 0; idx < alpha_count; idx++) {
    alpha4[idx].writeDisplay();
  }
}
