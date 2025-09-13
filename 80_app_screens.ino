// ---------------------------- Define windows + controls. --------------------------
// *** background frame ***
struct windw_s wnd_frame;



// *** wifi networks screen ***
struct list_s lst_nets;

struct tab_ctrl_s tab_ctrl_frame = 
      { ctrl_windw, &wnd_frame, };

struct tab_ctrl_s tab_ctrl_nets = 
      { ctrl_list, &lst_nets, };

struct tab_ctrl_s tab_ctrl_none = 
      { ctrl_type_none,  NULL, };

struct tab_ctrl_s *ctrls_wifi[] =
  {
    &tab_ctrl_frame,
    &tab_ctrl_nets,
    &tab_ctrl_none,
  };

struct ctrl_screen_s wifi_screen = {
  ctrls_wifi,
  1, // focus_idx: ctrl_nets
};


// *** wifi password screen ***
struct windw_s wnd_plain;
struct label_s lbl_instr;
struct label_s lbl_net;
struct edit_s edit_pass;
struct button_s btn_ok_pass;
struct button_s btn_back;

struct tab_ctrl_s tab_ctrl_plain = 
      { ctrl_windw, &wnd_plain, };

struct tab_ctrl_s tab_ctrl_instr = 
      { ctrl_label, &lbl_instr, };

struct tab_ctrl_s tab_ctrl_net = 
      { ctrl_label, &lbl_net, };

struct tab_ctrl_s tab_ctrl_pass = 
      { ctrl_edit, &edit_pass, };

struct tab_ctrl_s tab_ctrl_ok_pass = 
      { ctrl_button, &btn_ok_pass, };

struct tab_ctrl_s tab_ctrl_back = 
      { ctrl_button, &btn_back, };

struct tab_ctrl_s *ctrls_pass[] =
  {
    &tab_ctrl_plain,
    &tab_ctrl_instr,
    &tab_ctrl_net,
    &tab_ctrl_pass,
    &tab_ctrl_ok_pass,
    &tab_ctrl_back,
    &tab_ctrl_none,
  };
  
struct ctrl_screen_s pass_screen = {
  ctrls_pass,
  3, // focus_idx: ctrl_pass
};

// *** offset screen ***
struct label_s lbl_hdr_offset;

struct label_s lbl_utc;
struct up_down_s updn_tzone;
struct check_box_s cb_daylight;

struct label_s lbl_gpsleap;
struct up_down_s updn_leapsec;

struct button_s btn_ok_offs;


struct tab_ctrl_s tab_ctrl_hdr_offset = 
      { ctrl_label, &lbl_hdr_offset, };


struct tab_ctrl_s tab_ctrl_utc = 
      { ctrl_label, &lbl_utc, };

struct tab_ctrl_s tab_ctrl_tzone = 
      { ctrl_up_down, &updn_tzone, };

struct tab_ctrl_s tab_ctrl_daylight = 
      { ctrl_check_box, &cb_daylight, };


struct tab_ctrl_s tab_ctrl_gpsleap = 
      { ctrl_label, &lbl_gpsleap, };

struct tab_ctrl_s tab_ctrl_leapsec = 
      { ctrl_up_down, &updn_leapsec, };


struct tab_ctrl_s tab_ctrl_ok_offs = 
      { ctrl_button, &btn_ok_offs, };


struct tab_ctrl_s *ctrls_offset[] =
  {
    &tab_ctrl_plain,
    &tab_ctrl_hdr_offset,

    &tab_ctrl_utc,
    &tab_ctrl_tzone,
    &tab_ctrl_daylight,

    &tab_ctrl_gpsleap,
    &tab_ctrl_leapsec,

    &tab_ctrl_ok_offs,
    &tab_ctrl_none, // end of list
  };

struct ctrl_screen_s offs_screen = {
  ctrls_offset,
  7, // focus_idx: btn_ok_offs
};


// *** display screen ***
struct label_s lbl_hdr_display;
struct check_box_s cb_lastcolon;

struct label_s lbl_brightness;
struct up_down_s updn_bright;

struct button_s btn_ok_disp;


struct tab_ctrl_s tab_ctrl_hdr_display = 
      { ctrl_label, &lbl_hdr_display, };

struct tab_ctrl_s tab_ctrl_lastcolon = 
      { ctrl_check_box, &cb_lastcolon, };


struct tab_ctrl_s tab_ctrl_brightness = 
      { ctrl_label, &lbl_brightness, };

struct tab_ctrl_s tab_ctrl_bright = 
      { ctrl_up_down, &updn_bright, };


struct tab_ctrl_s tab_ctrl_ok_disp = 
      { ctrl_button, &btn_ok_disp, };


struct tab_ctrl_s *ctrls_display[] =
  {
    &tab_ctrl_plain,
    &tab_ctrl_hdr_display,
    &tab_ctrl_lastcolon,

    &tab_ctrl_brightness,
    &tab_ctrl_bright,

    &tab_ctrl_ok_disp,
    &tab_ctrl_none, // end of list
  };

struct ctrl_screen_s disp_screen = {
  ctrls_display,
  5, // focus_idx: btn_ok_disp
};


// *** gps screen ***
struct label_s lbl_hdr_gps;

struct label_s lbl_datetime;
struct label_s lbl_fixqual;
struct label_s lbl_fix3d;
struct label_s lbl_sats;

struct button_s btn_ok_gps;

struct tab_ctrl_s tab_ctrl_hdr_gps = 
      { ctrl_label, &lbl_hdr_gps, };

struct tab_ctrl_s tab_ctrl_datetime = 
      { ctrl_label, &lbl_datetime, };

struct tab_ctrl_s tab_ctrl_fixqual = 
      { ctrl_label, &lbl_fixqual, };

struct tab_ctrl_s tab_ctrl_fix3d = 
      { ctrl_label, &lbl_fix3d, };

struct tab_ctrl_s tab_ctrl_sats = 
      { ctrl_label, &lbl_sats, };

struct tab_ctrl_s tab_ctrl_ok_gps = 
      { ctrl_button, &btn_ok_gps, };


struct tab_ctrl_s *ctrls_gps[] =
  {
    &tab_ctrl_plain,
    &tab_ctrl_hdr_gps,

    &tab_ctrl_datetime,
    &tab_ctrl_fixqual,
    &tab_ctrl_fix3d,
    &tab_ctrl_sats,

    &tab_ctrl_ok_gps,
    &tab_ctrl_none, // end of list
  };

struct ctrl_screen_s gps_screen = {
  ctrls_gps,
  5, // focus_idx: btn_ok_gps
};

// *** main app screen ***
struct label_s lbl_clock;
struct button_s btn_ip;
struct button_s btn_display;
struct button_s btn_offset;
struct button_s btn_gps;

struct tab_ctrl_s tab_ctrl_clock = 
      { ctrl_label, &lbl_clock, };


struct tab_ctrl_s tab_ctrl_ip = 
      { ctrl_button, &btn_ip, };

struct tab_ctrl_s tab_ctrl_display = 
      { ctrl_button, &btn_display, };

struct tab_ctrl_s tab_ctrl_offset = 
      { ctrl_button, &btn_offset, };

struct tab_ctrl_s tab_ctrl_gps = 
      { ctrl_button, &btn_gps, };


struct tab_ctrl_s *ctrls_app[] =
  {
    &tab_ctrl_plain,
    &tab_ctrl_clock,

    &tab_ctrl_ip,
    &tab_ctrl_display,
    &tab_ctrl_offset,
    &tab_ctrl_gps,

    &tab_ctrl_none, // end of list
  };
  
struct ctrl_screen_s app_screen = {
  ctrls_app,
  1, // focus_idx: ctrl_ip
};

// *** initial screen at start ***
//struct ctrl_screen_s *cur_screen = &wifi_screen;
//struct ctrl_screen_s *cur_screen = &pass_screen;
//struct ctrl_screen_s *cur_screen = &disp_screen;
struct ctrl_screen_s *cur_screen = &app_screen;

// ---------------------------- Set up screen controls -----------------------------------
void setup_app_screens(void) {
  // *** WiFi screen *** - setup elements
  SetupWindow (&wnd_frame, /*x0*/0, /*y0*/0, tft.width()-1, tft.height()-1, /*border*/1, /*buffered*/0);
  SetupList (&lst_nets, /*x*/20, /*y*/12, /*text_max_chars*/16, /*visible_count*/4, /*void cb_func(char *)*/&list_selected, /*item_prefix*/NULL);//"WiFi");

  // *** Password screen *** - setup elements
  //SetupWindow (&wnd_plain, /*x0*/0, /*y0*/0, tft.width()-1, tft.height()-1, /*border*/0, /*buffered*/0);
  SetupWindow (&wnd_plain, /*x0*/0, /*y0*/0, tft.width(), tft.height()-1, /*border*/0, /*buffered*/0);
  SetupLabel (&lbl_instr, /*x*/1, /*y*/1, /*max_chars*/19, /*text_default*/"Enter password for:");
  SetupLabel (&lbl_net, /*x*/1, /*y*/1+LABEL_HEIGHT_PIXELS+1, /*max_chars*/16, /*text_default*/"Default WiFi Name");
  SetupEdit (&edit_pass, /*x*/1, /*y*/1+LABEL_HEIGHT_PIXELS+1+LABEL_HEIGHT_PIXELS+1, /*text_max_chars*/16, "Doyourwork!22");
  SetupButton (&btn_ok_pass, /*x*/80, /*y*/1+LABEL_HEIGHT_PIXELS+1+LABEL_HEIGHT_PIXELS+1+EDIT_HEIGHT_PIXELS+3, "OK", /*void cb_func(void)*/&cb_passwd_ok_clicked);
  SetupButton (&btn_back, /*x*/1, /*y*/1+LABEL_HEIGHT_PIXELS+1+LABEL_HEIGHT_PIXELS+1+EDIT_HEIGHT_PIXELS+3, "Back", /*void cb_func(void)*/&cb_passwd_back_clicked);

  // *** Offset screen *** - setup elements

  //  title
  SetupLabel (&lbl_hdr_offset, /*x*/tft.width()/2-(6*CHAR_WIDTH_PIXELS)/2, /*y*/1, /*max_chars*/7, /*text_default*/"Offset");

  //  UTC offset
  SetupLabel (&lbl_utc, /*x*/1, /*y*/36, /*max_chars*/3, /*text_default*/"UTC");
  SetupUpDown (&updn_tzone, /*x*/60, /*y*/36, /*text_max_chars*/5, &cb_offs_tzone_set/*void cb_func(double sel_val)*/, NULL/*void cb_chg_func(double chg_val)*/, /*min_val*/-12, /*max_val*/+14, /*dfl_val*/-6, /*val_step*/0.5, /*val_prec*/1);

  //  DST check
  SetupCheckBox (&cb_daylight, /*x*/165, /*y*/36, /*text_str*/"DST", 1/*checked_default*/, &cb_offs_dst_set/*void cb_func(int check_val)*/);

  //  Leap sec
  SetupLabel (&lbl_gpsleap, /*x*/1, /*y*/69, /*max_chars*/10, /*text_default*/"Leap sec");
  SetupUpDown (&updn_leapsec, /*x*/140, /*y*/69, /*text_max_chars*/3, NULL/*void cb_func(double sel_val)*/, &cb_offs_leapsec_set/*void cb_chg_func(double chg_val)*/, /*min_val*/-59, /*max_val*/+59, /*dfl_val*/18, /*val_step*/1, /*val_prec*/0);

  SetupButton (&btn_ok_offs, /*x*/80, /*y*/1+LABEL_HEIGHT_PIXELS+1+LABEL_HEIGHT_PIXELS+1+EDIT_HEIGHT_PIXELS+3, "OK", /*void cb_func(void)*/&cb_offs_ok_clicked);


  // *** Display screen *** - setup elements

  //  title
  SetupLabel (&lbl_hdr_display, /*x*/tft.width()/2-(7*CHAR_WIDTH_PIXELS)/2, /*y*/1, /*max_chars*/7, /*text_default*/"Display");

  //  : check
  SetupCheckBox (&cb_lastcolon, /*x*/170, /*y*/1, /*text_str*/":", 0/*!checked_default*/, &cb_disp_colon_set/*void cb_func(int check_val)*/);

  //  Brightness
  SetupLabel (&lbl_brightness, /*x*/1, /*y*/69, /*max_chars*/10, /*text_default*/"Brightness");
  SetupUpDown (&updn_bright, /*x*/160, /*y*/69, /*text_max_chars*/2, NULL/*void cb_func(double sel_val)*/, &cb_disp_bright_set/*void cb_chg_func(double chg_val)*/, /*min_val*/-1, /*max_val*/+16, /*dfl_val*/16, /*val_step*/1, /*val_prec*/0);

  // OK
  SetupButton (&btn_ok_disp, /*x*/80, /*y*/1+LABEL_HEIGHT_PIXELS+1+LABEL_HEIGHT_PIXELS+1+EDIT_HEIGHT_PIXELS+3, "OK", /*void cb_func(void)*/&cb_disp_ok_clicked);

  // *** GPS screen *** - setup elements

  //  title
  SetupLabel (&lbl_hdr_gps, /*x*/tft.width()/2-(3*CHAR_WIDTH_PIXELS)/2, /*y*/1, /*max_chars*/3, /*text_default*/"GPS");

  //  GPS datetime
  SetupLabel (&lbl_datetime, /*x*/1, /*y*/36, /*max_chars*/19, /*text_default*/"starting GPS ...");

  //  fix quality
  SetupLabel (&lbl_fixqual, /*x*/24, /*y*/69, /*max_chars*/6, /*text_default*/"fix: no");

  //  fix 3D quality
  SetupLabel (&lbl_fix3d, /*x*/130, /*y*/69, /*max_chars*/5, /*text_default*/"3D: no");

  //  satellite count
  SetupLabel (&lbl_sats, /*x*/24, /*y*/102, /*max_chars*/7, /*text_default*/"no sats");

  // OK
  SetupButton (&btn_ok_gps, /*x*/140, /*y*/1+LABEL_HEIGHT_PIXELS+1+LABEL_HEIGHT_PIXELS+1+EDIT_HEIGHT_PIXELS+3, "OK", /*void cb_func(void)*/&cb_gps_ok_clicked);

  // *** App screen *** - setup elements

  //  title
  SetupLabel (&lbl_clock, /*x*/tft.width()/2-(5*CHAR_WIDTH_PIXELS)/2, /*y*/1, /*max_chars*/5, /*text_default*/"Clock");

  //  WiFi IP / NO Wifi button
  SetupButton (&btn_ip, /*x horiz*/24, /*y vert*/36, "000.000.000.000", /*void cb_func(void)*/&cb_app_ip_clicked); // force button to be big enough for full IP

  //  Display button
  SetupButton (&btn_display, /*x horiz*/24, /*y vert*/69, "Display", /*void cb_func(void)*/&cb_app_display_clicked);

  //  Offset button
  SetupButton (&btn_offset, /*x horiz*/130, /*y vert*/69, "Offset", /*void cb_func(void)*/&cb_app_offset_clicked);

  //  GPS status / No GPS button
//  SetupButton (&btn_gps, /*x*/1, /*y*/102, /*max_chars*/19, /*text_default*/"starting GPS ...");
  SetupButton (&btn_gps, /*x*/24, /*y*/102, "starting GPS", /*void cb_func(void)*/&cb_app_gps_clicked); // force button to be big enough for text


  app_prefs_get (app_settings);
  set_net_label (app_settings->ssid);
  SetEdit     (&edit_pass,   app_settings->password);

  SetUpDown   (&updn_tzone,  app_settings->gmt_offs);
  SetCheckBox (&cb_daylight, app_settings->dst);
  SetUpDown   (&updn_leapsec,  app_settings->leap_sec);

  SetCheckBox (&cb_lastcolon, app_settings->last_colon);
  SetUpDown   (&updn_bright,  app_settings->bright_steps);
  app_connect();
  ScreenDraw();
}
// ---------------------------- Manage list -----------------------------------
// set an item in the list
void set_list_item (int item_idx, const char *new_fmt, ...) {
  char new_text[LIST_MAX_TEXT];

  va_list args;
  va_start (args, new_fmt);
  vsnprintf (new_text, LIST_MAX_TEXT, new_fmt, args);
  va_end (args);

  SetListItem (&lst_nets, item_idx, new_text);
}

// add an item to the list
//  if it already exists, nothing will be added.
void add_list_item (const char *new_fmt, ...) {
  char new_text[LIST_MAX_TEXT];

  va_list args;
  va_start (args, new_fmt);
  vsnprintf (new_text, LIST_MAX_TEXT, new_fmt, args);
  va_end (args);

  AddListItem (&lst_nets, new_text);
}

// redraw the list
//  Call this after done setting/adding items.
void redraw_list (void) {
  if (lst_nets.needs_redraw) { // need to redraw the list of wifi networks (b/c we added 1 or more networks)
    ScreenRedrawCtrl(&tab_ctrl_nets);
  }
}

void list_selected(char *sel_net) {
  // Accept WiFi network
  strncpy (app_settings->ssid, sel_net, APP_SETTINGS_STR_MAX);
  // setup control on next screen with WiFi network name.
  set_net_label (sel_net);

  // Change to password screen.
  cur_screen = &pass_screen;
  ScreenDraw();
}

bool current_screen_is_wifi(void) {
  if (cur_screen == &wifi_screen) {
    return true; // we are looking at the screen that shows the wifi networks
  }
  return false;
}
// ---------------------------- WiFi Password Screen functions --------------------------
// Set WiFi Network label
void set_net_label (char *new_fmt, ...) {
  char new_text[LABEL_MAX_TEXT];
  va_list args;

  va_start (args, new_fmt);
  vsnprintf (new_text, LABEL_MAX_TEXT, new_fmt, args);
  va_end (args);

  SetLabel (&lbl_net, new_text);
}

void cb_passwd_back_clicked (void) {
  // Change back to WiFi (network selection) screen.
  cur_screen = &wifi_screen;
  ScreenDraw();
}

void cb_passwd_ok_clicked (void) {
  char *password;

  // Accept WiFi password.
  password = GetEdit(&edit_pass);
  strncpy (app_settings->password, password, APP_SETTINGS_STR_MAX);

  // start trying to connect.
  app_connect();

  // Go to main app screen.
  cur_screen = &app_screen;
  ScreenDraw();
}
// ---------------------------- Offset Screen functions --------------------------
void cb_offs_tzone_set (double tzone_hrs) {
 app_settings->gmt_offs = tzone_hrs; 
 app_prefs_put (app_settings);
 app_time_set();
}

void cb_offs_dst_set (int dst_checked) {
 app_settings->dst = dst_checked; 
 app_prefs_put (app_settings);
 app_time_set();
}

void cb_offs_leapsec_set (double leapsec_in) {
 app_settings->leap_sec = leapsec_in; 
 app_prefs_put (app_settings);
 app_time_set();
}

void cb_offs_ok_clicked (void) {
  // Go to main app screen.
  cur_screen = &app_screen;
  ScreenDraw();
}
// ---------------------------- Display Screen functions --------------------------
void cb_disp_colon_set (int colon_checked) {
 app_settings->last_colon = colon_checked; 
 app_prefs_put (app_settings);
}

void cb_disp_bright_set (double bright_steps) {
 app_settings->bright_steps = bright_steps; 
 app_prefs_put (app_settings);
 sevenseg_bright_set ();
}

void cb_disp_ok_clicked (void) {
  // Go to main app screen.
  cur_screen = &app_screen;
  ScreenDraw();
}
// ---------------------------- GPS Screen functions --------------------------
// Set label on this screen.
// If this screen is showing, and the text changed,
//  then draw the label.
void set_gps_v_label (struct label_s *lbl, char *new_fmt, va_list args) {
  char new_text[LABEL_MAX_TEXT];
  bool chg;

  vsnprintf (new_text, LABEL_MAX_TEXT, new_fmt, args);

  chg = SetLabel (lbl, new_text);
  //ScreenDraw();
  if (chg && current_screen_is_gps()) {
    DrawLabel (lbl, false/*!has_focus*/);
  }
}

// Set GPS DateTime label
void set_gps_datetime_label (char *new_fmt, ...) {
  va_list args;
  va_start (args, new_fmt);
  set_gps_v_label (&lbl_datetime, new_fmt, args);
  va_end (args);
}

// Set GPS Fix Quality label
void set_gps_fix_label (char *new_fmt, ...) {
  va_list args;
  va_start (args, new_fmt);
  set_gps_v_label (&lbl_fixqual, new_fmt, args);
  va_end (args);
}

// Set GPS Fix 3D label
void set_gps_fix3d_label (char *new_fmt, ...) {
  va_list args;
  va_start (args, new_fmt);
  set_gps_v_label (&lbl_fix3d, new_fmt, args);
  va_end (args);
}

// Set GPS Satellites label
void set_gps_sats_label (char *new_fmt, ...) {
  va_list args;
  va_start (args, new_fmt);
  set_gps_v_label (&lbl_sats, new_fmt, args);
  va_end (args);
}

bool current_screen_is_gps(void) {
  if (cur_screen == &gps_screen) {
    return true; // we are looking at the screen that shows the GPS details
  }
  return false;
}

void cb_gps_ok_clicked (void) {
  // Go to main app screen.
  cur_screen = &app_screen;
  ScreenDraw();
}
// ---------------------------- Main App Screen functions --------------------------
// Set text on IP Address button
void set_ip_btn (const char *new_fmt, ...) {
  char new_text[BUTTON_MAX_LABEL];

  va_list args;
  va_start (args, new_fmt);
  vsnprintf (new_text, BUTTON_MAX_LABEL, new_fmt, args);
  va_end (args);

  SetButtonText (&btn_ip, new_text);
  ScreenRedrawCtrl(&tab_ctrl_ip);
}

bool current_screen_is_app(void) {
  if (cur_screen == &app_screen) {
    return true; // we are looking at the screen that shows the main app controls
  }
  return false;
}

void app_connect(void) {
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(app_settings->ssid, app_settings->password);
  set_ip_btn ("Connecting");
}

bool is_wifi_connected(void) {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  return false;
}
bool wifi_has_ip(void) {
  IPAddress my_ip_addr;

  my_ip_addr = WiFi.localIP();
  if (my_ip_addr[0] == 0 &&
      my_ip_addr[1] == 0 &&
      my_ip_addr[2] == 0 &&
      my_ip_addr[3] == 0) {
    return false;
  }
  return true;
}

const char* ntpServer = "pool.ntp.org";
void app_time_set (void) {
  double tzone_hr;
  int gmtOffset_sec;
  int dst_checked;
  int daylightOffset_sec = 0;

  tzone_hr = app_settings->gmt_offs;//GetUpDown(&updn_tzone);
  gmtOffset_sec = tzone_hr * 3600; // convert FROM: hours offset TO: seconds offset

  dst_checked = app_settings->dst;//GetCheckBox(&cb_daylight);
  if (dst_checked) {
    daylightOffset_sec = 3600; // 1 hour
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

int was_connected = 0;//-1; // force 'NO WiFi', 'NO IP' or ip address to be set
int had_ip = 0;//-1;        // force 'NO WiFi', 'NO IP' or ip address to be set
bool did_init = false;

void app_update(void) {
  bool is_connected;
  bool has_ip = false;

  if (!did_init) {
    app_connect(); // wait until now to do initial connection?
  }

  is_connected = is_wifi_connected();
  if (is_connected) {
    has_ip = wifi_has_ip();
  }
  //Serial.print("app_update did_init: ");
  //Serial.print(did_init);
  //Serial.print(", was_connected: ");
  //Serial.print(was_connected);
  //Serial.print(", is_connected: ");
  //Serial.print(is_connected);
  //Serial.print(", has_ip: ");
  //Serial.println(has_ip);
  if ((!did_init || !was_connected) && is_connected) {
    if ((!did_init || !had_ip) && has_ip) {
      IPAddress my_ip_addr;

      my_ip_addr = WiFi.localIP();
      set_ip_btn ("%d.%d.%d.%d", my_ip_addr[0], my_ip_addr[1], my_ip_addr[2], my_ip_addr[3]);

      // save successful WiFi ssid and password.
      app_prefs_put (app_settings);
#ifdef OTA_UPDATE
      over_the_air_update_setup ();
#endif
      app_time_set();
    }
    if ((!did_init || had_ip) && !has_ip) {
      set_ip_btn ("NO IP");
    }
  }
  if ((!did_init || was_connected) && !is_connected) {
    set_ip_btn ("NO WiFi");
  }
  was_connected = is_connected;
  had_ip        = has_ip;
  did_init      = true;
}

void cb_app_ip_clicked (void) {
  // Change back to WiFi (network selection) screen.
  cur_screen = &wifi_screen;
  ScreenDraw();
}

void cb_app_offset_clicked (void) {
  // Change to Offset screen.
  cur_screen = &offs_screen;
  ScreenDraw();
}

void cb_app_display_clicked (void) {
  // Change to Display screen.
  cur_screen = &disp_screen;
  ScreenDraw();
}

void cb_app_gps_clicked (void) {
  // Change to GPS screen.
  cur_screen = &gps_screen;
  ScreenDraw();
}

// Set text on GPS button
void set_app_gps_btn (char *new_fmt, ...) {
  char new_text[BUTTON_MAX_LABEL];
  bool chg;

  va_list args;
  va_start (args, new_fmt);
  vsnprintf (new_text, BUTTON_MAX_LABEL, new_fmt, args);
  va_end (args);

  chg = SetButtonText (&btn_gps, new_text);
  //ScreenDraw();
  if (chg && current_screen_is_app()) {
    ScreenRedrawCtrl (&tab_ctrl_gps);
  }
}