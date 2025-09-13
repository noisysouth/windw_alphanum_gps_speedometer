// Preferences namespace where app settings are stored

#define PREFS_CREDS_SECTION "credentials"
#define PREFS_TIME_SECTION  "time"
#define PREFS_DISPLAY_SECTION  "display"

#define APP_SETTINGS_STR_MAX 100

struct app_settings_s {
  // 'credentials' section
  char ssid    [APP_SETTINGS_STR_MAX];
  char password[APP_SETTINGS_STR_MAX];
  // 'time' section
  bool dst;
  double  gmt_offs;
  int leap_sec;
  int bright_steps;
  bool last_colon;
};

struct app_settings_s app_settings[1];

Preferences preferences;

void app_prefs_print (struct app_settings_s *app_settings) {
  Serial.print("ssid: ");
  Serial.println(app_settings->ssid);
  Serial.print("password: ");
  Serial.println(app_settings->password);

  Serial.print("dst: ");
  Serial.println(app_settings->dst);
  Serial.print("gmt_offs: ");
  Serial.println(app_settings->gmt_offs);
  Serial.print("leap_sec: ");
  Serial.println(app_settings->leap_sec);

  Serial.print("bright_steps: ");
  Serial.println(app_settings->bright_steps);
  Serial.print("last_colon: ");
  Serial.println(app_settings->last_colon);
}

void app_prefs_put (struct app_settings_s *app_settings) {
  Serial.println("Writing preferences...");
  preferences.begin(PREFS_CREDS_SECTION, false /*not read-only*/);
  preferences.putString("ssid",     app_settings->ssid); 
  preferences.putString("password", app_settings->password);
  preferences.end(); // let other namespaces be used later.

  preferences.begin(PREFS_TIME_SECTION, false /*not read-only*/);
  preferences.putBool("dst", app_settings->dst);
  preferences.putDouble("gmt_offs", app_settings->gmt_offs);
  preferences.putInt("leap_sec", app_settings->leap_sec);
  preferences.end(); // let other namespaces be used later.

  preferences.begin(PREFS_DISPLAY_SECTION, false /*not read-only*/);
  preferences.putInt("bright_steps", app_settings->bright_steps);
  preferences.putBool("last_colon", app_settings->last_colon);
  preferences.end(); // let other namespaces be used later.

  Serial.println("Wrote:");
  app_prefs_print(app_settings);
}

void app_prefs_get (struct app_settings_s *app_settings) {
  Serial.println("Reading preferences...");

  preferences.begin(PREFS_CREDS_SECTION, true /*read-only*/);
  preferences.getString("ssid",     app_settings->ssid,     APP_SETTINGS_STR_MAX); 
  preferences.getString("password", app_settings->password, APP_SETTINGS_STR_MAX);
  preferences.end(); // let other namespaces be used later.

  preferences.begin(PREFS_TIME_SECTION, true /*read-only*/);
  app_settings->dst = preferences.getBool("dst", true/*default to observing Daylight Saving Time*/);
  app_settings->gmt_offs = preferences.getDouble("gmt_offs", -6.0/*default to  USA Central Standard Time (CST)*/);
  app_settings->leap_sec = preferences.getInt("leap_sec", 18/*announced to date*/);
  preferences.end(); // let other namespaces be used later.

  preferences.begin(PREFS_DISPLAY_SECTION, true /*read-only*/);
  app_settings->bright_steps = preferences.getInt("bright_steps", 16/*full brightness*/);
  app_settings->last_colon = preferences.getInt("last_colon", false);
  preferences.end(); // let other namespaces be used later.

  Serial.println("Read:");
  app_prefs_print(app_settings);
}