#define USE_INPUT_GAMEPAD
#ifdef USE_INPUT_GAMEPAD
# include "Adafruit_seesaw.h" // mini gamepad (on STEMMA QT)
#endif

// mini gamepad (on STEMMA QT)
#ifdef USE_INPUT_GAMEPAD
Adafruit_seesaw ss;
#define BUTTON_X         6
#define BUTTON_Y         2
#define BUTTON_A         5
#define BUTTON_B         1
#define BUTTON_SELECT    0
#define BUTTON_START    16
uint32_t button_mask = (1UL << BUTTON_X) | (1UL << BUTTON_Y) | (1UL << BUTTON_START) |
                       (1UL << BUTTON_A) | (1UL << BUTTON_B) | (1UL << BUTTON_SELECT);
//#define IRQ_PIN   5
#endif

bool gamepad_found = false;

// ------------------------ Start mini gamepad input ---------------------------
void setup_input_gamepad(void) {
#ifdef USE_INPUT_GAMEPAD
  gamepad_found = ss.begin(0x50);
  if (!gamepad_found) {
    Serial.println("seesaw not found - no gamepad will be used.");
  } else { // gamepad_found
    Serial.println("gamepad started");

    uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
    if (version != 5743) {
      Serial.print("Wrong firmware loaded? ");
      Serial.println(version);
      while(1) delay(10);
    }
    Serial.println("Found Product 5743");
    
    ss.pinModeBulk(button_mask, INPUT_PULLUP);
    ss.setGPIOInterrupts(button_mask, 1);

  #if defined(IRQ_PIN)
    pinMode(IRQ_PIN, INPUT);
  #endif
  }
#else
  Serial.println("gamepad skipped - USE_INPUT_GAMEPAD not defined at compile time");
#endif
}

// ------------------------ Check mini gamepad input ---------------------------
void loop_input_gamepad(void) {
#ifdef USE_INPUT_GAMEPAD
  static int last_x = 0;
  static int last_y = 0; // old joystick position
  int x_move = 0;
  int y_move = 0;

  if (!gamepad_found) {
    return;
  }
  // Reverse x/y values to match joystick orientation
  int joy_x = 1023 - ss.analogRead(14);
  int joy_y = 1023 - ss.analogRead(15);
  
  if ( (abs(joy_x - last_x) > 3)  ||  (abs(joy_y - last_y) > 3)) {
    Serial.print("joy_x: "); Serial.print(joy_x); Serial.print(", "); Serial.print("joy_y: "); Serial.println(joy_y);
    last_x = joy_x;
    last_y = joy_y;
  }
  // X joystick axis: be more sensitive, as it is harder to push
  if (joy_x < 400) {
    x_move = -1;
  }
  if (joy_x > 600) {
    x_move = 1;
  }
  // Y joystick axis: be less sensitive, as it is so easy to push (don't override X motions)
  if (joy_y < 128) {
    y_move = 1;
  }
  if (joy_y > 900) {
    y_move = -1;
  }
#if defined(IRQ_PIN)
  if(!digitalRead(IRQ_PIN)) {
    return;
  }
#endif

  uint32_t buttons = ss.digitalReadBulk(button_mask);
  if (! (buttons & (1UL << BUTTON_X))) {
    y_move = -1;
  }
  if (! (buttons & (1UL << BUTTON_B))) {
    y_move = 1;
  }
  if (! (buttons & (1UL << BUTTON_Y))) {
    x_move = -1;
  }
  if (! (buttons & (1UL << BUTTON_A))) {
    x_move = 1;
  }

  ScreenMove (x_move, y_move);
#endif
}