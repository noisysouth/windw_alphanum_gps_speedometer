#define USE_GPS
#ifdef USE_GPS
#include <Adafruit_GPS.h>

// Connect to the GPS on the hardware I2C port
Adafruit_GPS GPS(&Wire);
#endif

bool gps_found = false;

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

void setup_gps(void) {
  // --------------------------------- Start GPS input ----------------------------
#ifdef USE_GPS
  delay(3000);
 // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  gps_found = GPS.begin(0x10);  // The I2C address to use is 0x10

  if (!gps_found) {
    Serial.println("GPS not found - no GPS will be used.");
    set_app_gps_btn ("No GPS");
    set_gps_datetime_label ("No GPS on I2C");
  } else { // gps_found
    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    // uncomment this line to turn on only the "minimum recommended" data
    //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

    // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
    // the parser doesn't care about other sentences at this time
    // Set the update rate
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
    // For the parsing code to work nicely and have time to sort thru the data, and
    // print it out we don't suggest using anything higher than 1 Hz

    // Request updates on antenna status, comment out to keep quiet
    GPS.sendCommand(PGCMD_ANTENNA);

    delay(1000);

    // Ask for firmware version
    GPS.println(PMTK_Q_RELEASE);

    set_app_gps_btn ("GPS..");
    set_gps_datetime_label ("GPS syncing ..");
  }
#else
  Serial.println("GPS input skipped - compiled without USE_GPS defined.");
  set_app_gps_btn ("GPS Off");
  set_gps_datetime_label ("GPS Disabled");
#endif
}

// ---------------- read GPS latest message ------------
void loop_gps(void) {
#ifdef USE_GPS
  if (!gps_found) {
    return;
  }
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO) {
    if (c) {
      Serial.print(c);
    }
  }
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    struct tm gps_tm;
    struct timeval tv_gps;
    static int last_gps_sec = -1;
    //static int last_gps_min = -1;
    //static int last_gps_hr = -1;

    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) { // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
    }
    //if (last_gps_sec == GPS.seconds) {
      //return; // Don't set the clock too often.
    //}
    last_gps_sec = GPS.seconds;
    gps_tm.tm_year = (GPS.year + 2000) - 1900;
    gps_tm.tm_mon  = GPS.month - 1;
    gps_tm.tm_mday = GPS.day;

    gps_tm.tm_hour = GPS.hour;
    gps_tm.tm_min  = GPS.minute;
    gps_tm.tm_sec  = GPS.seconds;

    tv_gps.tv_usec = GPS.milliseconds * 1000;
    time_ref_put_gps (&gps_tm, &tv_gps);

    // Set button on App page with satellite count
    set_app_gps_btn ("%d GPS", GPS.satellites);

    // Set labels on GPS page
    set_gps_datetime_label ("%04d-%02d-%02d %02d:%02d:%02d",
                            GPS.year, GPS.month, GPS.day,
                            GPS.hour, GPS.minute, GPS.seconds);
    set_gps_fix_label ("fix: %d", GPS.fixquality);
    set_gps_fix3d_label ("3D: %d", GPS.fixquality_3d);
    set_gps_sats_label ("%d sats", GPS.satellites);
  }
#endif
}