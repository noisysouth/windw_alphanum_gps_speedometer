#define ESP32_REVERSE_BUTTONS

// on-board digital input button
#define PIN_D0_BOOT_BUTTON  0 // on ESP32 S3 TFT (and Reverse version)

#ifdef ESP32_REVERSE_BUTTONS // 2 more buttons
  #define PIN_D1_BUTTON  1 // on ESP32 S3 _Reverse_ TFT only
  #define PIN_D2_BUTTON  2 // on ESP32 S3 _Reverse_ TFT only
#endif

void setup_input_esp32_button(void) {
  // ------------------------  Set up on-board digital input button(s) -------------------------
  pinMode(PIN_D0_BOOT_BUTTON, INPUT_PULLUP);
  //digitalWrite(PIN_BOOT_BUTTON, HIGH); // enable pull-up

#ifdef ESP32_REVERSE_BUTTONS // 2 more buttons
  pinMode(PIN_D1_BUTTON, INPUT_PULLDOWN);
  pinMode(PIN_D2_BUTTON, INPUT_PULLDOWN);
#endif
}

void loop_input_esp32_button(void) {
  // ------------------------ Check onboard digital input button(s) ---------------------------
  int d0_in = digitalRead(PIN_D0_BOOT_BUTTON);
  static int old_d0_in = 1;
  bool d0_click = false;
  bool d0_unclick = false;

  if (!d0_in && old_d0_in) { // b/c pull-up
    d0_click = true;
  }
  if (d0_in && !old_d0_in) { // un-click
    d0_unclick = true;
  }
  
  old_d0_in = d0_in;

#ifdef ESP32_REVERSE_BUTTONS // 2 more buttons
  int d1_in = digitalRead(PIN_D1_BUTTON);
  int d2_in = digitalRead(PIN_D2_BUTTON);
  static int old_d1_in = 1;
  static int old_d2_in = 1;
  int y_move = 0; // buttons doing 'up' or 'down'

  if (d0_click) {
    y_move = 1; // move down
  }

  if (d1_in && !old_d1_in) { // (normal, no pull-up)
    do_click = true;
  }
  if (!d1_in && old_d1_in) { // un-click
    do_unclick = true;
  }
  old_d1_in = d1_in;

  if (d2_in && !old_d2_in) { // (normal, no pull-up)
    y_move = -1; // move up
  } 
  old_d2_in = d2_in;

  ScreenMove (0/*x_move*/, y_move);

#else // !defined ESP32_REVERSE_BUTTONS
  // d0 is 'click', not 'up'.
  if (d0_click) {
    do_click = true;
  }
  if (d0_unclick) {
    do_unclick = true;
  }
#endif
}