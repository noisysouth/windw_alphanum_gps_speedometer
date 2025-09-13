#include "my_tft.h"

// --------------------- colors ------------------------

enum color_e {
#ifdef ESP32_S3_TFT
  color_black = ST77XX_BLACK,
  color_white = ST77XX_WHITE,
  color_blue = ST77XX_BLUE,
  color_green = ST77XX_GREEN,
  color_yellow = ST77XX_YELLOW,
  color_red = ST77XX_RED,
  color_magenta = ST77XX_MAGENTA,
#elif defined HX8357
//#ifdef HX8357
  color_black = HX8357_BLACK,
  color_white = HX8357_WHITE,
  color_blue = HX8357_BLUE,
  color_green = HX8357_GREEN,
  color_yellow = HX8357_YELLOW,
  color_red = HX8357_RED,
  color_magenta = HX8357_MAGENTA,
#else
  color_black = 0x0000,
  color_white = 0xffff,
#endif
  // raw 565 format colors:
  // Source: https://github.com/newdigate/rgb565_colors/tree/main?tab=readme-ov-file#gray
  color_silver = 0xbdf7,
  color_dim_gray = 0x6B4D,
  color_lt_gray = 0xd69a,
  color_dk_gray = 0x29A7, // gunmetal gray

  color_backgr = color_black,
  color_highlight = color_silver, // color_white,
  //color_shadow = 0x328A, // dark slate gray
  color_shadow = color_dk_gray,
  color_dim_window = color_dim_gray,
  color_window = color_lt_gray,

  color_cap_shadow = color_black,
  color_cap_bar = color_shadow,
  color_cap_label = color_white,

  color_wndw_text = color_black,
  color_btn_text = color_black,
  color_edit_text = color_black,
  color_edit_backgr = color_white,
  color_edit_dim_backgr = color_dk_gray,
  color_edit_cursor = color_black,

  color_label_text = color_black,
  color_label_backgr = color_window,
  color_label_dim_backgr = color_dim_window,

  color_focus_border = color_black,

  color_list_text = color_black,
  color_list_backgr = color_window,
  color_list_dim_backgr = color_dim_window,

  color_up_down_text = color_black,
  color_up_down_backgr = color_white,
  color_up_down_dim_backgr = color_dk_gray,
  color_up_down_arrow = color_black,

  color_check_box_text = color_black,
  color_check_box_backgr = color_window,
  color_check_box_dim_backgr = color_dim_window,
  color_check_box_check = color_black,

  color_count,
};

// ----------------------- window structures ----------------------------

enum border_style_e {
  border_outset = 0,
  border_ridge,
  border_outset_narrow,
  border_inset,
  border_focus,
  border_style_count,
};

struct windw_s {
	int wx, wy, ww, wh; // Window coords.
	int border_style;		//  enum border_style_e
  int buffered;		    // Flag for buffer.
  GFXcanvas16 *canvas; // 16-bit, width x height pixels
};

#define WINDW_MAX_CAP 61
#define CHAR_WIDTH_PIXELS 12
#define CHAR_HEIGHT_PIXELS 16

struct cap_windw_s {
  struct windw_s windw;
  char label[WINDW_MAX_CAP]; // full cap
};

#define WINDW_MAX_LINE 61

struct cap_twindw_s {
  struct cap_windw_s cwindw;
  char line1[WINDW_MAX_LINE];
  char line2[WINDW_MAX_LINE];
  int button;
};

#define BUTTON_WIDTH_PIXELS  64
#define BUTTON_HEIGHT_PIXELS 32
#define BUTTON_MAX_LABEL 20

struct button_s {
  struct windw_s windw;
  char label_str[BUTTON_MAX_LABEL];
	unsigned hotkey;
	int altkey;
  //GFXcanvas16 canvas(BUTTON_WIDTH_PIXELS, BUTTON_HEIGHT_PIXELS); // 16-bit, width x height pixels
  GFXcanvas16 *label_canvas; // 16-bit, width-4 x height-4 pixels

  void (*cb_func)(void); // function to call when 'ok'/'Enter'/right button/center button is clicked.
  // callback function will be 'void my_func(void)'.
};

#define EDIT_WIDTH_PIXELS   64
#define EDIT_HEIGHT_PIXELS  32
#define EDIT_MAX_TEXT       WINDW_MAX_LINE
#define CURSOR_BLINK_COUNT  5

struct edit_s {
  struct windw_s windw;
  char text_str[EDIT_MAX_TEXT];
  int text_max_chars;
  int cursor_idx;
  bool cursor_blink_on;
  int  cursor_blink_count;
};

#define LABEL_WIDTH_PIXELS   64
#define LABEL_HEIGHT_PIXELS  32
#define LABEL_MAX_TEXT       WINDW_MAX_LINE

struct label_s {
  struct windw_s windw;
  char text_str[LABEL_MAX_TEXT];
  int text_max_chars;
};

#define LIST_WIDTH_PIXELS   64
#define LIST_MAX_TEXT   WINDW_MAX_LINE
#define LIST_MAX_ITEMS  100
#define LIST_MAX_VISIBLE_ITEMS  15
struct list_s {
  struct windw_s windw;
  int visible_count; // how many rows of list items will be displayed at the same time
  bool needs_redraw; // if contents have been changed since last draw

  int item_count; // how many items in list
  char text_str[LIST_MAX_ITEMS][LIST_MAX_TEXT]; // text items in list
  int text_max_chars; // truncate each individual item string to be no longer than this
  struct windw_s vis_windw[LIST_MAX_VISIBLE_ITEMS]; // 1 window per each list item row that will be displayed at the same time

  int selected_idx; // selection in list
  int scroll_idx;   // scrolling which of text_str[] are visible in 'visible_count' slots of 'vis_windw' windows.

  void (*cb_func)(char *sel_text); // function to call when 'ok'/'Enter'/right button/center button is clicked.
  // callback function will be 'void my_func(char *sel_text)'.
};

#define UP_DOWN_MAX_TEXT   WINDW_MAX_LINE
#define UP_DOWN_WIDTH_PIXELS   64
struct up_down_s {
  struct windw_s windw;
  double sel_val;

  int text_max_chars; // truncate value display string to be no longer than this
  double min_val;
  double max_val;
  double val_step;
  int val_prec; // precision: number of digits to display after the decimal
  bool interior_focus; // control has been clicked on with 'select'. now, up/down change value, until 'select' is clicked again.
                       //  then, up/down can go back to changing focus between controls.
  struct windw_s up_windw;
  struct windw_s down_windw;

  void (*cb_func)(double sel_val); // function to call when 'ok'/'Enter'/right button/center button is clicked.
  // callback function will be 'void my_func(double sel_val)'.
  void (*cb_chg_func)(double chg_val); // function to call when value is changed up/down, BEFORE 'OK'.
  // callback function will be 'void my_func(double chg_val)'.
};

#define CHECK_BOX_WIDTH_PIXELS   64
#define CHECK_BOX_HEIGHT_PIXELS  32
#define CHECK_BOX_MAX_TEXT       WINDW_MAX_LINE
#define CHECK_BOX_SQUARE_PIXELS  16
#define CHECK_BOX_CHECK_WIDTH_PIXELS   2
#define CHECK_BOX_CHECK_MARGIN   3
struct check_box_s {
  struct windw_s windw;
  struct windw_s square_windw; // the check mark or empty box area
  char text_str[CHECK_BOX_MAX_TEXT];
  int checked;

  void (*cb_func)(int check_val); // function to call when 'ok'/'Enter'/right button/center button is clicked.
  // callback function will be 'void my_func(int check_val)'.
};

enum ctrl_type_e {
  ctrl_type_none = 0,
  ctrl_windw,
  ctrl_cap_windw,
  ctrl_cap_twindw,
  ctrl_button,
  ctrl_edit,
  ctrl_label,
  ctrl_list,
  ctrl_up_down,
  ctrl_check_box,
  ctrl_scroll,  // TODO
  ctrl_type_count,
};

struct tab_ctrl_s {
  int   ctrl_type; // enum ctrl_type_e
  void *ctrl_obj;
};

#define SCREEN_MAX_CTRLS 10
struct ctrl_screen_s {
  struct tab_ctrl_s **screen_ctrls;//[SCREEN_MAX_CTRLS];
  int               focus_idx;
};

// ----------------------- draw Dashed lines -----------------------------

#define DOT_LINE_LEN 10
#define DOT_SPACE_LEN 3
#define FOCUS_MARGIN_PIXELS 4

// to TFT
#ifdef ESP32_S3_TFT
  void DrawDashedHLine(Adafruit_ST7789 *scrn, int line_x, int line_y, int line_w) {
#elif defined HX8357
  void DrawDashedHLine(Adafruit_HX8357 *scrn, int line_x, int line_y, int line_w) {
#else
  void DrawDashedHLine(GFXcanvas16 *scrn, int line_x, int line_y, int line_w) {
#endif
    // top and left sides
    int start_px, end_px;
    int width_px;
    int limit_px;

    start_px = line_x;
    limit_px = line_x + line_w;

    width_px = DOT_LINE_LEN;
    while (start_px < limit_px) {
      end_px = start_px + width_px;
      if (end_px > limit_px) {
        end_px = limit_px;
        width_px = end_px - limit_px;
      }

      scrn->drawFastHLine(start_px, line_y, width_px, color_focus_border);
      start_px += DOT_LINE_LEN + DOT_SPACE_LEN;
    }
}

#ifdef ESP32_S3_TFT
  void DrawDashedVLine(Adafruit_ST7789 *scrn, int line_x, int line_y, int line_w) {
#elif defined HX8357
  void DrawDashedVLine(Adafruit_HX8357 *scrn, int line_x, int line_y, int line_w) {
#else
  void DrawDashedVLine(GFXcanvas16 *scrn, int line_x, int line_y, int line_w) {
#endif
    // top and left sides
    int start_py, end_py;
    int height_py;
    int limit_py;

    start_py = line_y;
    limit_py = line_y + line_w;

    height_py = DOT_LINE_LEN;
    while (start_py < limit_py) {
      end_py = start_py + height_py;
      if (end_py > limit_py) {
        end_py = limit_py;
        height_py = end_py - limit_py;
      }
      scrn->drawFastVLine(line_x, start_py, height_py, color_focus_border);
      start_py += DOT_LINE_LEN + DOT_SPACE_LEN;
    }
}

// to Canvas
void DrawDashedHLineToCanvas(GFXcanvas16 *canvas, int line_x, int line_y, int line_w) {
    // top and left sides
    int start_px, end_px;
    int width_px;
    int limit_px;

    start_px = line_x;
    limit_px = line_x + line_w;

    width_px = DOT_LINE_LEN;
    while (start_px < limit_px) {
      end_px = start_px + width_px;
      if (end_px > limit_px) {
        end_px = limit_px;
        width_px = end_px - limit_px;
      }
      canvas->drawFastHLine(start_px, line_y, width_px, color_focus_border);
      start_px += DOT_LINE_LEN + DOT_SPACE_LEN;
    }
}

void DrawDashedVLineToCanvas(GFXcanvas16 *canvas, int line_x, int line_y, int line_w) {
    // top and left sides
    int start_py, end_py;
    int height_py;
    int limit_py;

    start_py = line_y;
    limit_py = line_y + line_w;

    height_py = DOT_LINE_LEN;
    while (start_py < limit_py) {
      end_py = start_py + height_py;
      if (end_py > limit_py) {
        end_py = limit_py;
        height_py = end_py - limit_py;
      }
      canvas->drawFastVLine(line_x, start_py, height_py, color_focus_border);
      start_py += DOT_LINE_LEN + DOT_SPACE_LEN;
    }
}

// ------------------- get colors ------------------------

int GetColorBackgr (void) {
  return color_backgr;
}

int GetColorWindow(void) {
  if (app_settings->bright_steps >= 0) {
    return color_window;
  } else {
    return color_dim_window;
  }
}

int GetColorEditBackgr(void) {
  if (app_settings->bright_steps >= 0) {
    return color_edit_backgr;
  } else {
    return color_edit_dim_backgr;
  }
}

int GetColorLabelBackgr(void) {
  if (app_settings->bright_steps >= 0) {
    return color_label_backgr;
  } else {
    return color_label_dim_backgr;
  }
}

int GetColorListBackgr(void) {
  if (app_settings->bright_steps >= 0) {
    return color_list_backgr;
  } else {
    return color_list_dim_backgr;
  }
}

int GetColorUpDownBackgr(void) {
  if (app_settings->bright_steps >= 0) {
    return color_up_down_backgr;
  } else {
    return color_up_down_dim_backgr;
  }
}

int GetColorCheckBoxBackgr(void) {
  if (app_settings->bright_steps >= 0) {
    return color_check_box_backgr;
  } else {
    return color_check_box_dim_backgr;
  }
}

// Adapted from: Borland C++ Power Programming by Clayton Walnum
// https://fabiensanglard.net/reverse_engineering_strike_commander/docs/Borland_C___Power_Programming_Book_and_Disk__Programming_.pdf
// pg. 129 (PDF pg. 143)

// ------------------- draw Border ------------------------

#ifdef ESP32_S3_TFT
  void DrawBorder(Adafruit_ST7789 *scrn, struct windw_s *wnd, int brd_style) {
#elif defined HX8357
  void DrawBorder(Adafruit_HX8357 *scrn, struct windw_s *wnd, int brd_style) {
#else
  void DrawBorder(GFXcanvas16 *scrn, struct windw_s *wnd, int brd_style) {
#endif
  int bx, by, bw, bh;

  bx = wnd->wx;
  by = wnd->wy;
  bw = wnd->ww;
  bh = wnd->wh;
  
  switch (brd_style) {
  case border_outset:
    // highlight - top and left sides, 2 pixels wide each
    scrn->drawFastHLine(bx,   by,   bw,   color_highlight); //  top edge - outer
    scrn->drawFastVLine(bx,   by,   bh,   color_highlight); // left edge - outer
    scrn->drawFastHLine(bx+1, by+1, bw-2, color_highlight); //  top edge - inner
    scrn->drawFastVLine(bx+1, by+1, bh-2, color_highlight); // left edge - innder

    // shadow - bottom and right sides, 2 pixels wide each
    scrn->drawFastHLine(bx+1,    by+bh-1, bw-1, color_shadow); // bottom edge - outer
    scrn->drawFastVLine(bx+bw-1, by,      bh,   color_shadow); //  right edge - outer
    scrn->drawFastHLine(bx+2,    by+bh-2, bw-3, color_shadow); // bottom edge - inner
    scrn->drawFastVLine(bx+bw-2, by+1,    bh-2, color_shadow); //  right edge - inner
    break;
  case border_ridge:
    scrn->drawFastHLine(bx+10,      by+10,      bw-20, color_shadow); //  top margin line
    scrn->drawFastVLine(bx+10,      by+10,      bh-20, color_shadow); // left margin line
    scrn->drawFastHLine(bx+10,      by+bh - 10, bw-20, color_highlight); // bottom margin line    
    scrn->drawFastVLine(bx+bw - 10, 10,         bh-20, color_highlight); //   left margin line
    break;
  case border_outset_narrow: // Draw caption bar
    scrn->drawFastVLine(bx + 20,      by+20, 20,      color_highlight);  //   left edge
    scrn->drawFastHLine(bx + 20,      by+20, bw - 40, color_highlight);  //    top edge
    scrn->drawFastVLine(bx + bw - 20, by+20, 21,      color_cap_shadow); //  right edge
    scrn->drawFastHLine(bx + 20,      by+40, bw - 40, color_cap_shadow); // bottom edge
    break;
  case border_inset:
    // shadow - top and left sides, 2 pixels wide each
    scrn->drawFastHLine(bx,   by,   bw-1, color_shadow); //  top edge - outer
    scrn->drawFastVLine(bx,   by,   bh-1, color_shadow); // left edge - outer
    scrn->drawFastHLine(bx+1, by+1, bw-3, color_shadow); //  top edge - inner
    scrn->drawFastVLine(bx+1, by+1, bh-3, color_shadow); // left edge - innder

    // highlight - bottom and right sides, 2 pixels wide each
    scrn->drawFastHLine(bx+1,      by + bh-1, bw-1, color_highlight); // bottom edge - outer
    scrn->drawFastVLine(bx + bw-1, by,        bh-1, color_highlight); //  right edge - outer
    scrn->drawFastHLine(bx+2,      by + bh-2, bw-3, color_highlight); // bottom edge - inner
    scrn->drawFastVLine(bx + bw-2, by+1,      bh-3, color_highlight); //  right edge - inner
    break;
  case border_focus:
    // for some reason using the scrn parameter does not work here?
    DrawDashedHLine(scrn, bx+FOCUS_MARGIN_PIXELS, by+FOCUS_MARGIN_PIXELS,      bw-FOCUS_MARGIN_PIXELS*2); // top edge outer
    DrawDashedHLine(scrn, bx+FOCUS_MARGIN_PIXELS, by+FOCUS_MARGIN_PIXELS+1,    bw-FOCUS_MARGIN_PIXELS*2); // top edge inner
    DrawDashedHLine(scrn, bx+FOCUS_MARGIN_PIXELS, by+bh-FOCUS_MARGIN_PIXELS-1, bw-FOCUS_MARGIN_PIXELS*2); // bottom edge inner
    DrawDashedHLine(scrn, bx+FOCUS_MARGIN_PIXELS, by+bh-FOCUS_MARGIN_PIXELS,   bw-FOCUS_MARGIN_PIXELS*2); // bottom edge outer

    DrawDashedVLine(scrn, bx+FOCUS_MARGIN_PIXELS,      by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // left edge outer
    DrawDashedVLine(scrn, bx+FOCUS_MARGIN_PIXELS+1,    by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // left edge inner
    DrawDashedVLine(scrn, bx+bw-FOCUS_MARGIN_PIXELS-1, by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // right edge inner
    DrawDashedVLine(scrn, bx+bw-FOCUS_MARGIN_PIXELS,   by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // right edge outer
    break;
  default:
    break;
  }
}

void DrawBorderToCanvas(GFXcanvas16 *canvas, struct windw_s *wnd, int brd_style) {
  int bx, by, bw, bh;
  
  bx = 0;
  by = 0;
  bw = wnd->ww;
  bh = wnd->wh;

  switch (brd_style) {
  case border_outset:
    // highlight - top and left sides, 2 pixels wide each
    canvas->drawFastHLine(bx,   by,   bw,   color_highlight); //  top edge - outer
    canvas->drawFastVLine(bx,   by,   bh,   color_highlight); // left edge - outer
    canvas->drawFastHLine(bx+1, by+1, bw-2, color_highlight); //  top edge - inner
    canvas->drawFastVLine(bx+1, by+1, bh-2, color_highlight); // left edge - innder

    // shadow - bottom and right sides, 2 pixels wide each
    canvas->drawFastHLine(bx+1,    by+bh-1, bw-1, color_shadow); // bottom edge - outer
    canvas->drawFastVLine(bx+bw-1, by,      bh,   color_shadow); //  right edge - outer
    canvas->drawFastHLine(bx+2,    by+bh-2, bw-3, color_shadow); // bottom edge - inner
    canvas->drawFastVLine(bx+bw-2, by+1,    bh-2, color_shadow); //  right edge - inner
    break;
  case border_ridge:
    canvas->drawFastHLine(bx+10,      by+10,      bw-20, color_shadow); //  top margin line
    canvas->drawFastVLine(bx+10,      by+10,      bh-20, color_shadow); // left margin line
    canvas->drawFastHLine(bx+10,      by+bh - 10, bw-20, color_highlight); // bottom margin line    
    canvas->drawFastVLine(bx+bw - 10, 10,         bh-20, color_highlight); //   left margin line
    break;
  case border_outset_narrow: // Draw caption bar
    canvas->drawFastVLine(bx + 20,      by+20, 20,      color_highlight);  //   left edge
    canvas->drawFastHLine(bx + 20,      by+20, bw - 40, color_highlight);  //    top edge
    canvas->drawFastVLine(bx + bw - 20, by+20, 21,      color_cap_shadow); //  right edge
    canvas->drawFastHLine(bx + 20,      by+40, bw - 40, color_cap_shadow); // bottom edge
    break;
  case border_inset:
    // shadow - top and left sides, 2 pixels wide each
    canvas->drawFastHLine(bx,   by,   bw-1, color_shadow); //  top edge - outer
    canvas->drawFastVLine(bx,   by,   bh-1, color_shadow); // left edge - outer
    canvas->drawFastHLine(bx+1, by+1, bw-3, color_shadow); //  top edge - inner
    canvas->drawFastVLine(bx+1, by+1, bh-3, color_shadow); // left edge - innder

    // highlight - bottom and right sides, 2 pixels wide each
    canvas->drawFastHLine(bx+1,      by + bh-1, bw-1, color_highlight); // bottom edge - outer
    canvas->drawFastVLine(bx + bw-1, by,        bh-1, color_highlight); //  right edge - outer
    canvas->drawFastHLine(bx+2,      by + bh-2, bw-3, color_highlight); // bottom edge - inner
    canvas->drawFastVLine(bx + bw-2, by+1,      bh-3, color_highlight); //  right edge - inner
    break;
  case border_focus:
    DrawDashedHLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS, by+FOCUS_MARGIN_PIXELS,      bw-FOCUS_MARGIN_PIXELS*2); // top edge outer
    DrawDashedHLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS, by+FOCUS_MARGIN_PIXELS+1,    bw-FOCUS_MARGIN_PIXELS*2); // top edge inner
    DrawDashedHLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS, by+bh-FOCUS_MARGIN_PIXELS-1, bw-FOCUS_MARGIN_PIXELS*2); // bottom edge inner
    DrawDashedHLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS, by+bh-FOCUS_MARGIN_PIXELS,   bw-FOCUS_MARGIN_PIXELS*2); // bottom edge outer

    DrawDashedVLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS,      by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // left edge outer
    DrawDashedVLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS+1,    by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // left edge inner
    DrawDashedVLineToCanvas(canvas, bx+bw-FOCUS_MARGIN_PIXELS-1, by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // right edge inner
    DrawDashedVLineToCanvas(canvas, bx+bw-FOCUS_MARGIN_PIXELS,   by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // right edge outer
    break;
  default:
    break;
  }
}

// ---------------------- basic Window  --------------------

void DrawWindowToCanvas(struct windw_s *wnd, GFXcanvas16 *canvas) {
  // in buffer, origin is (0,0)
  // on screen, origin is (wx,wy)
  DrawBorderToCanvas (canvas, wnd, border_outset);

  // center
  canvas->fillRect(2, 2, wnd->ww-4, wnd->wh-4, GetColorWindow());

  //Draw border, if requested.
  if (wnd->border_style == border_ridge) {
    DrawBorderToCanvas (canvas, wnd, border_ridge);
  }
}

void SetupWindow(struct windw_s *wnd, int x, int y, int w, int h, int brd_style, int buf) {
  wnd->wx = x;
  wnd->wy = y;
  wnd->ww = w;
  wnd->wh = h;
  wnd->border_style = brd_style;
  wnd->buffered = buf;
  Serial.print("SetupWindow(x=");
  Serial.print(x);
  Serial.print(", y=");
  Serial.print(y);
  Serial.print(", w=");
  Serial.print(w);
  Serial.print(", h=");
  Serial.print(h);
  Serial.print(", brd_style=");
  Serial.print(brd_style);
  Serial.print(", buf=");
  Serial.print(buf);
  Serial.println(")");
  if (buf) {
    wnd->canvas = new GFXcanvas16(w, h);
    DrawWindowToCanvas (wnd, wnd->canvas);
  } else {
    wnd->canvas = NULL;
  }
}

void DrawWindow(struct windw_s *wnd) {
  //Draw basic 3-D window.

  if (wnd->buffered) {
    tft.drawRGBBitmap(wnd->wx,   wnd->wy, wnd->canvas->getBuffer(), wnd->canvas->width(), wnd->canvas->height());
  } else { // !wnd->buffered
    DrawBorder (&tft, wnd, border_outset);

    // center
    tft.fillRect(wnd->wx+2, wnd->wy+2, wnd->ww-3, wnd->wh-3, GetColorWindow());

    //Draw border, if requested.
    if (wnd->border_style == border_ridge) {
      DrawBorder (&tft, wnd, border_ridge);
    }
  }
}



// ---------------------- Captioned Window  --------------------

void SetupCapWindow(struct cap_windw_s *cwnd, int x, int y, int w, int h, int brd, int buf, char *cap) {
  SetupWindow(&cwnd->windw, x, y, w, h, brd, buf);
  if (cap != NULL) {
    strncpy (cwnd->label, cap, WINDW_MAX_CAP);
  } else { // cap == NULL
    cwnd->label[0] = '\x0';
  }
  cwnd->label[WINDW_MAX_CAP-1] = '\x0'; // ensure null term.
}

void DrawCapWindow(struct cap_windw_s *cwnd) {
  struct windw_s *wnd;
  wnd = &cwnd->windw;

  DrawWindow (wnd);

  // Draw caption bar
  DrawBorder (&tft, wnd, border_outset_narrow);  

  // center
  tft.fillRect(wnd->wx+21, wnd->wy+21, wnd->ww-41, 19, color_cap_bar);

  // label
  tft.setTextSize(2);
	int x = (wnd->wx + wnd->ww / 2) - (strlen(cwnd->label) * CHAR_WIDTH_PIXELS / 2);
  tft.setCursor(x, wnd->wy+22);
  tft.setTextColor(color_cap_label);
  tft.setTextWrap(true);
  tft.print(cwnd->label);
}

// ---------------------- Captioned Text Window --------------------
void SetupCapTWindow(struct cap_twindw_s *ctwnd, char *cap, char *ln1, char *ln2) {
  int str_w;
  int wndw_w = 230; // minumum width
  int wndw_x;
  int wndw_y = 164; // default y
  int wndw_h = 150; // default height

  if (cap != NULL) {
    str_w = strlen(cap) * CHAR_WIDTH_PIXELS + 60;
    if (wndw_w < str_w) {
      wndw_w = str_w;
    }
  }
  if (ln1 != NULL) {
    str_w = strlen(ln1) * CHAR_WIDTH_PIXELS + 60;
    if (wndw_w < str_w) {
      wndw_w = str_w;
    }
  }
  if (ln2 != NULL) {
    str_w = strlen(ln2) * CHAR_WIDTH_PIXELS + 60;
    if (wndw_w < str_w) {
      wndw_w = str_w;
    }
  }
  if (wndw_w > tft.width()-1) { // limit to screen bounds
    wndw_w = tft.width()-1;
  }
  wndw_x = (tft.width()/2) - wndw_w/2;

  if (wndw_h > tft.height()-1) {
    wndw_h = tft.height()-1;
  }
  if (wndw_y + wndw_h > tft.height()) {
    wndw_y = (tft.height()/2) - wndw_h/2;
  }

  if (ln1 != NULL) {
    strncpy (ctwnd->line1, ln1, WINDW_MAX_LINE);
  } else { // ln1 == NULL
    ctwnd->line1[0] = '\x0';
  }
  ctwnd->line1[WINDW_MAX_LINE-1] = '\x0'; // ensure null term.

  if (ln2 != NULL) {
    strncpy (ctwnd->line2, ln2, WINDW_MAX_LINE);
  } else { // ln2 == NULL
    ctwnd->line2[0] = '\x0';
  }
  ctwnd->line2[WINDW_MAX_LINE-1] = '\x0';
  
  SetupCapWindow(&ctwnd->cwindw, wndw_x, wndw_y, wndw_w, wndw_h, 0/*border*/, 1/*buffered*/, cap);
}

void DrawCapTWindow(struct cap_twindw_s *ctwnd) {
  struct cap_windw_s *cwnd;
  struct windw_s *wnd;
  cwnd = &ctwnd->cwindw;
  wnd = &cwnd->windw;

  DrawCapWindow (cwnd);
	// Position and draw window body text.
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setTextColor(color_wndw_text);
	int x = (wnd->wx + wnd->ww / 2) - (strlen(ctwnd->line1) * CHAR_WIDTH_PIXELS) / 2;
	if (strlen(ctwnd->line2) == 0) {
    tft.setCursor(x, wnd->wy+68);
    tft.print(ctwnd->line1);
  } else { // strlen(line2) != 0
    tft.setCursor(x, wnd->wy+56);
    tft.print(ctwnd->line1);
	  x = (wnd->wx + wnd->ww / 2) - (strlen(ctwnd->line2) * CHAR_WIDTH_PIXELS) / 2;
    tft.setCursor(x, wnd->wy+71);
    tft.print(ctwnd->line2);
  }
}



// ---------------------- Button functions --------------------

void DrawButtonLabelToCanvas(struct button_s *btn) {
	int pos = -1;                  // position in 'btn->label_str' where '^' was found
	char tlabel[BUTTON_MAX_LABEL]; // copy of 'btn->label_str' with no '^' character
  GFXcanvas16 *lc;
  struct windw_s *wnd;
  wnd = &btn->windw;

  lc = btn->label_canvas;
  lc->fillScreen(GetColorWindow()); // Clear button label canvas (not tft)

	// Find and remove the ^ character and
	// set the appropriate hot key.
	strncpy(tlabel, btn->label_str, BUTTON_MAX_LABEL);
  tlabel[BUTTON_MAX_LABEL-1] = '\x0';
#ifdef OKALT
	for (int i = 0; i < strlen(tlabel); ++i) {
		if (tlabel[i] == '^') {
      char func_letter;
      
			pos = i;
      func_letter = toupper(tlabel[i + 1]);
      if (func_letter >= 'A' && func_letter < 'Z') {
			  btn->hotkey = func_keys[func_letter - 'A'];
      }
			for (int j = i; j < strlen(tlabel); ++j) {
				tlabel[j] = tlabel[j + 1];
      }
		}
	}
	if      (strcmp(tlabel, "OK"    ) == 0) btn->altkey = OKALT;
	else if (strcmp(tlabel, "CANCEL") == 0) btn->altkey = CANCELALT;
#endif

	// Center and draw text on button.
  lc->setTextSize(2);
  lc->setTextWrap(true);
  lc->setTextColor(color_btn_text);
  int x = (wnd->ww / 2) - (strlen(tlabel) * CHAR_WIDTH_PIXELS / 2);
  lc->setCursor(x, 8);
  lc->print(tlabel);

	// Underline the hot-key character.
	if (pos >= 0) {
    lc->drawFastHLine(x + pos * CHAR_WIDTH_PIXELS, 18, CHAR_WIDTH_PIXELS-1, color_btn_text);
  }
}

// set string label to be written inside of button 'btn', to new text 'label_str'
//  Note: does NOT make button wider to fit new text.
//  If this is a change, return 'true'.
bool SetButtonText(struct button_s *btn, const char *label_str) {
  bool chg = false;

  if (label_str != NULL) {
    if (strncmp (btn->label_str, label_str, BUTTON_MAX_LABEL) != 0) {
      chg = true;
    }
    strncpy (btn->label_str, label_str, BUTTON_MAX_LABEL);
  } else { // label_str == NULL
    if (btn->label_str[0] != '\x0') {
      chg = true;
    }
    btn->label_str[0] = '\x0';
  }
  btn->label_str[BUTTON_MAX_LABEL-1] = '\x0'; // ensure null term.

  if (chg) {
    DrawButtonLabelToCanvas (btn);
  }
  return chg;
}

// create a new button control 'btn' on screen.
//  draw at upper-left corner 'x', 'y'
//  inside of button will be written 'lbl' string label
//  when button is clicked (with 'Enter', right-arrow, middle button, etc.), void cb_func(void) will be called.
void SetupButton(struct button_s *btn, int x, int y, const char *lbl, void (*cb_func)(void)) {
  int w;

  // make button wider to fit label
  w = strlen(lbl) * CHAR_WIDTH_PIXELS + 12;
  if (w < BUTTON_WIDTH_PIXELS) {
    w = BUTTON_WIDTH_PIXELS;
  }
  //SetupWindow(&btn->windw, x, y, w, BUTTON_HEIGHT_PIXELS/*h*/, 0/*brd*/, 1/*buf*/); // buffer not working on HX8357?
  SetupWindow(&btn->windw, x, y, w, BUTTON_HEIGHT_PIXELS/*h*/, 0/*brd*/, 0/*buf*/);

#if 0
  if (lbl != NULL) {
    strncpy (btn->label_str, lbl, BUTTON_MAX_LABEL);
  } else { // lbl == NULL
    btn->label_str[0] = '\x0';
  }
  btn->label_str[BUTTON_MAX_LABEL-1] = '\x0'; // ensure null term.
#endif

  btn->altkey = 0;
  btn->hotkey = 0;

  btn->label_canvas = new GFXcanvas16(w-4, BUTTON_HEIGHT_PIXELS-4);
  //DrawButtonLabelToCanvas (btn);
  SetButtonText (btn, lbl);

  btn->cb_func = cb_func;
}

void DrawButton(struct button_s *btn, bool has_focus) {
  struct windw_s *wnd;
  wnd = &btn->windw;

  DrawWindow (wnd);

  if (wnd->buffered) {
    tft.drawRGBBitmap(wnd->wx+2,   wnd->wy+2, btn->label_canvas->getBuffer(),
                                              btn->label_canvas->width(),
                                              btn->label_canvas->height());
  } else { // !wnd->buffered
	  int pos = -1;                  // position in 'btn->label_str' where '^' was found
  	char tlabel[BUTTON_MAX_LABEL]; // copy of 'btn->label_str' with no '^' character

    // Find and remove the ^ character and
    // set the appropriate hot key.
    strncpy(tlabel, btn->label_str, BUTTON_MAX_LABEL);
    tlabel[BUTTON_MAX_LABEL-1] = '\x0';
#ifdef OKALT
    for (int i = 0; i < strlen(tlabel); ++i) {
      if (tlabel[i] == '^') {
        char func_letter;
        
        pos = i;
        func_letter = toupper(tlabel[i + 1]);
        if (func_letter >= 'A' && func_letter < 'Z') {
          btn->hotkey = func_keys[func_letter - 'A'];
        }
        for (int j = i; j < strlen(tlabel); ++j) {
          tlabel[j] = tlabel[j + 1];
        }
      }
    }
    if      (strcmp(tlabel, "OK"    ) == 0) btn->altkey = OKALT;
    else if (strcmp(tlabel, "CANCEL") == 0) btn->altkey = CANCELALT;
#endif

    // Center and draw text on button.
    tft.setTextSize(2);
    tft.setTextWrap(true);
    tft.setTextColor(color_btn_text);
    int x = (wnd->wx + wnd->ww / 2) - (strlen(tlabel) * CHAR_WIDTH_PIXELS / 2);
    tft.setCursor(x, wnd->wy+12);
    tft.print(tlabel);

    // Underline the hot-key character.
    if (pos >= 0) {
      tft.drawFastHLine(x + pos * CHAR_WIDTH_PIXELS, wnd->wy+20, CHAR_WIDTH_PIXELS-1, color_btn_text);
    }
  } // end else (!wnd->buffered)
  if (has_focus) {
    DrawBorder (&tft, wnd, border_focus);
  }
}

void ClickButton (struct button_s *btn) {
  struct windw_s *wnd;
  wnd = &btn->windw;

  // Shift the image on the button down and right
	// to simulate button movement.
  tft.drawRGBBitmap(wnd->wx+3,   wnd->wy+3, btn->label_canvas->getBuffer(),
                                            btn->label_canvas->width(),
                                            btn->label_canvas->height());

	// Draw the button’s borders so the
	// button appears to be pressed.
  DrawBorder (&tft, wnd, border_inset);

  // do something the button knows.
  if (btn->cb_func != NULL) {
    btn->cb_func();
  }
#ifdef USE_MP3
  if (mp3_found) {

# ifdef ESP32_S3_TFT
    musicPlayer.playFullFile("/radio_gaga.mp3");
    // "Interrupt based playback currently does not work on ESP32 platforms."
    // Source: https://learn.adafruit.com/adafruit-music-maker-featherwing/library-reference
    // Music will start, but after it starts playing, then more input forces the processor to reboot.
# else
    musicPlayer.startPlayingFile("/radio_gaga.mp3");
# endif

  }
#endif
}



// ---------------------- Event to Control functions --------------------

void DrawCtrl(struct tab_ctrl_s *ctrl, bool has_focus) {
  switch (ctrl->ctrl_type) {
  case ctrl_windw:
    DrawWindow ((struct windw_s *)ctrl->ctrl_obj);//, has_focus);
    break;
  case ctrl_cap_windw:
    DrawCapWindow ((struct cap_windw_s *)ctrl->ctrl_obj);//, has_focus);
    break;
  case ctrl_cap_twindw:
    DrawCapTWindow ((struct cap_twindw_s *)ctrl->ctrl_obj);//, has_focus);
    break;
  case ctrl_button:
    DrawButton ((struct button_s *)ctrl->ctrl_obj, has_focus);
    break;
  case ctrl_edit:
    DrawEdit ((struct edit_s *)ctrl->ctrl_obj, has_focus);
    break;
  case ctrl_label:
    DrawLabel ((struct label_s *)ctrl->ctrl_obj, false/*has_focus*/);
    break;
  case ctrl_list:
    DrawList ((struct list_s *)ctrl->ctrl_obj, has_focus);
    break;
  case ctrl_up_down:
    DrawUpDown ((struct up_down_s *)ctrl->ctrl_obj, has_focus);
    break;
  case ctrl_check_box:
    DrawCheckBox ((struct check_box_s *)ctrl->ctrl_obj, has_focus);
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
}

void OnKeyCtrl(struct tab_ctrl_s *ctrl, char kb_in) {
  switch (ctrl->ctrl_type) {
  case ctrl_button:
    break;
  case ctrl_edit:
    OnKeyEdit ((struct edit_s *)ctrl->ctrl_obj, kb_in);
    break;
  case ctrl_list:
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
}

void UpdateCtrl(struct tab_ctrl_s *ctrl) {
  switch (ctrl->ctrl_type) {
  case ctrl_button:
    break;
  case ctrl_edit:
    UpdateEdit ((struct edit_s *)ctrl->ctrl_obj);
    break;
  case ctrl_list:
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
}

struct windw_s *GetCtrlWindow(struct tab_ctrl_s *ctrl) {
  struct windw_s *ret = NULL;

  switch (ctrl->ctrl_type) {
  case ctrl_windw:
    ret = (struct windw_s *)ctrl->ctrl_obj;
    break;
  case ctrl_cap_windw:
    ret = &((struct cap_windw_s *)ctrl->ctrl_obj)->windw;
    break;
  case ctrl_cap_twindw:
    ret = &((struct cap_twindw_s *)ctrl->ctrl_obj)->cwindw.windw;
    break;
  case ctrl_button:
    ret = &((struct button_s *)ctrl->ctrl_obj)->windw;
    break;
  case ctrl_edit:
    ret = &((struct edit_s *)ctrl->ctrl_obj)->windw;
    break;
  case ctrl_label:
    ret = &((struct label_s *)ctrl->ctrl_obj)->windw;
    break;
  case ctrl_list:
    ret = &((struct list_s *)ctrl->ctrl_obj)->windw;
    break;
  case ctrl_up_down:
    ret = &((struct up_down_s *)ctrl->ctrl_obj)->windw;
    break;
  case ctrl_check_box:
    ret = &((struct check_box_s *)ctrl->ctrl_obj)->windw;
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
  return ret;
}

void OnClickCtrl(struct tab_ctrl_s *ctrl) {
  if (!was_click) {
    haptic_click ();
  }
  was_click = true;

  switch (ctrl->ctrl_type) {
  case ctrl_button:
    ClickButton ((struct button_s *)ctrl->ctrl_obj);
    break;
  case ctrl_list:
    ClickList ((struct list_s *)ctrl->ctrl_obj);
    break;
  case ctrl_up_down:
    ClickUpDownFocus ((struct up_down_s *)ctrl->ctrl_obj);
    break;
  case ctrl_check_box:
    ClickCheckBox ((struct check_box_s *)ctrl->ctrl_obj);
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
}

// try to move to next element inside control.
//  if moved to next element OK, return true.
//  if no next element inside control, return false.
bool CtrlTabNext(struct tab_ctrl_s *ctrl) {
  bool did_tab;

  switch (ctrl->ctrl_type) {
  case ctrl_list:
    did_tab = SelectNextListItem ((struct list_s *)ctrl->ctrl_obj);
    break;
  case ctrl_up_down:
    did_tab = ClickUpDownDown ((struct up_down_s *)ctrl->ctrl_obj);
    break;
  default: // other kind of control.
    did_tab = false; // can't tab around in it; only to next control.
    break;
  }
  return did_tab;
}

// try to move to previous element inside control.
//  if moved to previous element OK, return true.
//  if no previous element inside control, return false.
bool CtrlTabPrev(struct tab_ctrl_s *ctrl) {
  bool did_tab;

  switch (ctrl->ctrl_type) {
  case ctrl_list:
    did_tab = SelectPrevListItem ((struct list_s *)ctrl->ctrl_obj);
    break;
  case ctrl_up_down:
    did_tab = ClickUpDownUp ((struct up_down_s *)ctrl->ctrl_obj);
    break;
  default: // other kind of control.
    did_tab = false; // can't tab around in it; only to previous control.
    break;
  }
  return did_tab;
}

bool CtrlCanFocus(struct tab_ctrl_s *ctrl) {
  bool can_focus = true;

  switch (ctrl->ctrl_type) {
  case ctrl_type_none:
    can_focus = false;
    break;
  case ctrl_windw:
    can_focus = false;
    break;
  case ctrl_cap_windw:
    can_focus = false;
    break;
  case ctrl_cap_twindw:
    can_focus = false;
    break;
  case ctrl_button:
    can_focus = true;
    break;
  case ctrl_edit:
    can_focus = true;
    break;
  case ctrl_label:
    can_focus = false;
    break;
  case ctrl_list:
    can_focus = true;
    break;
  case ctrl_up_down:
    can_focus = true;
    break;
  default: // other kind of control.
    break; //  sure, why not.
  }
  return can_focus;
}

// ------------------- Edit control -------------------------

//void SetupEdit (struct edit_s *edt, int x, int y, int text_max_chars, const char *text_default) {
void SetupEdit (struct edit_s *edt, int x, int y, int text_max_chars, char *text_default) {
  int w;

  if (text_max_chars > EDIT_MAX_TEXT-1) {
    text_max_chars = EDIT_MAX_TEXT-1;
  }
  edt->text_max_chars = text_max_chars;

  // make control wider to fit allowed chars (Alternative: horizontal scrolling)
  w = text_max_chars * CHAR_WIDTH_PIXELS + 15;
  if (w < EDIT_WIDTH_PIXELS) {
    w = EDIT_WIDTH_PIXELS;
  }
  //SetupWindow(&edt->windw, x, y, w, EDIT_HEIGHT_PIXELS/*h*/, 0/*brd*/, 1/*buf*/);
  SetupWindow(&edt->windw, x, y, w, EDIT_HEIGHT_PIXELS/*h*/, 0/*brd*/, 0/*buf*/); // no use for buffering this

  SetEdit (edt, text_default);

  edt->cursor_blink_on = false;
  edt->cursor_blink_count = 0;
}

void DrawEdit (struct edit_s *edt, bool has_focus) {
  struct windw_s *wnd;
  wnd = &edt->windw;
  int tx, ty;

  tx = wnd->wx+9;
  ty = wnd->wy+9;

  DrawBorder(&tft, wnd, border_inset);

  tft.fillRect(wnd->wx+2, wnd->wy+2, wnd->ww-4, wnd->wh-4, GetColorEditBackgr());

  // Draw text in control.
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setTextColor(color_edit_text);
  tft.setCursor(tx, wnd->wy+9);
  tft.print(edt->text_str);
 
  if (has_focus) {
    DrawBorder(&tft, wnd, border_focus);
  }
}

void DrawEditCursor (struct edit_s *edt) {
  struct windw_s *wnd;
  wnd = &edt->windw;
  int tx, ty;
  uint16_t draw_erase_color; // cursor color or background color

  tx = wnd->wx+9;
  ty = wnd->wy+9;

  if (edt->cursor_blink_on) {
    draw_erase_color = color_edit_cursor; // draw
  } else { // !cursor_blink_on
    draw_erase_color = GetColorEditBackgr(); // erase
  }
  tft.drawFastVLine(tx + edt->cursor_idx*CHAR_WIDTH_PIXELS-2, ty-2, CHAR_HEIGHT_PIXELS+2, draw_erase_color);
  tft.drawFastVLine(tx + edt->cursor_idx*CHAR_WIDTH_PIXELS-1, ty-2, CHAR_HEIGHT_PIXELS+2, draw_erase_color);

  edt->cursor_blink_count = 0; // wait before blinking again
}

// Make the cursor blink, if it is time to blink. (Called when the control has focus.)
void UpdateEdit (struct edit_s *edt) {
  edt->cursor_blink_count++;
  if (edt->cursor_blink_count > CURSOR_BLINK_COUNT) {
    edt->cursor_blink_on = !edt->cursor_blink_on;
    DrawEditCursor (edt);
  }
}

void DebugPrintChars(char *str) {
  int idx;

  idx = 0;
  while (str[idx] != '\x0') {
    Serial.print(idx);
    Serial.print(": '");
    Serial.print(str[idx]);
    Serial.print("', ");
    idx++;
  }
  Serial.print(idx);
  Serial.println(": NULL");
}

// key 'kb_in' was pressed.
//  Put character in edit control.
//  Understands special characters to: move the cursor, or delete a character:
//   Uses definitions from CardKB (backspace, left, right), if they exist.
void OnKeyEdit (struct edit_s *edt, char kb_in) {
  int text_len;

  text_len = strlen (edt->text_str);
#ifdef KEY_BACKSPACE
  if (kb_in == KEY_BACKSPACE) {
    if (edt->cursor_idx > 0) {
      //edt->text_str[text_len-1] = '\x0';

      // Move all characters
      //  FROM range: cursor_idx   ... text_len+1
      //    TO range: cursor_idx-1 ... text_len
      int src_idx, dst_idx;
      dst_idx = edt->cursor_idx-1;
      while (dst_idx <= text_len) {
        src_idx = dst_idx+1;
        edt->text_str[dst_idx] = edt->text_str[src_idx];
        dst_idx++;
      }
      edt->cursor_idx--;
      DrawEdit (edt, true);
      edt->cursor_blink_on = true;
      DrawEditCursor (edt);
    }
  }
#endif

#ifdef MAX_PRINTABLE
  // insert a character
  if (kb_in >= MIN_PRINTABLE && kb_in <= MAX_PRINTABLE) {
    if (text_len < edt->text_max_chars) { // yes, enough room for more
      // Move all characters
      //  FROM range: cursor_idx   ... text_len
      //    TO range: cursor_idx+1 ... text_len+1
      int src_idx, dst_idx;
      src_idx = text_len;
      while (src_idx >= edt->cursor_idx) {
        dst_idx = src_idx+1;
        edt->text_str[dst_idx] = edt->text_str[src_idx];
        src_idx--;
      }
      edt->text_str[edt->cursor_idx] = kb_in; // user's character
      edt->cursor_idx++;
      DrawEdit (edt, true);
      edt->cursor_blink_on = true;
      DrawEditCursor (edt);
    }
  }
#endif

#ifdef KEY_LEFT
  if (kb_in == KEY_LEFT) {
    if (edt->cursor_idx > 0) {
      edt->cursor_blink_on = false;
      DrawEditCursor (edt);
      
      edt->cursor_idx--;

      edt->cursor_blink_on = true;
      DrawEditCursor (edt);
    }
  }
#endif
#ifdef KEY_RIGHT
  if (kb_in == KEY_RIGHT) {
    if (edt->cursor_idx < text_len) {
      edt->cursor_blink_on = false;
      DrawEditCursor (edt);

      edt->cursor_idx++;

      edt->cursor_blink_on = true;
      DrawEditCursor (edt);
    }
  }
#endif
}

// return contents of edit control string
char *GetEdit (struct edit_s *edt) {
  return edt->text_str;
}

// put a string into an edit control
void SetEdit (struct edit_s *edt, char *new_text) {
  if (new_text != NULL) {
    strncpy (edt->text_str, new_text, EDIT_MAX_TEXT);
  } else { // text_default == NULL
    edt->text_str[0] = '\x0';
  }
  edt->text_str[EDIT_MAX_TEXT-1] = '\x0'; // ensure null term.

  edt->cursor_idx = strlen(edt->text_str);
}

// ------------------- Label control -------------------------

// set label 'lbl' to new text 'text_str'
//  If this is a change, return 'true'.
bool SetLabel (struct label_s *lbl, const char *text_str) {
  bool chg = false;

  if (text_str != NULL) {
    if (strncmp (lbl->text_str, text_str, LABEL_MAX_TEXT) != 0) {
      chg = true;
    }
    strncpy (lbl->text_str, text_str, LABEL_MAX_TEXT);
  } else { // text_str == NULL
    if (lbl->text_str[0] != '\x0') {
      chg = true;
    }
    lbl->text_str[0] = '\x0';
  }
  lbl->text_str[LABEL_MAX_TEXT-1] = '\x0'; // ensure null term.
  return chg;
}

void SetupLabel (struct label_s *lbl, int x, int y, int text_max_chars, const char *text_default) {
  int w;

  if (text_max_chars > LABEL_MAX_TEXT-1) {
    text_max_chars = LABEL_MAX_TEXT-1;
  }
  lbl->text_max_chars = text_max_chars;

  // make control wider to fit allowed chars (Alternative: horizontal scrolling)
  w = text_max_chars * CHAR_WIDTH_PIXELS + 15;
  if (w < LABEL_WIDTH_PIXELS) {
    w = LABEL_WIDTH_PIXELS;
  }
  //SetupWindow(&lbl->windw, x, y, w, LABEL_HEIGHT_PIXELS/*h*/, 0/*brd*/, 1/*buf*/);
  SetupWindow(&lbl->windw, x, y, w, LABEL_HEIGHT_PIXELS/*h*/, 0/*brd*/, 0/*buf*/); // no use for buffering this

  SetLabel(lbl, text_default);
}

void DrawLabel (struct label_s *lbl, bool has_focus) {
  struct windw_s *wnd;
  wnd = &lbl->windw;
  int tx, ty;

  tx = wnd->wx+9;
  ty = wnd->wy+9;

  DrawBorder(&tft, wnd, border_inset);

  tft.fillRect(wnd->wx+2, wnd->wy+2, wnd->ww-4, wnd->wh-4, GetColorLabelBackgr());

  // Draw text in control.
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setTextColor(color_label_text);
  tft.setCursor(tx, wnd->wy+9);
  tft.print(lbl->text_str);
 
  //if (has_focus) {
  //  DrawBorder(&tft, wnd, border_focus);
  //}
}

// ------------------- List control -------------------------

// in list 'lst', set item at index 'item_idx' to string 'new_text'.
void SetListItem (struct list_s *lst, int item_idx, char *new_text) {
  if (item_idx < 0 ||
      item_idx >= LIST_MAX_ITEMS) {
    return;
  }
  strncpy (lst->text_str[item_idx], new_text, lst->text_max_chars);
  lst->text_str[item_idx][lst->text_max_chars+1] = '\x0'; // ensure null term.
  if (lst->item_count <= item_idx) {
    lst->item_count = item_idx+1;
  }
  lst->needs_redraw = true;
}

// in list 'lst', return the string at index 'item_idx'.
char *GetListItem (struct list_s *lst, int item_idx) {
  if (item_idx < 0 ||
      item_idx >= LIST_MAX_ITEMS ||
      item_idx >= lst->item_count) {
    return NULL;
  }
  return lst->text_str[item_idx];
}

// in list 'lst', add the string 'new_text'.
//  if it already exists, nothing will be added.
void AddListItem (struct list_s *lst, char *new_text) {
  int item_idx;
  int cmp_result;

  for (item_idx = 0; item_idx < lst->item_count; item_idx++) {
    cmp_result = strncmp (lst->text_str[item_idx], new_text, lst->text_max_chars);
    if (!cmp_result) {
      // match found.
      return; // don't add new_text again.
    }
  }
  // if we are here, match was not found.
  
  // add to the end of the list.
  SetListItem (lst, lst->item_count, new_text);
}

// scroll Down in list
//  this may scroll to the next item, which was not visible before.
bool SelectNextListItem (struct list_s *lst) {
  if (lst->selected_idx >= lst->item_count-1) {
    return false; // no next item
  }
  lst->selected_idx++;
  
  if (lst->selected_idx > lst->scroll_idx + (lst->visible_count-1)) {
    // scroll to selected list item that was not visbible (scrolled to) to yet.
    lst->scroll_idx = lst->selected_idx - (lst->visible_count-1);
  }
  //lst->needs_redraw = true;
  DrawList (lst, true/*has_focus*/);

  return true;
}

// scroll Up in list
//  this may scroll to the previous item, which was not visible before.
bool SelectPrevListItem (struct list_s *lst) {
  if (lst->selected_idx <= 0) {
    return false; // no previous item
  }
  lst->selected_idx--;
  
  if (lst->selected_idx < lst->scroll_idx) {
    // scroll to selected list item that was not visbible (scrolled to) to yet.
    lst->scroll_idx = lst->selected_idx;
  }
  //lst->needs_redraw = true;
  DrawList (lst, true/*has_focus*/);

  return true;
}

// create a new list control 'lst' on screen.
//  draw at upper-left corner 'x', 'y'
//  items will contain up to 'text_max_chars'. This determines list width.
//  items will be listed in 'visible_count' rows. This determins list height.
//  default items will begin with string 'item_prefix'. If 'item_prefix' is NULL, no default items will be added.
//  when an item is chosen (with 'Enter', right-arrow, middle button, etc.), void cb_func(char *sel_text) will be called.
void SetupList (struct list_s *lst, int x, int y, int text_max_chars, int visible_count, void (*cb_func)(char *), const char *item_prefix) {
  int w, h, item_idx;
  int item_height, item_y;
  int vis_idx; // visible index
  char dfl_text[LIST_MAX_TEXT];

  if (text_max_chars >= LIST_MAX_TEXT) {
    text_max_chars = LIST_MAX_TEXT-1;
  }
  lst->text_max_chars = text_max_chars;

  // make control wider to fit allowed chars (Alternative: horizontal scrolling)
  w = text_max_chars * CHAR_WIDTH_PIXELS + 15;
  if (w < LIST_WIDTH_PIXELS) {
    w = LIST_WIDTH_PIXELS;
  }

  if (visible_count < 1) {
      visible_count = 1;
  }
  if (visible_count > LIST_MAX_ITEMS) {
      visible_count = LIST_MAX_ITEMS;
  }
  lst->visible_count = visible_count;
  item_height = CHAR_HEIGHT_PIXELS + (FOCUS_MARGIN_PIXELS * 2) + 4;
  h = visible_count * item_height;

  SetupWindow(&lst->windw, x, y, w, h, 0/*brd*/, 0/*buf*/); // no use for buffering this
  lst->scroll_idx = 0;
  lst->selected_idx = 0;
  lst->item_count = visible_count;//0;

  lst->item_count = 0;
  if (item_prefix != NULL) {
    // add default items.
    for (item_idx = 0; item_idx < visible_count; item_idx++) {
      if (item_prefix != NULL) {
        snprintf (dfl_text, LIST_MAX_TEXT, "%s %d", item_prefix, item_idx+1);
      }
      AddListItem (lst, dfl_text);
    } // end for (item_idx)
  }

  item_y = y;
  for (vis_idx = 0; vis_idx < visible_count; vis_idx++) {
    SetupWindow(&lst->vis_windw[vis_idx], x, item_y, w, item_height, 0/*brd*/, 0/*buf*/); // no use for buffering this
    item_y += item_height;
  } // end for (item_idx)
  lst->needs_redraw = true;

  lst->cb_func = cb_func;
}

void DrawList (struct list_s *lst, bool has_focus) {
  struct windw_s *wnd;
  struct windw_s *item_wnd;
  wnd = &lst->windw;
  int tx, ty, vis_idx; // visible index
  int item_idx;

  tft.fillRect(wnd->wx, wnd->wy, wnd->ww, wnd->wh, GetColorListBackgr());

  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setTextColor(color_list_text);
  item_idx = lst->scroll_idx;
  for (vis_idx = 0; vis_idx < lst->visible_count; vis_idx++) {
    item_wnd = &lst->vis_windw[vis_idx];
    DrawBorder(&tft, item_wnd, border_outset);

    // Draw text in control.
    tx = item_wnd->wx+3+FOCUS_MARGIN_PIXELS;
    ty = item_wnd->wy+3+FOCUS_MARGIN_PIXELS;
    tft.setCursor(tx, ty);

    Serial.print("vis_idx: ");
    Serial.print(vis_idx);
    Serial.print(" item_idx: ");
    Serial.print(item_idx);
    Serial.print(" x: ");
    Serial.print(tx);
    Serial.print(" y: ");
    Serial.print(ty);
    Serial.print(" text: \"");
    Serial.print(lst->text_str[item_idx]);
    Serial.println("\"");
    
    tft.print(lst->text_str[item_idx]);
 
    // Draw selection box
    if (has_focus && item_idx == lst->selected_idx) {
      DrawBorder(&tft, item_wnd, border_focus);
    }

    item_idx++;
  }
  Serial.print("item_count: ");
  Serial.println(lst->item_count);
  lst->needs_redraw = false;
}

void ClickList (struct list_s *lst) {
  char *sel_text;

  if (lst->cb_func == NULL) {
    return; // no callback - nothing to click.
  }
  // If we are here, lst->cb_func != NULL.
  sel_text = lst->text_str[lst->selected_idx];
  lst->cb_func(sel_text);
}

// ------------------- Up/Down control -------------------------

// create a new up/down control 'lst' on screen.
//  draw at upper-left corner 'x', 'y'
//  text field will contain up to 'text_max_chars'. This determines width on screen.
//  default value will be 'dfl_val'. min/max limits are 'min_val' to 'max_val'. Step size is 'val_step'.
//  when an item is chosen (with 'Enter', right-arrow, middle button, etc.), void cb_func(double sel_val) will be called.
//  when value is adjusted (with up-arrow, down-arrow, up button, down button, etc.), void cb_chg_func(double chg_val) will be called.
// When displayed, 'val_prec' is the number of digits that will be displayed after the decimal.
void SetupUpDown (struct up_down_s *updn, int x, int y, int text_max_chars, void (*cb_func)(double), void (*cb_chg_func)(double), double min_val, double max_val, double dfl_val, double val_step, int val_prec) {
  int w, h;
  char dfl_text[UP_DOWN_MAX_TEXT];

  if (text_max_chars >= UP_DOWN_MAX_TEXT) {
    text_max_chars = UP_DOWN_MAX_TEXT-1;
  }
  updn->text_max_chars = text_max_chars;

  // make control wider to fit allowed chars (Alternative: horizontal scrolling)
  w = text_max_chars * CHAR_WIDTH_PIXELS + 15;
  if (w < UP_DOWN_WIDTH_PIXELS) {
    w = UP_DOWN_WIDTH_PIXELS;
  }
  h = CHAR_HEIGHT_PIXELS + (FOCUS_MARGIN_PIXELS * 2) + 4;

  SetupWindow(&updn->     windw, x,   y,            w,             h,    0/*brd*/, 0/*buf*/); // no use for buffering this
  SetupWindow(&updn->  up_windw, x+w, y,       CHAR_WIDTH_PIXELS, (h/2), 0/*brd*/, 0/*buf*/);
  SetupWindow(&updn->down_windw, x+w, y+(h/2), CHAR_WIDTH_PIXELS, (h/2), 0/*brd*/, 0/*buf*/);

  updn->min_val = min_val;
  updn->max_val = max_val;
  updn->val_step = val_step;
  updn->val_prec = val_prec;
  updn->interior_focus = false;

  updn->sel_val = dfl_val;

  updn->cb_func = cb_func;
  updn->cb_chg_func = cb_chg_func;
}

void DrawUpDown (struct up_down_s *updn, bool has_focus) {
  struct windw_s *     wnd;
  struct windw_s *  up_wnd;
  struct windw_s *down_wnd;
  wnd      = &updn->     windw;
  up_wnd   = &updn->  up_windw;
  down_wnd = &updn->down_windw;
  int tx, ty;
  char sel_str[UP_DOWN_MAX_TEXT];

  tft.fillRect(wnd->wx, wnd->wy, wnd->ww, wnd->wh, GetColorUpDownBackgr());

  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setTextColor(color_up_down_text);
  if (updn->val_step < 1.0 || updn->val_prec > 0) {
    char   sign_part;
    int    int_part;

    double fract;
    double  scale;
    int    dec_part;

    if (updn->sel_val < 0) {
      sign_part = '-';
    } else {
      sign_part = ' ';
    }
    int_part = fabs((updn->sel_val)); // sign is added as a separately character.

    scale = pow10(updn->val_prec);
    fract = fmod(updn->sel_val,1);
    dec_part = round(fract * scale);
    dec_part = abs(dec_part); // force the decimal portion to be positive. can't have it display "-0.-5"

    if (updn->min_val < 0) {
      snprintf (sel_str, updn->text_max_chars+1, "%c%d.%d", sign_part, int_part, dec_part);
    } else { // don't save a place for the sign. All values will be >=0.
      snprintf (sel_str, updn->text_max_chars+1,   "%d.%d",            int_part, dec_part);
    }

    Serial.print("sel_val: ");
    Serial.print(updn->sel_val);
    Serial.print(" scale: ");
    Serial.print(scale);
    Serial.print(" fract: ");
    Serial.print(fract);
    Serial.print(" dec_part: ");
    Serial.print(dec_part);
    Serial.print(" sel_str: \"");
    Serial.print(sel_str);
    Serial.println("\"");      
  } else {
    Serial.print("sel_val: ");
    Serial.print(updn->sel_val);
    snprintf (sel_str, updn->text_max_chars+1, "%d", (int)(updn->sel_val));
    Serial.print(" sel_str: \"");
    Serial.print(sel_str);
    Serial.println("\"");
  }

  DrawBorder(&tft,      wnd, border_inset);
  DrawBorder(&tft,   up_wnd, border_outset);
  DrawBorder(&tft, down_wnd, border_outset);
  //#define UP_DOWN_ARROW_MARGIN 3
  // Drawing the triangle where the tip moves 1 pixel left or right makes it 'even'/straight.
  tft.fillTriangle(
                   up_wnd->wx + 3,                up_wnd->wy + up_wnd->wh - 5, // bottom  left corner
                   up_wnd->wx + (up_wnd->ww/2)-1, up_wnd->wy + 3,              // top point of triangle
                   up_wnd->wx + up_wnd->ww - 4,   up_wnd->wy + up_wnd->wh - 5, // bottom right corner
                   color_up_down_arrow);
  tft.fillTriangle(
                   up_wnd->wx + 3,                up_wnd->wy + up_wnd->wh - 5, // bottom  left corner
                   up_wnd->wx + (up_wnd->ww/2),   up_wnd->wy + 3,              // top point of triangle
                   up_wnd->wx + up_wnd->ww - 4,   up_wnd->wy + up_wnd->wh - 5, // bottom right corner
                   color_up_down_arrow);
  tft.fillTriangle(
                   down_wnd->wx + 3,                  down_wnd->wy + 3,                // top left corner
                   down_wnd->wx + (down_wnd->ww/2)-1, down_wnd->wy + down_wnd->wh - 7, // bottom point of triangle: note - hard to make this pointy, so stops with 7 pixels of pad below it, instead of 5
                   down_wnd->wx + down_wnd->ww - 4,   down_wnd->wy + 3,                // top right corner
                   color_up_down_arrow);
  tft.fillTriangle(
                   down_wnd->wx + 3,                  down_wnd->wy + 3,                // top left corner
                   down_wnd->wx + (down_wnd->ww/2),   down_wnd->wy + down_wnd->wh - 7, // bottom point of triangle
                   down_wnd->wx + down_wnd->ww - 4,   down_wnd->wy + 3,                // top right corner
                   color_up_down_arrow);
  // add a pointer tip to the 'down' arrow.
  tft.drawLine(down_wnd->wx + (down_wnd->ww/2)-1,   down_wnd->wy + 3,
               down_wnd->wx + (down_wnd->ww/2)-1,   down_wnd->wy + down_wnd->wh - 5,
               color_up_down_arrow);
  tft.drawLine(down_wnd->wx + (down_wnd->ww/2),   down_wnd->wy + 3,
               down_wnd->wx + (down_wnd->ww/2),   down_wnd->wy + down_wnd->wh - 5,
               color_up_down_arrow);

  // Draw text in control.
  tx = wnd->wx+3+FOCUS_MARGIN_PIXELS;
  ty = wnd->wy+3+FOCUS_MARGIN_PIXELS;
  tft.setCursor(tx, ty);
  tft.print(sel_str);
 
  // Draw selection box
  if (has_focus) {//} && item_idx == lst->selected_idx) {
    DrawBorder(&tft, wnd, border_focus);
  }
  //lst->needs_redraw = false;
}

// toggle between:
//  interior_focus == false: up/down moves switch between this control and previous/next controls in dialog
//  interior_focus == true: up/down moves increase/decrease value in this control
void ClickUpDownFocus (struct up_down_s *updn) {
  updn->interior_focus = !updn->interior_focus;
  if (!updn->interior_focus) {
    if (updn->cb_func != NULL) {
      updn->cb_func(updn->sel_val);
    }
  }
}

void RoundUpDownNearest (struct up_down_s *updn) {
  double scale;
  
  scale = pow10(updn->val_prec);
  Serial.print("RoundUpDownNearest() updn->sel_val: ");
  Serial.print(updn->sel_val);
  Serial.print(", scale: ");
  Serial.print(scale);
  updn->sel_val = round (updn->sel_val * scale) / scale;
  Serial.print(", rounded updn->sel_val: ");
  Serial.println(updn->sel_val);
}

//  up/down control goes 'down' in value
//  decreases the selected value by the step amount, up to the min.
//  Triggers re-draw.
bool ClickUpDownDown (struct up_down_s *updn) {
  if (!updn->interior_focus) {
    return false; // can't go 'down' in value; instead, 'screen' will select Next control.
  }

  updn->sel_val -= updn->val_step;
  RoundUpDownNearest (updn);
  if (updn->sel_val < updn->min_val) {
    updn->sel_val = updn->min_val;
  }
  DrawUpDown (updn, true/*has_focus*/);

  if (updn->cb_chg_func != NULL) {
    updn->cb_chg_func(updn->sel_val);
  }

  return true;
}

//  up/down control goes 'up' in value
//  increases the selected value by the step amount, up to the max.
//  Triggers re-draw.
bool ClickUpDownUp (struct up_down_s *updn) {
  if (!updn->interior_focus) {
    return false; // can't go 'up' in value; instead, 'screen' will select Previous control.
  }

  updn->sel_val += updn->val_step;
  RoundUpDownNearest (updn);
  if (updn->sel_val > updn->max_val) {
    updn->sel_val = updn->max_val;
  }
  DrawUpDown (updn, true/*has_focus*/);

  if (updn->cb_chg_func != NULL) {
    updn->cb_chg_func(updn->sel_val);
  }

  return true;
}

// return contents of check box control
double GetUpDown(struct up_down_s *updn) {
  return updn->sel_val;
}

// set contents of check box control
void SetUpDown(struct up_down_s *updn, double set_val) {
  if (set_val < updn->min_val) {
    return;
  }
  if (set_val > updn->max_val) {
    return;
  }
  updn->sel_val = set_val;
}

// ------------------- Check Box control -------------------------

void SetCheckBoxText (struct check_box_s *cb, const char *text_str) {
  if (text_str != NULL) {
    strncpy (cb->text_str, text_str, CHECK_BOX_MAX_TEXT);
  } else { // text_str == NULL
    cb->text_str[0] = '\x0';
  }
  cb->text_str[CHECK_BOX_MAX_TEXT-1] = '\x0'; // ensure null term.
}

// Create a Check Box control 'cb' at 'x', 'y'. Width is defined by 'text_str', but a minimum of 'CHECK_BOX_WIDTH_PIXELS'.
//  Height is CHECK_BOX_HEIGHT_PIXELS. Initial check box state is 'checked_default'.
//  Callback function 'cb_func' will have signature e.g. 'void my_func(int check_val)'.
void SetupCheckBox (struct check_box_s *cb, int x, int y, const char *text_str, int checked_default, void (*cb_func)(int)) {
  int num_chars, w;

  num_chars = strlen(text_str);
  // make control wider to fit text_str label
  w = 8 + CHECK_BOX_SQUARE_PIXELS + 3 + ((num_chars * CHAR_WIDTH_PIXELS) + 9);
  if (w < CHECK_BOX_WIDTH_PIXELS) {
    w = CHECK_BOX_WIDTH_PIXELS;
  }
  SetupWindow(&cb->windw,        x,   y,                             w,   CHECK_BOX_HEIGHT_PIXELS/*h*/, 0/*brd*/, 0/*buf*/);
  SetupWindow(&cb->square_windw, x+8, y + ((CHECK_BOX_HEIGHT_PIXELS-CHECK_BOX_SQUARE_PIXELS)/2)+1, // vertically center square_windw next to text_str
                                            CHECK_BOX_SQUARE_PIXELS/*w*/, CHECK_BOX_SQUARE_PIXELS/*h*/, 0/*brd*/, 0/*buf*/);

  SetCheckBoxText(cb, text_str);
  cb->checked = checked_default;
  cb->cb_func = cb_func;
}

void DrawCheckBox (struct check_box_s *cb, bool has_focus) {
  struct windw_s *   wnd;
  struct windw_s *sq_wnd;
  wnd    = &cb->       windw;
  sq_wnd = &cb->square_windw;
  int tx, ty;
  int x1,y1,x2,y2;

  tx = wnd->wx+8+CHECK_BOX_SQUARE_PIXELS+3;
  ty = wnd->wy+9;

  tft.fillRect(wnd->wx, wnd->wy, wnd->ww, wnd->wh, GetColorCheckBoxBackgr());

  DrawBorder(&tft, sq_wnd, border_inset);

  // Draw text in control.
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setTextColor(color_check_box_text);
  tft.setCursor(tx, wnd->wy+10);
  tft.print(cb->text_str);

  if (cb->checked) {
    int offs;

    for (offs = 0; offs <= CHECK_BOX_CHECK_WIDTH_PIXELS; offs++) {
      // top left corner, offset 0 then to the right - to make line thicker.
      x1 = sq_wnd->wx + CHECK_BOX_CHECK_MARGIN + offs;
      y1 = sq_wnd->wy + CHECK_BOX_CHECK_MARGIN;
      // bottom right corner, offset to the left then fully to the right into the corner.
      x2 = sq_wnd->wx + sq_wnd->ww - CHECK_BOX_CHECK_MARGIN - 1 - CHECK_BOX_CHECK_WIDTH_PIXELS + offs;
      y2 = sq_wnd->wy + sq_wnd->wh - CHECK_BOX_CHECK_MARGIN - 1;
      tft.drawLine(x1, y1, x2, y2, color_check_box_check);
      tft.drawLine(x1, y2, x2, y1, color_check_box_check);
    }
    for (offs = 0; offs <= CHECK_BOX_CHECK_WIDTH_PIXELS; offs++) {
      // top left corner, offset 0 then down.
      x1 = sq_wnd->wx + CHECK_BOX_CHECK_MARGIN;
      y1 = sq_wnd->wy + CHECK_BOX_CHECK_MARGIN + offs;
      // bottom right corner, offset up, then fully down into the corner.
      x2 = sq_wnd->wx + sq_wnd->ww - CHECK_BOX_CHECK_MARGIN - 1;
      y2 = sq_wnd->wy + sq_wnd->wh - CHECK_BOX_CHECK_MARGIN - 1 - CHECK_BOX_CHECK_WIDTH_PIXELS + offs;
      tft.drawLine(x1, y1, x2, y2, color_check_box_check);
      tft.drawLine(x1, y2, x2, y1, color_check_box_check);
    }
  }
  if (has_focus) {
    DrawBorder(&tft, wnd, border_focus);
  }
}

void ClickCheckBox (struct check_box_s *cb) {
  int checked_val;

  cb->checked = !cb->checked;

  if (cb->cb_func == NULL) {
    return; // no callback - nothing to click.
  }
  // If we are here, cb->cb_func != NULL.
  cb->cb_func(cb->checked);
}

// return contents of check box control
int GetCheckBox (struct check_box_s *cb) {
  return cb->checked;
}

// set contents of check box control
void SetCheckBox (struct check_box_s *cb, int checked) {
  cb->checked = checked;
}