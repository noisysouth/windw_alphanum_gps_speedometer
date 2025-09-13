// mini keyboard (on Grove connector, can be adapted to STEMMA QT) 
// ---------------------- CardKB mini keyboard constants ------------------
#define KB_ADDR 0x5f
#define MIN_PRINTABLE ' '
#define MAX_PRINTABLE '~'

// When you press Fn+A thru Fn+Z on cardkb,
//  the cardkb will return these values.
#define FUNC_KEY_COUNT ('Z'-'A'+1)
unsigned func_keys[FUNC_KEY_COUNT] =  // Fn+
{	0x9a, 0xaa, 0xa8, 0x9c, 0x8f, 0x9d, // abc def
	0x9e, 0x9f, 0x94, 0xa0, 0xa1, 0xa2, // ghi jkl
	0xac, 0xab, 0x95, 0x96, 0x8d, 0x90, // mno pqr
	0x9b, 0x91, 0x93, 0xa9, 0x8e, 0xa7, // stu vwx
	0x92, 0xa6 }; // yz
#define OKALT      '\n'
#define CANCELALT  0x1b
#define KEY_BACKSPACE 0x8
#define KEY_LEFT  0xb4 // <--
#define KEY_UP    0xb5 // ^
#define KEY_DOWN  0xb6 // v
#define KEY_RIGHT 0xb7 //  -->

// ------------------------ Start cardkb input ---------------------------
void setup_input_cardkb(void) {
  // nothing kb-specific. Just make sure you do:
  //Wire.begin();        // join i2c bus (address optional for master)
  // somewhere in setup(), as is also needed for other I2C devices.
}

// ----------------------  Check CardKB mini keyboard input --------------------------
// returns keyboard input pressed key value, or 0 if nothing was pressed
char loop_input_cardkb(void) {
  char        kb_in = 0; // no key pressed
  int x_move;
  int y_move;

  Wire.requestFrom(KB_ADDR, 1);    // request 1 byte from peripheral device KB_ADDR

  x_move = 0;
  y_move = 0;

  if (Wire.available()) { // peripheral may send less than requested
    kb_in = Wire.read(); // receive a byte as character
    switch (kb_in) {
    case 0x00: // nothing pressed. do nothing.
      break;
    case 0xd: // Enter
      kb_in = '\n';
      do_click = true;
      //was_click = true;
      break;
    case 0x1b: // Escape
      break;
    case KEY_BACKSPACE: // Backspace
      break;
    case 0x9: // Tab
      break;
    case KEY_LEFT: // <- Left
      x_move = -1;
      break;
    case KEY_UP: // ^ Up
      y_move = -1;
      break;
    case KEY_DOWN: // v Down
      y_move = 1;
      break;
    case KEY_RIGHT: // Right ->
      x_move = 1;
      break;
    default:
      if (kb_in >= MIN_PRINTABLE && kb_in <= MAX_PRINTABLE) {
        // A-Z, 1-9, punctuation, space
      } else {
        // Someone probably pressed "Function" + key, got 0x80 thru 0xAF
        // See https://docs.m5stack.com/en/unit/cardkb_1.1
      }
    }
    //Serial.print(kb_in);         // print the character
  }

  ScreenMove (x_move, y_move);

  return kb_in;
}