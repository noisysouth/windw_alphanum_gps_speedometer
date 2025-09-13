//#define TIME_REF_DEBUG_GPS

//const char* timezone = "CST-6";

void setup_time_ref(void) {
}

// ---------------- set newest GPS time ------------
void time_ref_put_gps(struct tm *gps_tm, struct timeval *tv_gps) {
  time_t gps_tv_sec;
  struct tm now_tm;

  gps_tv_sec = mktime(gps_tm);
  gps_tv_sec += app_settings->leap_sec;
  gps_tv_sec += app_settings->gmt_offs * 60 * 60;
  tv_gps->tv_sec = gps_tv_sec;

  //setenv("TZ", timezone, 1); // Set the TZ.
  //tzset();

  settimeofday (tv_gps, 0);

  getLocalTime(&now_tm); // this function will block for 5sec, if Real Time Clock < 1 Jan 2016
#ifdef TIME_REF_DEBUG_GPS
  Serial.print("gps_tm->tm_hour: ");
  Serial.print(gps_tm->tm_hour);
  Serial.print(", now_tm.tm_hour: ");
  Serial.println(now_tm.tm_hour);
#endif
}

// ---------------- get time ------------
void time_ref_get(void) {
}