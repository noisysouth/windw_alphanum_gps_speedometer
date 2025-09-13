/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 */
#ifdef ESP8266
#  include <ESP8266WiFi.h>
#else // ESP32
#  include "WiFi.h"
#endif

// Store last WiFi ssid and password. Docs: https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
#include <Preferences.h>

//#define OTA_UPDATE
#ifdef OTA_UPDATE
// Over-the-air updates ('OTA'): https://lastminuteengineers.com/esp32-ota-updates-arduino-ide/
# include <ESPmDNS.h>
# include <WiFiUdp.h>
# include <ArduinoOTA.h>
#endif

#include "my_tft.h"

void setup()
{
  delay(3500); // to get Serial Monitor COM port to enumerate, a long delay BEFORE Serial.begin() is key..
  Serial.begin(115200);
  Serial.println("Opened serial");
  //delay(2000);

  setup_tft();
  setup_app_screens();

  setup_input_esp32_button();

  Wire.begin();        // join i2c bus (address optional for master)
  setup_input_gamepad();
  setup_gps();

  setup_wifi();

  sevenseg_setup();
  Serial.println("Setup done");
}

// do_click can be set by touch, kb_in Enter press, or digital input button.
bool  do_click  = false;
bool was_click  = false;

bool  do_unclick = false;
int until_scan = 0;
int until_io = 0;
void loop()
{
  char kb_in = 0; // no keyboard input

  //Serial.println("loop() start");

  if (until_io <= 0) {
    do_unclick = false;

    if (current_screen_is_wifi()) {
      if (until_scan <= 0) {
        loop_wifi_scan(); // do a scan
        until_scan = 50; // Wait a bit before scanning again.
      }
      until_scan--;
    } else { // !current_screen_is_wifi()
      // looking at some other screen
      until_scan = 50; // if we return to the wifi screen, wait before scanning again.
    }
    if (current_screen_is_app()) {
      app_update();
    }

    loop_input_esp32_button();
    loop_input_gamepad();
    kb_in = loop_input_cardkb();

    loop_screen(kb_in, do_unclick);
    until_io = 10; // Wait a bit before doing I/O again.
  }
  until_io--;

#ifdef OTA_UPDATE
  if (is_wifi_connected() && wifi_has_ip()) {
    over_the_air_update_loop();
  } else {
    delay(10);
  }
#else
  // delay(10);
#endif

  loop_gps();
  sevenseg_loop();

  //Serial.println("loop() end");
}
